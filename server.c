// Гостиница сервер
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_ROOMS 30
#define MAX_BENCH 100

int rooms[MAX_ROOMS] = {0}; // 0 значит комната пустая
int bench[MAX_BENCH] = {0}; // 0 значит скамейка пустая

int find_empty_room() {
    for (int i = 0; i < MAX_ROOMS; i++) {
        if (rooms[i] == 0) {
            return i;
        }
    }
    return -1; // нет свободной комнаты
}

int find_empty_bench() {
    for (int i = 0; i < MAX_BENCH; i++) {
        if (bench[i] == 0) {
            return i;
        }
    }
    return -1; // нет свободной скамейки
}

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
        error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");

    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    while(1) {
        newsockfd = accept(sockfd,
                           (struct sockaddr *) &cli_addr,
                           &clilen);

        if (newsockfd < 0)
            error("ERROR on accept");

        bzero(buffer,256);
        n = read(newsockfd,buffer,255);

        if (n < 0) error("ERROR reading from socket");

        printf("Here is the message: %s\n",buffer);

        // Дает комната, если свободна , дает скамейку, иначе
        int room_num = find_empty_room();
        if (room_num != -1) {
            rooms[room_num] = 1; // дает комнату
            sprintf(buffer, "Guest assigned to room %d", room_num);
        } else {
            int bench_num = find_empty_bench();
            if (bench_num != -1) {
                bench[bench_num] = 1; // дает скамейку
                sprintf(buffer, "No room available. Guest waiting on bench %d", bench_num);
            } else {
                sprintf(buffer, "No room or bench available. Guest turned away.");
            }
        }

        n = write(newsockfd,buffer,strlen(buffer));

        if (n < 0) error("ERROR writing to socket");

        close(newsockfd);
    }

    close(sockfd);
    return 0;
}
