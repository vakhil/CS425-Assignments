#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>


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
  pid_t pid;
  //This loop suggests that the server is running continously
  for(;;)
    {
     descrip = accept(sock_desc , (struct sockaddr*) &client_addr , &addrlen);
    if(descrip == -1)
      fputs("Descirptor not successfully returned!!", stderr);

    //Child process is spawned to service the client
    if ( (pid = fork()) == 0 ) 
    {
      //Listening is stopped on the socket
      close(sock_desc);
      //This is a buffer used for sending the service to the client
      char* buf = (char*)malloc(4096*(sizeof(char)));
      memset(buf,0,4096);
      //This fread is for reading the request from browser or client
      int number =read(descrip, buf, 4096);

      if(number < 0 )
      {
        fputs("Problem while reading from socket!!!", stderr);
        exit(1);
      }

      // printf("%s\n",buf );

      //Array used for storing the file-name
      char file_name[30];
      memset(file_name,0,sizeof(file_name));
      //Checking whether the request is a GET Request
      if(buf[0]=='G' && buf[1]=='E' && buf[2] == 'T')
      {   

          int j;
          //Determine the end of first line. Necessary for calculating the file name.
          for (j = 4; buf[j] != '\r' && buf[j+1] != '\n'; ++j)
          {
            
          }
          int k;
          // File name is known after the loop ends
          for(k= 5; k < j-9 ; k++)
          {
            file_name[k-5] = buf[k];
          }
          

          int y =0;                    
          int check = 0;
          //This string is the final filename string i.e the original filename appended by "./webfiles/"
          char* files;
          files = (char *)malloc(90*sizeof(char));
          memset(files,'\0',90);

          //If no file-name is specified,re-direct it to index.html
          if(  ((int)strlen(file_name)) == 0  )
          {

            memset(file_name,0,30);
            sprintf(file_name,"webfiles/index.html");
          }

          //If file name is given as / again redirect it to index.html
          else if( file_name[k-6] == '/')
          {
            strcat(file_name, "index.html");
          }

          //To ensure any file asked, the path is appended by "./webfiles/"
          else
          {
            check = 1;            
            sprintf(files, "webfiles/");
            strcat(files,file_name);
          }

          FILE* fp;

          //This if check is quite redundant and just there because I did not want to take risks at last minute
          if(check == 0)
            fp = fopen(file_name, "r");
          else
            fp = fopen(files,"r");         

          memset(buf,0,4096);
          //If file does not exist, make the client know about it
          if(!fp)
          {
              char *final = (char*)malloc(sizeof(char)*(300));
              memset(final,0, 300);
            
              sprintf(final, "HTTP/1.1 404 Not Found\r\nServer: NCSA/1.5\r\nContent-type: text/html\r\n\r\nThe file is not found");
              char* msg1 = final;
           
           //The following ensures that no part of message is left behind in the buffer and it is transferred fully
             int msglen1 = strlen(final);
               while (msglen1 > 0)
              {
                   int len = write(descrip, msg1, msglen1);
                   if (len <= 0) 
                    return;
                   msg1 += len;
                   msglen1 -= len;
                   
              }
          }
          //If file exists
          else
          {   //This array stores the extension of the file requested
              char extension[20];
              for (y = 0; file_name[k-6] != '.'  && (k-6)>0; y++ , k--)
              {
                extension[y] = file_name[k-6];
              }
              extension[y] = '\0';

              //Based on the different extension, attach different  content-type to the server response. The extension is read from behind and hence the names are reverse
              char *content_type = (char*)malloc(sizeof(char)*(20));
              memset(content_type,0, 20);
              if( !strcmp(extension,"lmth") || !strcmp(extension,"mth"))
              {
                sprintf(content_type, "text/html");
              }

              if( !strcmp(extension,"txt"))
              {
                sprintf(content_type, "text/plain");
              }

              if( !strcmp(extension,"gepj") || !strcmp(extension,"gpj"))
              {
                sprintf(content_type, "image/jpeg");
              }

              if( !strcmp(extension,"fig"))
              {
                sprintf(content_type, "image/gif");
              }          

              if( !strcmp(extension,"fdp"))
              {
                sprintf(content_type, "Application/pdf"); 
              }

              //To knwo the size of the file requested
              fseek(fp, 0L, SEEK_END);
              int sz = ftell(fp);
              rewind(fp);
             
             //Store the file requested in text array
              char* text = (char*)malloc(sizeof(char)*(sz));
              int p = 0;
              char ch;
              while ((ch=fgetc(fp)) != EOF)
              {
                text[p] = ch;
                p++;
              }

              //final contains the response to the client which requests the file
              char *final = (char*)malloc(sizeof(char)*(300));
              memset(final,0, 300);

              sprintf(final , "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: %s\r\nConnection: Keep-Alive\r\n\r\n",sz-1,content_type);
              //Again to ensure that no chunk of data is left behind
               char* msg1 = final;
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
               int file;
               //Again redudncay like before . If condition is not required
               if(check ==0 )
               file = open(file_name, O_RDONLY);
             else
               file = open(files, O_RDONLY);
               memset(buf, 0, 4096);
               
              while ((bytes = read(file, buf, sizeof(buf))) > 0)
                  { 
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
                  //Free the pointers
                  free(final);
          }

          free(buf);
      }
    //Close the descriptor and close down the child process
    close(descrip);
    exit(0);   
    }
      close(descrip);
    }
  return 0;
}


