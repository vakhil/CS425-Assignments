#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>

int main(int argc, char *argv[]) {
   //Intialize the buffer to hold all fread and fwrite strings
   char* buffer = (char*)malloc(4096*(sizeof(char)));
   //this is the port number obtained from argument   
   int port = atoi(argv[2]);
   //opens the socket here
   int sock_desc = socket(AF_INET, SOCK_STREAM, 0);   
   if (sock_desc < 0) {
      fputs("Cannot connect to socket", stderr);
      exit(1);
   }
   // Ip-address of the server
   struct hostent* server = gethostbyname(argv[1]);
   //If IP-Address not found
   if (server == NULL) {
      fputs("no such Ip-address found", stderr);
      return;
   }
   //This data structure stores the port and IP-Address of server
   struct sockaddr_in addr;
   memset((char *) &addr,0 ,sizeof(addr) );
   //bzero((char *) &addr, sizeof(addr));
   addr.sin_family = AF_INET;

   bcopy((char *)server->h_addr, (char *)&addr.sin_addr.s_addr, server->h_length);
   addr.sin_port = htons(port);
   
   /* Now connect to the server */
  connect(sock_desc, (struct sockaddr*)&addr, sizeof(addr));
 
   
   /* read stream */
   FILE* rx = fdopen(sock_desc, "r");

   /*write stream */
   FILE* tx = fdopen(dup(sock_desc), "w");

   int number = fread(buffer,1,255,rx);
   //Welcome message is being read
   printf("%s\n",buffer );
   if (number < 0) 
      {
         perror("ERROR reading from socket");
         exit(1);
      }

//This while loop indicates that client can request any number of files. This will break only if client disconnects
while(1)
{
   printf("Please type the name of the file : ");
   memset(buffer , 0, 4096);
   //taking input from user regarding the file
   fgets(buffer,4096,stdin);
   //Sending the name of the file to server
   number = fwrite(buffer , 1 , 4096 , tx );
   fflush(tx);
   
   memset(buffer , '\0', 4096);
   //Reading the size of the file from Server
   number = fread(buffer,1,10,rx);
   int size  = atoi(buffer);
   int quotient = atoi(buffer)/4096;
   int remainder = atoi(buffer)%4096;
   memset(buffer , '\0', 4096);
   //If file doesn't exist, then Display no file has been found
   if(size == 0)
      {
      number = fread(buffer,1,4096,rx);
      printf("%s\n",buffer );
      }
   //If file is there, take in buffer of 4096 bytes and keep printing 4096 bytes
   else 
      {
      printf("The text from above file is:" );
      int i = 0;
      while(i < quotient)
      {
      number = fread(buffer,1,4096,rx);
      //Keep printing buffer of 4096 bytes from a large file
      printf("%s",buffer );
      i = i +1;
      }
      //If any bytes remaing, print them too
      if(remainder != 0)
      {
         number = fread(buffer,1,4096,rx);
         printf("%s\n",buffer ); }
      else
         printf("\n");
      }
 }
fclose(rx);
fclose(tx);
   //bzero(buffer,4096);
   //scanf("%s",buffer);

   //printf("%s\n",buffer);
   //stream = fdopen(sock_desc, "r+b");
   //n = fread(buffer,1,4096,stdin);
   //n = fwrite(buffer , 1 , 4095 , stream );
   //n = write(sock_desc,buffer,255);

return 0;




}