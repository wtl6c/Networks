/******************************************************************************/
/*                                                                            */
/* ENTITY IMPLEMENTATIONS                                                     */
/*                                                                            */
/******************************************************************************/

// Student names:
// Student computing IDs:
//
//
// This file contains the actual code for the functions that will implement the
// reliable transport protocols enabling entity "A" to reliably send information
// to entity "B".
//
// This is where you should write your code, and you should submit a modified
// version of this file.
//
// Notes:
// - One way network delay averages five time units (longer if there are other
//   messages in the channel for GBN), but can be larger.
// - Packets can be corrupted (either the header or the data portion) or lost,
//   according to user-defined probabilities entered as command line arguments.
// - Packets will be delivered in the order in which they were sent (although
//   some can be lost).
// - You may have global state in this file, BUT THAT GLOBAL STATE MUST NOT BE
//   SHARED BETWEEN THE TWO ENTITIES' FUNCTIONS. "A" and "B" are simulating two
//   entities connected by a network, and as such they cannot access each
//   other's variables and global state. Entity "A" can access its own state,
//   and entity "B" can access its own state, but anything shared between the
//   two must be passed in a `pkt` across the simulated network. Violating this
//   requirement will result in a very low score for this project (or a 0).
//
// To run this project you should be able to compile it with something like:
//
//     $ gcc entity.c simulator.c -o myproject
//
// and then run it like:
//
//     $ ./myproject 0.0 0.0 10 500 3 test1.txt
//
// Of course, that will cause the channel to be perfect, so you should test
// with a less ideal channel, and you should vary the random seed. However, for
// testing it can be helpful to keep the seed constant.
//
// The simulator will write the received data on entity "B" to a file called
// `output.dat`.

#include <stdio.h>
#include <string.h>
#include "entity.h"
#include "simulator.h"


// Entity A Buffer and Pointers
struct pkt prev_packets_A[1000];
struct pkt * last_packet_A;

int waiting_A;
int seq_num_A;

// Entity B : List of seen sequence numbers
int received_seqnums_B[1000];
int received_index;

// Entity B Buffer and Pointers
struct pkt prev_packets_B[1000];
struct pkt * last_packet_B;

int seq_num_B;

// ETC
int num_messages;


/**** A ENTITY ****/

void A_init() {

	waiting_A = 0;
	seq_num_A = 0;
	num_messages = 1;
	init_pkt_buffers();
}


void A_output(struct msg message) {

	if(!waiting_A){

		printf("\n************** Message %d **************", num_messages);
		
		tolayer3_A(make_packet(message));
		starttimer_A(1000.0);
	}

	else {

		printf("Entity A: Message #%d dropped from layer 5_A\n", num_messages);
		printf("\t  length: %d\n", message.length);
		printf("\t  data: %.*s\n\n", message.length, message.data);
	}

	num_messages++;
}


void A_input(struct pkt packet) {

	if(packet.checksum == checkSum(packet)){

		if(packet.acknum == 1) {
	
			printf("ENTITY A: ACK Received from Layer 4_B\n");
			printf("\t  seqnum: %d\n", packet.seqnum);
			printf("\t  acknum: %d\n", packet.acknum);
			printf("\t  checksum: %d\n", packet.checksum);
			printf("\t  length: %d\n", packet.length);
			printf("\t  payload: %.*s\n\n", packet.length, packet.payload);

			stoptimer_A();
			waiting_A = 0;
		}

		else{

			printf("ENTITY A: Nack Received from Layer 4_B, Resending Following Packet\n");
			printf("\t  seqnum: %d\n", last_packet_A->seqnum);
			printf("\t  acknum: %d\n", last_packet_A->acknum);
			printf("\t  checksum: %d\n", last_packet_A->checksum);
			printf("\t  length: %d\n", last_packet_A->length);
			printf("\t  payload: %.*s\n\n", last_packet_A->length, last_packet_A->payload);
			
			waiting_A = 1;
			tolayer3_A(*last_packet_A);
		}
	}

	else {

		printf("ENTITY A: ACK or NACK from Layer 4_B was Garbled, Retransmitting Last Packet\n\n");
		printf("\t  seqnum: %d\n", last_packet_A->seqnum);
		printf("\t  acknum: %d\n", last_packet_A->acknum);
		printf("\t  checksum: %d\n", last_packet_A->checksum);
		printf("\t  length: %d\n", last_packet_A->length);
		printf("\t  payload: %.*s\n\n", last_packet_A->length, last_packet_A->payload);

		waiting_A = 1;
		tolayer3_A(*last_packet_A);
	}
}


void A_timerinterrupt() {

	printf("Timer_A Interrupt, Retransmitting Last Packet\n");
	printf("\t  seqnum: %d\n", last_packet_A->seqnum);
	printf("\t  acknum: %d\n", last_packet_A->acknum);
	printf("\t  checksum: %d\n", last_packet_A->checksum);
	printf("\t  length: %d\n", last_packet_A->length);
	printf("\t  payload: %.*s\n\n", last_packet_A->length, last_packet_A->payload);

	tolayer3_A(*last_packet_A);
	starttimer_A(1000.0);
}


