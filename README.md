#   💬 Multiparty-Chat
----
Client server based file sharing and text exchanging application in which clients communicate through a server and then client messages/ data is send to all connected clients on server.


----


#   🔍 OverView
----
MultiParty Chat is an application in which:
  - There is a server which recives connection requests.
  - Clients which connects to server.
  - Clients communicate with each other through server.
  - Each clients message is first sent to server which in turn send it to all connected clients in chat.
  - Clients can send text as well as multi media files, binary files etc.
  - It uses TCP protocol for sending and recieving data.

----
#  💭 How To Run!
---
This code compiles on UNIX systems because of differences in APIs in windows and UNIX based systems. To compile and run this open terminal in folder where code files are and type:
  - run 'g++ -o server server.cpp -lpthread'
  - run 'g++ -o client client.cpp -lpthread'  
This creates two application files with name server and client. Open two terminal windows and in one first run server by typing './server' in terminal and in second one run client by typing './client'. Now at this point one client has connected to server. Run multiple clients in same way and share messages/ files.


----
#  Output
## Multiple clients connecting to server
![image](https://user-images.githubusercontent.com/63426759/170823733-2e9ff9a4-ca99-4c16-bfe5-4d3f185b8b8f.png)
## Sending Message from clients
![image](https://user-images.githubusercontent.com/63426759/170823858-189e3351-f372-462e-afe0-6337875768d8.png)
## Sending File from client, recieved to server and other clients
![image](https://user-images.githubusercontent.com/63426759/170824004-a5b323b9-4074-41a9-914f-90422d6cdac5.png)
