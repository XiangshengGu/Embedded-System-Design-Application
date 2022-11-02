/**
 * lcd.h
 * Copyright (C) 2001-2021, Tony Givargis
 */

#ifndef _LCD_H
#define _LCD_H

void lcd_init(void);

void lcd_clr(void);

void lcd_pos(unsigned char r, unsigned char c);

void lcd_put(char c);

void lcd_puts1(const char *s);

void lcd_puts2(const char *s);

#endif /* _LCD_H */