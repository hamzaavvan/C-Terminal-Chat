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
		if (clients[i].id == pid) continue;
		
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
	msg_board->client_count++;
}

void send_msg(struct shared_board *msg_board, int pid, int to, char text[MSG_SZ]) 
{
	char msg[MSG_SZ];
	
	sprintf(msg, "%d-%d-%s", to, pid, text); // concatenating msg info
	strncpy(msg_board->msg, msg, strlen(msg)-1);
	printf("Message sent!\n");

	msg_board->has_msg = 1;
	delay(2);
}

int read_msg(struct shared_board *msg_board, int pid)
{
	char spid[CLID_LN];
	sprintf(spid, "%d", pid);

	if (strncmp(msg_board->msg, spid, strlen(spid)) == 0)
	{
		/* Manage message handling here */
		printf("You got msg: %s\n", msg_board->msg);
		msg_board->msg[0] = '\0';

		delay(2);
		return 1;
	}

	return 0;
}

void check_for_msg(struct shared_board *msg_board, int *check_msg, int pid) 
{
	int msg_for_client = 0;

	if (msg_board->msg)
		msg_for_client = read_msg(msg_board, pid);

	if (!msg_for_client)
		printf("No new message !\n");

	*check_msg = 0;
}

void show_searching_msg(int *intervals)
{
	// char padd[10];
	// sprintf(padd, "%s%d%s", "|%-", (*(intervals+1)-1), "s|");

	// if (*intervals/3 ==1) {
	// 	printf("\b\b\b");
	// 	*intervals = 0;
	// } 

	// if (*intervals == 0) {
		printf("Searching");
	// } else {
	// 	for (int i = 0; i < *intervals; i++)
	// 	{
	// 		printf(".");
	// 	}
	// }

	// *intervals+= 1;
	delay(4);
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

	struct client _client = {pid};
	getname(&_client);
	register_client(msg_board, _client);

	int *client_count = &msg_board->client_count, intervals = 0,
		check_msg = 0, refresh_clients = 0, action = 0, to = 0, initial_list = 1;

	while(running) {
		if (*client_count < 2) {
			show_searching_msg(&intervals);
			continue;
		}

		if (check_msg) 
			check_for_msg(msg_board, &check_msg, pid);

		if (refresh_clients || initial_list){
			list_clients(msg_board, client_count, &refresh_clients, pid);
			initial_list = 0;
		}

		printf("\nChoose actions:\n1) Check For Messsage\n2) Send Message \n3) Refresh Clients List\nEnter #: ");
		scanf("%i", &action);

		switch (action) {
			case 1:
				check_msg = 1;
				continue;
			case 3:
				refresh_clients = 1;
				continue;
		}
		
		printf("Enter client id: ");
		scanf("%d", &to);
		
		setbuf(stdin, NULL);
		printf("Send message: ");
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
                sem_wait(shared_stuff->msg);
	            strcat(list,"List----");
                strcat(list,client->name); 	
                strncpy(msg_board->msg, quit, 16);
                sem_signal(shared_stuff->msg); 
		     }

		if (msg_board->has_msg) {
			printf("Message board not empty, wait until it gets free!\n");
			continue;
		}

		// send message if message board is free/empty
		send_msg(msg_board, pid, to, text);
	}

	exit(EXIT_SUCCESS);
}