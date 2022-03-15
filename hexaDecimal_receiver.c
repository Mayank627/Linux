//Receiver
//Same as the sender
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/msg.h>

struct my_msg_st {
	long int my_msg_type;
	char some_text[BUFSIZ];
};

int main()
{
	int running = 1;
	int msgid;
	struct my_msg_st some_data;
	long int msg_to_receive = 16;

	msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
	
	if (msgid == -1) {
		fprintf(stderr, "msgget failed with error: %d\n", errno);
		exit(EXIT_FAILURE);
	}

	
	while(running) {

		//we receive a msg with a msgid,address and size of the message
		if (msgrcv(msgid, (void *)&some_data, BUFSIZ, msg_to_receive, 0) == -1) {
			fprintf(stderr, "msgrcv failed with error: %d\n", errno);
			exit(EXIT_FAILURE);
		}
		
        //Decimal to binary
		if (strncmp(some_data.some_text, "end", 3) == 0) { // type -1 to terminate the program
			running = 0;
			printf("Program Terminated\n");
			break;
		}
		

		printf("Decimal to Hexadecimal : %s\n",some_data.some_text);
	}

	//Delete id,when the work is over
	//RMID means remove id
	if (msgctl(msgid, IPC_RMID, 0) == -1) {
		fprintf(stderr, "msgctl(IPC_RMID) failed\n");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
	
}
