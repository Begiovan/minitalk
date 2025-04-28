#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

volatile sig_atomic_t g_bit_count = 0;
volatile sig_atomic_t g_char = 0;
volatile sig_atomic_t g_client_pid = 0;

void handle_sigusr1(int signum)
{
    (void)signum;
    
    // Add 0 bit (already 0 by left shift)
    g_char <<= 1;
    g_bit_count++;
    
    if (g_bit_count == 8)
    {
        char c = g_char;
        write(1, &c, 1);
        g_bit_count = 0;
        g_char = 0;
    }
    
    // Send acknowledgement back to client
    if (g_client_pid > 0)
    {
        kill(g_client_pid, SIGUSR1);
    }
}

void handle_sigusr2(int signum)
{
    (void)signum;
    
    // Add 1 bit
    g_char <<= 1;
    g_char |= 1;
    g_bit_count++;
    
    if (g_bit_count == 8)
    {
        char c = g_char;
        write(1, &c, 1);
        g_bit_count = 0;
        g_char = 0;
    }
    
    // Send acknowledgement back to client
    if (g_client_pid > 0)
    {
        kill(g_client_pid, SIGUSR1);
    }
}

void handle_sigint(int signum)
{
    (void)signum;
    printf("\nServer shutting down\n");
    exit(0);
}

// New handler to receive client PID
void handle_sigusr3(int signum, siginfo_t *info, void *context)
{
    (void)signum;
    (void)context;
    g_client_pid = info->si_pid;
    printf("Connected to client PID: %d\n", g_client_pid);
}

int main(void)
{
    struct sigaction sa_usr1, sa_usr2, sa_int, sa_usr3;
    sigset_t block_mask;
    
    // Create a signal mask to block both SIGUSR1 and SIGUSR2 during handlers
    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGUSR1);
    sigaddset(&block_mask, SIGUSR2);
    
    // Set up SIGUSR1 handler (0 bit)
    sa_usr1.sa_handler = handle_sigusr1;
    sa_usr1.sa_mask = block_mask;
    sa_usr1.sa_flags = 0;
    
    // Set up SIGUSR2 handler (1 bit)
    sa_usr2.sa_handler = handle_sigusr2;
    sa_usr2.sa_mask = block_mask;
    sa_usr2.sa_flags = 0;
    
    // Set up SIGINT handler (Ctrl+C)
    sa_int.sa_handler = handle_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    
    // Set up SIGTERM handler to get client PID
    sa_usr3.sa_sigaction = handle_sigusr3;
    sigemptyset(&sa_usr3.sa_mask);
    sa_usr3.sa_flags = SA_SIGINFO;
    
    // Register signal handlers
    sigaction(SIGUSR1, &sa_usr1, NULL);
    sigaction(SIGUSR2, &sa_usr2, NULL);
    sigaction(SIGINT, &sa_int, NULL);
    sigaction(SIGTERM, &sa_usr3, NULL);
    
    // Print PID
    printf("Server PID: %d\n", getpid());
    printf("Waiting for client connection...\n");
    fflush(stdout);
    
    // Wait for signals
    while (1)
    {
        pause();
    }
    
    return 0;
}