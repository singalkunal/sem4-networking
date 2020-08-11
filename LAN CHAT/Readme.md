### LAN CHATTING APP - 


  Write a server program that allows chatting among multiple clients. Each Client must have a 2-digit number as user_code
and 1 digit number as group_code (specified as command line arguments while executing client program) and these code are
informed to all other clients. Any Client recognize the other clients by this user_code only. When a new client joins, server provides it the list of all available other clients (client’s user_code, group_code) and other clients are also updated with the newly joined client. Server will pass a message from some source client to either some designated destination client or clients having same group_code or all clients active in the system. Every client will send acknowledgement for the received message to the server. Server will inform the source client about status (successful / unsuccessful) of transmission of message.
Optional Feature: When some client does not participate in chatting for some amount of time (configurable - say 30 sec or 1 min or 2 minutes), it is removed from active client list by the system and all active clients are notified about this event.
