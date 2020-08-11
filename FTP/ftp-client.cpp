#include <iostream>
using namespace std;

#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>

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
        char *buff = new char[1024];
        long bytesrec = 0;
        bytesrec = read(sockfd, buff, 1024);
        cout<<buff<<endl<<flush;
            fwrite(buff, 1, bytesrec, f);

        if(bytesrec < 0)
            cout<<"Error reading "<<endl;

        fclose(f);
}

void sendData(int newsockfd, const char* fname) {
    FILE *fp;
    fp = fopen(fname, "r");
    if(fp == NULL){
        cout<<"Error opening file "<<endl;
        FILE *f = fopen("temps.txt", "w");
        const char* msg = "FILE NOT FOUND ";
        fwrite(msg, 1, strlen(msg), f);
        sendData(newsockfd, "temps.txt");
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

vector<string> splitString(string mybuff){
    vector<string> v;
    string t;
    for(char c: mybuff){
        if(c == ' ' && t.length() > 1){
            v.push_back(t);
            t="";
        }
        else {
            t += c;
        }
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
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

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
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
        error((char*)"ERROR connecting");
    cout<<"[+]Connected to Server..."<<endl;

    while(1){
    bzero(buffer,256);
    cout<<endl<<"2018UCO1643_ftp-client> ";
    cin.getline(buffer, 256);

    string mybuff = buffer;
    vector<string> v = splitString(mybuff);

    cout<<endl;
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
         error((char*)"ERROR writing to socket");

    if(v[0] ==  "s_ls"){

        saveIncomingData(sockfd, "tempc.txt");
        system("cat tempc.txt");
    }
    else if(v[0] == "s_pwd"){
            saveIncomingData(sockfd, "tempc.txt");
            system("cat tempc.txt");
    }
    else if(v[0] == "s_cd"){
        saveIncomingData(sockfd, "tempc.txt");
        system("cat tempc.txt");
    }
    else if(v[0] == "c_ls"){
            system("ls");
    }
    else if(v[0] == "c_pwd"){
            system("pwd");
    }
    else if(v[0] == "c_cd") {
            char *cmd = stringTochararr(v);
            int cd = chdir(cmd);
            if(cd == 0)
                cout<<"Directory changed to "<<v[1]<<endl;
            else
                cout<<"Invalid path..."<<endl;
    }
    else if(v[0] == "get"){
        char *fname = stringTochararr(v);
        saveIncomingData(sockfd, fname);
    }
    else if(v[0] == "put"){
        char *fname = stringTochararr(v);
        sendData(sockfd, fname);
    }
    else if(v[0] == "quit"){
        cout<<"[-]Disconnecting from server..."<<endl;
        close(sockfd);
        exit(1);
    }
}
    return 0;
}
