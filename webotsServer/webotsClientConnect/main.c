/*
 * File:         client.c
 * Date:         October, 2003
 * Description:  A simple client program to connect to the TCP/IP server
 * Author:       Darren Smith
 * Modifications:
 *
 * Copyright (c) 2006 Cyberbotics - www.cyberbotics.com
 */

/*
 * Linux:   compile with gcc -Wall client.c -o client
 * Windows: compile with gcc -Wall -mno-cygwin client.c -o client -lws2_32
 */

#include <stdio.h>
#include <string.h>




#ifdef WIN32
#include <winsock.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#endif

#define SOCKET_PORT 1510
#define SOCKET_SERVER "130.160.47.134"   /* local host */

char result[256];
char buffer[256];
char holder[256];
FILE *fp;



int readFile(void)
{


    int c;
    int i = 0;

    char result[256];
    //char result[3];

     // error check this!

    // this while-statement assigns into c, and then checks against EOF:
/*
    while((c = fgetc(fp)) != EOF && (i < 5)) {
        //putchar(c);
        printf("%d\n",i);
        result[i++] = c;
        //printf("Result: %s \n", result);
    }
    */



    printf("exit \n");


    return 0;
}






int main(int argc, char *argv[])
{
    printf("here");
    struct sockaddr_in address;
    struct hostent *server;
    int fd, rc, n;
    char buffer[256];
    fp = fopen("path.txt", "r");


    /* create the socket */

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        printf("cannot create socket\n");

        return -1;
    }

    /* fill in the socket address */
    memset(&address, 0, sizeof(struct sockaddr_in));
    address.sin_family = AF_INET;
    address.sin_port = htons(SOCKET_PORT);
    server = gethostbyname(SOCKET_SERVER);

    if (server) {
        memcpy((char *) &address.sin_addr.s_addr, (char *) server->h_addr,
               server->h_length);
    } else {
        printf("cannot resolve server name: %s\n", SOCKET_SERVER);

        return -1;
    }

    /* connect to the server */
    rc = connect(fd, (struct sockaddr *) &address, sizeof(struct sockaddr));
    if (rc == -1) {
        printf("cannot connect to the server\n");

        return -1;
    }

    for (;;) {



       // readFile();
        //printf("Enter command: ");
        //scanf("%255s", holder);
        strcpy(buffer,result);
       // printf("Buffer: %s \n", buffer);
        n = strlen(buffer);
        buffer[0] = 1;
      //  buffer[n++] = '\n';     /* append carriage return */
      //  buffer[n] = '\0';
        n = send(fd, buffer, n, 0);

        if (strncmp(buffer, "exit", 4) == 0) {
            break;
        }

      //  n = recv(fd, buffer, 256, 0);
        //buffer[n] = '\0';
        //printf("> %s", buffer);
        //memset(result,'\0',256);
    }

    close(fd);
    fclose(fp);



    return 0;
}
