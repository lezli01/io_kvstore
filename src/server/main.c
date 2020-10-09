#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <unistd.h>
#include <threads.h>
#include <pthread.h>
#include <signal.h>

#include "map.h"

static const unsigned int BUFFER_LENGTH = 1 << 16;

typedef struct {
    pthread_rwlock_t * lock;
    thrd_t             id;
    Map *              map;
    int                clientSocket;
} ThreadData;

static pthread_rwlock_t lock;
static Map * map;

void
printHelp();
void
terminate();

int
clientThread(void * data);
int
handleClient(ThreadData * data);
void
closeClient(ThreadData * data);

int
main(int argc, char ** argv)
{
    signal(SIGINT, terminate);
    signal(SIGTERM, terminate);

    int serverSocket;
    int clientSocket;
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;

    int port = 5555;

    if (argc > 1) {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
            printHelp();
            return 0;
        }

        port = (int) strtol(argv[1], NULL, 10);

        if (port == 0) {
            fprintf(stderr, "Invalid port value provided.\n");
            return 1;
        }
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket < 0) {
        fprintf(stderr, "Error creating socket.\n");
        return 1;
    }

    bzero((char *) &serverAddress, sizeof(serverAddress));

    serverAddress.sin_family      = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port        = htons(port);

    if (bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        fprintf(stderr, "Error binding socket on port: %d\n", port);
        return 1;
    }

    listen(serverSocket, 5);
    socklen_t clientLength = sizeof(clientAddress);

    printf("Listening on port: %d\n", port);

    pthread_rwlock_init(&lock, NULL);
    map = mapCreate();

    while (1) {
        clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &clientLength);

        if (clientSocket < 0) {
            fprintf(stderr, "Error accepting incoming connection.\n");
            return 1;
        }

        ThreadData * data = malloc(sizeof(ThreadData));
        data->lock         = &lock;
        data->map          = map;
        data->clientSocket = clientSocket;

        thrd_create(&data->id, clientThread, data);
        thrd_detach(data->id);
    }
} /* main */

void
printHelp()
{
    printf("usage: ./server [-h | --help] [port]\n\n");
    printf("Start key value server on configured TCP port on default interface.\n\n");
    printf("  port PORT value to open the TCP socket on. [default: 5555]\n");
}

void
terminate()
{
    if (map) {
        pthread_rwlock_destroy(&lock);
        mapDestroy(map);
        exit(0);
    }
}

int
clientThread(void * data)
{
    ThreadData * threadData = data;

    int retVal = handleClient(threadData);

    closeClient(threadData);

    return retVal;
}

int
handleClient(ThreadData * data)
{
    unsigned int length;
    char command;
    char key[BUFFER_LENGTH + 1];

    bzero(key, BUFFER_LENGTH);

    if (read(data->clientSocket, &command, 1) != 1) {
        fprintf(stderr, "Error reading from client.\n");
        return 1;
    }

    if (read(data->clientSocket, &length, 4) != 4) {
        fprintf(stderr, "Error reading key length.\n");
        return 1;
    }

    if (length > BUFFER_LENGTH) {
        fprintf(stderr, "Key sent is too long.\n");
        return 1;
    }

    if (read(data->clientSocket, key, length) != length) {
        fprintf(stderr, "Error reading key with length: %d.\n", length);
        return 1;
    }

    if (command == 'G') {
        pthread_rwlock_rdlock(data->lock);
        const char * value = mapGet(data->map, key);

        if (!value) {
            printf("No value for key: %s\n", key);
            length = 0;
            write(data->clientSocket, &length, 4);
            pthread_rwlock_unlock(data->lock);
            return 1;
        }

        printf("Getting %s = %s\n", key, value);

        length = strlen(value);

        write(data->clientSocket, &length, 4);
        write(data->clientSocket, value, strlen(value));

        pthread_rwlock_unlock(data->lock);
        return 0;
    }

    if (command == 'P') {
        char value[BUFFER_LENGTH + 1];

        if (read(data->clientSocket, &length, 4) != 4) {
            fprintf(stderr, "Error reading value length.\n");
            return 1;
        }

        if (length > BUFFER_LENGTH) {
            fprintf(stderr, "Value sent is too long.\n");
            return 1;
        }

        if (read(data->clientSocket, value, length) != length) {
            fprintf(stderr, "Error reading value with length: %d.\n", length);
            return 1;
        }

        printf("Setting %s = %s\n", key, value);
        pthread_rwlock_wrlock(data->lock);
        mapPut(data->map, key, value);
        pthread_rwlock_unlock(data->lock);

        write(data->clientSocket, value, strlen(value));
        return 0;
    }

    return 1;
} /* handleClient */

void
closeClient(ThreadData * data)
{
    close(data->clientSocket);
    free(data);
}
