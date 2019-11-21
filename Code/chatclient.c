#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>

#define MSG_SZ 256
#define NM_SZ 10

struct client {
	int id;
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

void list_clients(struct shared_board *msg_board, int *client_count, int pid) {
	printf("Available Clients:\n");
	struct client *clients = msg_board->clients;

	for (int i = 0; i < *client_count; i++)
	{
		if (clients[i].id == pid) continue;
		
		printf("Id: %d, Name: %s\n", clients[i].id, clients[i].name);
	}
}

void getname(struct client *_client) {
	printf("Enter name: ");
	fgets(_client->name, NM_SZ, stdin);
	
	_client->name[strlen(_client->name)-1] = '\0'; // removing linebreak
}

void register_client(struct shared_board *msg_board, struct client _client) {
	msg_board->clients[msg_board->client_count] = _client;
	msg_board->client_count += 1;
}

void main() 
{
	void *shm = (void *)0;
	struct shared_board *msg_board;
	int shmid, pid = getpid(), running = 1;

	srand((unsigned int)getpid());
	shmid = shmget((key_t)1337, sizeof(struct shared_board), 0666);

	if(shmid == -1){
		fprintf(stderr, "shmget failed: Error creating shm\n");
		exit(EXIT_FAILURE);
	}

	shm = shmat(shmid, (void *)0, 0);
	msg_board = (struct shared_board *)shm;

	char msg[MSG_SZ];
	char text[MSG_SZ];

	struct client _client = {pid};
	getname(&_client);
	register_client(msg_board, _client);

	int *client_count = &msg_board->client_count;

	while(running) {
		if (*client_count < 2) {
			printf("Searching...\n");
			delay(4);
			continue;
		}

		list_clients(msg_board, client_count, pid);
		printf("Enter some text: ");
		fgets(text, MSG_SZ, stdin);
		
		if(strncmp(text, "end", 3) == 0) break;

		if (msg_board->has_msg) {
			printf("Message board not empty!\n");
			continue;
		}

		sprintf(msg, "%d-%s", pid, text); // concatenating msg info
		strncpy(msg_board->msg, msg, strlen(msg)-1);

		msg_board->has_msg = 1;
	}

	exit(EXIT_SUCCESS);
}