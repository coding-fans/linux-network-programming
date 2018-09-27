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

#include <argp.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define DEFAULT_LISTEN_ADDR "0.0.0.0"
#define DEFAULT_LISTEN_PORT "55555"
#define BUFFER_SIZE 256

/**
 * struct for storing command line arguments.
 **/
struct arguments {
    // address to listen
    char const *bind_addr;

    // port to listen
    char const *bind_port;
};

/**
 * opt_handler function for GNU argp.
 **/
static error_t opt_handler(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch(key) {
        case 'a':
            arguments->bind_addr = arg;
            break;
        case 'p':
            arguments->bind_port = arg;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

/**
 * Parse command line arguments given by argc, argv.
 *
 *  Arguments
 *      argc: the same with main function.
 *
 *      argv: the same with main function.
 *
 *  Returns
 *      Pointer to struct arguments if success, NULL if error.
 **/
static struct arguments const *parse_arguments(int argc, char *argv[])
{
    // docs for program and options
    static char const doc[] = "server: udp echo server";
    static char const args_doc[] = "";

    // command line options
    static struct argp_option const options[] = {
        // Option -a --bind-address: address to listen to
        {"bind-address", 'a', "BIND_ADDR", 0, "address to listen"},

        // Option -p --bind-port: port to listen to
        {"bind-port", 'p', "BIND_PORT", 0, "port to listen"},

        { 0 },
    };

    static struct argp const argp = {
        options,
        opt_handler,
        args_doc,
        doc,
        0,
        0,
        0,
    };

    // for storing results
    static struct arguments arguments = {
        .bind_addr = DEFAULT_LISTEN_ADDR,
        .bind_port = DEFAULT_LISTEN_PORT,
    };

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    return &arguments;
}

int main(int argc, char *argv[])
{
    // parse command line options to struct arguments
    struct arguments const *arguments = parse_arguments(argc, argv);
    if (arguments == NULL) {
        fprintf(stderr, "Bad command line options given\n");
        return 1;
    }

    // fill address struct
    struct sockaddr_in bind_addr;
    bzero(&bind_addr, sizeof(bind_addr));
    bind_addr.sin_family = AF_INET;
    inet_aton(arguments->bind_addr, &bind_addr.sin_addr);
    bind_addr.sin_port = htons(atoi(arguments->bind_port));

    // create udp socket
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == -1) {
        perror("fail to create socket");
        return 1;
    }

    // bind address
    int rv = bind(s, (struct sockaddr *)&bind_addr, sizeof(bind_addr));
    if (rv == -1) {
        perror("fail to bind address");
        return 1;
    }

    // loop for incoming message[:w
    for (;;) {
        // buffer for storing data recieve
        char buffer[BUFFER_SIZE];

        // peer address(client address)
        struct sockaddr_in peer_addr;
        int peer_addr_len = sizeof(peer_addr);

        // wait util recieve some data
        int bytes = recvfrom(s, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&peer_addr, &peer_addr_len);
        if (bytes < 0) {
            perror("fail to recieve data");
            break;
        }

        // print activity info
        printf("Recieve %3d bytes from %15s:%d\n", bytes, inet_ntoa(peer_addr.sin_addr), ntohs(peer_addr.sin_port));

        // send data back, say echo
        bytes = sendto(s, buffer, bytes, 0, (struct sockaddr *)&peer_addr, peer_addr_len);
        if (bytes < 0) {
            perror("fail to send data back");
            break;
        }
    }

    // close socket
    rv = close(s);
    if (rv == -1) {
        perror("fail to close socket");
        return 1;
    }

    return 0;
}
