/**
 * FileName:   show_mac.c
 * Author:     Chen Yanfei
 * @contact:   fasionchan@gmail.com
 * @version:   $Id$
 *
 * Description:
 *
 * Changelog:
 *
 **/

#include <net/if.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>


/**
 *  Convert binary MAC address to readable format.
 *
 *  Arguments
 *      n: binary format, must be 6 bytes.
 *
 *      a: buffer for readable format, 18 bytes at least(`\0` included).
 **/
void mac_ntoa(unsigned char *n, char *a) {
    // traverse 6 bytes one by one
    for (int i=0; i<6; i++) {
        // format string
        char *format = ":%02x";

        // first byte without leading `:`
        if(0 == i) {
            format = "%02x";
        }

        // format current byte
        a += sprintf(a, format, n[i]);
    }
}


int main(int argc, char *argv[]) {
    // create a socket, any type is ok
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == s) {
        perror("Fail to create socket");
        return 1;
    }

    // fill iface name to struct ifreq
    struct ifreq ifr;
    strncpy(ifr.ifr_name, argv[1], 15);

    // call ioctl to get hardware address
    int ret = ioctl(s, SIOCGIFHWADDR, &ifr);
    if (-1 == ret) {
        perror("Fail to get mac address");
        return 2;
    }

    // convert to readable format
    char mac[18];
    mac_ntoa((unsigned char *)ifr.ifr_hwaddr.sa_data, mac);

    // output result
    printf("IFace: %s\n", ifr.ifr_name);
    printf("MAC: %s\n", mac);

    return 0;
}
