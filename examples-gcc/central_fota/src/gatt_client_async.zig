//! This module provides async functions for GATT Client.
//! CAUTION: This is an experimental module.

const ingble = @import("ingble.zig");
const GATT_EVENT_SERVICE_QUERY_RESULT = ingble.GATT_EVENT_SERVICE_QUERY_RESULT;
const GATT_EVENT_QUERY_COMPLETE = ingble.GATT_EVENT_QUERY_COMPLETE;
const GATT_EVENT_CHARACTERISTIC_QUERY_RESULT = ingble.GATT_EVENT_CHARACTERISTIC_QUERY_RESULT;
const GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT = ingble.GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT;
const GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT = ingble.GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT;
const gatt_client_read_value_of_characteristic_using_value_handle = ingble.gatt_client_read_value_of_characteristic_using_value_handle;

const hci_con_handle_t = ingble.hci_con_handle_t;
const gatt_client_service_t = ingble.gatt_client_service_t;
const gatt_client_characteristic_t = ingble.gatt_client_characteristic_t;
const gatt_client_characteristic_descriptor_t = ingble.gatt_client_characteristic_descriptor_t;

const gatt_event_query_complete_parse = ingble.gatt_event_query_complete_parse;
const gatt_event_characteristic_query_result_parse = ingble.gatt_event_characteristic_query_result_parse;
const gatt_client_discover_characteristics_for_service = ingble.gatt_client_discover_characteristics_for_service;
const gatt_client_discover_characteristics_for_handle_range_by_uuid16 = ingble.gatt_client_discover_characteristics_for_handle_range_by_uuid16;
const gatt_client_discover_characteristics_for_handle_range_by_uuid128 = ingble.gatt_client_discover_characteristics_for_handle_range_by_uuid128;
const gatt_event_all_characteristic_descriptors_query_result_parse = ingble.gatt_event_all_characteristic_descriptors_query_result_parse;
const gatt_client_discover_characteristic_descriptors = ingble.gatt_client_discover_characteristic_descriptors;
const gatt_client_write_value_of_characteristic = ingble.gatt_client_write_value_of_characteristic;

var session_complete: bool = true;

const print = ingble.platform_printf;

pub var all_services_discoverer: AllServicesDiscoverer = undefined;

const AllServicesDiscoverer = struct {
    status: u8 = 0,
    frame: ? (anyframe -> ?gatt_client_service_t)= null,
    service: gatt_client_service_t = undefined,

    fn callback(packet_type: u8, _: u16, packet: [*c] const u8, size: u16) callconv(.C) void {
        _ = size; _= packet_type;
        switch (packet[0]) {
            GATT_EVENT_SERVICE_QUERY_RESULT => {
                var result = ingble.gatt_event_service_query_result_parse(packet);
                all_services_discoverer.service = result.*.service;
            },
            GATT_EVENT_QUERY_COMPLETE => {
                session_complete = true;
                all_services_discoverer.status = gatt_event_query_complete_parse(packet).*.status;
            },
            else => { return; }
        }
        if (all_services_discoverer.frame) |f| {
            resume f;
        }
    }

    pub fn start(self: *AllServicesDiscoverer, conn_handle: hci_con_handle_t) bool {
        if (!session_complete) return false;
        self.frame = null;
        session_complete = false;
        if (0 == ingble.gatt_client_discover_primary_services(callback, conn_handle)) {
            return !session_complete;
        } else {
            session_complete = true;
            return false;
        }
    }

    pub fn next(self: *AllServicesDiscoverer) ?gatt_client_service_t {
        if (session_complete) return null;

        suspend {
            self.frame = @frame();
        }

        if (session_complete) {
            return null;
        } else {
            return self.service;
        }
    }
};

pub var service_discoverer: ServiceDiscoverer = undefined;

const ServiceDiscoverer = struct {
    status: u8 = 0,
    frame: ? (anyframe -> ?gatt_client_service_t)= null,
    service: ?gatt_client_service_t = undefined,

    fn callback(packet_type: u8, _: u16, packet: [*c] const u8, size: u16) callconv(.C) void {
        _ = packet_type; _ = size;
        switch (packet[0]) {
            GATT_EVENT_SERVICE_QUERY_RESULT => {
                var result = ingble.gatt_event_service_query_result_parse(packet);
                service_discoverer.service = result.*.service;
            },
            GATT_EVENT_QUERY_COMPLETE => {
                session_complete = true;
                service_discoverer.status = gatt_event_query_complete_parse(packet).*.status;
                if (service_discoverer.frame) |f| {
                    resume f;
                }
            },
            else => { }
        }
    }

    pub fn discover_16(self: *ServiceDiscoverer, conn_handle: hci_con_handle_t, uuid: u16) ?gatt_client_service_t {
        if (!session_complete) return null;

        self.frame = null;
        self.service = null;
        session_complete = false;
        if (0 != ingble.gatt_client_discover_primary_services_by_uuid16(callback, conn_handle, uuid)) {
            session_complete = true;
            return null;
        }

        suspend {
            self.frame = @frame();
        }

        return self.service;
    }

    pub fn discover_128(self: *ServiceDiscoverer, conn_handle: hci_con_handle_t, uuid: [*c] const u8) ?gatt_client_service_t {
        if (!session_complete) return null;

        self.frame = null;
        self.service = null;
        session_complete = false;
        if (0 != ingble.gatt_client_discover_primary_services_by_uuid128(callback, conn_handle, uuid)) {
            session_complete = true;
            return null;
        }

        suspend {
            self.frame = @frame();
        }

        return self.service;
    }
};

