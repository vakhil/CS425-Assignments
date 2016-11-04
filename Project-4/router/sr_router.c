/**********************************************************************
 * file:  sr_router.c
 * date:  Mon Feb 18 12:50:42 PST 2002
 * Contact: casado@stanford.edu
 *
 * Description:
 *
 * This file contains all the functions that interact directly
 * with the routing table, as well as the main entry method
 * for routing.
 *
 **********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "sr_if.h"
#include "sr_rt.h"
#include "sr_router.h"
#include "sr_protocol.h"
#include "sr_arpcache.h"
#include "sr_utils.h"

/* TODO: Add constant definitions here... */

/* TODO: Add helper functions here... */

/* See pseudo-code in sr_arpcache.h */
void handle_arpreq(struct sr_instance* sr, struct sr_arpreq *req){
  /* TODO: Fill this in */

   if(time(NULL) - req->sent > 1.0)
   {
      if(req->times_sent >= 5)
      {
       
/*        struct sr_packet* pack = req->packets;

        while(pack != NULL)
        {
          uint8_t final[sizeof(sr_icmp_t3_hdr_t)+ sizeof(sr_ip_hdr_t)+ sizeof(sr_ethernet_hdr_t)];
          sr_icmp_t3_hdr_t ICMP;
          ICMP.icmp_type = 3;
          ICMP.icmp_code = 1;

          memcpy(ICMP.data,pack->buf,ICMP_DATA_SIZE);
          ICMP.icmp_sum = cksum((void*)&ICMP,sizeof(sr_icmp_t3_hdr_t));
          

          sr_ip_hdr_t ip_head;
          struct sr_if* interf = sr_get_interface(sr,pack->iface);
          ip_head.ip_src = interf->ip;
          ip_head.ip_dst = ((sr_ip_hdr_t*)pack->buf)->ip_src;
          ip_head.ip_p = ip_protocol_icmp;
          ip_head.ip_sum = cksum((void*)&ip_head, sizeof(sr_ip_hdr_t));

          /*This code is debatable. Write it after handling rest things. look up 454 enroute route_ip_packet. K t b
*/ /*
          sr_ethernet_hdr_t ether;
          memcpy(ether.ether_shost,interf->addr,6); 
          memcpy(final,&ether,sizeof(sr_ethernet_hdr_t)); 
          memcpy(&final[sizeof(sr_ethernet_hdr_t)],&ip_head,sizeof(sr_ip_hdr_t));
          memcpy(&final[sizeof(sr_ethernet_hdr_t)+sizeof(sr_ip_hdr_t)],&ICMP,sizeof(sr_icmp_t3_hdr_t));
          sr_send_packet(sr, final, sizeof(sr_ethernet_hdr_t)+ sizeof(sr_arp_hdr_t)+sizeof(sr_icmp_t3_hdr_t),interf->name);

          pack = pack->next;
        }
        sr_arpreq_destroy(&sr->cache,req);*/
      }

    else
     {
      struct sr_packet* pack = req->packets;


      /* print_hdrs(pack,sizeof(sr_ethernet_hdr_t)+ sizeof(sr_icmp_t3_hdr_t)+sizeof(sr_ip_hdr_t));
       Constructing an ethernet Packet which is an ARP address */
      sr_ethernet_hdr_t eth;
      eth.ether_type = htons(ethertype_arp);
      int i =0;
      for (i = 0; i < 6; i++)
      {
        eth.ether_dhost[i] = 255;
      }
       
      struct sr_if* interf = sr_get_interface(sr,pack->iface);
      
      memcpy(eth.ether_shost,interf->addr,6); 
      /*eth.ether_shost = interf->ip;  */

      /*Sending a Arp request. */

      /*while(pack != NULL) 
        {*/

      sr_arp_hdr_t* header = (sr_arp_hdr_t*)malloc(sizeof(sr_arp_hdr_t));
      header->ar_sip = (interf -> ip);
      header->ar_tip = htonl(req->ip) ;
      header->ar_op = htons(arp_op_request);

      header->ar_hrd = htons(arp_hrd_ethernet);
      header->ar_pro = htons(2048);
      header->ar_hln = ETHER_ADDR_LEN;
      header->ar_pln = sizeof(uint32_t);
      



      /*header->ar_tip = req->ip;
      memcpy(header->ar_sha, interf->addr, 6); 
      /*sr_send_packet(sr, pack->buf, pack->len, pack->iface);
      //pack = pack -> next; */
      uint8_t send[6]; /*6*8 = 48  */
      int j=0;
      for (j = 0; j < 6; j++)
      {
        send[j] = 255;
      }



      memcpy(header->ar_tha, send,6);

      memcpy(header->ar_sha,&eth.ether_shost ,6);

      /*Make a big something which has both ethernet and ip header */

      uint8_t* total = (uint8_t*)malloc(sizeof(sr_ethernet_hdr_t) + sizeof(sr_arp_hdr_t));
      
      memcpy(total,&eth,sizeof(sr_ethernet_hdr_t));
      memcpy(&total[sizeof(sr_ethernet_hdr_t)],header,sizeof(sr_arp_hdr_t));

      sr_send_packet(sr, total, sizeof(sr_ethernet_hdr_t) + sizeof(sr_arp_hdr_t),pack->iface); 

      
       req->sent = time(NULL) ; 
       req->times_sent++ ;
       
     }
      
   }




  
}

