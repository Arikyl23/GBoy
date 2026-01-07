#include <stdio.h>

#include <log.h>

#include <cpu/registers/registers.h>

int main(void) {
    printf("Hello World!\n");

    registers_set_BC(0xAB45);

    log_info("Value of BC: 0x%.4X\n", registers_get_BC());

    return 0;
}