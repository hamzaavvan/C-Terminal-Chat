#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>

#define MSG_SZ 256

struct client {
	int id;
	char name[10];
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

void main() 
{
	void *shm = (void *)0;
	struct shared_board *msg_board;
	int shmid;

	srand((unsigned int)getpid());
	shmid = shmget((key_t)1337, sizeof(struct shared_board), 0666 | IPC_CREAT);

	shm = shmat(shmid, (void *)0, 0);

	msg_board = (struct shared_board *)shm;

	msg_board->client_count = 0;
	int *client_count = &msg_board->client_count;

	while (1) {
		if (msg_board->has_msg) {
			printf("Server Log:\n Msg recieved: %s\n", msg_board->msg);
			msg_board->has_msg = 0;
			
			delay(4);
		}

		if (*client_count >0) {
			/* Client management code here */

			// printf("Name: %s\n", msg_board->clients[0].name);
			delay(4);
		}
	}

	exit(EXIT_SUCCESS);
}