#include <iostream>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h> 
using namespace std;

int main() {
    pid_t pid = fork();
    
    //pid is 0 =>   child process
    //pid is > 0 => parent process
    if(pid == -1){
        cout<<"Something went wrong"<<endl;
    }
    else if(pid == 0){

        cout<<"I am child process"<<endl;
        int randomtime = rand() % 5;

        cout<<"Process ids:"<<endl;
        cout<<"My Process id: ";
        cout<<getpid()<<endl;
        cout<<"Parent Process id: ";
        cout<<getppid()<<endl;

        cout<<"Now i will sleep for "<<randomtime<<"s"<<endl;
        sleep(randomtime);
        cout<<"Exiting child process"<<endl;
        exit(2);
    }
    else{

        cout<<"This is Parent process"<<endl;
    
        int status;

        waitpid(pid, &status, 0);   //wait until chlid terminates (when pid > 0)

        if(WIFEXITED(status)){
            int exit_status = WEXITSTATUS(status);
            cout<<"Exit status of the child was: "<<exit_status<<endl;
            }
    
     }
    return 0;
}