pub var all_characteristics_discoverer: AllCharacteristicsDiscoverer = undefined;

const AllCharacteristicsDiscoverer = struct {
    status: u8 = 0,
    frame: ?(anyframe -> ?gatt_client_characteristic_t) = null,
    characteristic: gatt_client_characteristic_t = undefined,

    fn callback(packet_type: u8, _: u16, packet: [*c] const u8, size: u16) callconv(.C) void {
        _ = size; _= packet_type;
        switch (packet[0]) {
            GATT_EVENT_CHARACTERISTIC_QUERY_RESULT => {
                var result = gatt_event_characteristic_query_result_parse(packet);
                all_characteristics_discoverer.characteristic = result.*.characteristic;
            },
            GATT_EVENT_QUERY_COMPLETE => {
                session_complete = true;
                all_characteristics_discoverer.status = gatt_event_query_complete_parse(packet).*.status;
            },
            else => { }
        }
        if (all_characteristics_discoverer.frame) |f| {
            resume f;
        }
    }

    pub fn start(self: *AllCharacteristicsDiscoverer, conn_handle: hci_con_handle_t,
                 start_group_handle: u16, end_group_handle: u16) bool {
        if (!session_complete) return false;
        self.frame = null;
        session_complete = false;
        if (0 == gatt_client_discover_characteristics_for_service(callback, conn_handle,
                                                                  start_group_handle,
                                                                  end_group_handle)) {
            return !session_complete;
        } else {
            session_complete = true;
            return false;
        }
    }

    pub fn next(self: *AllCharacteristicsDiscoverer) ?gatt_client_characteristic_t {
        if (session_complete) return null;

        suspend {
            self.frame = @frame();
        }

        if (session_complete) {
            return null;
        } else {
            return self.characteristic;
        }
    }
};

pub var characteristics_discoverer: CharacteristicsDiscoverer = undefined;

const CharacteristicsDiscoverer = struct {
    status: u8 = 0,
    frame: ? (anyframe -> ?gatt_client_characteristic_t)= null,
    characteristic: ?gatt_client_characteristic_t = undefined,

    fn callback(packet_type: u8, _: u16, packet: [*c] const u8, size: u16) callconv(.C) void {
        _ = size; _= packet_type;
        switch (packet[0]) {
            GATT_EVENT_CHARACTERISTIC_QUERY_RESULT => {
                var result = gatt_event_characteristic_query_result_parse(packet);
                characteristics_discoverer.characteristic = result.*.characteristic;
            },
            GATT_EVENT_QUERY_COMPLETE => {
                session_complete = true;
                characteristics_discoverer.status = gatt_event_query_complete_parse(packet).*.status;
                if (characteristics_discoverer.frame) |f| {
                    resume f;
                }
            },
            else => { }
        }
    }

    pub fn discover_16(self: *CharacteristicsDiscoverer, conn_handle: hci_con_handle_t,
                    start_handle: u16, end_handle: u16, uuid: u16) ?gatt_client_characteristic_t {
        if (!session_complete) return null;

        self.frame = null;
        self.characteristic = null;
        session_complete = false;
        if (0 != gatt_client_discover_characteristics_for_handle_range_by_uuid16(callback, conn_handle,
            start_handle, end_handle, uuid)) {
            session_complete = true;
            return null;
        }

        suspend {
            self.frame = @frame();
        }

        return self.characteristic;
    }

    pub fn discover_128(self: *CharacteristicsDiscoverer, conn_handle: hci_con_handle_t,
                    start_handle: u16, end_handle: u16, uuid: [*c] const u8) ?gatt_client_characteristic_t {
        if (!session_complete) return null;

        self.frame = null;
        self.characteristic = null;
        session_complete = false;
        if (0 != gatt_client_discover_characteristics_for_handle_range_by_uuid128(callback, conn_handle,
            start_handle, end_handle, uuid)) {
            session_complete = true;
            return null;
        }

        suspend {
            self.frame = @frame();
        }

        return self.characteristic;
    }
};

