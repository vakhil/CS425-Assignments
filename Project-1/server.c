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
    //FILE* rx = fdopen(descrip, "r");
    /*write stream */
    //FILE* tx = fdopen(dup(descrip), "w");

    

    
    //This loop indicates that server listens to a particular client unless it closes it's connection
    while(1)
      {
      //This fread is for reading the file's name from the client
      int number =read(descrip, buf, 4096);

      if(number < 0 )
      {
        fputs("Problem while reading from socket!!!", stderr);
        exit(1);
      }

      printf("THIS is it %s\n",buf );

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
            printf("Good boy %s\n",file_name );
          }
          
          FILE* fp = fopen(file_name, "r");
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
            
            strcat(content_type ,"text/html");
          }

          if( !strcmp(extension,"txt"))
          {
            
            strcat(content_type ,"text/plain");
          }

          if( !strcmp(extension,"gepj") || !strcmp(extension,"gpj"))
          {
            
            strcat(content_type ,"image/jpeg");
          }

          if( !strcmp(extension,"fig"))
          {
            
            strcat(content_type ,"image/gif");
          }          

          if( !strcmp(extension,"fdp"))
          {
            
            strcat(content_type ,"Application/pdf");
          }

          
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

          

          char *final = (char*)malloc(sizeof(char)*(sz+300));
          strcat(final , "HTTP/1.1 200 OK\r\n");
          strcat(final, "Content-Length: ");
          char file_sz[10];
          memset(file_sz,0, 10);
          sprintf(file_sz, "%d", sz-1);
          strcat(final,file_sz);
          strcat(final,"\r\nContent-Type: ");
          strcat(final, content_type);
           strcat(final, "\r\nConnection: Keep-Alive\r\n\r\n");
          strcat(final, text);

          

          int n = write(descrip,final,sz+300);
          printf("Number is %d\n",n );



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
          //fwrite(sizes,1,10,tx);
          snprintf(buf, 4096, "Wrong file name!\n ");

          //fwrite(buf,1,4096, tx);
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
          //fwrite(sizes,1,10,tx);
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
            //      fwrite(buf,1,4096,tx);
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
              //fwrite(buf,1,4096,tx);
            }
          }
      
      //fflush(tx);
      }
    //fclose(rx);
    //fclose(tx);
    }
  return 0;
}