const std = @import("std");
const builtin = @import("builtin");

const ingble = @import("ingble.zig");

const gatt = @import("gatt_client_async.zig");

const print = ingble.platform_printf;
const enable_print = ingble.print_info == 1;

const rom_crc: ingble.f_crc_t = @intToPtr(ingble.f_crc_t, ingble.ROM_FUNC_ADDR_CRC);

fn same_version(a: *const ingble.prog_ver_t, b: * const ingble.prog_ver_t) bool {
    return (a.*.major == b.*.major) and (a.*.minor == b.*.minor) and (a.*.patch == b.*.patch);
}

const HASH_SIZE = 32;
const PK_SIZE = 64;
const SK_SIZE = 32;

const FullSigMeta = packed struct {
    cmd_id: u8,
    sig: [64]u8,
    crc_value: u16,
    entry: u32,
    blocks: [ingble.MAX_UPDATE_BLOCKS]ingble.fota_update_block_t,
};

const MAX_PAGE_SIZE = 8192;

const EccKeys = struct {
    // externally provided buffer
    peer_pk: [64]u8,
    dh_sk: [32]u8,
    xor_sk: [32]u8,
    page_buffer: [MAX_PAGE_SIZE]u8,
    page_end_cmd: [5 + 64]u8,
};

const Client = struct {
    status: u8 = 0,
    conn_handle: u16,
    handle_ver: u16,
    handle_ctrl: u16,
    handle_data: u16,
    handle_pk: u16,
    mtu: u16,
    sig_meta: FullSigMeta = undefined,
    ecc_driver: ?*ingble.ecc_driver_t,
    ecc_keys: ?*EccKeys = null,

    fn fota_make_bitmap(self: *Client, optional_latest: ?*const ingble.ota_ver_t) ?u16 {
        var bitmap: u16 = 0xffff;
        if (optional_latest) |latest| {
            var value_size: u16 = undefined;
            if (gatt.value_of_characteristic_reader.read(self.conn_handle, self.handle_ver, &value_size)) |value| {
                if (value_size != @sizeOf(ingble.ota_ver_t)) {
                    return null;
                }

                const local = @ptrCast(*const ingble.ota_ver_t, @alignCast(2, value));
                if (enable_print) {
                    print("device version:\n");
                    print("platform: %d.%d.%d\n", local.*.platform.major, local.*.platform.minor, local.*.platform.patch);
                    print("     app: %d.%d.%d\n", local.*.app.major, local.*.app.minor, local.*.app.patch);
                }
                if (same_version(&local.*.platform, &latest.*.platform)) {
                    bitmap &= ~@as(u16, 0b01);
                    if (same_version(&local.*.app, &latest.*.app))
                        bitmap &= ~@as(u16, 0b10);
                }
            }
            else
                return null;
        }
        return bitmap;
    }

    fn write_cmd2(self: *Client, len: usize, data: [*c] const u8) u8 {
        return ingble.gatt_client_write_value_of_characteristic_without_response(self.conn_handle, self.handle_ctrl, @intCast(u16, len), data);
    }

    fn read_status(self: *Client) u8 {
        var value_size: u16 = undefined;
        return (gatt.value_of_characteristic_reader.read(self.conn_handle, self.handle_ctrl, &value_size) orelse return 0xff).*;
    }

    fn check_status(self: *Client) bool {
        return self.read_status() == ingble.OTA_STATUS_OK;
    }

    fn enable_fota(self: *Client) bool {
        const cmd = [_]u8{ingble.OTA_CTRL_START, 0, 0, 0, 0};
        _ = self.write_cmd2(cmd.len, &cmd[0]);
        return self.check_status();
    }

    fn reboot(self: *Client) bool {
        const cmd = [_]u8{ingble.OTA_CTRL_REBOOT};
        _ = self.write_cmd2(cmd.len, &cmd[0]);
        return true;
    }

    fn burn_page(self: *Client, data: [*c] u8, target_addr: u32, size: usize,
                 page_end_cmd: [*c]u8, cmd_size: usize) bool {
        if (enable_print) {
            print("burn_page: %p, %08x, %d\n", data, target_addr, size);
        }
        var cmd = page_end_cmd;
        cmd[0] = ingble.OTA_CTRL_PAGE_BEGIN;
        @memcpy(@ptrCast([*]u8, &cmd[1]), @ptrCast([*]const u8, &target_addr), 4);
        _ = self.write_cmd2(5, &cmd[0]);
        if (!self.check_status()) return false;
        var remain = size;
        var read = data;

        while (remain > 0) {
            const block = if (remain >= self.mtu) self.mtu else remain;
            if (ingble.gatt_client_write_value_of_characteristic_without_response(self.conn_handle,
                self.handle_data, @intCast(u16, block), read) != 0) {
                if (!self.check_status())
                    return false;
                continue;
            }
            read = read + block;
            remain -= block;
        }
        var crc_value = rom_crc.?(data, @intCast(u16, size));
        cmd[0] = ingble.OTA_CTRL_PAGE_END;
        cmd[1] = @intCast(u8, size & 0xff);
        cmd[2] = @intCast(u8, size >> 8);
        cmd[3] = @intCast(u8, crc_value & 0xff);
        cmd[4] = @intCast(u8, crc_value >> 8);
        _ = self.write_cmd2(cmd_size, &cmd[0]);
        if (!self.check_status()) return false;
        return true;
    }

    fn encrypt(self: *Client, buff: [*c]u8, len: usize) void {
        var i: usize = 0;
        while (i < len) : (i += 1) {
            buff[i] ^= self.ecc_keys.?.xor_sk[i & 0x1f];
        }
    }

    fn burn_item(self: *Client, local_storage_addr: u32,
            target_storage_addr: u32,
            size0: u32, PAGE_SIZE: u32) bool {
        var local = local_storage_addr;
        var target = target_storage_addr;
        var size = size0;
        var cmd = [_]u8{0, 0, 0, 0, 0 };
        while (size > 0) {
            const block = if (size >= PAGE_SIZE) PAGE_SIZE else size;
            if (self.ecc_keys) |keys| {
                if (PAGE_SIZE > MAX_PAGE_SIZE) return false;
                @memcpy(@ptrCast([*c] u8, &keys.page_buffer[0]), @intToPtr([*c] u8, local), block);
                self.ecc_driver.?.sign.?(&self.ecc_driver.?.session_sk[0], &keys.page_buffer[0], @intCast(c_int, block), &keys.page_end_cmd[5]);
                self.encrypt(&keys.page_buffer[0], block);
                if (!self.burn_page(&keys.page_buffer[0], target, block, &keys.page_end_cmd[0], @sizeOf(@TypeOf(keys.page_end_cmd)))) return false;
            } else {
                if (!self.burn_page(@intToPtr([*c] u8, local), target, block, &cmd[0], @sizeOf(@TypeOf(cmd)))) return false;
            }
            local += block;
            target += block;
            size -= block;
        }
        return true;
    }

    fn burn_items(self: *Client, bitmap: u16, item_cnt: c_int, items: [*c] const ingble.ota_item_t, page_size: u32) bool {
        var index: usize = 0;
        while (index < item_cnt) : (index += 1) {
            if ((bitmap & (@as(u16, 1) << @intCast(u4, index))) == 0) continue;
            if (enable_print) {
                print("start new item: %d\n", index);
            }
            if (!self.burn_item(items[index].local_storage_addr,
                                items[index].target_storage_addr,
                                items[index].size,
                                page_size)) return false;
        }
        return true;
    }

    fn prepare_meta_info(self: *Client, bitmap: u16, item_cnt: c_int, items: [*c] const ingble.ota_item_t, entry: u32,
                         data: *FullSigMeta) c_int {
        _ = self;
        var index: usize = 0;
        var cnt: usize = 0;
        data.entry = entry;
        while (index < item_cnt) : (index += 1) {
            if ((bitmap & (@as(u16, 1) << @intCast(u4, index))) == 0) continue;
            data.blocks[cnt].src = items[index].target_storage_addr;
            data.blocks[cnt].src = items[index].target_storage_addr;
            data.blocks[cnt].dest = items[index].target_load_addr;
            data.blocks[cnt].size = items[index].size;
            cnt += 1;
        }
        return @intCast(c_int, cnt);
    }

    fn burn_meta(self: *Client, bitmap: u16, item_cnt: c_int, items: [*c] const ingble.ota_item_t, entry: u32, check_meta: bool) bool {
        const cnt = self.prepare_meta_info(bitmap, item_cnt, items, entry, &self.sig_meta);

        var total: c_int = undefined;
        var p: [*c]u8 = undefined;
        if (self.ecc_driver) |driver| {
            driver.sign.?(&self.ecc_driver.?.session_sk[0], @ptrCast([*c] u8, &self.sig_meta.entry), @intCast(u16, cnt * 12 + 4),
                        &self.sig_meta.sig[0]);
            self.encrypt(@ptrCast([*c] u8, &self.sig_meta.entry), @intCast(usize, cnt * 12 + 4));
            total = @sizeOf(FullSigMeta) - ingble.MAX_UPDATE_BLOCKS * @sizeOf(ingble.fota_update_block_t) + cnt * 12;
            p = @ptrCast([*c] u8, &self.sig_meta);

        } else {
            p = &self.sig_meta.sig[63];
            total = cnt * 12 + 4 + 2 + 1;
        }

        p[0] = ingble.OTA_CTRL_METADATA;
        self.sig_meta.crc_value = rom_crc.?(@ptrCast([*c] u8, &self.sig_meta.entry), @intCast(u16, cnt * 12 + 4));
        _ = self.write_cmd2(@intCast(usize, total), p);
        if (check_meta) {
            return self.check_status();
        } else {
            return true;
        }
    }

    pub fn update(self: *Client, target_family: c_int, optional_latest: ?*const ingble.ota_ver_t,
                              conn_handle: u16,
                              handle_ver: u16, handle_ctrl: u16, handle_data: u16, handle_pk: u16,
                              item_cnt: c_int, items: [*c] const ingble.ota_item_t,
                              entry: u32,
                              driver: ?*ingble.ecc_driver_t) c_int {
        self.conn_handle = conn_handle;
        self.handle_ver = handle_ver;
        self.handle_ctrl = handle_ctrl;
        self.handle_data = handle_data;
        self.handle_pk = handle_pk;
        self.ecc_driver = driver;
        if (item_cnt > ingble.MAX_UPDATE_BLOCKS) return -127;
        _ = ingble.gatt_client_get_mtu(conn_handle, &self.mtu);
        self.mtu -= 3;

        defer {
            if (self.ecc_keys) |keys| self.ecc_driver.?.free.?(keys);
        }

        if (self.ecc_driver) |a_driver| {
            var value_size: u16 = 0;
            const value = gatt.value_of_characteristic_reader.read(conn_handle, handle_pk, &value_size) orelse return -20;
            if (value_size != PK_SIZE) return -21;
            self.ecc_keys = @ptrCast(*EccKeys, a_driver.alloc.?(@sizeOf(EccKeys)) orelse return -22);

            @memcpy(@ptrCast([*]u8, &self.ecc_keys.?.peer_pk[0]), value, PK_SIZE);

            var buf = @ptrCast([*c]u8, a_driver.alloc.?(PK_SIZE * 2));
            defer self.ecc_driver.?.free.?(buf);

            @memcpy(@ptrCast([*]u8, &buf[0]), a_driver.session_pk, PK_SIZE);
            a_driver.sign.?(a_driver.root_sk, a_driver.session_pk, PK_SIZE, &buf[PK_SIZE]);
            if (0 != ingble.gatt_client_write_value_of_characteristic_without_response(conn_handle, handle_pk, PK_SIZE * 2, &buf[0])) return -24;
            if (self.read_status() == ingble.OTA_STATUS_ERROR)
                return -25;

            a_driver.shared_secret.?(&a_driver.session_sk[0], &self.ecc_keys.?.peer_pk[0], &self.ecc_keys.?.dh_sk[0]);
            a_driver.sha_256.?(&self.ecc_keys.?.xor_sk[0], &self.ecc_keys.?.dh_sk[0], @sizeOf(@TypeOf(self.ecc_keys.?.dh_sk)));
        }

        const bitmap = ((@intCast(u16, 1) << @intCast(u4, item_cnt)) - 1) & (fota_make_bitmap(self, optional_latest) orelse return -1);
        if (bitmap == 0) return 1;
        if (enable_print) {
            print("update bitmap: %02x\n", bitmap);
        }

        const page_size: u32 = if (target_family == ingble.INGCHIPS_FAMILY_918) 8192 else 4096;
        const check_meta = (target_family == ingble.INGCHIPS_FAMILY_918);

        if (!self.enable_fota()) return -2;
        if (!self.burn_items(bitmap, item_cnt, items, page_size)) return -3;
        if (!self.burn_meta(bitmap, item_cnt, items, entry, check_meta)) return -4;
        if (check_meta and !self.reboot()) return -5;
        return 0;
    }
};