/*---------------------------------------------------------------------
 * Method: sr_init(void)
 * Scope:  Global
 *
 * Initialize the routing subsystem
 *
 *---------------------------------------------------------------------*/

void sr_init(struct sr_instance* sr)
{
    /* REQUIRES */
    assert(sr);

    /* Initialize cache and cache cleanup thread */
    sr_arpcache_init(&(sr->cache));

    pthread_attr_init(&(sr->attr));
    pthread_attr_setdetachstate(&(sr->attr), PTHREAD_CREATE_JOINABLE);
    pthread_attr_setscope(&(sr->attr), PTHREAD_SCOPE_SYSTEM);
    pthread_attr_setscope(&(sr->attr), PTHREAD_SCOPE_SYSTEM);
    pthread_t thread;

    pthread_create(&thread, &(sr->attr), sr_arpcache_timeout, sr);
    
    /* TODO: (opt) Add initialization code here */

} /* -- sr_init -- */

/*---------------------------------------------------------------------
 * Method: sr_handlepacket(uint8_t* p,char* interface)
 * Scope:  Global
 *
 * This method is called each time the router receives a packet on the
 * interface.  The packet buffer, the packet length and the receiving
 * interface are passed in as parameters. The packet is complete with
 * ethernet headers.
 *
 * Note: Both the packet buffer and the character's memory are handled
 * by sr_vns_comm.c that means do NOT free either (signified by "lent" comment).  
 * Make a copy of the
 * packet instead if you intend to keep it around beyond the scope of
 * the method call.
 *
 *---------------------------------------------------------------------*/

