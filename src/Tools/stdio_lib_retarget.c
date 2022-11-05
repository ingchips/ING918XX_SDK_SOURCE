/* retarget std io functions for C run-time library  */

#include <stdint.h>

#ifndef FILEHANDLE
typedef int FILEHANDLE;
#endif

__attribute((weak)) int32_t cb_putc(char *c, void *dummy)
{
    return 0;
}

/*
 * These names are used during library initialization as the
 * file names opened for stdin, stdout, and stderr.
 * As we define _sys_open() to always return the same file handle,
 * these can be left as their default values.
 */
const char __stdin_name[] = ":tt";
const char __stdout_name[] = ":tt";
const char __stderr_name[] = ":tt";

void _ttywrch(int ch)
{
    char c = ch;
    cb_putc(&c, (void *)0);
}

FILEHANDLE _sys_open(const char *name, int openmode)
{
    return 1; /* everything goes to the same output */
}

int _sys_close(FILEHANDLE fh)
{
    return 0;
}

int _sys_write(FILEHANDLE fh, const unsigned char *buf,
               unsigned len, int mode)
{
    unsigned i;
    for (i = 0; i < len; i++)
        _ttywrch((int)*((char *)buf + i));
    return 0;
}

int _sys_read(FILEHANDLE fh, unsigned char *buf,
              unsigned len, int mode)
{
    return -1; /* not supported */
}

int _sys_istty(FILEHANDLE fh)
{
    return 0; /* buffered output */
}

int _sys_seek(FILEHANDLE fh, long pos)
{
    return -1; /* not supported */
}

long _sys_flen(FILEHANDLE fh)
{
    return -1; /* not supported */
}

char *_sys_command_string(char *cmd, int len)
{
    return (char *)0;
}

#if (__ARMCC_VERSION >= 6000000)
__asm(".global __ARM_use_no_argv\n\t");
#else
#warning check things like __ARM_get_argv in your IDE/toolchain:
// __asm void __ARM_get_argv(void)
// {
//    mov r0, #0
//    mov r1, #0
//    bx lr
// }
#endif
