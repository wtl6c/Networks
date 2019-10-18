/******************************************************************************/
/*                                                                            */
/* ENTITY IMPLEMENTATIONS                                                     */
/*                                                                            */
/******************************************************************************/

// Student names: William Trace LaCour & Justin Vaughn Javier
// Student computing IDs: WTL6C & JTJ5GS
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


#define Window_Size	8
#define Buffer_Size	1000

// #define DEBUG      1
#define DEBUG2

// Entity A Buffer and Pointers
struct pkt prev_packets_A[Buffer_Size];

// ETC
int num_messages;
int base;
int next_seqnum_A;
int expected_seqnum_B;
int toSend;


/**** A ENTITY ****/

void A_init() {

	next_seqnum_A = 1;
    toSend = 1;
	num_messages = 1;
	base = 1;
	init_pkt_buffer_A();
}


void A_output(struct msg message) {

    struct pkt packet;

    if(toSend < 1000) {

        #ifdef DEBUG2
        printf("\nENTITY A: Message %d received from Layer 5_A\n", toSend);
        printf("\t      Incoming Message: %.*s\n", message.length, message.data);
        printf("\t      Incoming Message Length: %d\n", message.length);
        #endif

        prev_packets_A[toSend] = make_packet(message);

        sendNext();

        toSend++;
    }

    else {

        #ifdef DEBUG2
        printf("\nENTITY A: Packets are being dropped due to buffer being full\n");
        #endif
    }
}


void A_input(struct pkt packet) {

	if(packet.checksum == checkSum(packet)){

	    if(packet.acknum != base) {
            
            #ifdef DEBUG2
            printf("\nENTITY A: Packet from entity B ignored, out of order\n");
            #endif

            return;
        }

        #ifdef DEBUG2
		printf("\nENTITY A: ACK received from entity B\n");
		printf("\t      seqnum: %d\n", packet.seqnum);
		printf("\t      acknum: %d\n", packet.acknum);
		printf("\t      checksum: %d\n", packet.checksum);
		printf("\t      length: %d\n", packet.length);
		printf("\t      payload: %.*s\n\n", packet.length, packet.payload);
        #endif

        base = packet.acknum + 1;

        if(base == next_seqnum_A){
            

            stoptimer_A();
            sendNext();

            #ifdef DEBUG
            printf("Timer_A Stopped\n\n");
            #endif
        }

        else {

            stoptimer_A();
            starttimer_A(100.0);

            #ifdef DEBUG
            printf("Timer_A Started\n\n");
            #endif
        }
    }

    else {

        #ifdef DEBUG2
        printf("\nENTITY A: ACK is garbled, waiting for TimeOut\n");
        #endif
    }
}


void A_timerinterrupt() {

	int x;
	
    #ifdef DEBUG2
	printf("\nENTITY A: Timer_A Interrupt, retransmitting the following messages:\n");
    #endif

	starttimer_A(100.0);

    #ifdef DEBUG
    printf("Timer_A Started\n");
    #endif

	for(x = base; x < next_seqnum_A; x++) {	
	
        if(prev_packets_A[x].seqnum >= 1) {
    		
            tolayer3_A(prev_packets_A[x]);

            #ifdef DEBUG2
    		printf("   \t      Message #%d\n", prev_packets_A[x].seqnum);
    		printf("\t      seqnum: %d\n", prev_packets_A[x].seqnum);
    		printf("\t      acknum: %d\n", prev_packets_A[x].acknum);
    		printf("\t      checksum: %d\n", prev_packets_A[x].checksum);
    		printf("\t      length: %d\n", prev_packets_A[x].length);
    		printf("\t      payload: %.*s\n\n", prev_packets_A[x].length, prev_packets_A[x].payload);
            #endif
        }
	} 
}


/**** B ENTITY ****/

void B_init() {

	expected_seqnum_B = 1;
}


void B_timerinterrupt() {

}


void B_input(struct pkt packet) {

	if(packet.checksum == checkSum(packet)){

		if(expected_seqnum_B == packet.seqnum){
			
			tolayer5_B(make_message(packet));
			tolayer3_B(create_ACK(expected_seqnum_B));
            expected_seqnum_B++;
		}

        else {

            #ifdef DEBUG2       
            printf("\nENTITY B: Packet out of order sending latest acked packet\n");
            #endif

            if(expected_seqnum_B > packet.seqnum){

                tolayer3_B(create_ACK(packet.seqnum));
            }

            else{

                tolayer3_B(create_ACK(expected_seqnum_B - 1));
            }
        }
	}

    else {

        #ifdef DEBUG2
        printf("\nENTITY B: Checksum from entity A garbled, sending latest acked packet\n");
        #endif

        tolayer3_B(create_ACK(expected_seqnum_B - 1));
    }
}


struct pkt make_packet(struct msg message) {

	struct pkt temp;

	temp.seqnum = toSend;
	temp.acknum = 0;
	temp.length = message.length;

	memcpy(temp.payload, message.data, message.length);
	temp.checksum = checkSum(temp);

    #ifdef DEBUG
	printf("\nENTITY A: Sending Packet to B from A\n");
	printf("\t      seqnum: %d\n", temp.seqnum);
	printf("\t      acknum: %d\n", temp.acknum);
	printf("\t      checksum: %d\n", temp.checksum);
	printf("\t      length: %d\n", temp.length);
	printf("\t      payload: %.*s", temp.length, temp.payload);
    printf("\n\n");
    #endif

	return temp;
}


struct msg make_message(struct pkt packet) {

	struct msg temp;

	temp.length = packet.length;

	memcpy(temp.data, packet.payload, packet.length);

    #ifdef DEBUG2
	printf("ENTITY B: Sending Message to Layer 5_B from Layer 4_B\n");
	printf("\t      length: %d\n", temp.length);
	printf("\t      data: %.*s", temp.length, temp.data);
    printf("\n\n");
    #endif

	return temp;
}

struct pkt create_ACK(int seqnum) {

	struct pkt ACK;
	char msg[13] = {'T', 'h', 'i', 's', ' ', 'i', 's', ' ', 'a', ' ', 'A', 'C', 'K'};
	int x;

	ACK.seqnum = seqnum;
	ACK.acknum = seqnum;
	ACK.length = 13;	
	memcpy(ACK.payload, msg, ACK.length);

	ACK.checksum = checkSum(ACK);

    #ifdef DEBUG2
	printf("\nENTITY B: Sending ACK from entity B to entity A\n");
	printf("\t      seqnum: %d\n", ACK.seqnum);
	printf("\t      acknum: %d\n", ACK.acknum);
	printf("\t      checksum: %d\n", ACK.checksum);
	printf("\t      length: %d\n", ACK.length);
	printf("\t      payload: %.*s", ACK.length, ACK.payload);
    printf("\n\n");
    #endif

	return ACK;
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


void sendNext(void){

    if((next_seqnum_A <= toSend) && (next_seqnum_A < (base + Window_Size))){

        if(prev_packets_A[next_seqnum_A].seqnum >= 1){

           
            tolayer3_A(prev_packets_A[next_seqnum_A]);
            printf("\nENTITY A: Message #%d sent to entity B\n", prev_packets_A[next_seqnum_A].seqnum);

            if(base == next_seqnum_A) {
                
                starttimer_A(100.0);
            }

            next_seqnum_A++;
        }
    }
}


void init_pkt_buffer_A(void) {

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
    }
}