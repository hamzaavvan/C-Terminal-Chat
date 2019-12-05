#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>
#include <semaphore.h>

#define MSG_SZ 256
#define NM_SZ 10
#define CLID_LN 10

struct client {
	int id;
	int active;
	char name[NM_SZ];
};
struct shared_board {
	int has_msg;
	int client_count;
	char msg[MSG_SZ];
	struct client clients[];
};

void delay(int time) {
	sleep(rand()%time);
}

void list_clients(struct shared_board *msg_board, int *client_count, int *refresh_clients, int pid) {
	struct client *clients = msg_board->clients;

	printf("Available Clients:\n");

	for (int i = 0; i < *client_count; i++)
	{
		if (clients[i].id == pid || clients[i].active == 0) continue;
		
		printf("Id: %d, Name: %s\n", clients[i].id, clients[i].name);
	}

	*refresh_clients = 0;
}

void getname(struct client *_client) {
	printf("Enter name: ");
	fgets(_client->name, NM_SZ, stdin);
	
	_client->name[strlen(_client->name)-1] = '\0'; // removing linebreak
}

void register_client(struct shared_board *msg_board, struct client _client) {
	msg_board->clients[msg_board->client_count] = _client;

}

void main() 
{
	void *shm = (void *)0;
	struct shared_board *msg_board;
	int shmid, pid = getpid(), running = 1;
    char *quit[16];
	char *list[16];

	srand((unsigned int)getpid());
	shmid = shmget((key_t)1337, sizeof(struct shared_board), 0666);

	if(shmid == -1){
		fprintf(stderr, "shmget failed: Error creating shm\n");
		exit(EXIT_FAILURE);
	}

	shm = shmat(shmid, (void *)0, 0);
	msg_board = (struct shared_board *)shm;

	char text[MSG_SZ];

	struct client _client = {pid, 1};
	getname(&_client);
	register_client(msg_board, _client);



	while(running) {
		if (*client_count < 2) {
			show_searching_msg(&intervals);
			continue;
		}


		fgets(text, MSG_SZ, stdin);
		
		if(strncmp(text, "end", 3) == 0) break;
         if(strncmp(text,"@Quit",5)==0)
             { 
	            quit[0]='/0';
	            sem_wait(msg_board->msg); 
	            strcat(quit,"Quit----");
                strcat(quit,client->name); 	
                strncpy(msg_board->msg, quit, 16);
                 sem_signal(msg_board->msg); 
                exit(1);
		     }
        if(strncmp(text,"@List",5)==0)
             {  
	            list[0]='/0';
                sem_wait(msg_board->msg);
	            strcat(list,"List----");
                strcat(list,client->name); 	
                strncpy(msg_board->msg, quit, 16);
                sem_signal(shared_stuff->msg); 
		     }
		strncpy(msg_board->msg, text,MSG_SZ);
		sem_signal(msg_board->msg); 
		running=0;

		if (msg_board->has_msg) {
			printf("Message board not empty, wait until it gets free!\n");
			continue;
		}

		// send message if message board is free/empty
		send_msg(msg_board, pid, to, text);
	}

	exit(EXIT_SUCCESS);
}
