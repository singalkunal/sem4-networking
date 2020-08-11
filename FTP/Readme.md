## FTP 

Write a simple Server and Client program using the socket programming that can simulate the functionality of a real FTP Server and Client.
#### Client Specifications: 
Client program should accept as parameters from the command line: 1) the IP Address and 2) the port number of the server. 

The order of the parameters in the command line should be in the order mentioned above. If, either, there is less number of command line arguments or are not in required order, your program must exit after displaying appropriate message. Client should show a prompt like….
your_roll_number_ftp-client > on which any of the following commands may be issued so as to provide usual ftp client’s functionalities:

    s_ls                      to list the files in the current directory on the server side.
  
    s_pwd                  to list the current directory on the server side.
  
    s_cd <dir_name>   	to change the directory on the server side.
  
    c_ls			to list the files in the current directory on the client side.
  
    c_pwd                   	to list the current directory on the client side.
  
    c_cd <dir_name>   	to change the directory on the client side.
  
    put <file_name>     	to send a file from the client to the server.
  
    get <file_name>     	to receive a file from the server to the client.
  
    quit                          	to close the client program.
  
  <br>
  
If any of the command cannot be completed successfully, proper error message must be displayed. The client should receive messages from the server and print all lines to STDOUT until no more lines are received. In case of file transfer command, instead of displaying the contents of the file on the STDOUT, these should be saved in the current directory with the same file name as on the server.

#### Server Specifications: 
Your server should accept a single parameter from the command line the port number to listen to. The server should execute the commands received from the server and send their output to the client. For the get command, after receiving the name of the file from the client, if the file cannot be opened in the current directory, then the server should return appropriate message to client so that it may print “File not found” error message. If the file is opened successfully, then the server should send all the characters of the file to the client until the end of the file is reached. After sending the file, the server should close the file and then continue to listen for more client requests, in an iterative fashion. Server should print out appropriate informative messages as it executes. You should do appropriate error detection for the TCP socket calls. 

