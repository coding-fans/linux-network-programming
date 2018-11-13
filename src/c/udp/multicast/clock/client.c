/**
 * FileName:   client.c
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

#define MCAST_ADDR "224.0.0.88"
#define MCAST_PORT 8888
#define BUFFER_SIZE 20

int main()
{
    // create udp socket
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == -1) {
        perror("create socket");
        exit(1);
    }

    // fill bind address with multicast port
    struct sockaddr_in bind_addr;
    bzero(&bind_addr, sizeof(bind_addr));
    bind_addr.sin_family = AF_INET;
    bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    bind_addr.sin_port = htons(MCAST_PORT);

    // bind address
    int rv = bind(s, (struct sockaddr *)&bind_addr, sizeof(bind_addr));
    if (rv == -1) {
        perror("bind address");
        exit(2);
    }

    // allow loop so that localhost can receive
    int loop = 1;
    rv = setsockopt(s, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));
    if (rv == -1) {
        perror("setsockopt loop");
        exit(3);
    }

    // add self to multicast group
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(MCAST_ADDR);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    rv = setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    if (rv == -1) {
        perror("setsockopt add membership");
        exit(3);
    }

    // receive message from multicast server
    char buffer[BUFFER_SIZE];
    int bytes = recvfrom(s, buffer, BUFFER_SIZE, 0, NULL, NULL);
    if (bytes == -1) {
        perror("receive datetime");
        exit(2);
    }

    // output datetime string
    printf("%s\n", buffer);

    return 0;
}
