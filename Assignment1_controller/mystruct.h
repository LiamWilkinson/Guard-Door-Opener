#ifndef MYSTRUCT_H_
#define MYSTRUCT_H_

#include <stdlib.h>
#include <stdio.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <fcntl.h>

#define MAX_CODES 11
char *codes[MAX_CODES] = {"ls", "rs", "ws", "lo", "ro", "lc", "rc", "gro", "grc", "glc", "glo"};

#define START_PREAMBLE "Enter the device (ls= left scan, rs= right scan, ws= weight scale, lo =left open, ro=right open, lc = left closed, rc = right closed , gro = guard right open, grc = guard right closed, glc=guard left closed, glo = guard left open)"
#define WEIGHT_VARIANCE 2
#define MAX_RECORDS 100
#define MESSAGE_LENGTH 500

typedef struct {
	int person_id;
	double weight;
} person_record;

typedef struct {
	int input;
	double weight;
	int person_id;
} input_state;

union {
	input_state In;
	char stIn[sizeof(input_state)];
} input_en;

enum {
	ls, //Left scan
	rs, //Right scan
	ws, //Weight scale
	lo, //Left open
	ro, //Right open
	lc, //Left closed
	rc, //Right closed
	gro, //Guard right open
	grc, //Guard right closed
	glc, //Guard left closed
	glo //Guard left open
}intypes;



//States
typedef void *(*StateFunc)();

void* state0();
void* state1();
//void* state2();
void* state3();
void* state4();
void* state5();
void* state6();
void* state7();
void* state8();
void* state9();
void* state10();
void* state11();
void* state12();
void* state13();
void* state14();
void* state15();
void* state16();
void* state17();
void* state18();
void* state19();
void* state20();
void* state21();
void* errorState();

#endif /* MYSTRUCT_H_ */
