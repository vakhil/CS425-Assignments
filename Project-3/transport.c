/*
 * transport.c 
 *
 *	Project 3		
 *
 * This file implements the STCP layer that sits between the
 * mysocket and network layers. You are required to fill in the STCP
 * functionality in this file. 
 *
 */


#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <arpa/inet.h>
#include "mysock.h"
#include "stcp_api.h"
#include "transport.h"


enum { CSTATE_ESTABLISHED , FIN_WAIT, FIN_SENT_ACK_RECIEVED, CLOSE_WAIT, LAST_ACK };    /* you should have more states */


/* this structure is global to a mysocket descriptor */
typedef struct
{
    bool_t done;    /* TRUE once connection is closed */

    int connection_state;   /* state of the connection (established, etc.) */
    tcp_seq initial_sequence_num;

    /* any other connection-wide global variables go here */
} context_t;


static void generate_initial_seq_num(context_t *ctx);
static void control_loop(mysocket_t sd, context_t *ctx);


/* initialise the transport layer, and start the main loop, handling
 * any data from the peer or the application.  this function should not
 * return until the connection is closed.
 */
void transport_init(mysocket_t sd, bool_t is_active)
{
    context_t *ctx;
   

    ctx = (context_t *) calloc(1, sizeof(context_t));
    assert(ctx);

    generate_initial_seq_num(ctx);

    if(is_active)
    {
    	//All memory stars pointers not working

    	//STCPHeader* head = (STCPHeader*)malloc(sizeof(STCPHeader));
    	STCPHeader head;
    	head.th_seq = ctx->initial_sequence_num;
    	ctx->initial_sequence_num = ctx->initial_sequence_num + 1;
    	head.th_flags = TH_SYN;
    	stcp_network_send(sd, &head,sizeof(STCPHeader),NULL  );
    	printf("COMPLETED FIRST SEND\n");


    	STCPHeader recv ;
    	//STCPHeader* recv = (STCPHeader*)malloc(sizeof(STCPHeader));
    	
    	unsigned int wait2 =  stcp_wait_for_event(sd,NETWORK_DATA,NULL );
    	if(wait2 && NETWORK_DATA)
    	{
	    	stcp_network_recv(sd, &recv, sizeof(STCPHeader));
	    	int seq = recv.th_seq;



	    	STCPHeader ack;
	    	//STCPHeader* ack = (STCPHeader*)malloc(sizeof(STCPHeader));
	    	ack.th_seq = ctx->initial_sequence_num;
	    	ctx->initial_sequence_num = ctx->initial_sequence_num + 1;
	    	ack.th_ack = seq+1;
	    	ack.th_flags = TH_ACK;
	    	stcp_network_send(sd, &ack,(sizeof(ack)), NULL);
	    	printf("COMPLETED SECOND SEND\n");
    	}
    }

    if(!is_active)
    {

    	//STCPHeader* recv1 = (STCPHeader*)malloc(sizeof(STCPHeader));
    	
    	unsigned int wait1 =  stcp_wait_for_event(sd,ANY_EVENT,NULL);
    	STCPHeader recv1;
    	
    	if(wait1 && NETWORK_DATA)
    	{
    	stcp_network_recv(sd, &recv1, sizeof(STCPHeader));
	    	printf("COMPLETED RECEIVE\n");
    	int seq = recv1.th_seq;
    	

    	STCPHeader send1;
    	//STCPHeader* send1 = (STCPHeader*)malloc(sizeof(STCPHeader));
    	send1.th_seq = ctx->initial_sequence_num;
    	ctx->initial_sequence_num = ctx->initial_sequence_num + 1;
    	send1.th_ack = seq+1;
    	send1.th_flags = TH_ACK + TH_SYN;
    	stcp_network_send(sd, &send1, sizeof(STCPHeader), NULL);

    	STCPHeader recv2;
    	unsigned int wait2 =  stcp_wait_for_event(sd,NETWORK_DATA,NULL );

    	if(wait2 && NETWORK_DATA)
    		stcp_network_recv(sd, &recv2, sizeof(STCPHeader));
    	}
    	
    }
    /* XXX: you should send a SYN packet here if is_active, or wait for one
     * to arrive if !is_active.  after the handshake completes, unblock the
     * application with stcp_unblock_application(sd).  you may also use
     * this to communicate an error condition back to the application, e.g.
     * if connection fails; to do so, just set errno appropriately (e.g. to
     * ECONNREFUSED, etc.) before calling the function.
     */

    ctx->connection_state = CSTATE_ESTABLISHED;
    stcp_unblock_application(sd);

    control_loop(sd, ctx);

    /* do any cleanup here */
    free(ctx);
}


