#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h>

#define MSG_SZ 256

struct client {
	int id;
	char *name;
};
struct shared_board {
	int has_msg;
	int client_count;
	char msg[MSG_SZ];
	struct client clients[];
};

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
	char buffer[MSG_SZ];

	struct client _client = {pid, "hamza"};
	memcpy(msg_board->clients, &_client, sizeof _client);

	msg_board->client_count += 1;

	while(running){		
		printf("Enter some text: ");
		fgets(buffer, MSG_SZ, stdin);
		
		// if(strncmp(buffer, "end", 3) == 0) running = 0;

		// if (msg_board->has_msg) {
		// 	printf("Message board not empty!\n");
		// 	continue;
		// }

		// sprintf(msg, "%d-%s", pid, buffer); // concatenating msg info

		// strncpy(msg_board->msg, msg, strlen(msg));

		// msg_board->has_msg = 1;
	}

	exit(EXIT_SUCCESS);
}