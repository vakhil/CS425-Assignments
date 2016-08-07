#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>


int main(int argc, char* argv[])
{

int sock_desc = socket(AF_INET, SOCK_STREAM, 0);
struct sockaddr_in addr, client_addr;
memset(&client_addr , 0, sizeof(struct sockaddr));
addr.sin_port = htons(atoi(argv[1]));
inet_pton(AF_INET,"127.0.0.1" , &addr.sin_addr );
addr.sin_family = AF_INET;
if ( bind(sock_desc, (struct sockaddr*) &addr,sizeof(struct sockaddr)) < 0 )
	{	fputs("Cannot bind to the Port !!", stderr);
	}

int list = listen(sock_desc,10);
if(list < 0 )
	fputs("Cannot listen on the port!!", stderr);

int addrlen = sizeof(struct sockaddr);

while(1)
{
int descrip = accept(sock_desc , (struct sockaddr*) &client_addr , &addrlen);

if(descrip == -1)
	fputs("Descirptor not successfully returned!!", stderr);

printf("Client is connected\n");
char* buf = (char*)malloc(4096*(sizeof(char)));
/* read stream */
   FILE* rx = fdopen(descrip, "r");

/*write stream */
   FILE* tx = fdopen(dup(descrip), "w");

//memset(buf , 0, 4095);

char wel_msg[] = "Welcome to the program, Sir!!\0";
int number = fwrite(wel_msg, 1 , 255,tx );
fflush(tx);
if(number < 0)
{
  fputs("Failed to write", stderr);
  
}

while(1)
{

number = fread(buf, 1, 4095 , rx);
if(feof(rx))
{
  printf("Client has disconnected\n");
  break;
}
int i;
for (i = 0; buf[i] != '\0'; i++){
  
}
char* f_name = (char*)malloc(i*(sizeof(char)));
//char f_name[i];
//memset(f_name, '\0', sizeof(f_name));

int j;i--;
for (j = 0; j < i; j++){
  f_name[j] = buf[j];
}




//strcpy(f_name, buf);
memset(buf, 0, sizeof(f_name));

FILE* fp = fopen(f_name, "r");
int c;
i = 0;
int done = 1;
if (!fp) {
    /* error message if appropriate */
    //fputs("Wrong file name !!", stderr);
  snprintf(buf, 4095, "Wrong file name!\n ");
    done = 0;
}
if(done == 1)
{
while(1)
{
  if(i == 4094)
  {
    fwrite(buf,1,4095, tx);
    memset(buf,0,4095);
    i= 0;
  }


  c = fgetc(fp);
  buf[i] = (char) c;
  if( feof(fp) )
  { 
     break ;
  }
  //printf("%c", c);
  i++;
}
buf[i] = '\0';
}

fwrite(buf,1,4095, tx);
fflush(tx);
//fclose(fp);



}




//printf("%s\n",buf);
fclose(rx);
fclose(tx);

}
//int n = write(descrip,"I got your message",5);
/*FILE * stream = fdopen(descrip, "r+b");
char* buf = (char*)malloc(4096*(sizeof(char)));
memset(buf , 0, 4096);
buf = "Welcome to the program, Sir!!\n";
int number = fwrite(buf , 1 , strlen(buf) , stream );
if(number < 0)
	fputs("Failed to write", stderr);*/
	

//fflush(stream);

//rewind(stream);
//stream = fdopen(descrip, "r+b");
//number = fread(buf,1,4096,stream);
// number = read(descrip,buf, 255);
//printf("%s\n",buf );

/*memset(&buf , 0, 4096);
number = fread(buf, 1, 30 , stream);
if(number < 0) 
	fputs("Failed to read", stderr);
printf("")*/














return 0;

}