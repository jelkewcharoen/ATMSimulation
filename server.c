/*///////////////////////////////////////////////////////////
*
* FILE:		server.c
* AUTHOR:	Panalee
* PROJECT:	CS 3251 Project 1 - Professor Jun Xu
* DESCRIPTION:	Network Server Code
* CREDIT:	Adapted from Professor Traynor
*
*////////////////////////////////////////////////////////////

/*Included libraries*/

#include <stdio.h>	  /* for printf() and fprintf() */
#include <sys/socket.h>	  /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>	  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>	  /* supports all sorts of functionality */
#include <unistd.h>	  /* for close() */
#include <string.h>	  /* support any string ops */
#include <time.h>

#define RCVBUFSIZE 512		/* The receive buffer size */
#define SNDBUFSIZE 512		/* The send buffer size */
#define BUFSIZE 40		/* Your name can be as many as 40 chars*/

    struct acc {
	char* name;
	int bal;
    };				//account info

    typedef struct tnode {
	time_t time;
	struct tnode *next;
    }node;			//timer node

    typedef struct tlist {
	node* head;
	int size;
    }list;			//list of timers
	

/* The main function */
int main(int argc, char *argv[])
{
    
    int serverSock;				/* Server Socket */
    int clientSock;				/* Client Socket */
    struct sockaddr_in changeServAddr;		/* Local address */
    struct sockaddr_in changeClntAddr;		/* Client address */
    unsigned short changeServPort;		/* Server port */
    unsigned int clntLen = sizeof(changeClntAddr);			/* Length of address data struct */

    char nameBuf[BUFSIZE];			/* Buff to store account name from client */
    char *token;
    char *arg[3];			
    int amount;
    int  balance = -1;				/* Place to record account balance result */
    int newBal;					//store new balance after a transaction

    if (argc != 3) 
    {
	printf("Incorrect number of arguments. The correct format is:\n\tIP Port\n ");
	exit(1);
    }
	//pre-configure the accounts and balances
	struct acc arr[5] = {
		{"mySavings", 1000},{"myChecking", 2000},{"myCD",3000},{"my401k",4000},{"my529",5000}
	};

    /* Create new TCP Socket for incoming requests*/
    /*	    FILL IN	*/
	if ((serverSock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Error creating new TCP socket.");
		exit(1);
	}

    /* Construct local address structure*/
    /*	    FILL IN	*/
	changeServPort = atoi(argv[2]);
	memset(&changeServAddr, 0, sizeof(changeServAddr));
	changeServAddr.sin_family = AF_INET;
	changeServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	changeServAddr.sin_port = htons(changeServPort);
    
    /* Bind to local address structure */
    /*	    FILL IN	*/
	if (bind(serverSock, (struct sockaddr*) &changeServAddr,sizeof(changeServAddr)) < 0) {
		printf("Error: bind() failed.");
		exit(1);
	}

    /* Listen for incoming connections */
    /*	    FILL IN	*/
	if (listen(serverSock, 5) < 0) {
		printf("Error: listen() failed.");
		exit(1);
	}

	list* arrList[5]; //array to keep the timer linked lists
	int a;
	for (a = 0; a<5; a++) {
		arrList[a] = (list*) malloc(sizeof(list));
		arrList[a]->size = 0;
		arrList[a]->head = (node*) malloc(sizeof(node));
		arrList[a]->head->time = time(0);
	}

    /* Loop server forever*/
    while(1)
    {
	/* Accept incoming connection */
	/*	FILL IN	    */
		clientSock = accept(serverSock,(struct sockaddr*) &changeClntAddr, &clntLen);
		if (clientSock < 0) {
			printf("Error: accept() failed.");
			exit(1);
		}
	/* Extract the account name from the packet, store in nameBuf */
		int bytes = recv(clientSock, nameBuf, BUFSIZE-1, 0);
		if (bytes < 0) {
			printf("Error: recv() from the client failed.");
			exit(1);
		} else if (bytes == 0) {
			printf("Error: receive() connection closed prematurely.");
			exit(1);
		}
		nameBuf[bytes] = '\0'; //null terminate string

		char *temp = "////";
		token = strtok(nameBuf, temp); //accountName
		arg[0] = token;
		int index = 1;
		while( token!= NULL ) {
			token = strtok(NULL,temp); //function
			if (token != NULL)
				arg[index++] = token;			
		}

	/* Look up account balance, store in balance */
	/*	FILL IN	    */
		
		
		int i=0; //index of the first account
		for (i;i<5;i++) {
			if (strcmp(arg[0], arr[i].name) == 0){ //if the name match
				balance = arr[i].bal;
				break;
			}
		}
		if (i == 5 && balance == -1) {
			printf("Account name does not exist.");
			exit(1);
		}

		if (index == 2) { //WITHDRAW
			amount = atoi(arg[1]);
    			if (balance < amount) {
				printf("Error: Insufficient Funds!.\n");
				exit(1);
			} else {
				node* head = arrList[i]->head; //the list of this account
				node* current = head;
		
				if (arrList[i]->size > 2 && (time(0) - head->time < 60)) {	//if there are already 3 transactions in the list, then check if another one would timeout
					printf("Error: too many withdrawals in a minute.\n");
				} else {
					if (arrList[i]->size > 2) {			//if there are already 3 transactions and another one wouldn't timeout, move up the list by removing the head
						arrList[i]->head = current->next;
						(arrList[i]->size)--;
					}
					if (current == NULL) {				//if head is null, create the head
						arrList[i]->head = (node*)malloc(sizeof(struct tnode));
						current = arrList[i]->head;
					} else {
						while(current->next != NULL)
							current = current->next;
						current->next = (node*)malloc(sizeof(struct tnode));	//create the latest node
						current = current->next;
					}
					current->time = time(0);
					(arrList[i]->size)++;
					printf("Withdraw Completed.\n");
					arr[i].bal -= amount;
					balance -= amount;
				} 
			}

    		} else if (index == 3) { //TRANSFER
			amount = atoi(arg[2]);
			int j=0; //index of the second account
			for (j;j<5;j++) {
				if (strcmp(arg[1], arr[j].name) == 0){ //if the name match
					break;
				}
			}
			if (j == 5) {
				printf("This account name does not exist.");
				exit(1);
			}
			if (balance < amount) {
				printf("Error: Insufficient Funds!.\n");
				exit(1);
			} else {
				printf("Transfer Completed.\n");
				arr[i].bal -= amount;
				balance -= amount;
				arr[j].bal += amount;
			}
    		}
	/* Return account balance to client */
	/*	FILL IN	    */
		/*convert int to string*/
		int len = snprintf(NULL, 0, "%d", balance);
		char* balStr = malloc(len+1);
		snprintf(balStr, len+1, "%d", balance);
		bytes = send(clientSock, balStr, strlen(balStr), 0);

		if (bytes < 0) {
			printf("Error: fail to send the balance to the client.");
			exit(1);
		} else if (bytes != strlen(balStr)) {
			printf("Error: send() sent unexpected number of bytes.");
			exit(1);
		}

    } //end while
return 0;
}
	

