/**************************************************
 *
 * Part one of the system initialization code, contains low-level
 * initialization, plain thumb variant.
 *
 * Copyright 2019 INGCHIPS, Co.
 *
 **************************************************/

        MODULE  ?cstartup
          
        SECTION .text:CODE:REORDER(2)
        THUMB
        PUBLIC __iar_program_start
__iar_program_start:
        
        IMPORT __iar_data_init3
        IMPORT app_main
        
        PUSH    {R1, LR}
        
        BL      __iar_data_init3
        
        LDR     R0, =app_main
        BLX     R0
        
        POP     {R1, PC}
        
        REQUIRE __vector_table
        
        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        PUBLIC  __vector_table

        DATA

__iar_init$$done:               ; The vector table is not needed
                                ; until after copy initialization is done

__vector_table
        DCD     0
        DCD     __iar_program_start        

        END 
