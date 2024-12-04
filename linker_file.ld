ENTRY(reset_handler)
PROVIDE(_stacktop = ORIGIN(RAM) + LENGTH(RAM));

MEMORY{
    FLASH (rx) : ORIGIN = 0x8000000, LENGTH = 512K
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 96K
}

SECTIONS{

    .isr_vector : {
        *(.isr*)
        KEEP(*(.isr_vector*))
        /* KEEP(*reset_handler*) */

    } > FLASH

    .text : {
        *(.text*)
    } > FLASH

    .rodata : {
        *(.rodata*)
        _end_rodata = .;
    } > FLASH

    .data : {
        _ram_data_start = .;
        *(.data*)
        _ram_data_end = .;
    } > RAM AT > FLASH

    .bss : {
        _ram_bss_start = .;
        *(.bss*)
        _ram_bss_end = .;
    } > RAM AT > FLASH

}