void sr_handlepacket(struct sr_instance* sr,
        uint8_t * packet/* lent */,
        unsigned int len,
        char* interface/* lent */){

  /* REQUIRES */
  assert(sr);
  assert(packet);
  assert(interface);



  printf("*** -> Received packet of lengths %d\n",len);


print_hdrs(packet,len);
  
  /* TODO: Add forwarding logic here */
  sr_ethernet_hdr_t* header = (sr_ethernet_hdr_t*) packet ;

  if( htons(header->ether_type) == ethertype_ip)
  { 
      

     sr_ip_hdr_t* ip  = (sr_ip_hdr_t*) (&packet[sizeof(sr_ethernet_hdr_t)]);
    
    
    /* ip->ip_ttl = ip->ip_ttl - 1; */

    struct sr_rt* route = sr->routing_table;
    struct sr_rt* longs;
    uint32_t dest_ip = htonl(ip->ip_dst);
    uint32_t longest_match =0;

    /* print_addr_ip_int(dest_ip);  */

    while(route != NULL)
    {

      /*This operation verifies that the address in the routing table and x` */
      
      uint32_t comp1 = htonl(route->dest.s_addr) & htonl(route->mask.s_addr);
      uint32_t comp2 = htonl(route->mask.s_addr) & dest_ip; 
      /* printf("comp1 is %u",route->dest.s_addr);  */
      if( (comp1 == comp2) && (longest_match <= htonl(route->mask.s_addr)) )
      {
        longest_match = htonl(route->mask.s_addr);
        longs = route;
      }
      route = route -> next;

    }





    
     

    struct sr_if* inter = sr_get_interface(sr,longs->interface);
    struct sr_arpentry *entry = sr_arpcache_lookup(&(sr->cache), longs->dest.s_addr );
    printf("OK\n");

    if(entry != NULL)
    {
      sr_ethernet_hdr_t eth ;
      memcpy(&eth, header, sizeof(sr_ethernet_hdr_t));
      
      memcpy(eth.ether_dhost, entry->mac, ETHER_ADDR_LEN);
      memcpy(eth.ether_shost, inter->addr , ETHER_ADDR_LEN);
      memcpy(header, &eth,sizeof(sr_ethernet_hdr_t) );
      sr_send_packet(sr,(uint8_t *)header, len, longs->interface);
    } 
  else
    { 
      
      struct sr_arpreq *req = sr_arpcache_queuereq(&(sr->cache), ntohl(longs->dest.s_addr), (uint8_t *)packet, len, longs->interface);
      handle_arpreq(sr, req);
      


    }
    
  }
  

  if(htons(header->ether_type) == ethertype_arp)
  {

    
    sr_arp_hdr_t* arp = (sr_arp_hdr_t*)( packet + sizeof(sr_ethernet_hdr_t) );
    /* If it is an arp_op_request and the requested IP matches my IP, then send my MAC address back*/
    if( htons(arp->ar_op) == arp_op_request)
    {
       printf("ok1\n");
      struct sr_if* inter = sr_get_interface(sr,interface);
      /*print_addr_ip_int(htonl(inter->ip));   */
      /*print_hdrs(packet,sizeof(sr_ethernet_hdr_t)+sizeof(sr_arp_hdr_t)); */

      /*printf("Good Job %d\n",ntohl(arp->ar_tip) ); */

      if( htonl(inter->ip) == ntohl(arp->ar_tip))
      {

        arp->ar_op = ntohs(arp_op_reply);
        
        memcpy(arp->ar_tha, arp->ar_sha, ETHER_ADDR_LEN);
        memcpy(arp->ar_sha, inter->addr, ETHER_ADDR_LEN);
        uint32_t sip = (arp->ar_sip);
        arp->ar_sip = (arp->ar_tip);
        arp->ar_tip = sip;
        
        memcpy(header->ether_dhost, header->ether_shost, ETHER_ADDR_LEN);
        memcpy(header->ether_shost, inter->addr, ETHER_ADDR_LEN);

        sr_send_packet(sr, (uint8_t*) header, sizeof(sr_ethernet_hdr_t)+sizeof(sr_arp_hdr_t), interface);
        return;
      }

      else
      {
        printf("SORRY I AM NOT THE CORRECT COMPUTER\n");
      }
    }

    
    if(htons(arp->ar_op) == arp_op_reply)
    {


      /* Just got the reply for the mac you were waiting for */
      
       
      struct sr_arpreq *reply = sr_arpcache_insert(&(sr->cache),arp->ar_sha,ntohl(arp->ar_sip)); 

      if(reply)
      {
        
        struct sr_packet* packet = reply->packets;
        printf("GUUCHI\n");
        while(packet != NULL)
        {
          sr_ethernet_hdr_t* eth = (sr_ethernet_hdr_t*)(packet->buf);
          struct sr_if* inter = sr_get_interface(sr,packet->iface);
          eth->ether_type = htons(ethertype_ip);
          memcpy(eth->ether_shost,inter->addr ,ETHER_ADDR_LEN);
          memcpy(eth->ether_dhost,arp->ar_sha ,ETHER_ADDR_LEN);
          
          sr_send_packet(sr,eth, packet->len ,packet->iface);
          packet = packet->next;
        }
      }
      sr_arpreq_destroy(&(sr->cache),reply);

      return;
    }
  }




 
  

}/* -- sr_handlepacket -- */

