/**
 * FileName:   show.c
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

void show_mem_rep(unsigned char* start, int n)
{
    // show bytes one by one from start to start+n
    int i;
    for (i=0; i<n; i++) {
        printf(" %.2x", start[i]);
    }
    printf("\n");
}

int main()
{
    int i = 0x01234567;
    show_mem_rep((unsigned char*)&i, sizeof(i));
    return 0;
}
