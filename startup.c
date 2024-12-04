extern char _end_rodata;
extern char _ram_data_start;
extern char _ram_data_end;
extern char _ram_bss_start;
extern char _ram_bss_end;
extern void _stacktop(void);

extern int main(void);


void reset_handler(void)
{
    char *src = &_end_rodata;
    char *ram_data_start = &_ram_data_start;

    while(ram_data_start < &_ram_data_end) //Copying .data section to the RAM from the FLASH
    {
        *ram_data_start = *src;
        ram_data_start++;
        src++;
    }

    char *ram_bss_start = &_ram_bss_start;
    while(ram_bss_start < &_ram_bss_end) //Initializing .bss with 0x00
    {
        *ram_bss_start = 0x00;
        ram_bss_start++;
    }

    main();
    while(1);
}

void hardfault_handler()
{
    while(1);
}

void NMI__handler()
{
    hardfault_handler();
}

void mem_management_fault_handler()
{
    hardfault_handler();
}
void bus_flt_handler()
{
    hardfault_handler();
}
void usage_flt_handler()
{
    hardfault_handler();
}
void sv_call_handler()
{
    hardfault_handler();
}
void pendSV_handler();
void systick_handler();

void reserved()
{
    while(1);
}
void reserved_dbg()
{
    while(1);
}


__attribute__((used, section(".isr_vector"))) void (*const vectors[])(void) = {
    &_stacktop,
    reset_handler,
    NMI__handler,
    hardfault_handler,
    mem_management_fault_handler,
    bus_flt_handler,
    usage_flt_handler,
    reserved,
    reserved,
    reserved,
    reserved,
    sv_call_handler,
    reserved_dbg,
    reserved,
    pendSV_handler,
    systick_handler

};
