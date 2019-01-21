/**
 * FileName:   ping.c
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
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define MAGIC "1234567890"
#define MAGIC_LEN 11
#define MTU 1500
#define RECV_TIMEOUT_USEC 100000

struct icmp_echo {
    // header
    uint8_t type;
    uint8_t code;
    uint16_t checksum;

    uint16_t ident;
    uint16_t seq;

    // data
    double sending_ts;
    char magic[MAGIC_LEN];
};

double get_timestamp()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + ((double)tv.tv_usec) / 1000000;
}

uint16_t calculate_checksum(unsigned char* buffer, int bytes)
{
    uint32_t checksum = 0;
    unsigned char* end = buffer + bytes;

    // odd bytes add last byte and reset end
    if (bytes % 2 == 1) {
        end = buffer + bytes - 1;
        checksum += (*end) << 8;
    }

    // add words of two bytes, one by one
    while (buffer < end) {
        checksum += buffer[0] << 8;
        checksum += buffer[1];
        buffer += 2;
    }

    // add carry if any
    uint32_t carray = checksum >> 16;
    while (carray) {
        checksum = (checksum & 0xffff) + carray;
        carray = checksum >> 16;
    }

    // negate it
    checksum = ~checksum;

    return checksum & 0xffff;
}

int send_echo_request(int sock, struct sockaddr_in* addr, int ident, int seq)
{
    // allocate memory for icmp packet
    struct icmp_echo icmp;
    bzero(&icmp, sizeof(icmp));

    // fill header files
    icmp.type = 8;
    icmp.code = 0;
    icmp.ident = htons(ident);
    icmp.seq = htons(seq);

    // fill magic string
    strncpy(icmp.magic, MAGIC, MAGIC_LEN);

    // fill sending timestamp
    icmp.sending_ts = get_timestamp();

    // calculate and fill checksum
    icmp.checksum = htons(
        calculate_checksum((unsigned char*)&icmp, sizeof(icmp))
    );

    // send it
    int bytes = sendto(sock, &icmp, sizeof(icmp), 0,
        (struct sockaddr*)addr, sizeof(*addr));
    if (bytes == -1) {
        return -1;
    }

    return 0;
}

int recv_echo_reply(int sock, int ident)
{
    // allocate buffer
    char buffer[MTU];
    struct sockaddr_in peer_addr;

    // receive another packet
    int addr_len = sizeof(peer_addr);
    int bytes = recvfrom(sock, buffer, sizeof(buffer), 0,
        (struct sockaddr*)&peer_addr, &addr_len);
    if (bytes == -1) {
        // normal return when timeout
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }

        return -1;
    }

    // find icmp packet in ip packet
    struct icmp_echo* icmp = (struct icmp_echo*)(buffer + 20);

    // check type
    if (icmp->type != 0 || icmp->code != 0) {
        return 0;
    }

    // match identifier
    if (ntohs(icmp->ident) != ident) {
        return 0;
    }

    // print info
    printf("%s seq=%d %5.2fms\n",
        inet_ntoa(peer_addr.sin_addr),
        ntohs(icmp->seq),
        (get_timestamp() - icmp->sending_ts) * 1000
    );

    return 0;
}

int ping(const char *ip)
{
    // for store destination address
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));

    // fill address, set port to 0
    addr.sin_family = AF_INET;
    addr.sin_port = 0;
    if (inet_aton(ip, (struct in_addr*)&addr.sin_addr.s_addr) == 0) {
        return -1;
    };

    // create raw socket for icmp protocol
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sock == -1) {
        return -1;
    }

    // set socket timeout option
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = RECV_TIMEOUT_USEC;
    int ret = setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    if (ret == -1) {
        return -1;
    }

    double next_ts = get_timestamp();
    int ident = getpid();
    int seq = 1;

    for (;;) {
        // time to send another packet
        if (get_timestamp() >= next_ts) {
            // send it
            ret = send_echo_request(sock, &addr, ident, seq);
            if (ret == -1) {
                perror("Send failed");
            }

            // update next sendint timestamp to one second later
            next_ts += 1;
            // increase sequence number
            seq += 1;
        }

        // try to receive and print reply
        ret = recv_echo_reply(sock, ident);
        if (ret == -1) {
            perror("Receive failed");
        }
    }

    return 0;
}

int main(int argc, const char* argv[])
{
    return ping(argv[1]);
}
