#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <iostream>
#include <arpa/inet.h>
#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#define PORT 8080
#define size 1024
using namespace std;
int clients[3];
char send_filename[100];

void make_file(void *conn) 
{
    long connfd = (long) conn; //connection handler
    char fileData[size]; //buffer to store incoming file data.
    char filename[100];
    memset(&filename,0,sizeof(filename));
    read(connfd,&filename,sizeof(filename));
    memset(&send_filename,0,sizeof(send_filename));
    strncpy(send_filename,filename,sizeof(filename));
    cout<<"RECIEVED FILE: "<<filename<<endl;
    //clear buffers
    memset(&fileData, 0, sizeof(fileData));
    //declaring variables
    int file_size = 0, read_size = 0, recv_size = 0, write_size = 0, current_stat = 0;
    //get size of incoming file
    do 
    {
        current_stat = read(connfd, &file_size, sizeof(file_size));
    }
    while (current_stat < 0);
    //creating handle for file
    FILE * fp;
    fp = fopen(filename, "w"); //open file in write/read mode
    if (fp) 
    {
        while (recv_size < file_size) 
        {
            //read a chunk of data
            do 
            {
                read_size = read(connfd, fileData, sizeof(fileData));
                //write chunk of data to file
                write_size = fwrite(fileData, 1, read_size, fp);
                recv_size += read_size;
            }
            while (read_size < 0);
        }
        fclose(fp); //close file
    } 
    else 
    {
        cout << "ERROR OPENING FILE AT SERVER SIDE" << endl;
    }
}
void send_file(void *conn) 
{
    char filename[100];
    memset(&filename,0,sizeof(filename));
    strncpy(filename,send_filename,sizeof(send_filename));
    long connfd = (long) conn;
    char buffer[size];
    char file[20] = "file";
    for (int i = 0; i < 2; i++) 
    {
        if (clients[i] == connfd) 
        {
            continue;
        } 
        
        else 
        
        {
            connfd = clients[i];
            //replacing connfd with clients[i]
            write(clients[i], file, strlen(file));
            usleep(10000);
            write(clients[i],filename,strlen(filename));
            usleep(100000);
            FILE * fp;
            fp = fopen(filename, "r");
            if (fp) 
            
            {
                fseek(fp, 0, SEEK_END);
                int file_size = ftell(fp);
                fseek(fp, 0, SEEK_SET);
                int stat, read_size, x;
                //send data size
                write(clients[i], &file_size, sizeof(int));
                usleep(100000);
                //read file into large buffer.
                memset(&buffer, 0, sizeof(buffer));
                while (!feof(fp)) 
                {
                    read_size = fread(buffer, 1, sizeof(buffer) - 1, fp);
                    stat = write(clients[i], buffer, read_size);
                    usleep(100);
                    memset(&buffer, 0, sizeof(buffer));
                }
            }
            fclose(fp);
        }
    }
}
void *handle_recv(void *conn) 

{
    long connfd = (long) conn;
    char buffer[1024];
    int n;
    char check[1024]; //checks if incoming message is going to be file or not
    while (true) 
    
    {
        memset(&check, 0, sizeof(check));
        //recv data to check if file is coming or not
        n = recv(connfd, (char*) &check, sizeof(check), 0);
        if (n <= 0) break;
        //file is key word sent by client in order to start sending file
        if (strcmp(check, "file") == 0) 
        
        {
            make_file(conn);
            usleep(1000);
            send_file(conn);
        } 
        else 
        
        {
            //if check does not contain key word 'file' than its a message
            cout << "[Client]: " << check << endl;
            //each client response should be send to all other clients.
            for (int i = 0; i < 2; i++) 
            {
                if (clients[i] == connfd) continue;
                else send(clients[i], (char*) &check, strlen(check), 0);
            }
        }
    }
    return NULL;
}

void *handle_send(void *conn) 

{
    char buffer[1024];
    while (true)
    
    {
        string data;
        getline(cin, data);
        // clear buffer
        memset(&buffer, 0, sizeof(buffer));
        strcpy(buffer, data.c_str());
        //send_file(conn);
        for (int i = 0; i < 2; i++)
        {
            send(clients[i], (char*) &buffer, strlen(buffer), 0);
        }
    }
}

int main(int argc, char const *argv[])

{
    int server_fd;
    intptr_t connfd;
    struct sockaddr_in address, cliaddr;
    int cli_addrlen = sizeof(address);
    char buffer[1024];
    server_fd = socket(AF_INET, SOCK_STREAM, 0); //CREATES A TCP SOCKET
    if (server_fd <= 0)
        {
            perror("SOCKET CREATION FAILED");
            exit(1);
        }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT); //BIND TO PORT 8080
        if (bind(server_fd, (struct sockaddr *) &address, sizeof(address)) < 0)
        {
            perror("BIND FAILED");
            exit(1);
        }
        if (listen(server_fd, 2) < 0)
        {
            perror("LISTEN FAILED");
            exit(1);
        }
    pthread_t send_th;
    pthread_create(&send_th, NULL, handle_send, NULL);
    int i = 0;
        while (true)
        {
            connfd = accept(server_fd, (struct sockaddr *) &cliaddr, (socklen_t*)
            &cli_addrlen);
                if (connfd < 0)
                {
                    perror("SOCKET RECVIEVE FAILURE");
                    exit(1);
                }
            char *dot_ip = inet_ntoa(cliaddr.sin_addr);
            cout << "NEW CLIENT: IP- " << dot_ip << " PORT-: " << cliaddr.sin_port << endl;
            clients[i] = connfd;
            pthread_t recv_th;
            pthread_create(&recv_th, NULL, handle_recv, (void*) connfd);
            i++;
        }
    return 0;
}
