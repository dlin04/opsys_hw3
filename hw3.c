#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>

extern int total_guesses;
extern int total_wins;
extern int total_losses;
extern char ** words;

// function to fill char ** words from given file
void parse_words(const char *filename, int num_words) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("ERROR: Unable to open dictionary file");
        return;
    }

    words = (char **)calloc(num_words, sizeof(char *));
    if (words == NULL) {
        perror("ERROR: Unable to allocate memory for words list");
        close(fd);
        return;
    }

    char *buffer = (char*)calloc(6, sizeof(char));
    int bytes_read;
    int index = 0;
    while((bytes_read = read(fd, buffer, 5)) > 0){
        if (bytes_read < 5) { break; }
        *(buffer + bytes_read) = '\0';
        if (index < num_words) {
            *(words + index) = (char *)calloc(6, sizeof(char));
            if (*(words + index) == NULL) {
                perror("ERROR: Unable to allocate memory for word");
                return;
            }
            strncpy(*(words + index), buffer, 6);
            index++;
        }

        if (read(fd, buffer, 1) == 1 && *(buffer) != '\n') {
            lseek(fd, -1, SEEK_CUR);
        }
    }

    *(words + num_words) = NULL; // last entry must be null
    free(buffer);
    close(fd);
}

// handles inputs from client and updates data
void *handle_client(void *arg){
    int num_guesses = 0;
    char *buffer = (char*)calloc(6, sizeof(char));
    while (num_guesses < 5){
        num_guesses++;
    }
}

int wordle_server(int argc, char **argv){
    if (argc != 5){
        fprintf(stderr, "ERROR: Invalid argument(s)\n");
        fprintf(stderr, "USAGE: hw3.out <listener-port> <seed> <dictionary-filename> <num-words>\n");
        return EXIT_FAILURE;
    }

    int PORT = atoi(*(argv + 1));
    int seed = atoi(*(argv + 2));
    char * filename = *(argv + 3);
    int num_words = atoi(*(argv + 4));

    srand(seed);
    parse_words(filename, num_words);
    printf("MAIN: opened %s (%d words)\n", filename, num_words);
    printf("MAIN: seeded pesudo-random number generator with %d\n", seed);

    // create tcp socket and setup conection to client
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1 ) { 
        perror( "ERROR: socket() failed"); 
        return EXIT_FAILURE; 
    }

    // setup address structure
    struct sockaddr_in tcp_server;
    tcp_server.sin_family = AF_INET;
    tcp_server.sin_port = htons(PORT);
    tcp_server.sin_addr.s_addr = INADDR_ANY;

    // bind the socket
    if(bind(server_socket, (struct sockaddr *)& tcp_server, sizeof(tcp_server)) == -1){
        perror("ERROR: bind() failed");
        close(server_socket);
        return EXIT_FAILURE;
    }

    // listen for connections, second parameter for max queued at once
    // not sure what the number should be???
    if(listen(server_socket, 10) == -1){
        perror("ERROR: listen() failed");
        close(server_socket);
        return EXIT_FAILURE;
    }

    printf("MAIN: Wordle sever listening on port {%d}\n", PORT);
    // start accepting clients
    while(1) {
        int client_socket = accept(server_socket, NULL, NULL);
        if (client_socket == -1){
            perror("ERROR: failed to accept");
            continue;
        }

        printf("MAIN: rcvd incoming connection request\n");
        
    }

    close(server_socket);
    return EXIT_SUCCESS;
}