fn fota_client_do_update_async(target_family: c_int, optional_latest: ?*const ingble.ota_ver_t,
                              conn_handle: u16,
                              handle_ver: u16, handle_ctrl: u16, handle_data: u16,
                              handle_pk: u16,
                              item_cnt: c_int, items: [*c] const ingble.ota_item_t,
                              entry: u32,
                              on_done: fn (err_code: c_int) callconv(.C) void,
                              driver: ?*ingble.ecc_driver_t) void {
    var client: Client = undefined;
    on_done(client.update(target_family, optional_latest, conn_handle, handle_ver, handle_ctrl, handle_data, handle_pk,
                          item_cnt, items, entry, driver));
}

var fota_frame: @Frame(fota_client_do_update_async) = undefined;

export fn fota_client_do_update(target_family: c_int, optional_latest: ?*const ingble.ota_ver_t,
                              conn_handle: u16,
                              handle_ver: u16, handle_ctrl: u16, handle_data: u16,
                              item_cnt: c_int, items: [*c] const ingble.ota_item_t,
                              entry: u32,
                              on_done: fn (err_code: c_int) callconv(.C) void) void {
    fota_frame = async fota_client_do_update_async(target_family, optional_latest, conn_handle,
                                                   handle_ver, handle_ctrl, handle_data, 0xffff,
                                                   item_cnt, items, entry, on_done,
                                                   null);
}

export fn secure_fota_client_do_update(target_family: c_int, optional_latest: ?*const ingble.ota_ver_t,
                              conn_handle: u16,
                              handle_ver: u16, handle_ctrl: u16, handle_data: u16, handle_pk: u16,
                              item_cnt: c_int, items: [*c] const ingble.ota_item_t,
                              entry: u32,
                              on_done: fn (err_code: c_int) callconv(.C) void,
                              driver: *ingble.ecc_driver_t) void {
    fota_frame = async fota_client_do_update_async(target_family, optional_latest, conn_handle,
                                                   handle_ver, handle_ctrl, handle_data, handle_pk,
                                                   item_cnt, items, entry, on_done,
                                                   driver);
}

pub fn panic(message: []const u8, stack_trace: ?*std.builtin.StackTrace) noreturn {
    _ = stack_trace;
    print("\n!KERNEL PANIC!\n");
    for (message) |value| {
        print("%c", value);
    }
    ingble.platform_raise_assertion("panic", 0);
    while (true) {}
}