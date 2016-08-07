#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

int main(int argc, char *argv[]) {
   int sockfd, portno, n;
   struct sockaddr_in serv_addr;
   struct hostent *server;
   
   char* buffer = (char*)malloc(4096*(sizeof(char)));
   char* buffer2 = (char*)malloc(4096*(sizeof(char)));
   
   if (argc < 3) {
      fprintf(stderr,"usage %s hostname port\n", argv[0]);
      exit(0);
   }
   
   portno = atoi(argv[2]);
   
   /* Create a socket point */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
   
   server = gethostbyname(argv[1]);
   
   if (server == NULL) {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
   }
   
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = htons(portno);
   
   /* Now connect to the server */
  connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
 
   
/* read stream */
   FILE* rx = fdopen(sockfd, "r");

/*write stream */
   FILE* tx = fdopen(dup(sockfd), "w");


   //memset(buffer , 0, 4095);
   //bzero(buffer,4095);
   n = fread(buffer,1,255,rx);
   
   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }

   printf("%s\n",buffer);
   
   printf("Please type the name of the file\n");
   //memset(buffer , 0, 4095);
   scanf("%s",buffer2);
   printf("%s\n",buffer2);
   n = fwrite(buffer2 , 1 , 4096 , tx );
   fflush(tx);

fclose(rx);
fclose(tx);
   //bzero(buffer,4096);
   //scanf("%s",buffer);

   //printf("%s\n",buffer);
   //stream = fdopen(sockfd, "r+b");
   //n = fread(buffer,1,4096,stdin);
   //n = fwrite(buffer , 1 , 4095 , stream );
   //n = write(sockfd,buffer,255);

return 0;




}