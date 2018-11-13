/**
 * FileName:   server.c
 * Author:     Fasion Chan
 * @contact:   fasionchan@gmail.com
 * @version:   $Id$
 *
 * Description:
 *
 * Changelog:
 *
 **/

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define MCAST_ADDR "224.0.0.88"
#define MCAST_PORT 8888
#define MCAST_INTERVAL 1
#define BUFFER_SIZE 20

/**
 * fetch current time and format it into string with format: xxxx-xx-xx xx:xx:xx
 *
 * WARN:
 *   This function is non-reentrant, and thus not thread-safe, since static 
 * buffer is used.
 *
 **/
const char *fetch_datetime() {
    // static buffer for storing result
    static char buffer[BUFFER_SIZE];

    // get current time
    struct timeval tv;
    gettimeofday(&tv, NULL);

    // convert to datetime
    struct tm *tm = localtime(&tv.tv_sec);

    // format datetime
    strftime(buffer, BUFFER_SIZE, "%Y-%m-%d %H:%M:%S", tm);

    return buffer;
}

int main()
{
    // create udp socket
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == -1) {
        perror("create socket");
        exit(1);
    }

    // fill multicast address and port
    struct sockaddr_in mcast_addr;
    bzero(&mcast_addr, sizeof(mcast_addr));
    mcast_addr.sin_family = AF_INET;
    mcast_addr.sin_addr.s_addr = inet_addr(MCAST_ADDR);
    mcast_addr.sin_port = htons(MCAST_PORT);

    for (;;) {
        // fetch current datetime string
        const char *datetime = fetch_datetime();

        // send datetime to multicast address
        int bytes = sendto(s, datetime, BUFFER_SIZE, 0, (struct sockaddr *)&mcast_addr, sizeof(mcast_addr));
        if (bytes == -1) {
            perror("send datetime");
            exit(2);
        }

        // wait for a while
        sleep(MCAST_INTERVAL);
    }

    return 0;
}
