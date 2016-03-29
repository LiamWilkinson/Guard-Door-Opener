#include <stdlib.h>
#include <stdio.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define MESSAGE_LENGTH 500

int main(int argc, char *argv[]) {
	int chan;
	int rchan;
	char msg[MESSAGE_LENGTH];
	chan = ChannelCreate(0);
	if (chan == -1){
		printf("Error: Could not attach\n");
		return EXIT_FAILURE;
	}
	printf("The controller is running as process_id %d", getpid());
	fflush(stdout);
	while(1){
		rchan = MsgReceive(chan, msg, MESSAGE_LENGTH, NULL);
		if (rchan == -1){
			printf("Error: Message not received\n");
			return EXIT_FAILURE;
		}
		printf("[status update: %s]\n", msg);
		fflush(stdout);
		if (MsgReply(rchan, EOK, NULL, 0) == -1){
			printf("Error: Message not sent\n");
			return EXIT_FAILURE;
		}
	}
	if (ChannelDestroy(chan) == -1){
		printf("Error: Couldn't destroy channel\n");
	}
	return EXIT_SUCCESS;
}
