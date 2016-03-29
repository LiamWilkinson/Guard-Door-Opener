#include "mystruct.h"

int main(int argc, char *argv[]) {
	char lex[10];
	int i;
	int chan;
	double tempWeight;
	//setup connection
	pid_t pid = atoi(argv[1]);
	chan = ConnectAttach(ND_LOCAL_NODE, pid, 1, _NTO_SIDE_CHANNEL, NULL);
	if(chan == -1){
		printf("Error: Could not attach");
		exit(1);
	}
	printf(START_PREAMBLE);
	while(1){
		scanf("%s", lex);
		input_en.In.input = -1;
		//loop codes.
		/*
		 * This was originally a simple case statement but instead it was optimised to an array that corresponds to an enum
		 * Enums in C just increment up from zero so using this we're able to just check increment i and check if the
		 * value at that array of codes that corresponds with the enum matches.
		 * */
		for (i = 0; i < MAX_CODES; i++){
			//check if input is an code
			if (strcmp (lex, codes[i]) == 0){
				//set input to corresponding enum value
				input_en.In.input = i;
			}
		}
		//This fixes a typo in the assignment, the myinputsfile actually refers to w but the command is ws
		if (strcmp(lex, "w") == 0){
			input_en.In.input = ws;
		}

		//check if input is invalid
		if (input_en.In.input == -1){
			printf("Error: Enter a valid input code.");
			continue;
		}
		//Check weight
		if (input_en.In.input == ws){
			printf("Enter weight:");
			scanf("%lf", &tempWeight);
			input_en.In.input = ws;
			input_en.In.weight = tempWeight;
		}
		//check if input is a scan
		if (input_en.In.input == ls || input_en.In.input == rs){
			printf("Enter the person_id:");
			scanf("%d", &input_en.In.person_id);
		}

		//send the message
		if (MsgSend(chan, input_en.stIn, sizeof(input_en.In), NULL, 0) == -1){
			printf("Error: Message not sent\n");
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}
