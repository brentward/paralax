#include <rp6502.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <fcntl.h>

int load(const char *name)
{
    int fd, result;
    xram0_struct_set(0xFF00, vga_mode3_config_t, x_pos_px, -1000);
    xram0_struct_set(0xFF00, vga_mode3_config_t, xram_data_ptr, 0x0200);
    xram0_struct_set(0xFF00, vga_mode3_config_t, xram_palette_ptr, 0x0000);
    fd = open(name, O_RDONLY);
    if (fd < 0)
        return fd;
    result = read_xram(0x0000, 0x6000, fd);
    if (result < 0)
        return result;
    result = read_xram(0x6000, 0x6000, fd);
    if (result < 0)
        return result;
    result = read_xram(0xC000, 0x2300, fd);
    if (result < 0)
        return result;
    result = close(fd);
    if (result < 0)
        return result;
    xram0_struct_set(0xFF00, vga_mode3_config_t, x_pos_px, 0);
    xram0_struct_set(0xFF00, vga_mode3_config_t, y_pos_px, 0);
}

void clear()
{
    unsigned i;
    RIA.addr0 = 0;
    RIA.step0 = 1;
    for (i = 0; i < 57600u; i++)
        RIA.rw0 = 0;
}

void box(unsigned qty)
{
    srand(6);
    while (qty--)
    {
        int color, x1, y1, x2, y2, x, y;
        color = rand();
        x1 = ((uint32_t)rand() * 320) >> 15;
        y1 = ((uint32_t)rand() * 180) >> 15;
        x2 = ((uint32_t)rand() * 320) >> 15;
        y2 = ((uint32_t)rand() * 180) >> 15;
        if (x1 > x2)
        {
            x = x1;
            x1 = x2;
            x2 = x;
        }
        if (y1 > y2)
        {
            y = y1;
            y1 = y2;
            y2 = y;
        }
        RIA.step1 = 0;
        for (y = y1; y < y2; y++)
        {
            RIA.addr0 = 320 * y + x1;
            for (x = x1; x < x2; x++)
            {
                RIA.rw0 = color;
            }
        }
    }
}

void scroll(bool x_scroll, bool y_scroll)
{
    int x = 0, y = 0, bg_x = 0, man_x = 0;
    
    uint8_t v = RIA.vsync;
    while (1)
    {

        if (RIA.vsync == v)
            continue;
        v = RIA.vsync;
        if (x_scroll)
        {
            xram0_struct_set(0xFF00, vga_mode3_config_t, x_pos_px, bg_x >> 1);
            xram0_struct_set(0xFF10, vga_mode3_config_t, x_pos_px, x);
            if (--x <= -320)
                x = 320;
            if (--bg_x <= -640)
                bg_x = 640;
        }
        if (++man_x == 9)
        {
            xram0_struct_set(0xFF20, vga_mode3_config_t, xram_data_ptr, 0xAB40);
            // xram0_struct_set(0xFF20, vga_mode3_config_t, xram_palette_ptr, 0x1000);
        }
        else if (man_x == 18)
        {
            xram0_struct_set(0xFF20, vga_mode3_config_t, xram_data_ptr, 0xAC60);
            // xram0_struct_set(0xFF20, vga_mode3_config_t, xram_palette_ptr, 0x2000);

        }
        else if (man_x == 27)
        {
            xram0_struct_set(0xFF20, vga_mode3_config_t, xram_data_ptr, 0x0AB40);
            // xram0_struct_set(0xFF20, vga_mode3_config_t, xram_palette_ptr, 0x0000);
        }
        
        else if (man_x == 36)
        {
            xram0_struct_set(0xFF20, vga_mode3_config_t, xram_data_ptr, 0xAA20);
            // xram0_struct_set(0xFF20, vga_mode3_config_t, xram_palette_ptr, 0x0000);
            man_x = 0;
        }
        
        if (y_scroll)
        {
            xram0_struct_set(0xFF00, vga_mode3_config_t, y_pos_px, y);
            if (++y >= 180)
                y = -180;
        }
    }
}

