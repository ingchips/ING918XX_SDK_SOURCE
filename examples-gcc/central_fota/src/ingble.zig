pub usingnamespace @cImport({
    @cInclude("platform_api.h");
    @cInclude("btstack_defines.h");
    @cInclude("gatt_client.h");
    @cInclude("sig_uuid.h");
    @cInclude("gap.h");
    @cInclude("FreeRTOS.h");
    @cInclude("task.h");
    @cInclude("rom_tools.h");
    @cDefine("_INCLUDE_BY_ZIG", {});
    @cInclude("fota_client.h");
});