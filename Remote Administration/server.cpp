#include <iostream>
using namespace std;

#include<unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <cstring>
#include <set>


set<int> clients;
char buffer[256];

void error(char *msg)
{
    perror(msg);
    exit(1);
}


void saveIncomingData(int sockfd, const char* fname) {
    FILE *f;
        f = fopen(fname, "a");
        if(f == NULL){
            cout<<"Error opening file "<<endl;
            exit(1);
        }
        long words;
        int n = read(sockfd, &words, sizeof(long));
        if(n < 0){
            cout<<"ERROR reading from socket..."<<endl;
            exit(1);
        }
        char *buff = new char[words];
        long bytesrec = 0;
        bytesrec = read(sockfd, buff, words);
        fwrite(buff, 1, bytesrec, f);
        if(bytesrec < 0)
            cout<<"Error reading "<<endl;

        fclose(f);
}

void* client_handler (void* arg){
    int* p = (int*)arg;
    int newsockfd = *p;

    //firstrun
    clients.insert(newsockfd);
    write(newsockfd,"---Welcome to Remote Administration---\n", 255);
    return NULL;
}

void* handle_input(void* args){
    while(1){
     int n;
     cin.getline(buffer, 256);
     fclose(fopen("temps.txt", "w"));


     // polling
     for(auto it:clients) n = write(it, buffer, 256);

      if(!strcmp(buffer, "getip") || !strcmp(buffer, "getusers") || !strcmp(buffer, "getinfo") || !strcmp(buffer, "getrecords")){
          for(auto it: clients)
              saveIncomingData(it, "temps.txt");

          system("cat temps.txt");
          cout<<endl<<flush;
      }
      //student record file
      else if(!strcmp(buffer, "putrecord")){
          cout<<"Enter record: "<<flush;
          bzero(buffer, 256);
          cin.getline(buffer, 256);
          for(auto it:clients)
          n = write(it, buffer, 256);
      }
      else if(!strcmp(buffer, "quit")){
          for(auto it:clients){
              close(it);
          }
          exit(1);
      }
      //shutdown remote
      else if(!strcmp(buffer, "shutdown")){
          bzero(buffer, 256);
          cout<<"ip: "<<flush;
          cin.getline(buffer, 256);
          for(auto it:clients) n = write(it, buffer, 256);
    }
       bzero(buffer, 256);
 
     }

    return args;
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

    pthread_t p1, p2;
    pthread_create(&p1, NULL, handle_input, (void*)NULL);
    cout<<"Server listening on port "<<ntohs(serv_addr.sin_port)<<endl<<flush;
     while (1) {
         newsockfd = accept(sockfd, 
               (struct sockaddr *) &cli_addr, (socklen_t*)&clilen);
         if (newsockfd < 0) 
             error((char*)"ERROR on accept");
         char ip4[INET_ADDRSTRLEN];
         inet_ntop(AF_INET, &(cli_addr.sin_addr.s_addr), ip4, INET_ADDRSTRLEN);
         int c_port = ntohs(cli_addr.sin_port);
         cout<<"[+]Connection accepted from "<<ip4<<" "<<c_port<<" "<<endl;
         pthread_create(&p2, NULL, client_handler, (void*)&newsockfd);


    }


    pthread_join(p2, NULL);
     return 0; /* we never get here */
}
