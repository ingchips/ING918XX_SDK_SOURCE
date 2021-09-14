const std = @import("std");
const builtin = @import("builtin");

pub usingnamespace @import("ingble.zig");

pub usingnamespace @import("gatt_client_async.zig");

const print = platform_printf;
const enable_print = print_info == 1;

const rom_crc: f_crc_t = @intToPtr(f_crc_t, 0x00000f79);

fn same_version(a: *const prog_ver_t, b: * const prog_ver_t) bool {
    return (a.*.major == b.*.major) and (a.*.minor == b.*.minor) and (a.*.patch == b.*.patch);
}

const PAGE_SIZE = 8192;

const FullMeta = packed struct {
    cmd_id: u8,
    crc_value: u16,
    entry: u32,
    blocks: [MAX_UPDATE_BLOCKS]fota_update_block_t,
};

const Client = struct {
    status: u8 = 0,
    conn_handle: u16,
    handle_ver: u16,
    handle_ctrl: u16,
    handle_data: u16,
    mtu: u16,
    meta_data: FullMeta = undefined,

    fn fota_make_bitmap(self: *Client, optional_latest: ?*const ota_ver_t) ?u16 {
        var bitmap: u16 = 0xffff;
        if (optional_latest) |latest| {
            var value_size: u16 = undefined;
            if (value_of_characteristic_reader.read(self.conn_handle, self.handle_ver, &value_size)) |value| {
                print("size: %d\n", value_size);
                if (value_size != @sizeOf(ota_ver_t)) {
                    return null;
                }

                const local = @ptrCast(*const ota_ver_t, @alignCast(2, value));
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
        return gatt_client_write_value_of_characteristic_without_response(self.conn_handle, self.handle_ctrl, @intCast(u16, len), data);
    }

    fn read_status(self: *Client) u8 {
        var value_size: u16 = undefined;
        return (value_of_characteristic_reader.read(self.conn_handle, self.handle_ctrl, &value_size) orelse return 0xff).*;
    }

    fn check_status(self: *Client) bool {
        return self.read_status() == OTA_STATUS_OK;
    }

    fn enable_fota(self: *Client) bool {
        const cmd = [_]u8{OTA_CTRL_START, 0, 0, 0, 0};
        _ = self.write_cmd2(cmd.len, &cmd[0]);
        return self.check_status();
    }

    fn reboot(self: *Client) bool {
        const cmd = [_]u8{OTA_CTRL_REBOOT};
        _ = self.write_cmd2(cmd.len, &cmd[0]);
        return true;
    }

    fn burn_page(self: *Client, data: [*c] u8, target_addr: u32, size: usize) bool {
        if (enable_print) {
            print("burn_page: %p, %08x, %d\n", data, target_addr, size);
        }
        var cmd = [_]u8{OTA_CTRL_PAGE_BEGIN, 0, 0, 0, 0 };
        @memcpy(@ptrCast([*]u8, &cmd[1]), @ptrCast([*]const u8, &target_addr), 4);
        _ = self.write_cmd2(cmd.len, &cmd[0]);
        if (!self.check_status()) return false;
        var remain = size;
        var read = data;

        while (remain > 0) {
            const block = if (remain >= self.mtu) self.mtu else remain;
            if (gatt_client_write_value_of_characteristic_without_response(self.conn_handle,
                self.handle_data, @intCast(u16, block), read) != 0) {
                if (!self.check_status())
                    return false;
                continue;
            }
            read = read + block;
            remain -= block;
        }

        var crc_value = rom_crc.?(data, @intCast(u16, size));
        cmd[0] = OTA_CTRL_PAGE_END;
        cmd[1] = @intCast(u8, size & 0xff);
        cmd[2] = @intCast(u8, size >> 8);
        cmd[3] = @intCast(u8, crc_value & 0xff);
        cmd[4] = @intCast(u8, crc_value >> 8);
        _ = self.write_cmd2(cmd.len, &cmd[0]);
        if (!self.check_status()) return false;
        return true;
    }

    fn burn_item(self: *Client, local_storage_addr: u32,
            target_storage_addr: u32,
            size0: u32,) bool {
        var local = local_storage_addr;
        var target = target_storage_addr;
        var size = size0;
        while (size > 0) {
            const block = if (size >= PAGE_SIZE) PAGE_SIZE else size;
            if (!self.burn_page(@intToPtr([*c] u8, local), target, block)) return false;
            local += block;
            target += block;
            size -= block;
        }
        return true;
    }

    fn burn_items(self: *Client, bitmap: u16, item_cnt: c_int, items: [*c] const ota_item_t) bool {
        var index: usize = 0;
        while (index < item_cnt) : (index += 1) {
            if ((bitmap & (@as(u16, 1) << @intCast(u4, index))) == 0) continue;
            if (enable_print) {
                print("start new item: %d\n", index);
            }
            if (!self.burn_item(items[index].local_storage_addr,
                                items[index].target_storage_addr,
                                items[index].size)) return false;
        }
        return true;
    }

    fn burn_meta(self: *Client, bitmap: u16, item_cnt: c_int, items: [*c] const ota_item_t, entry: u32) bool {
        var index: usize = 0;
        var cnt: usize = 0;
        while (index < item_cnt) : (index += 1) {
            if ((bitmap & (@as(u16, 1) << @intCast(u4, index))) == 0) continue;
            self.meta_data.blocks[cnt].src = items[index].target_storage_addr;
            self.meta_data.blocks[cnt].dest = items[index].target_load_addr;
            self.meta_data.blocks[cnt].size = items[index].size;
            cnt += 1;
        }
        self.meta_data.cmd_id = OTA_CTRL_METADATA;
        self.meta_data.entry = entry;
        const total = 7 + cnt * 12;
        self.meta_data.crc_value = rom_crc.?(@ptrCast([*c] u8, &self.meta_data.entry), @intCast(u16, total - 3));
        _ = self.write_cmd2(total, @ptrCast([*c] u8, &self.meta_data));
        return self.check_status();
    }

    pub fn update(self: *Client, optional_latest: ?*const ota_ver_t,
                              conn_handle: u16,
                              handle_ver: u16, handle_ctrl: u16, handle_data: u16,
                              item_cnt: c_int, items: [*c] const ota_item_t,
                              entry: u32) c_int {
        self.conn_handle = conn_handle;
        self.handle_ver = handle_ver;
        self.handle_ctrl = handle_ctrl;
        self.handle_data = handle_data;
        if (item_cnt > MAX_UPDATE_BLOCKS) return -127;
        _ = gatt_client_get_mtu(conn_handle, &self.mtu);
        self.mtu -= 3;

        const bitmap = ((@intCast(u16, 1) << @intCast(u4, item_cnt)) - 1) & (fota_make_bitmap(self, optional_latest) orelse return -1);
        if (bitmap == 0) return 0;
        if (enable_print) {
            print("update bitmap: %02x\n", bitmap);
        }

        if (!self.enable_fota()) return -2;
        if (!self.burn_items(bitmap, item_cnt, items)) return -3;
        if (!self.burn_meta(bitmap, item_cnt, items, entry)) return -4;
        if (!self.reboot()) return -5;
        return 0;
    }
};

fn fota_client_do_update_async(optional_latest: ?*const ota_ver_t,
                              conn_handle: u16,
                              handle_ver: u16, handle_ctrl: u16, handle_data: u16,
                              item_cnt: c_int, items: [*c] const ota_item_t,
                              entry: u32,
                              on_done: fn (err_code: c_int) callconv(.C) void) void {
    var client: Client = undefined;
    on_done(client.update(optional_latest, conn_handle, handle_ver, handle_ctrl, handle_data, item_cnt, items, entry));
}

var fota_frame: @Frame(fota_client_do_update_async) = undefined;

export fn fota_client_do_update(optional_latest: ?*const ota_ver_t,
                              conn_handle: u16,
                              handle_ver: u16, handle_ctrl: u16, handle_data: u16,
                              item_cnt: c_int, items: [*c] const ota_item_t,
                              entry: u32,
                              on_done: fn (err_code: c_int) callconv(.C) void) void {
    fota_frame = async fota_client_do_update_async(optional_latest, conn_handle, handle_ver, handle_ctrl, handle_data, item_cnt, items, entry, on_done);
}

pub fn panic(message: []const u8, stack_trace: ?*std.builtin.StackTrace) noreturn {
    print("\n!KERNEL PANIC!\n");
    for (message) |value| {
        print("%c", value);
    }
    platform_raise_assertion("panic", 0);
    while (true) {}
}