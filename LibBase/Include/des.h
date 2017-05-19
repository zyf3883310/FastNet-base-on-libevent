#pragma once

#define u_char unsigned char
#define _u8 unsigned char
#define _u16 unsigned short
#define _u32 unsigned int

int DECRYPT( u_char *key, unsigned char *s,unsigned char *d, unsigned short len );
int ENCRYPT( u_char *key, unsigned char *s,unsigned char *d,unsigned short len );

