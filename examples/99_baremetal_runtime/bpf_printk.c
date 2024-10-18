//
// Created by Davide Collovigh on 26/09/24.
//

#include "bpf_helpers.h"
#include "qemu_rv_uart.h"

const int DEBUG_PRINTK = 0;

void *get_or_rebase_arg(void *ptr, void *rebase_base)
{
    if (DEBUG_PRINTK) {
        printf("\n\n");
        printf("get_or_rebase_arg:\naddress: 0x%16x\nrebase_base: 0x%16x\n", ptr, rebase_base);
        printf("\n");
        printf("__stack_top:\t0x%16x\n", &__stack_top);
        printf("__stack_bottom:\t0x%16x\n", &__stack_bottom);
        printf("\n");
        printf("rodata_bpf_start:\t0x%16x\n", &rodata_bpf_start);
        printf("rodata_str1_bpf_start:\t0x%16x\n", &rodata_str1_bpf_start);
        printf("rodata_bpf_end:\t0x%16x\n\n", &rodata_bpf_end);

        int rodata_str1_len = &rodata_bpf_end - &rodata_str1_bpf_start;
        char * debug = (char *) &rodata_str1_bpf_start;

        printf("%s\n", debug);

        for(int k = 0; k < rodata_str1_len; k++) {
            printf("0x%16x: %2x (%c)\n", &debug[k], debug[k], debug[k]);
        }
    }

    // if address is not from stack -> rebase
    if((ptr < (void *) &__stack_bottom) || (ptr > (void *) &__stack_top)) {

        // rebase
        char *string = (void *)(rebase_base) + (uint64_t) ptr;

        if (DEBUG_PRINTK) {
            printf("rebased: (0x%16x) %s", string, string);
        }

        return (void *) string;
    }

    return ptr;
}


uint64_t _bpf_helper_ext_0006(const char *fmt, uint64_t fmt_size, ...) {
    
    /**********************************
     * PRINTK args rebase
     *********************************/

    const char *fmt_str = fmt;
    char *fmt_relocated = (void *)(&rodata_bpf_start) + (uint64_t) fmt;
    uint64_t relo_len = (void *)(&rodata_bpf_end) - (void *)(fmt_relocated);
    uint64_t rodata_len = (void *)(&rodata_bpf_end) - (void *)(&rodata_bpf_start);

    if (DEBUG_PRINTK) {
        printf("\n\n_bpf_helper_ext_0006 DEBUG\n");

        printf("FMT:\t\t0x%16x\n", fmt);
        printf("RELOCATED:\t0x%16x\n", fmt_relocated);
        printf("fmt_size:\t%d\n", fmt_size);
        printf("rodata_start:\t0x%16x\n", (void *)(&rodata_bpf_start));
        printf("rodata_end:\t0x%16x\n", (void *)(&rodata_bpf_end));
        printf("rodata_len: %d\n", rodata_len);

        for(int i = 0; i < fmt_size; i++) {
            printf("%2x", fmt_relocated[i]);
        }

        uart_putc('\n');
    }


    // fmt string is always placed inside the .rodata, but I did not find any docs about it
    // so I keep a check to make sure.
    if((void *) fmt_str < (void *) BASE_ADDRESS_QEMU_RV64) {

        // try to check if the string is into .rodata.bpf
        if (fmt_relocated != NULL) {

            fmt_str = fmt_relocated;
            if (DEBUG_PRINTK) {
                printf("Chosen relocation %s\n", fmt_relocated);
            }
        } else {

            uart_puts("called _bpf_helper_ext_0006 with empty pointer as fmt\n");
            return 0;
        }
    }

    if (DEBUG_PRINTK) {

        for(int i = 0; i < fmt_size; i++) {
            printf("%2x", fmt_str[i]);
        }

        uart_putc('\n');

        printf("_bpf_helper_ext_0006 END DEBUG\n\n");
    }


    va_list args;
    va_start(args, fmt_size);

    const char* format = (const char*)fmt_str;
    char buffer[32];
    int len = 0;
    int i = 0;
    unsigned int ascii_code = 0;

    // S0: print chars until '\0' or '%'
s0:

    if (format[i] == '%') {
        i++;
        goto s1;
    }

    if (format[i] == '\0')
        goto end;

    if (i >= fmt_size)
      goto end;

    uart_putc(format[i]);
    i++;
    goto s0;

    // s1: if exists -> read number of char to use for printing decimal/hex
s1:

    ascii_code = format[i];
    if ((ascii_code - '0' >= 0) && (ascii_code - '0' < 10)) {
        int value = ascii_code - '0';
        len = (len *10) + value;
        i++;
        goto s1;
    }

    goto s2;

    // s2: read param type and convert param to string
s2:

    if (format[i] == 'd') { // Integer
        int val = va_arg(args, int);
        itoa(val, buffer);
        uart_puts(buffer);
    } else if (format[i] == 'x') { // Hexadecimal
        uint32_t val = va_arg(args, uint32_t);
        hextoa(val, buffer, len);
        uart_puts(buffer);
    } else if (format[i] == 's') { // String -> only type that is actually a pointer to what needs to be printed -> rebase if necessary

        const char* str = va_arg(args, const char*);
        const char *str2 = (char *) get_or_rebase_arg((void *) str, (void *) &rodata_str1_bpf_start);
        uart_puts(str2);
    } else if (format[i] == 'c') { // Character
        char c = (char)va_arg(args, int);
        uart_putc(c);
    }

    len = 0; // reset len
    i++;

    goto s0;

end:

    va_end(args);
    return 0;
}