void main()
{
    int fd, result;
    const char *name_bg_palatte = "mm_bg.palette.bin";
    const char *name_bg = "mm_bg.data.bin";
    const char *name_mg_palatte = "mm_mg.palette.bin";
    const char *name_mg = "mm_mg.data.bin";
    const char *name_walk_palatte = "mm_run_1.palette.bin";
    const char *name_mm_walk_1 = "mm_run_1.data.bin";
    const char *name_mm_walk_2 = "mm_run_2.data.bin";
    const char *name_mm_walk_3 = "mm_run_3.data.bin";
    xreg(1, 0, 0, 1); // Canvas
    clear();
    
    xram0_struct_set(0xFF00, vga_mode3_config_t, x_wrap, true);
    xram0_struct_set(0xFF00, vga_mode3_config_t, y_wrap, true);
    xram0_struct_set(0xFF00, vga_mode3_config_t, x_pos_px, 0);
    xram0_struct_set(0xFF00, vga_mode3_config_t, y_pos_px, 0);
    xram0_struct_set(0xFF00, vga_mode3_config_t, width_px, 320);
    xram0_struct_set(0xFF00, vga_mode3_config_t, height_px, 240);
    xram0_struct_set(0xFF00, vga_mode3_config_t, xram_data_ptr, 0x0020);
    xram0_struct_set(0xFF00, vga_mode3_config_t, xram_palette_ptr, 0x0000);

    xram0_struct_set(0xFF10, vga_mode3_config_t, x_wrap, true);
    xram0_struct_set(0xFF10, vga_mode3_config_t, y_wrap, false);
    xram0_struct_set(0xFF10, vga_mode3_config_t, x_pos_px, 0);
    xram0_struct_set(0xFF10, vga_mode3_config_t, y_pos_px, 104);
    xram0_struct_set(0xFF10, vga_mode3_config_t, width_px, 80);
    xram0_struct_set(0xFF10, vga_mode3_config_t, height_px, 124);
    xram0_struct_set(0xFF10, vga_mode3_config_t, xram_data_ptr, 0x9640);
    xram0_struct_set(0xFF10, vga_mode3_config_t, xram_palette_ptr, 0x9620);

    xram0_struct_set(0xFF20, vga_mode3_config_t, x_wrap, false);
    xram0_struct_set(0xFF20, vga_mode3_config_t, y_wrap, false);
    xram0_struct_set(0xFF20, vga_mode3_config_t, x_pos_px, 148);
    xram0_struct_set(0xFF20, vga_mode3_config_t, y_pos_px, 184);
    xram0_struct_set(0xFF20, vga_mode3_config_t, width_px, 24);
    xram0_struct_set(0xFF20, vga_mode3_config_t, height_px, 24);
    xram0_struct_set(0xFF20, vga_mode3_config_t, xram_data_ptr, 0xAA20);
    xram0_struct_set(0xFF20, vga_mode3_config_t, xram_palette_ptr, 0xAA00);

    xreg(1, 0, 1, 3, 2, 0xFF00, 0, 0, 0); // Mode 3
    xreg(1, 0, 1, 3, 2, 0xFF10, 1, 0, 0); // Mode 3
    xreg(1, 0, 1, 3, 2, 0xFF20, 2, 0, 0); // Mode 3
    
    fd = open(name_bg_palatte, O_RDONLY);
    if (fd < 0)
        puts("open error 1");
    result = read_xram(0x0000, 0x0020, fd);
    if (result < 0)
        puts("read_xram error 1");
    result = close(fd);

    fd = open(name_bg, O_RDONLY);
    if (fd < 0)
        puts("open error 2");
    result = read_xram(0x0020, 0x6000, fd);
    if (result < 0)
        puts("read_xram error 2a");
    result = read_xram(0x6020, 0x3600, fd);
    if (result < 0)
        puts("read_xram error 2b");
    result = close(fd);

    fd = open(name_mg_palatte, O_RDONLY);
    if (fd < 0)
        puts("open error 3");
    result = read_xram(0x9620, 0x0020, fd);
    if (result < 0)
        puts("read_xram error 3");
    result = close(fd);

    fd = open(name_mg, O_RDONLY);
    if (fd < 0)
        puts("open error 4");
    result = read_xram(0x9640, 0x1376, fd);
    if (result < 0)
        puts("read_xram error 4");
    result = close(fd);

    fd = open(name_walk_palatte, O_RDONLY);
    if (fd < 0)
        puts("open error 5");
    result = read_xram(0xAA00, 0x0020, fd);
    if (result < 0)
        puts("read_xram error 5");
    result = close(fd);

    fd = open(name_mm_walk_1, O_RDONLY);
    if (fd < 0)
        puts("open error 6");
    result = read_xram(0xAA20, 0x0120, fd);
    if (result < 0)
        puts("read_xram error 6");
    result = close(fd);

    fd = open(name_mm_walk_2, O_RDONLY);
    if (fd < 0)
        puts("open error 7");
    result = read_xram(0xAB40, 0x0120, fd);
    if (result < 0)
        puts("read_xram error 7");
    result = close(fd);

    fd = open(name_mm_walk_3, O_RDONLY);
    if (fd < 0)
        puts("open error 8");
    result = read_xram(0xAC60, 0x0120, fd);
    if (result < 0)
        puts("read_xram error 8");
    result = close(fd);



    // while(1)
    // {}
    // box(10);
    scroll(true, false);
}
