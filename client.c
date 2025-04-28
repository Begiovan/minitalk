#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

volatile sig_atomic_t g_ack_received = false;

// Signal handler for server acknowledgement
void handle_ack(int signum)
{
    (void)signum;
    g_ack_received = true;
}

void send_bit(int pid, int bit)
{
    // Reset acknowledgement flag
    g_ack_received = false;
    
    // Send the bit signal
    if (bit == 1)
        kill(pid, SIGUSR2);
    else
        kill(pid, SIGUSR1);
    
    // Wait for acknowledgement
    while (!g_ack_received)
        usleep(100); // Small sleep to avoid busy waiting
}

void send_char(int pid, char c)
{
    int i;
    
    for (i = 7; i >= 0; i--)
    {
        int bit = (c >> i) & 1;
        send_bit(pid, bit);
    }
}

int main(int argc, char *argv[])
{
    int server_pid;
    char *str;
    int i = 0;
    struct sigaction sa;
    
    if (argc != 3)
    {
        printf("Usage: %s <server_pid> <string>\n", argv[0]);
        return 1;
    }
    
    server_pid = atoi(argv[1]);
    if (server_pid <= 0)
    {
        printf("Invalid PID\n");
        return 1;
    }
    
    str = argv[2];
    
    // Set up acknowledgement signal handler
    sa.sa_handler = handle_ack;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);
    
    // Send our PID to the server first
    printf("Connecting to server PID: %d\n", server_pid);
    kill(server_pid, SIGTERM);
    usleep(10000); // Wait 10ms for server to set up
    
    // Send each character
    printf("Sending message: %s\n", str);
    while (str[i])
    {
        send_char(server_pid, str[i]);
        i++;
    }
    
    // Send a newline at the end
    send_char(server_pid, '\n');
    
    printf("Message sent successfully\n");
    
    return 0;
}