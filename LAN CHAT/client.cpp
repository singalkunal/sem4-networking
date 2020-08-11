#include <iostream>
using namespace std;

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>
#include <signal.h>
#include <pthread.h>

void error(char *msg)
{
    perror(msg);
    exit(0);
}

void* recieveMsg(void* args){
    int* argp = (int*)args;
    int sockfd = *argp;
    char buffer[256];
    while(1){
    bzero(buffer,256);
   int n = read(sockfd,buffer,255);
    if (n < 0) 
         error((char*)"ERROR reading from socket");
    cout<<buffer<<flush;
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    int usr_id, grp_no;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 5) {
       fprintf(stderr,"usage %s hostname port usr_id grp_no\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);

    usr_id = atoi(argv[3]);
    grp_no = atoi(argv[4]);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error((char*)"ERROR opening socket");
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error((char*)"ERROR connecting");
    cout<<"[+]Connected to Server..."<<endl;
    bool firstrun = true;
    pthread_t p;
    pthread_create(&p, NULL, recieveMsg, (void*)&sockfd);
    while(1){
    bzero(buffer,256);
    
    if(firstrun){
        buffer[0] = usr_id;
        buffer[1] = grp_no;
        n = write(sockfd,buffer,strlen(buffer));
        firstrun = false;
        continue;
    }

    cin.getline(buffer, 256);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error((char*)"ERROR writing to socket");

    if(!strcmp(buffer, "SEND")){

        cin.getline(buffer, 256);
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) 
            error((char*)"ERROR writing to socket");

    }
    else if(!strcmp(buffer, "quit")){
        close(sockfd);
        break;
    }
    }
    return 0;
}
