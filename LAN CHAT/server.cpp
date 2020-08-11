#include <iostream>
using namespace std;

#include<unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <vector>
#include <cstring>

class Args{
    public:
        int sockfd, port;
        char *ip;
    Args(int sockfd, char *ip, int port){
        this->sockfd = sockfd;
        this->ip = ip;
        this->port = port;
    }
};
class Client{
public:
    int sockfd, usr_id, grp_no;
    Client(int s, int u, int g){
        sockfd = s;
        usr_id = u;
        grp_no = g;
    }
};

//usr_id will be the index
vector<Client*> clients(100);

void error(char *msg)
{
    perror(msg);
    exit(1);
}


char* stringtochararr(string s){
    int l = s.length();
    char *msg = new char[l];
    for(int i=0; i<l; i++) msg[i] = s[i];
    return msg;
}

void listActiveUsers(int u, int newsockfd){
    int n = write(newsockfd,">>>>Online Users<<<<\n", 255);
    for(auto it: clients){
             if(it){
                 string s = "";
                 if(u == it->usr_id)
                    s += "(You)";
                 s += "Usr_id:" + to_string(it->usr_id) + " Grp_no: " + to_string(it->grp_no) + "\n";
                 char *msg = new char[s.length()+1];
                 strcpy(msg, s.c_str());
                 n = write(newsockfd, msg, 255);
                 if(n < 0)
                     error((char*)"Error writing to socket");
             }
         }

    n = write(newsockfd, "-----------------------------\n", 255);
}


void* client_handler (void* arg){
    Args* args = (Args*)arg;
    int newsockfd = args->sockfd;
    char *ip = args->ip;
    int port = args->port;
    char buffer[256];

    //firstrun
    int n = read(newsockfd,buffer,255);
    if (n < 0) error((char*)"ERROR reading from socket");
    n = write(newsockfd,"---Welcome to chatroom---\n-------------------- \n", 255);
    int u = (int)buffer[0]; //current client's' user id
    int g = (int)buffer[1]; //current client's group no

    Client* newcli = new Client(newsockfd, u, g);
    clients[u] = newcli;

    listActiveUsers(u, newsockfd);

    //Sending guidelines
    n = write(newsockfd, "<<Guidelines>>\nLIST -> print all active users\nSEND -> send msg(three modes available\n\t0 <your msg here> -> Broadcast to all\n\t1 <your msg here> -> Broadcast to same grp members\n\t2 <usr_id> <your msg here> -> Send msg to specifed usr_id\nquit > exit chatroom\n", 256);
    if(n < 0)
        error((char*)"Error writing to socket");


    while(1){
     bzero(buffer, 256);
     n = read(newsockfd,buffer,256);
     if (n < 0) error((char*)"ERROR reading from socket");
     buffer[n] = '\0';
     if(!strcmp((const char*)buffer, "LIST")){
         listActiveUsers(u, newsockfd);
     }
     else if(!strcmp((const char*)buffer, "SEND")){
         n = read(newsockfd, buffer, 256);
         if (n < 0) error((char*)"ERROR reading from socket");

         int mode = (int)(buffer[0]-'0');
         if(mode == 0){
             //Broadcast to all clients
             for(auto it: clients){
                 if(it && it->usr_id != u){
                     string s = "Usr_" + to_string(u) + " with IPaddr: ";
                     s += ip;s += " "+to_string(port)+" sent msg>>";
                     s += &buffer[2]; s += "\n";
                     char* msg = new char[s.length()+1];
                     strcpy(msg, s.c_str());
                     n = write(it->sockfd, msg, 255);
                 }
             }
         }
         else if(mode == 1){
             //Broadcast to same group members
                for(auto it: clients){
                 if(it && it->grp_no == g && it->usr_id != u){
                     string s = "Usr_" + to_string(u) + " with IPaddr: ";
                     s += ip; s += " "+to_string(port)+" sent msg>>";
                     s += &buffer[2]; s += "\n";
                     char* msg = new char[s.length()+1];
                     strcpy(msg, s.c_str());
                     n = write(it->sockfd, msg, 255);
                 }
             }
         }
         else{
             //Send to specified user
             int i, dest_u;
             if(buffer[3] >= 48 && buffer[3] <= 67){
                 dest_u = (buffer[2]-'0')*10 + (buffer[3]-'0');
                 i = 5;
             }
             else{
                 dest_u = (buffer[2]-'0');
                 i = 4;
             }
                for(auto it: clients){
                 if(it && it->usr_id == dest_u && it->usr_id != u){
                     string s = "Usr_" + to_string(u) + " with IPaddr: ";
                     s += ip; s += " "+to_string(port)+" sent msg>>";
                     s += &buffer[2]; s += "\n";
                     char* msg = new char[s.length()+1];
                     strcpy(msg, s.c_str());
                     n = write(it->sockfd, msg, 255);
                     break;
                 }
             }
         }
     }
     else if(!strcmp((const char*)buffer, "quit")){
         cout<<"[-]Disconnected from "<<ip<<" "<<port<<endl;
         clients[u] = NULL; 
         close(newsockfd);
         break;
      }
      bzero(buffer, 256);

    }
    return NULL;
}



int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen;
     struct sockaddr_in serv_addr, cli_addr;


     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error((char*)"ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
         error((char*)"ERROR on binding");
     listen(sockfd,2);
     clilen = sizeof(cli_addr);

    pthread_t p;
    cout<<"Server listening on port "<<ntohs(serv_addr.sin_port)<<endl<<flush;
     while (1) {
         newsockfd = accept(sockfd, 
               (struct sockaddr *) &cli_addr, (socklen_t*)&clilen);
         if (newsockfd < 0) 
             error((char*)"ERROR on accept");
         char ip4[INET_ADDRSTRLEN];
         inet_ntop(AF_INET, &(cli_addr.sin_addr.s_addr), ip4, INET_ADDRSTRLEN);
         int c_port = ntohs(cli_addr.sin_port);
         cout<<"[+]Connection accepted from "<<ip4<<" "<<c_port<<" "<<ntohs(serv_addr.sin_port)<<endl;
         Args* args = new Args(newsockfd, ip4, c_port);
         pthread_create(&p, NULL, client_handler, (void*)args);

    }
    pthread_join(p, NULL);
     return 0; /* we never get here */
}
