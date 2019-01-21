/**
 * FileName:   endianness.c
 * Author:     Fasion Chan
 * @contact:   fasionchan@gmail.com
 * @version:   $Id$
 *
 * Description:
 *
 * Changelog:
 *
 **/

#include <stdio.h>

int main()
{
    int value = 1;
    if (((char*)&value)[0]) {
        printf("Little endian\n");
    }
    else {
        printf("Big endian\n");
    }

    return value;
}
