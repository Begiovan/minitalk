#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define END_TRANSMISSION '\0'

volatile sig_atomic_t g_client_pid = 0;

void    handle_signal(int sig, siginfo_t *info, void *context)
{
    static unsigned char    current_char;
    static int              bit_index;
    
    (void)context;
    
    g_client_pid = info->si_pid;
    
    if (sig == SIGUSR1)
        current_char |= (1 << bit_index);
    
    bit_index++;
    if (bit_index == 8)
    {
        if (current_char == END_TRANSMISSION)
            printf("\n");
        else
            printf("%c", current_char);
        fflush(stdout);
        bit_index = 0;
        current_char = 0;
    }
    usleep(50);
    kill(g_client_pid, SIGUSR1);
}

int    main(void)
{
    struct sigaction    sa;
    
    printf("Server PID: %d\n", getpid());
    fflush(stdout);
    
    sa.sa_sigaction = handle_signal;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGUSR1);
    sigaddset(&sa.sa_mask, SIGUSR2);
    
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    
    while (1)
        pause();
    
    return (0);
}