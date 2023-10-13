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
            xram0_struct_set(0xFF00, vga_mode3_config_t, x_pos_px, bg_x / 2);
            xram0_struct_set(0xFF10, vga_mode3_config_t, x_pos_px, x);
            if (--x <= -320)
                x = 280;
            if (--bg_x <= -640)
                bg_x = 640;
        }
        if (++man_x == 6)
        {
            xram0_struct_set(0xFF20, vga_mode3_config_t, xram_data_ptr, 0xFB00);
        }
        else if (man_x == 12)
        {
            xram0_struct_set(0xFF20, vga_mode3_config_t, xram_data_ptr, 0xF900);
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
    int fd_bg, fd_tree, fd_man1, fd_man2, result_bg, result_tree, result_man1, result_man2;
    const char *name_bg = "background2.bin";
    const char *name_tree = "tree.bin";
    const char *name_man1 = "man1.bin";
    const char *name_man2 = "man2.bin";
    xreg(1, 0, 0, 2); // Canvas
    clear();
    
    xram0_struct_set(0xFF00, vga_mode3_config_t, x_wrap, true);
    xram0_struct_set(0xFF00, vga_mode3_config_t, y_wrap, true);
    xram0_struct_set(0xFF00, vga_mode3_config_t, x_pos_px, 0);
    xram0_struct_set(0xFF00, vga_mode3_config_t, y_pos_px, 0);
    xram0_struct_set(0xFF00, vga_mode3_config_t, width_px, 320);
    xram0_struct_set(0xFF00, vga_mode3_config_t, height_px, 180);
    xram0_struct_set(0xFF00, vga_mode3_config_t, xram_data_ptr, 0x0200);
    xram0_struct_set(0xFF00, vga_mode3_config_t, xram_palette_ptr, 0x0000);

    xram0_struct_set(0xFF10, vga_mode3_config_t, x_wrap, true);
    xram0_struct_set(0xFF10, vga_mode3_config_t, y_wrap, false);
    xram0_struct_set(0xFF10, vga_mode3_config_t, x_pos_px, 0);
    xram0_struct_set(0xFF10, vga_mode3_config_t, y_pos_px, 60);
    xram0_struct_set(0xFF10, vga_mode3_config_t, width_px, 50);
    xram0_struct_set(0xFF10, vga_mode3_config_t, height_px, 100);
    xram0_struct_set(0xFF10, vga_mode3_config_t, xram_data_ptr, 0xE500);
    xram0_struct_set(0xFF10, vga_mode3_config_t, xram_palette_ptr, 0xE300);

    xram0_struct_set(0xFF20, vga_mode3_config_t, x_wrap, false);
    xram0_struct_set(0xFF20, vga_mode3_config_t, y_wrap, false);
    xram0_struct_set(0xFF20, vga_mode3_config_t, x_pos_px, 152);
    xram0_struct_set(0xFF20, vga_mode3_config_t, y_pos_px, 140);
    xram0_struct_set(0xFF20, vga_mode3_config_t, width_px, 16);
    xram0_struct_set(0xFF20, vga_mode3_config_t, height_px, 32);
    xram0_struct_set(0xFF20, vga_mode3_config_t, xram_data_ptr, 0xF900);
    xram0_struct_set(0xFF20, vga_mode3_config_t, xram_palette_ptr, 0xFFFF);

    xreg(1, 0, 1, 3, 2, 0xFF00, 0, 0, 0); // Mode 3
    xreg(1, 0, 1, 3, 2, 0xFF10, 1, 0, 0); // Mode 3
    xreg(1, 0, 1, 3, 2, 0xFF20, 2, 0, 0); // Mode 3
    
    fd_bg = open(name_bg, O_RDONLY);
    if (fd_bg < 0)
        puts("open error");
    result_bg = read_xram(0x0000, 0x6000, fd_bg);
    if (result_bg < 0)
        puts("read_xram error 1");
    result_bg = read_xram(0x6000, 0x6000, fd_bg);
    if (result_bg < 0)
        puts("read_xram error 2");
    result_bg = read_xram(0xC000, 0x2300, fd_bg);
    if (result_bg < 0)
        puts("read_xram error 3");
    result_bg = close(fd_bg);

    fd_tree = open(name_tree, O_RDONLY);
    if (fd_tree < 0)
        puts("open error");
    result_tree = read_xram(0xE300, 0x1588, fd_tree);
    if (result_tree < 0)
        puts("read_xram error 1");
    result_tree = close(fd_tree);

    fd_man1 = open(name_man1, O_RDONLY);
    if (fd_man1 < 0)
        puts("open error");
    result_man1 = read_xram(0xF900, 0x200, fd_man1);
    if (result_man1 < 0)
        puts("read_xram error 1");
    result_man1 = close(fd_man1);

    fd_man2 = open(name_man2, O_RDONLY);
    if (fd_man2 < 0)
        puts("open error");
    result_man2 = read_xram(0xFB00, 0x200, fd_man2);
    if (result_man2 < 0)
        puts("read_xram error 1");
    result_man2 = close(fd_man2);



    // while(1)
    // {}
    // box(10);
    scroll(true, false);
}
