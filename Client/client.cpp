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
#define MAX_SIZE 10240
using namespace std;
void send_file(void *conn) 

{
    long sock = (long) conn;
    char file[20] = "file";
    char data_buffer[1024];
    char buffer[MAX_SIZE];
    memset(&data_buffer, 0, sizeof(data_buffer));
    char filename[100];
    send(sock, file, strlen(file), 0);
    cout << "ENTE FILE NAME:" << endl;
    memset(&filename, 0, sizeof(filename));
    scanf("%s", filename);
    write(sock,(void*)&filename,sizeof(filename));
    FILE * fp;
    fp = fopen(filename, "r");
    if (fp) //if successful open
    {
        fseek(fp, 0, SEEK_END);
        int file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        int stat, read_size, x;
        //send data size
        write(sock, (void*) &file_size, sizeof(int));
        //read file into large buffer.
        memset(&buffer, 0, sizeof(buffer));
        while (!feof(fp))
        {
            read_size = fread(buffer, 1, sizeof(buffer) - 1, fp);
            do 
            
            {
                stat = write(sock, buffer, read_size);
            }
            while (stat < 0);
            memset(&buffer, 0, sizeof(buffer));
        }
    }
    else
    {
        cout << "CAN NOT OPEN FILE" << endl;
    }
    fclose(fp);
}


void recv_file(void *conn)
{
    char filename[100];
    memset(&filename,0,sizeof(filename));
    long connfd = (long) conn;
    //get file name:
    read(connfd,filename,sizeof(filename));
    cout<<"RECIEVED FILE : "<<filename<<endl;
    char fileData[size]; //buffer to store incoming file data.
    //clear buffers
    memset(&fileData, 0, sizeof(fileData));
    //declaring variables
    int file_size = 0, read_size = 0, recv_size = 0, write_size = 0, current_stat = 0;
    //get size of incoming file

    current_stat = read(connfd, (int*) &file_size, sizeof(int));
    usleep(1000);
    
    //creating handle for file
    FILE * fp;
    fp = fopen(filename, "w"); //open file in write/read mode
    if (fp) //successful opening of file
    {
        while (recv_size < file_size)
        {
            do 
            
            { //read a chunk of data
                read_size = read(connfd, fileData, sizeof(fileData));
                usleep(100);
//write chunk of data to file
                write_size = fwrite(fileData, 1, read_size, fp);
                recv_size += read_size;
            }
            while (read_size <= 0);
        }
        fclose(fp); //close file
    }
    else
    {
        cout << "ERROR OPENING FILE AT CLIENT SIDE" << endl;
    }
    
}


void *handle_recv(void *conn)
{
    long connfd = (long) conn;
    int n;
    char check[1024];
    
    while (true)
    {
        memset(&check, 0, sizeof(check));
        n = recv(connfd, (char*) &check, sizeof(check), 0);
        if (n <= 0) break;
        usleep(1000);
        check[n] = '\0';
        if (strncmp(check, "file", 4) == 0)
        {
            recv_file((void*) conn);
        }
        else
        {
// print server response
            cout << "[SERVER]: " << check << endl;
        }
    }
    return NULL;
}


int main(int argc, char const *argv[])


{
    intptr_t sock;
    struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    
    {
        perror("SOCKET CREATION FAILED\n");
        exit(1);
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_aton("127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        printf("INVALID ADDRESS\n");
        exit(1);
    }
    if (connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("CONNECTION FAILED\n");
        exit(1);
    }
    pthread_t th;
    pthread_create(&th, NULL, handle_recv, (void*) sock);
    cout << ">>>>" << endl;
    int choice = 1;
    while (choice != 3)
    {
        int num;
        cout<< "***Welcome to the chat***"<<endl;
        cout<<"ENTER:"<<endl;
        cout<<"1: SEND FILE"<<endl;
        cout<<"2: SEND MESSAGE "<<endl;
        cout<<"3: EXIT "<<endl;
        cin >> num;
        choice = num;
        
        switch (num)
        
        {
            case 1:
                send_file((void*) sock);
                break;
            case 2:
            {
                char buffer[1024];
                cout << "ENTER MESSAGE: " << endl;
                string data;
                //getline(cin, data,'\0');
                cin >> data;
                memset(&buffer, 0, sizeof(buffer));
                strcpy(buffer, data.c_str());
                send(sock, (char*) &buffer, strlen(buffer), 0);
                break;
            }
            case 3:
                cout<<"EXITING"<<endl;
                break;
        }
    }
    return 0;
}
