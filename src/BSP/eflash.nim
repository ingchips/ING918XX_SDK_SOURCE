## *
##  @brief Write a block of data to flash
##  @param[in] dest_addr         target address (unified address) in flash
##  @param[in] buffer            buffer to be written
##  @param[in] size              byte number to be written
##  @return                      0 if successful else non-0
##

proc program_flash*(dest_addr: uint32; buffer: ptr uint8; size: uint32): cint {.
    importc: "program_flash", header: "eflash.h".}