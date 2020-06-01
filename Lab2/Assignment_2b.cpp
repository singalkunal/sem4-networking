#include <bits/stdc++.h>
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <cstring>

int main(){
    string s;
    do{
        cout<<"ps -> list running process\nkill <pid> -> kills process\nquit\n";
        cin>>s;
        if(s == "ps")
            system("ps");
        else if(s == "kill"){
            int pid; cin>>pid;
            kill(pid, SIGINT);
        }
    }while(s != "quit");
}

