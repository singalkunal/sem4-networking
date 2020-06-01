#include <iostream>
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

int pid;

void sigint_handler(int signal)
{
    cout<<"AHh sigint "<<endl<<flush;
    kill(pid, SIGKILL);
}

void sig_handler(int sig){
    kill(pid, SIGINT);
}

int main(void)
{

    int pid;
        struct sigaction sa;
        sa.sa_handler = sig_handler;
        sa.sa_flags = 0; // or SA_RESTART
        sigemptyset(&sa.sa_mask);

        if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
        }



    if((pid=fork()) < 0)
        cout<<"Fork error "<<endl<<flush;


    else if(pid == 0){
        sleep(2);
        cout<<"Hey i am a child process\n"<<"My process id: "<<getpid()<<endl;
        signal(SIGINT, sigint_handler);
        while(1){}
    }

        // parent process
        int status;
        waitpid(pid, &status, 0);
        if(WIFEXITED(status)){
            int exit_status = WEXITSTATUS(status);
            cout<<"Exit status of the child was: "<<exit_status<<endl;
        }

    return 0;
}
