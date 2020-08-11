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
#include <arpa/inet.h>

void error(char *msg)
{
    perror(msg);
    exit(0);
}

/*
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
*/
void sendData(int newsockfd, const char* fname){
    FILE *fp = fopen(fname, "r");
    if(fp == NULL){
        cout<<"Error opening file "<<endl;
        exit(1);
    }

    // obtain file size:
    fseek (fp , 0 , SEEK_END);
    long lSize = ftell (fp);
    rewind (fp);

    char *buff = new char[lSize];
    write(newsockfd, &lSize, sizeof(long));
    int nread = fread(buff, sizeof(char), lSize, fp);
    if(nread != lSize){
        cout<<"Reading error"<<endl;
        exit(1);
    }
    write(newsockfd, buff, nread);

    fclose(fp);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server, *host;

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[2]);


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
    gethostname(buffer, 256);
    cout<<buffer<<endl;
    host = gethostbyname(buffer);
    char* ip = inet_ntoa(*((struct in_addr*)host->h_addr_list[0]));
    cout<<"ip: "<<ip<<endl;
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error((char*)"ERROR connecting");
    cout<<"[+]Connected to Server..."<<endl;

    while(1){
    bzero(buffer,256);
    
    n = read(sockfd,buffer,256);
    if (n < 0) 
         error((char*)"ERROR reading socket");

    if(!strcmp(buffer, "getip")){
        FILE* fp = fopen("tempc.txt", "w");
        fwrite(ip, sizeof(char), strlen(ip),fp);
        fclose(fp);
        sendData(sockfd, "tempc.txt");
    }
    else if(!strcmp(buffer, "getusers")){
        system("users > tempc.txt");
        sendData(sockfd, "tempc.txt");
    }
    else if(!strcmp(buffer, "getinfo")){
        system("who > tempc.txt");
        sendData(sockfd, "tempc.txt");
    }
    else if(!strcmp(buffer, "getrecords")){
        sendData(sockfd, "records.txt");
    }
    else if(!strcmp(buffer, "putrecord")){
        n = read(sockfd, buffer, 256);
        cout<<"Updating records..."<<endl<<flush;
        FILE *fp = fopen("records.txt", "a+");
        fwrite("\n", sizeof(char), 1, fp);
        fwrite(buffer, sizeof(char), n, fp);
        fclose(fp);
    }
    else if(!strcmp(buffer, "clearrecords")) 
        fclose(fopen("records.txt", "w"));
    else if(!strcmp(buffer, "shutdown")){
        n = read(sockfd, buffer, 256);
        if(!strcmp(ip, buffer)){
            cout<<"Server wants to shutdown..."<<endl<<flush;
            system("sudo shutdown -hf now > tempc.txt");
        }
    }
    else if(!strcmp(buffer, "quit")){
        close(sockfd);
        exit(1);
    }
    }

    return 0;
}
