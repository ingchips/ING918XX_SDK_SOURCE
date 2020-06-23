## *
##  @brief Erase a block of data in flash then write data.
##
##  @param[in] dest_addr         page aligned target address (unified address) in flash
##  @param[in] buffer            buffer to be written
##  @param[in] size              byte number to be written (must be multiple of 4 bytes)
##  @return                      0 if successful else non-0
##

proc program_flash*(dest_addr: uint32; buffer: ptr uint8; size: uint32): cint {.
    importc: "program_flash", header: "eflash.h".}
## *
##  @brief Write a block of data to flash without erasing.
##         Note: `dest_addr` must points to a block of flash that has been erased, otherwise,
##               data can't be written into it.
##  @param[in] dest_addr         32-bit aligned target address (unified address) in flash
##  @param[in] buffer            buffer to be written
##  @param[in] size              byte number to be written (must be multiple of 4 bytes)
##  @return                      0 if successful else non-0
##

proc write_flash*(dest_addr: uint32; buffer: ptr uint8; size: uint32): cint {.
    importc: "write_flash", header: "eflash.h".}