/* generate random initial sequence number for an STCP connection */
static void generate_initial_seq_num(context_t *ctx)
{
    assert(ctx);

#ifdef FIXED_INITNUM
    /* please don't change this! */
    ctx->initial_sequence_num = 1;
#else
    /* you have to fill this up */
    /*ctx->initial_sequence_num =;*/
#endif
}


/* control_loop() is the main STCP loop; it repeatedly waits for one of the
 * following to happen:
 *   - incoming data from the peer
 *   - new data from the application (via mywrite())
 *   - the socket to be closed (via myclose())
 *   - a timeout
 */
static void control_loop(mysocket_t sd, context_t *ctx)
{

    assert(ctx);
    assert(!ctx->done);
    //Keep away from these//
    int SWS = 3072;
    int LAR ;
    int LFS ;

    ///
    


    int left = ctx->initial_sequence_num;
    int right = ctx->initial_sequence_num + 3071;
    int send_wind = 3072;
    int event_chosen = ANY_EVENT;
    int times = 3;
    while (!ctx->done)
    {
        unsigned int event;

        /* see stcp_api.h or stcp_api.c for details of this function */
        /* XXX: you will need to change some of these arguments! */
        
        event = stcp_wait_for_event(sd, event_chosen, NULL);

        
        if(event & NETWORK_DATA)
        {
        	
        	char buf[556];
        	memset(buf,0,556);
        	int n1 = stcp_network_recv(sd, &buf, 556);
        	
        	
        	STCPHeader recv;
        	memcpy((void*)&recv, (&buf), 20 );


        	if(recv.th_flags != TH_ACK && recv.th_flags != TH_FIN)
        	{
        		printf("SOMETHING RECEIVED\n");
	        	char app[n1];
	        	memset(app,0,n1-20);
	        	memcpy(app, &buf[20], n1-20);
	        	stcp_app_send(sd,app, n1-20 );
	        	

	        	STCPHeader ack;
	        	ack.th_ack = recv.th_seq + n1-20;
	        	ack.th_seq = ctx->initial_sequence_num;
	        	
	        	ctx->initial_sequence_num = ctx->initial_sequence_num + 1;
	        	left = ctx ->initial_sequence_num;
	        	ack.th_flags = TH_ACK;
	        	stcp_network_send(sd, &ack, 20, NULL);
        	}

        	

        	if(recv.th_flags == TH_ACK)
        	{

 

        		if(ctx->connection_state == FIN_WAIT)
        		{	
        			printf("YA I HAVE Recieved the his ACK\n");
        			ctx->connection_state = FIN_SENT_ACK_RECIEVED;
        			right++;
        			ctx->initial_sequence_num = recv.th_ack;



        		}

        		if(ctx->connection_state == LAST_ACK)
        		{
        			printf("Thanks i am done too!!\n");
        			ctx->done = TRUE;

        		}

        		else{
        		
        		//printf("CHAR IS %d\n",recv.th_ack );
        		//printf("ACKNOWLEDGMENT RECEIVED\n");
        		//printf("acknowledged is %d\n",recv.th_ack );
        		right = right +   recv.th_ack - ctx->initial_sequence_num;
        		send_wind = send_wind + recv.th_ack - ctx->initial_sequence_num;
				ctx->initial_sequence_num = recv.th_ack;
				


	 			//left = ctx->initial_sequence_num;
	 			if(event_chosen & NETWORK_DATA)
	 				{

	 						event_chosen = ANY_EVENT;


	 					//printf("EVENT CHOSEN %d\n",event_chosen );
	 				}

	 			}
        	}


        	if(recv.th_flags == TH_FIN)
        	{

        		if(ctx->connection_state == FIN_SENT_ACK_RECIEVED)
        		{
        			STCPHeader head;
        			head.th_seq = left;
        			head.th_flags = TH_ACK;
        			stcp_network_send(sd, &head, 20, NULL);
        			ctx->done = TRUE;
        		}
        		if(ctx->connection_state == CSTATE_ESTABLISHED)
        		{
        			printf("YA i have received the first FIN\n");

        			STCPHeader head;
        			head.th_flags = TH_ACK;
        			head.th_seq = left;
        			ctx->initial_sequence_num++;
        			left++;
        			right++;

        			stcp_network_send(sd,&head,sizeof(STCPHeader), NULL);
					ctx->connection_state = CLOSE_WAIT;
					stcp_fin_received(sd);
        		}


        	}

        	
        	//continue;
        }




        if (event & APP_DATA )
        {
        	
        	
        	char app[536];
        	memset(app, 0, 536);
        

        	
        	printf("APP DATA\n");
        	int byte_send = MIN(send_wind, 536);
        	int n =(stcp_app_recv(sd,app,byte_send));

        	int packet_load = MIN(MIN(send_wind,536), n);

        	//printf("packet data is %d\n",packet_load );
        	
        	if( n > 0 )
        	{
        		
 				char packet[packet_load+20];
 				memset(packet,0,packet_load+20);
 				memcpy(&packet[20], app, packet_load);
 				STCPHeader header;
 				header.th_flags = -1;
 				header.th_seq = left;

 				//printf("NUM is %d\n",header.th_seq );
 				

 				memcpy(packet, (void*)(&header),20);
 				//printf("ni is %d\n",header.th_flags );
 				stcp_network_send(sd, packet, packet_load+20, NULL);
 				//ctx->initial_sequence_num = ctx->initial_sequence_num+ n;
 				left = left + packet_load;
 				send_wind = send_wind - packet_load;
 				

 				if(send_wind == 0)
 				{	
 					printf("GOOD\n");
 					//return;
 					
 					event_chosen = NETWORK_DATA;

 				}

	 			 //printf("KANE\n");
	 			  //n = stcp_app_recv(sd,app,536);


        	}

        	//continue;
        }


         if(event & APP_CLOSE_REQUESTED)
        {
        	printf("GOOCHY\n");
        	if(ctx->connection_state == CLOSE_WAIT)
        	{
        		printf("APPlication has finally asked me to close the connection\n");
        		STCPHeader head;
        		head.th_flags = TH_FIN;
        		head.th_seq = left;
        		left++;
        		stcp_network_send(sd,&head,sizeof(STCPHeader),NULL);
        		ctx->connection_state = LAST_ACK;
        	}

        	else
        	{
        		printf("Initiation has started for close!!\n");
        	STCPHeader header;
        	header.th_flags = TH_FIN;
        	header.th_seq = left;
        	left++;
        	//right++; 
        	//ctx->initial_sequence_num ++;
        	stcp_network_send(sd,&header,sizeof(STCPHeader), NULL);
        	ctx->connection_state = FIN_WAIT;
        	}



        }

        /* check whether it was the network, app, or a close request */


        /* etc. */
    }
}


/**********************************************************************/
/* our_dprintf
 *
 * Send a formatted message to stdout.
 * 
 * format               A printf-style format string.
 *
 * This function is equivalent to a printf, but may be
 * changed to log errors to a file if desired.
 *
 * Calls to this function are generated by the dprintf amd
 * dperror macros in transport.h
 */
void our_dprintf(const char *format,...)
{
    va_list argptr;
    char buffer[1024];

    assert(format);
    va_start(argptr, format);
    vsnprintf(buffer, sizeof(buffer), format, argptr);
    va_end(argptr);
    fputs(buffer, stdout);
    fflush(stdout);
}



