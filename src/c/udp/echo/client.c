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

#include <argp.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define DEFAULT_DATA "Hello, world!"
#define DEFAULT_LISTEN_PORT "55555"
#define BUFFER_SIZE 256

#define MAX(a, b) ((a)>(b)?(a):(b))

/**
 * struct for storing command line arguments.
 **/
struct arguments {
    // address of the udp echo server
    char const *server_addr;

    // port of the udp echo server
    char const *server_port;

    // data to send
    char const *data;
};

/**
 * opt_handler function for GNU argp.
 **/
static error_t opt_handler(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;

    switch(key) {
        case 'a':
            arguments->server_addr = arg;
            break;
        case 'd':
            arguments->data = arg;
            break;
        case 'p':
            arguments->server_port = arg;
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
    static char const doc[] = "client: talk to the udp server";
    static char const args_doc[] = "";

    // command line options
    static struct argp_option const options[] = {
        // Option -a --server-address: address of the udp server
        {"server-address", 'a', "SERVER_ADDR", 0, "address of the udp server"},

        // Option -p --server-port: port of the udp server
        {"server-port", 'p', "SERVER_PORT", 0, "port of the udp server"},

        // Option -d --data: data to send
        {"data", 'd', "DATA", 0, "data to send"},

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
        .server_port = DEFAULT_LISTEN_PORT,
        .data = DEFAULT_DATA,
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
    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_aton(arguments->server_addr, &server_addr.sin_addr);
    server_addr.sin_port = htons(atoi(arguments->server_port));

    // create udp socket
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == -1) {
        perror("fail to create socket");
        return 1;
    }

    char buffer[BUFFER_SIZE];
    strncpy(buffer, arguments->data, BUFFER_SIZE);
    buffer[BUFFER_SIZE-1] = '\0';

    int bytes_sent = sendto(s, buffer, strlen(buffer)+1, 0,
            (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (bytes_sent == -1) {
        perror("fail to send data");
        return 1;
    }

    struct sockaddr_in peer_addr;
    int peer_addr_len = sizeof(peer_addr);
    int bytes_recieved = recvfrom(s, buffer, BUFFER_SIZE, 0,
            (struct sockaddr *)&peer_addr, &peer_addr_len);
    if (bytes_recieved == -1) {
        perror("fail to recieve data from server");
        return 1;
    }

    buffer[MAX(bytes_recieved-1, 0)] = '\0';
    printf("%s\n", buffer);

    // close socket
    int rv = close(s);
    if (rv == -1) {
        perror("fail to close socket");
        return 1;
    }

    return 0;
}
