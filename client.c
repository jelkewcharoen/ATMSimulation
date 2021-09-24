/*///////////////////////////////////////////////////////////
*
* FILE:		client.c
* AUTHOR:	Panalee Kewcharoen
* PROJECT:	CS 3251 Project 1 - Professor Jun Xu 
* DESCRIPTION:	Network Client Code
* CREDIT:	Adapted from Professor Traynor
*
*////////////////////////////////////////////////////////////

/* Included libraries */

#include <stdio.h>		    /* for printf() and fprintf() */
#include <sys/socket.h>		    /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>		    /* for sockaddr_in and inet_addr() */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Constants */
#define RCVBUFSIZE 512		    /* The receive buffer size */
#define SNDBUFSIZE 512		    /* The send buffer size */
#define REPLYLEN 32

/* The main function */
int main(int argc, char *argv[])
{

    int clientSock;		    /* socket descriptor */
    struct sockaddr_in serv_addr;   /* server address structure */

    char *accountName;		    /* Account Name  */
    char *servIP;		    /* Server IP address  */
    unsigned short servPort;	    /* Server Port number */
    char *account2;			//second account for trasnfer function
    

    char sndBuf[SNDBUFSIZE];	    /* Send Buffer */
    char rcvBuf[RCVBUFSIZE];	    /* Receive Buffer */
    
    int balance;		    /* Account balance */

    /* Get the Account Name from the command line */

	if (argc < 5 || argc > 7) {
		printf("Invalid arguments.");
		exit(1);
	}
	accountName = argv[1];
	
    	memset(&sndBuf, 0, SNDBUFSIZE);
    	memset(&rcvBuf, 0, RCVBUFSIZE);

    /* Get the addditional parameters from the command line */
    /*	    FILL IN	*/

	servIP = argv[2];
	servPort = atoi(argv[3]);
	
	char *temp = "////";	//a slash to split strings
	strcat(sndBuf, accountName);
	
	if (argc == 6 && strcmp(argv[4],"WITHDRAW") == 0) { //WITHDRAW
		strcat(sndBuf, temp);
		strcat(sndBuf, argv[5]);
    	} else if (argc == 7 && strcmp(argv[4],"TRANSFER") == 0) {//TRANSFER
		strcat(sndBuf, temp);
		strcat(sndBuf, argv[5]);
		strcat(sndBuf, temp);
		strcat(sndBuf, argv[6]);
    	} else if (argc != 5 || strcmp(argv[4],"BAL") != 0){ //BAL
		printf("Invalid arguments.");
		exit(1);
    	}

    /* Create a new TCP socket*/
    /*	    FILL IN	*/

	if ((clientSock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error creating a new TCP socket.");
		exit(1);
	}

    /* Construct the server address structure */
    /*	    FILL IN	 */

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(servIP);
	serv_addr.sin_port = htons(servPort);

    /* Establish connection to the server */
    /*	    FILL IN	 */

	int connectVal = connect(clientSock, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
	if (connectVal < 0) {
		perror("error\n");
		printf("Error: fail to establish a connection to the server");
		exit(1);
	}
    	
    /* Send the string to the server */
    /*	    FILL IN	 */

	int len = strlen(sndBuf);
	int bytes = send(clientSock, sndBuf, len, 0);
	if (bytes < 0) {
		printf("Error: fail to send the string to the server.");
		exit(1);
	} else if (bytes != len) {
		printf("Error: send() sent unexpected number of bytes.");
		exit(1);
	}

    /* Receive and print response from the server */
    /*	    FILL IN	 */

	bytes = recv(clientSock, rcvBuf, RCVBUFSIZE-1, 0);
	if (bytes < 0) {
		printf("Error: fail to receive the response from the server.");
		exit(1);
	} else if (bytes == 0) {
		printf("Error: receive() connection closed prematurely.");
		exit(1);
	}
	rcvBuf[bytes] = '\0'; 	//null terminate
	balance = atoi(rcvBuf);

    printf("%s\n", accountName);
    printf("Balance is: %i\n", balance);

    return 0;
}

