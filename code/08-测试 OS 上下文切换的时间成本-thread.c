#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define NUM_THREADS 5

void *client_thread(void *arg)
{
    int sockfd;
    struct sockaddr_in servaddr;
    char buffer[BUFFER_SIZE];
    int thread_id = *((int *)arg);

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setup server address
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Send and receive data
    // snprintf 格式化拼接字符串到缓冲区
    snprintf(buffer, sizeof(buffer), "Hello from thread %d", thread_id);
    send(sockfd, buffer, strlen(buffer), 0);
    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("Thread %d received: %s\n", thread_id, buffer);

    // Close socket
    close(sockfd);
    return NULL;
}

void *server_thread(void *arg)
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cliaddr;
    socklen_t len;
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Setup server address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    // Bind socket
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(sockfd, 5) < 0)
    {
        perror("Listen failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        // Accept connection
        len = sizeof(cliaddr);
        if ((connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &len)) < 0)
        {
            perror("Accept failed");
            continue;
        }

        // Receive and send data
        recv(connfd, buffer, sizeof(buffer), 0);
        send(connfd, buffer, strlen(buffer), 0);

        // Close connection
        close(connfd);
    }

    close(sockfd);
    return NULL;
}

int main()
{
    pthread_t server_tid, client_tids[NUM_THREADS];
    int thread_ids[NUM_THREADS];
    struct timespec start, end;
    long total_time = 0;

    // Start server thread
    pthread_create(&server_tid, NULL, server_thread, NULL);
    sleep(1); // Give server time to start

    // Start client threads and measure time
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        thread_ids[i] = i;
        pthread_create(&client_tids[i], NULL, client_thread, &thread_ids[i]);
    }

    // Wait for client threads to finish
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        pthread_join(client_tids[i], NULL);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculate total time in nanoseconds
    total_time = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
    printf("Total time: %ld ns\n", total_time);

    // Stop server thread (in a real-world scenario, you might use a more graceful shutdown)
    pthread_cancel(server_tid);
    pthread_join(server_tid, NULL);

    return 0;
}