/**** B ENTITY ****/

void B_init() {

	seq_num_B = 0;
}


void B_input(struct pkt packet) {

	struct pkt ackornack;

	if(packet.checksum == checkSum(packet)){

		ackornack = create_ACK();

		if(seq_num_B == packet.seqnum){
			
			tolayer5_B(make_message(packet));
		}
	}

	else{

		ackornack = create_NACK();
	}

	prev_packets_B[seq_num_B] = ackornack;

	if(seq_num_B) {

		seq_num_B = 0;
	}

	else {

		seq_num_B = 1;

	}

	tolayer3_B(ackornack);
}


void B_timerinterrupt() {

}


struct pkt make_packet(struct msg message) {

	struct pkt temp;

	waiting_A = 1;

	temp.seqnum = seq_num_A;
	temp.length = message.length;

	memcpy(temp.payload, message.data, message.length);
	temp.checksum = checkSum(temp);

	printf("\nENTITY A: Sending Packet to Layer 4_B from Layer 4_A\n");
	printf("\t  seqnum: %d\n", temp.seqnum);
	printf("\t  acknum: %d\n", temp.acknum);
	printf("\t  checksum: %d\n", temp.checksum);
	printf("\t  length: %d\n", temp.length);
	printf("\t  payload: %.*s\n\n", temp.length, temp.payload);

	prev_packets_A[seq_num_A] = temp;

	last_packet_A = &prev_packets_A[seq_num_A];
	
	if(seq_num_A) {

		seq_num_A = 0;
	}

	else {

		seq_num_A = 1;
	}

	return temp;
}


struct msg make_message(struct pkt packet) {

	struct msg temp;

	temp.length = packet.length;

	memcpy(temp.data, packet.payload, packet.length);

	printf("ENTITY B: Sending Message to Layer 5_B from Layer 4_B\n");
	printf("\t  length: %d\n", temp.length);
	printf("\t  data: %.*s\n\n", temp.length, temp.data);

	return temp;
}

struct pkt create_ACK(void) {

	struct pkt ACK;
	char msg[13] = {'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ', 'A', 'C', 'K'};
	int x;

	ACK.seqnum = seq_num_B;
	ACK.acknum = 1;
	ACK.length = 13;	
	memcpy(ACK.payload, msg, ACK.length);

	ACK.checksum = checkSum(ACK);

	printf("ENTITY B: Sending ACK to Layer 4_A from Layer 4_B\n");
	printf("\t  seqnum: %d\n", ACK.seqnum);
	printf("\t  acknum: %d\n", ACK.acknum);
	printf("\t  checksum: %d\n", ACK.checksum);
	printf("\t  length: %d\n", ACK.length);
	printf("\t  payload: %.*s\n\n", ACK.length, ACK.payload);

	return ACK;
}

struct pkt create_NACK(void) {

	char msg[14] = {'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ', 'N', 'A', 'C', 'K'};
	struct pkt NACK;
	int x;

	NACK.seqnum = seq_num_B;
	NACK.acknum = 0;
	NACK.length = 14;
	memcpy(NACK.payload, msg, NACK.length);

	NACK.checksum = checkSum(NACK);

	printf("ENTITY B: Sending NACK to Layer 4_A from Layer 4_B\n");
	printf("\t  seqnum: %d\n", NACK.seqnum);
	printf("\t  acknum: %d\n", NACK.acknum);
	printf("\t  checksum: %d\n", NACK.checksum);
	printf("\t  length: %d\n", NACK.length);
	printf("\t  payload: %.*s\n\n", NACK.length, NACK.payload);

	return NACK;
}


void init_pkt_buffers(void) {

	int x; 
	struct pkt zero;

	zero.seqnum = -1;
	zero.acknum = 0;
	zero.checksum = 0;
	zero.length = 0;

	for(x = 0; x < 20; x++) {

		zero.payload[x] = 0;
	}

	for(x = 0; x < 1000; x++) {

		prev_packets_A[x] = zero;
		prev_packets_B[x] = zero;
	}
}


void init_received_seqnums_B(void) {

	int x; 

	received_index = 0;

	for(x = 0; x < 1000; x++) {

		received_seqnums_B[x] = -1;
	}
}

int checkSum(struct pkt packet) {

	int x;
	int return_val = 0;

	return_val += packet.seqnum;
	return_val += packet.acknum;
	return_val += packet.length;

	for(x = 0; x < 20; x++){

		return_val += packet.payload[x];
	}

	return return_val;
}

int check_seqnums(int seqnum) {

	int x;

	for(x = 0; received_seqnums_B[x] != -1; x++) {

		if(seqnum == received_seqnums_B[x]) {

			return 1;
		}
	}

	return 0;
}