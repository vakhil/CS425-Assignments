#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>


int main(int argc, char* argv[])
{
  //First call to socket function
  int sock_desc = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in addr, client_addr;
  //Setting all variables to NULL
  memset(&client_addr , 0, sizeof(struct sockaddr));
  //First argument is the port number. Here it is converted to Netwrok Byte Order
  addr.sin_port = htons(atoi(argv[1]));
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_family = AF_INET;
  //If port not available inform the user about that
  if(bind(sock_desc, (struct sockaddr*) &addr,sizeof(struct sockaddr)) < 0)
    { 
      fputs("Cannot bind to the Port !!", stderr);
    }

  int list = listen(sock_desc,10);
  if(list < 0 )
    fputs("Cannot listen on the port!!", stderr);

  int addrlen = sizeof(struct sockaddr);

  //This loop suggests to continously wait for all clients
  while(1)
    {
    int descrip = accept(sock_desc , (struct sockaddr*) &client_addr , &addrlen);
    if(descrip == -1)
      fputs("Descirptor not successfully returned!!", stderr);

    //Client is connected
    printf("Client is connected\n");
    //Buffer size of length 4096 for reading inputs from clients and sending message to clients
    char* buf = (char*)malloc(4096*(sizeof(char)));

    /* read stream */
    FILE* rx = fdopen(descrip, "r");
    /*write stream */
    FILE* tx = fdopen(dup(descrip), "w");

    //Sending welcome message to the client to indicate succesful connection of client
    char wel_msg[] = "Welcome to the program, Sir!!\0";

    //number indicates the number of bytes written to the client
    int number = fwrite(wel_msg, 1 , 255,tx );
    fflush(tx);
    if(number < 0)
      {
        fputs("Failed to write", stderr);    
      }
    //This loop indicates that server listens to a particular client unless it closes it's connection
    while(1)
      {
      //This fread is for reading the file's name from the client
      number = fread(buf, 1, 4096 , rx);
      //If a client closes down it's connection, the loop breaks 
      if(feof(rx))
        {
          printf("Client has disconnected\n");
          break;
        }
      //i counts the number of characters in the name of the file name sent by client
      int i;
      for (i = 0; buf[i] != '\0'; i++)
        {
          
        }
      //f_name is the file's name as sent by client.
      char* f_name = (char*)malloc(i*(sizeof(char)));
      int j;i--;
      for (j = 0; j < i; j++)
        {
          f_name[j] = buf[j];
        }
      //Since buf is no longer used, we initialize it to NULL
      memset(buf, 0, sizeof(buf));
      //Opens the file here
      FILE* fp = fopen(f_name, "r");
      //This is for knowing the size of the requested file and sending it to client

      //If file does not exist, the following at the bottom happens
      if (!fp) 
        {
          int size = 0;
          char sizes[10];
          sprintf(sizes, "%d", size);
          fwrite(sizes,1,10,tx);
          snprintf(buf, 4096, "Wrong file name!\n ");

          fwrite(buf,1,4096, tx);
        }
      //if file opens succesfully, all the text sending part to client happens here
      else 
        {
          fseek(fp, 0L, SEEK_END);
          //This is done to know the size of the file
          int size = ftell(fp);
          char sizes[10];
          sprintf(sizes, "%d", size);
          //telling the client the size of the file
          fwrite(sizes,1,10,tx);
          //Resetting the file pointer
          rewind(fp);
          int c;
          i = 0;
          int done = 1;
          //This while loop runs till it reaches the end of file
          while(1)
            {
              if(i == 4096)
                {
                  fwrite(buf,1,4096,tx);
                  memset(buf,0,4096);
                  i= 0;
                }
              c = fgetc(fp);
              buf[i] = (char) c;
              //The while loop breaks out here if end of file is reached
              if(feof(fp))
                { 
                   break ;
                }
              i++;
            }
          //If the file is not exatly of size 4096 bytes
          if(i!=0)
            {
              buf[i] = '\0';
              fwrite(buf,1,4096,tx);
            }
          }
      
      fflush(tx);
      }
    fclose(rx);
    fclose(tx);
    }
  return 0;
}