pub var all_descriptors_discoverer: AllDescriptorsDiscoverer = undefined;

const AllDescriptorsDiscoverer = struct {
    status: u8 = 0,
    frame: ? (anyframe -> ?gatt_client_characteristic_descriptor_t) = null,
    descriptor: gatt_client_characteristic_descriptor_t = undefined,

    fn callback(packet_type: u8, _: u16, packet: [*c] const u8, size: u16) callconv(.C) void {
        _ = size; _= packet_type;
        switch (packet[0]) {
            GATT_EVENT_ALL_CHARACTERISTIC_DESCRIPTORS_QUERY_RESULT => {
                var result = gatt_event_all_characteristic_descriptors_query_result_parse(packet);
                all_descriptors_discoverer.descriptor = result.*.descriptor;
            },
            GATT_EVENT_QUERY_COMPLETE => {
                session_complete = true;
                all_descriptors_discoverer.status = gatt_event_query_complete_parse(packet).*.status;
            },
            else => { }
        }
        if (all_descriptors_discoverer.frame) |f| {
            resume f;
        }
    }

    pub fn start(self: *AllDescriptorsDiscoverer, conn_handle: hci_con_handle_t,
                 characteristic: *gatt_client_characteristic_t) bool {
        if (!session_complete) return false;
        self.frame = null;
        session_complete = false;
        if (0 == gatt_client_discover_characteristic_descriptors(callback,
                                                        conn_handle,
                                                        characteristic)) {
            return !session_complete;
        } else {
            session_complete = true;
            return false;
        }
    }

    pub fn next(self: *AllDescriptorsDiscoverer) ?gatt_client_characteristic_descriptor_t {
        if (session_complete) return null;

        suspend {
            self.frame = @frame();
        }

        if (session_complete) {
            return null;
        } else {
            return self.descriptor;
        }
    }
};

pub var value_of_characteristic_reader: ValueOfCharacteristicReader = undefined;

extern fn gatt_helper_value_query_result_parse(packet: [*c] const u8, size: u16, value_size: [*c]u16) [*c] const u8;

const ValueOfCharacteristicReader = struct {
    status: u8 = 0,
    frame: ? (anyframe -> ?[*c] const u8) = null,
    value_size: u16,
    value: ?[*c] const u8,

    fn callback(packet_type: u8, _: u16, packet: [*c] const u8, size: u16) callconv(.C) void {
        _ = size; _= packet_type;
        switch (packet[0]) {
            GATT_EVENT_CHARACTERISTIC_VALUE_QUERY_RESULT => {
                value_of_characteristic_reader.value = gatt_helper_value_query_result_parse(packet, size,
                                &value_of_characteristic_reader.value_size);
            },
            GATT_EVENT_QUERY_COMPLETE => {
                session_complete = true;
                value_of_characteristic_reader.status = gatt_event_query_complete_parse(packet).*.status;
                if (value_of_characteristic_reader.frame) |f| {
                    resume f;
                }
            },
            else => { }
        }
    }

    pub fn read(self: *ValueOfCharacteristicReader, conn_handle: hci_con_handle_t, value_handle: u16, value_size: *u16) ?[*c] const u8 {
        value_size.* = 0;
        self.value_size = 0;
        self.value = null;
        if (!session_complete) return null;

        session_complete = false;

        if (0 != gatt_client_read_value_of_characteristic_using_value_handle(
                callback,
                conn_handle,
                value_handle)) {
            session_complete = true;
            return null;
        }

        if (session_complete) return null;

        suspend {
            self.frame = @frame();
        }

        self.frame = null;

        value_size.* = self.value_size;
        return self.value;
    }
};

pub var value_of_characteristic_writer: ValueOfCharacteristicWriter = undefined;

const ValueOfCharacteristicWriter = struct {
    status: u8 = 0,
    frame: ? (anyframe -> bool) = null,

    fn callback(packet_type: u8, _: u16, packet: [*c] const u8, size: u16) callconv(.C) void {
        _ = size; _= packet_type;
        switch (packet[0]) {
            GATT_EVENT_QUERY_COMPLETE => {
                value_of_characteristic_writer.status = gatt_event_query_complete_parse(packet).*.status;
                if (value_of_characteristic_writer.frame) |f| {
                    resume f;
                }
            },
            else => { }
        }
    }

    pub fn write(self: *ValueOfCharacteristicWriter, conn_handle: hci_con_handle_t, value_handle: u16,
                 value_size: u16, value: [*c] u8) bool {
        if (!session_complete) return false;

        session_complete = false;

        if (0 != gatt_client_write_value_of_characteristic(
                callback,
                conn_handle,
                value_handle,
                value_size,
                value)) {
            session_complete = true;
            return false;
        }

        if (session_complete) return false;

        suspend {
            self.frame = @frame();
        }

        self.frame = null;

        return self.status == 0;
    }
};