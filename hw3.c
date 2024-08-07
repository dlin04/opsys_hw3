#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

extern int total_guesses;
extern int total_wins;
extern int total_losses;
extern char ** words;

void load_words(const char *filename, int num_words) {
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

    free(buffer);
    close(fd);
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
    load_words(filename, num_words);
    printf("MAIN: opened %s (%d words)\n", filename, num_words);
    printf("MAIN: seeded pesudo-random number generator with %d\n", seed);
    printf("MAIN: Wordle sever listening on port {%d}\n", PORT);

    return EXIT_SUCCESS;
}
