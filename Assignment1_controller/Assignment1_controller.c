#include "mystruct.h"

/*These all need to be globals because message passing between states gets very annoying fast.
It would also be dumb to have to pass all the messages information back and forth
through the stack, and it would essentially increase all the complexity that was intended to be decreased by
putting it in a function that just runs the code*/
person_record* records[MAX_RECORDS] = {0};
StateFunc statefunc = state1;
person_record sessionRecord;
/*This array contains the message being sent to the display. Each state simply strcpys a message into this array
 *That message is then sent out to the display using message passing.
 * */
char st_msg[MESSAGE_LENGTH];
//Channel for input
int input_chan;
//Channel for the display
int display_chan;
pid_t display_pid;
//Replys
int rchan;

void setupConnections(){
	//Connection is setup here
	input_chan = ChannelCreate(0);
	if (input_chan == -1){
		printf("Error: Could not attach \n");
		exit(EXIT_FAILURE);
	}
	printf("The display is running as process_id %d", getpid());
	fflush(stdout);
	display_chan = ConnectAttach(ND_LOCAL_NODE, display_pid, 1, _NTO_SIDE_CHANNEL, NULL);
	if(display_chan == -1){
		printf("Error: Could not attach display\n");
		exit(EXIT_FAILURE);
	}
}

int main(int argc, char *argv[]) {
	//Need to assign this immediately so we can run setupConnection without it crashing
	display_pid = atoi(argv[1]);
	setupConnections();

	//Loops forever
	while(1){
		//Receive from input
		rchan = MsgReceive(input_chan, input_en.stIn , sizeof(input_en.In), NULL);
		if (rchan == -1){
			printf("Error: Message not received\n");
			cleanup();
			return EXIT_FAILURE;
		}
		//This runs the function pointer state machine
		statefunc = (StateFunc)(*statefunc)();

		//produce output
		if (MsgSend(display_chan, st_msg, MESSAGE_LENGTH, NULL, 0) == -1){
			printf("Error: Message not sent\n");
			cleanup();
			return EXIT_FAILURE;
		}

		//reply to input
		if (MsgReply(rchan, EOK, NULL, 0) == -1){
			printf("Error: Message not sent\n");
			cleanup();
			return EXIT_FAILURE;
		}
	}
	cleanup();
	return EXIT_SUCCESS;
}

void cleanup(){
	int i;
	//This just frees all the records so there aren't memory leaks
	for (i = 0; i < MAX_RECORDS; i++){
		free(records[i]);
	}
}

//Simple general error state
void* errorState(){
	return state0;
}

//System startup
void* state0(){
	strcpy(st_msg, "System Started. Left Door: Locked, Right Door: Locked. System waiting for input\n");
	return state1;
}

//System idles until someone wants to get in or out
void* state1(){
	int i;
	//Checks if record exists
	for (i = 0; i < MAX_RECORDS; i++){
		if (records[i] != NULL && records[i]->person_id == sessionRecord.person_id){
			strcpy(st_msg, "Error: id already exists.");
			return errorState;
		}
	}
	//Checks token and goes to either of the next states
	switch(input_en.In.input){
		case rs:
			strcpy(st_msg, "Right id scanned");
			return state12;
			break;
		case ls:
			strcpy(st_msg, "Left id scanned");
			return state3;
			break;
		default:
			strcpy(st_msg, "Scan from right of left. System waiting for valid input.");
			return state1;
			break;
	}
	return state1;
}

void* state3(){
	if (input_en.In.input == glo){
		strcpy(st_msg, "Guard opened left door");
		return state4;
	} else {
		return errorState;
	}
}

void* state4(){
	if (input_en.In.input == lo){
		strcpy(st_msg, "Left door open");
		return state5;
	} else {
		return errorState;
	}
}

void* state5(){
	int i;
	if (input_en.In.input == ws){
		strcpy(st_msg, "Weight inputed, saving session record.");
		for (i = 0; i < MAX_RECORDS; i++){
			if (records[i] != NULL && records[i]->person_id == sessionRecord.person_id){
				strcpy(st_msg, "Error: id already exists.");
				return errorState;
			}
			//Creates new record
			if (records[i] == NULL){
				records[i] = (person_record*)malloc(sizeof(person_record));
				*records[i] = sessionRecord;
				return state6;
			}
			strcpy(st_msg, "Error: out of record space, please fire some employees");
			return errorState
		}
	}
	return errorState;
}

void* state6(){
	if (input_en.In.input == lc){
		strcpy(st_msg, "Left door closed");
		return state7;
	} else {
		return errorState;
	}
}

void* state7(){
	if (input_en.In.input == glc){
		strcpy(st_msg, "Guard left door closed");
		return state8;
	} else {
		return errorState;
	}
}

void* state8(){
	if (input_en.In.input == gro){
		strcpy(st_msg, "Guard right door open");
		return state9;
	} else {
		return errorState;
	}
}

void* state9(){
	if (input_en.In.input == ro){
		strcpy(st_msg, "Right door open");
		return state10;
	} else {
		return errorState;
	}
}

void* state10(){
	if (input_en.In.input == rc){
		strcpy(st_msg, "Right door closed");
		return state11;
	} else {
		return errorState;
	}
}

void* state11(){
	if (input_en.In.input == grc){
		strcpy(st_msg, "Guard right door closed");
		return state12;
	} else {
		return errorState;
	}
}

void* state12(){
	int i;
	for (i = 0; i < MAX_RECORDS; i++){
		if (records[i] != NULL  && records[i]->person_id == sessionRecord.person_id){
			sessionRecord = *records[i];
			strcpy(st_msg, "Record loaded, valid id");
			return state13;
		}
	}
	strcpy(st_msg, "Incorrect card id");
	return errorState;
}

void* state13(){
	if (input_en.In.input == gro){
		strcpy(st_msg, "Guard opens right open");
		return state14;
	} else {
		return errorState;
	}
}

void* state14(){
	if (input_en.In.input == ro){
		strcpy(st_msg, "Right door open");
		return state15;
	} else {
		return errorState;
	}
}

void* state15(){
	if (input_en.In.input == ws){
		if (sessionRecord.weight > (input_en.In.weight - WEIGHT_VARIANCE) && sessionRecord.weight < (input_en.In.weight + WEIGHT_VARIANCE)){
		}
		strcpy(st_msg, "Weight in range, verified for exit");
		return state16;
		strcpy(st_msg, "Weight doesn't match record");
		return errorState;
	} else {
		return errorState;
	}
}

void* state16(){
	if (input_en.In.input == rc){
		strcpy(st_msg, "Right door closed");
		return state17;
	} else {
		return errorState;
	}
}

void* state17(){
	if (input_en.In.input == grc){
		strcpy(st_msg, "Guard right door closed");
		return state18;
	} else {
		return errorState;
	}
}

void* state18(){
	if (input_en.In.input == glo){
		strcpy(st_msg, "Guard left door open");
		return state19;
	} else {
		return errorState;
	}
}

void* state19(){
	if (input_en.In.input == lo){
		strcpy(st_msg, "Left door open");
		return state20;
	} else {
		return errorState;
	}
}

void* state20(){
	if (input_en.In.input == lc){
		strcpy(st_msg, "Left door closed");
		return state21;
	} else {
		return errorState;
	}
}

void* state21(){
	if (input_en.In.input == glc){
		strcpy(st_msg, "Guard left door closed");
		return state1;
	} else {
		return errorState;
	}
}
