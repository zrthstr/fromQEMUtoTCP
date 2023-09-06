ENTRY(main)

SECTIONS
{
    /* Load the kernel at the physical address 0x7c00 */
    /*. = 0x7c00; */
    /* . = 0x0600; */
    . = 0x0000;

    /* Define the text section */
    .text :
    {
        *(.text)
    }

    /* Define the data section */
    .data :
    {
        *(.data)
    }

    /* Define the bss section */
    .bss :
    {
        *(.bss)
    }

    /* Define the end of the kernel */
    end_of_kernel = .;
}
