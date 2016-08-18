#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>



int descrip;
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
     descrip = accept(sock_desc , (struct sockaddr*) &client_addr , &addrlen);
    if(descrip == -1)
      fputs("Descirptor not successfully returned!!", stderr);


    //Client is connected
    printf("Client is connected\n");
    //Buffer size of length 4096 for reading inputs from clients and sending message to clients

    /* read stream */
    //FILE* rx = fdopen(descrip, "r");
    /*write stream */
    //FILE* tx = fdopen(dup(descrip), "w");

    

    
    //This loop indicates that server listens to a particular client unless it closes it's connection
    while(1)
      {
       char* buf = (char*)malloc(4096*(sizeof(char)));
       memset(buf,0,4096);
      //This fread is for reading the file's name from the client
      int number =read(descrip, buf, 4096);

      if(number < 0 )
      {
        fputs("Problem while reading from socket!!!", stderr);
        exit(1);
      }

      printf("%s\n",buf );

      

      char file_name[30];
      memset(file_name,0,sizeof(file_name));
      //To ensure that the message is a GET Request
      if(buf[0]=='G' && buf[1]=='E' && buf[2] == 'T')
      {   
          int j;
          for (j = 4; buf[j] != '\r' && buf[j+1] != '\n'; ++j)
          {
            
          }
          int k;
          for(k= 5; k < j-9 ; k++)
          {
            file_name[k-5] = buf[k];
          }
          file_name[k-5] = '\0';

          int y =0;
          
          if(  ((int)strlen(file_name)) == 0   )
          {
            memset(file_name,0,30);
            strcat(file_name,"index.html");
            //printf("Good boy %s\n",file_name );
          }
          
          FILE* fp = fopen(file_name, "r");
         // memset(file_name,0,sizeof(file_name));
          memset(buf,0,4096);
          if(!fp)
          {
            char error[1000];
            memset(error,0,1000);
            sprintf(&error[0], "HTTP/1.0 404 Not Found\r\n "
                         "Server: NCSA/1.5\r\n"
                "Content-type: text/html\r\n"
                "\r\n"
                "<HEAD><TITLE>404 Not Found</TITLE></HEAD>\r\n"
                "<BODY><H1>404 Not Found</H1>\r\n"
                "The requested URL was not found on this server. Please come back after some time !!!\r\n "
                "</BODY>");
            

            int n = write(descrip,error,1000);
            memset(error,0,1000);

          }

          char extension[20];
          for (y = 0; file_name[k-6] != '.' ; y++ , k--)
          {
            extension[y] = file_name[k-6];
          }
          extension[y] = '\0';
          printf("extension is %d\n",(int)strlen(extension) );
          
          char *content_type = (char*)malloc(sizeof(char)*(20));
          memset(content_type,0, 20);
          if( !strcmp(extension,"lmth") || !strcmp(extension,"mth"))
          {
            sprintf(content_type, "text/html");
            //strcat(content_type ,"text/html");
          }

          if( !strcmp(extension,"txt"))
          {
            sprintf(content_type, "text/plain");
           // strcat(content_type ,"text/plain");
          }

          if( !strcmp(extension,"gepj") || !strcmp(extension,"gpj"))
          {
            sprintf(content_type, "image/jpeg");
           // strcat(content_type ,"image/jpeg");
          }

          if( !strcmp(extension,"fig"))
          {
            sprintf(content_type, "image/gif");
            //strcat(content_type ,"image/gif");
          }          

          if( !strcmp(extension,"fdp"))
          {
            sprintf(content_type, "Application/pdf"); 
            //strcat(content_type ,"Application/pdf");
          }

          //memset(content_type,0,20);
          
          fseek(fp, 0L, SEEK_END);
          int sz = ftell(fp);
          rewind(fp);
          char* text = (char*)malloc(sizeof(char)*(sz));
          int p = 0;
          char ch;
          while ((ch=fgetc(fp)) != EOF)
          {
            text[p] = ch;
            p++;
          }

          

          char *final = (char*)malloc(sizeof(char)*(300));
          memset(final,0, 300);

          sprintf(final , "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: %s\r\nConnection: Keep-Alive\r\n\r\n",sz-1,content_type);

          // send_BIB(descrip, final ,300);
           char* msg1 = final;
           //printf("JAFFA is %c\n",msg1[0] );
           int msglen1 = strlen(final);
             while (msglen1 > 0)
            {
                 int len = write(descrip, msg1, msglen1);
                 if (len <= 0) 
                  return;
                 msg1 += len;
                 msglen1 -= len;
                 
            }


           int bytes ;
           int file = open(file_name, O_RDONLY);
           memset(buf, 0, 4096);
           
          while ((bytes = read(file, buf, sizeof(buf))) > 0)
              { // send_BIB(descrip, buf, bytes); 
                char *msg =  buf;
                 int msglen = bytes;
                 while ( msglen > 0)
                  {
                       int len = write(descrip, msg, msglen);
                       if (len <= 0) 
                        return;
                       msg = msg + len;
                       msglen = msglen - len;
                  }
              }
          //strcat(final, text);

          

          //int n = write(descrip,final,sz+300);
          //printf("Number is %d\n",n );
              
              free(buf);
              free(final);

      }
  
    printf("Good Boy\n");    
      //fflush(tx);
      }
    //fclose(rx);
    //fclose(tx);
    }
  return 0;
}



void response (void *message, int msglen)
{
    char *msg = (char*) message;

    while (msglen > 0)
    {
         int len = write(descrip, msg, msglen);
         if (len <= 0) return;
         msg += len;
         msglen -= len;
    }
}