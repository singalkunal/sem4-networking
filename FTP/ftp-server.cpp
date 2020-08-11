#include <iostream>
using namespace std;

#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <string.h>

void sendData(int newsockfd, const char* fname) {
    FILE *fp;
    fp = fopen(fname, "r");
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

void saveIncomingData(int sockfd, const char* fname) {
    FILE *f;
        f = fopen(fname, "w");
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

vector<string> splitString(string mybuff){
    vector<string> v;
    string t;
    for(char c: mybuff){
        if(c == ' ' && t.length() > 1){
            v.push_back(t);
            t="";
        }
        else
            t+=c;
    }

    if(t != "")
        v.push_back(t);

    return v;
}

char* stringTochararr(vector<string> v) {
    char *cmd = new char[v[1].length()+1];
         cmd[v[1].length()] = '\0';
         for(int i=0; i<(int)v[1].length(); i++){
             cmd[i] = v[1][i];
         }


    return cmd;
}

void error(char *msg)
{
    perror(msg);
    _exit(1);
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         _exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error((char*)"ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error((char*)"ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,(socklen_t *)&clilen);
     if (newsockfd < 0) 
          error((char*)"ERROR on accept");
     cout<<"[+]Connected to Client..."<<endl;
     while(1){
     bzero(buffer, 256);
     n = read(newsockfd,buffer,256);
     if (n < 0) error((char*)"ERROR reading from socket");

     string mybuff = buffer;
     vector<string> v = splitString(mybuff);

     if(v[0] == "s_ls"){

             system("ls > temps.txt");
             sendData(newsockfd, "temps.txt");
        }
     else if(v[0] == "s_pwd"){

            system("pwd > temps.txt");
            sendData(newsockfd, "temps.txt");
     }
     else if(v[0] == "s_cd") {
         char *cmd = stringTochararr(v);
         int cd = chdir(cmd);

         delete [] cmd;

         FILE *fp = fopen("temps.txt", "w");
         const char *msg;

         if(cd == 0){
             cout<<"Directory changed to "<<v[1]<<endl;
             msg = "Directory changed successfully";
         }
         else{
             cout<<"Invalid path.."<<endl;
             msg = "Invalid path...";
         }

         fwrite(msg, 1, strlen(msg), fp);
         fclose(fp);

         sendData(newsockfd, "temps.txt");

     }
     else if(v[0] == "get") {
         char *fname = stringTochararr(v);
         sendData(newsockfd, fname);
     }
     else if(v[0] == "put") {
         char *fname = stringTochararr(v);
         saveIncomingData(newsockfd, fname);
     }
     else if(v[0] == "quit"){
             cout<<"[-]Disconnecting client..."<<endl;
             close(newsockfd);
             exit(1);
      }

     }
     return 0; 
}
