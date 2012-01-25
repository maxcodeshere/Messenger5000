/*server-programm Nils-Felix Neumann Simon Rauchel Fabian Stoller*/


#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#include<time.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>

#define MAXCLIENTS 50
#define BUFSIZE 1024
#define TIME 25
#define NAME 128
#define BACkTRACE 4096

struct username{
	int   client;
	char  username[NAME];
	char *time;
};

int com(struct username *, fd_set *,int);



int set_fds(int, struct username *,fd_set *);

int main(){
	struct username username[MAXCLIENTS];
	fd_set rfds;
	time_t t;
	int server_socket, client_socket, max, i, j, laenge, anzahln, erg;
	struct sockaddr_in serverinfo, clientinfo;
	unsigned short int portnummer=5000;
	char serverIp[]="127.0.0.1";
	FILE *verlauf;
	
	for(i=0;i<MAXCLIENTS;i++){
		memset(username[i].username,'\0',NAME);
	}

	verlauf=fopen("./chat_Verlauf.txt","r");
	if(verlauf==NULL){
		verlauf=fopen("./chat_Verlauf.txt","w");
		printf("\nchat_Verlauf.txt neu erstellt");
	}
	fclose(verlauf);

	
	printf("\nserver: socket()..");
	
	server_socket=socket(AF_INET,SOCK_STREAM,0);
	
	serverinfo.sin_family=AF_INET;
	inet_pton(AF_INET,serverIp,&serverinfo.sin_addr.s_addr);
	serverinfo.sin_port=htons(portnummer);
	laenge=sizeof(serverinfo);

	printf("\nserver:bind()...");
	
	bind(server_socket, (struct sockaddr *)&serverinfo, laenge);

	printf("\nserver:listen()...");
	printf("\nserver mit IP: %s",serverIp);
	printf("\nan Port 5000 wartet...");
	

	listen(server_socket,15);
	
	
	for(i=0;i<MAXCLIENTS; i++){
		username[i].client=-1;
	}
	
	i=0;
	

while(1){
	fflush(stdout);
	max = set_fds(server_socket, username, &rfds);
	
	select(max+1, &rfds, NULL, NULL, NULL);

	if(FD_ISSET(server_socket, &rfds)){
		if(i<MAXCLIENTS){
			client_socket=accept(server_socket, NULL, 0);
			laenge=read(client_socket,username[i].username,sizeof(username[i].username));
			username[i].username[laenge]='\0';
						


			t=time(NULL);
			username[i].time=(char *)malloc(TIME);
			username[i].time=ctime(&t);
			username[i].time[24]='\0';
		
			printf("\nNeuer Teilnehmer ist aufgenommen. %s ist seit %s online!\n",username[i].username,username[i].time);
			username[i].client=client_socket;
			for(j=0;j<MAXCLIENTS;j++)
				if(username[j].client==username[i].client)
					continue;
				write(username[j].client,username[i].username,sizeof(username[i].username));	
			i++;
		}	
		else{
			printf("\nServer voll\n");
			send(client_socket,"\nServer ist voll!\n",sizeof("\nServer ist voll!\n"),0);
			continue;
		}
		continue;
	}
	i = com(username,&rfds, i);
	
}
}




int com(struct username *username, fd_set *fds, int c){
	int i, j, k, bytes=0, sent, len, temp, log;
	time_t t;			//Uhrzeit
	struct tm *ts;			//Uhrzeit
	char *tim, ch;
	char buffer[BUFSIZE], backtrace[BUFSIZE];
	FILE *verlauf;
	memset(buffer, '\0', BUFSIZE);
	
	verlauf=fopen("./chat_Verlauf.txt","a");
	if(verlauf==NULL){
		verlauf=fopen("./chat_Verlauf.txt","a");
			if(verlauf==NULL){
				printf("\nFehler beim öffnen der Datei chat_Verlauf.txt");
			}
	}	
	
	
	for(i=0; i<c; i++){


		

		if(username[i].client<0)
			continue;
		/*Testen ob in einem clientsocket Daten liegen*/		
		if(FD_ISSET(username[i].client, fds)){

			bytes=recv(username[i].client, buffer, sizeof(buffer),0);
			buffer[strlen(buffer)]='\0';
			printf("buffer: %s\n, strelen(): %d\n", buffer, strlen(buffer));
			fprintf(verlauf,"\n%s hat gesgat:%s\n",username[i].username, buffer);			
					

			fclose(verlauf);
			//buffer[0]='\0';
			if(tim=strstr(buffer, "givemethelog")!=NULL){
				printf("\nLoading Logfile");
				log=open("./chat_Verlauf.txt",S_IROTH);
				if(verlauf==NULL){
					perror("FUCK!, Datei nicht geöffnet");
				}
				len=read(log, backtrace, BUFSIZE);
				write(username[i].client, backtrace, strlen(backtrace));
								
			
				close(log);
			}
			
			if(tim=strstr(buffer, "whoisonline")!=NULL){							/*-who? Teilnehmer*/
					for(k=0;k<c;k++){
						if(username[k].client>-1)	{					
						
							printf("\n*** %s ******* %s ***",username[k].username,username[k].time);
							
							
							printf("\n%s",username[k].username);
							write(username[i].client, username[k].username, strlen(username[k].username));
							write(username[i].client, " seit ",strlen(" seit "));
							write(username[i].client, username[k].time, strlen(username[k].time));
							write(username[i].client, "\n",strlen("\n"));
							//printf("%s\n%s",username[k].time);	
							
							if(sent!=-1){
								printf("\nSenden erfolgreich");
							}
						}
					}
				}


			if(bytes==0){
				close(username[i].client);
				username[i].client=-1;
				printf("\nClient [%d] left",i);
				
				
				c--;
				return 0;
			}
			if(bytes==-1){
				username[i].client=-1;
				printf("\nClient[%d] left",i);
				c--;
				return 0;
			}
			if(i==MAXCLIENTS){
				printf("\nKeiner war aktiv!");
				return 0;
			}
			printf("\nDaten gefunden...");
			for(j=0;j<MAXCLIENTS;j++){
				if(username[j].client<0){
					continue;
					//printf("\nDaten gefunden...");				
				}
				if(j==i)
					continue;
				
				write(username[j].client, username[i].username, strlen(username[i].username));
				write(username[j].client, " hat gesagt:", strlen(" hat gesagt:"));
				write(username[j].client, buffer, bytes);
				if(sent==-1){
					username[j].client=-1;
					printf("\n Client hat sich abgemeldet. Senden ist fehlgeschlagen");
				
				}
				
			}				

		}
	}

	//close(verlauf);

	return c;
}

int set_fds( int s, struct username *username, fd_set *fds){
	int i, max;
	FD_ZERO(fds);
	FD_SET(s, fds);

	max =s;
//printf("test");
	for (i=0; i<MAXCLIENTS; i++){
		if ( username[i].client!=-1){
			if (username[i].client>max)
				max=username[i].client;
			FD_SET(username[i].client, fds);
		}
	}
	
	
	return max;
}
