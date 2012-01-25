/*client-programm Nils-Felix Neumann Simon Rauchel Fabian Stoller */

#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include "message.h"

#define BUFSIZE 4096




void com(int s){
	int len;
	fd_set fds;
	char buffer[BUFSIZE];
	FILE *test;

	test=fopen("./test_stderr.txt","w");
	if(test==NULL)
		perror("Fehler beim öffnen");
	

	
	system("clear");
	printf("   Keywords: Logfile: ----> 'givemethelog' <----\n   User:              ----> 'whoisonline'  <----\n\n");
	fflush(stdout);
	while(1){
		memset(buffer,'\0',BUFSIZE);
		FD_ZERO(&fds);
		FD_SET(0, &fds);
		FD_SET(s, &fds);

		if(select(FD_SETSIZE, &fds, NULL, NULL, NULL)<0)
			break;

		if(FD_ISSET(0, &fds)){
			if((len=read(0, buffer, sizeof(buffer)))<=0)
				break;
			if(send(s, buffer, len, 0)<=0)
				break;
		}
		
		if(FD_ISSET(s, &fds)){
			if((len=recv(s, buffer, sizeof(buffer), 0))<=0)
				break;
			if(write(2 , buffer, len+1)<=0)
				break;
		}
		usleep(10);
	}
	fclose(test);
}

int main(){
	int socket_nummer;
	int laenge;
	struct sockaddr_in adressinfo;
	int ergebnis, anzahl;
	char name[100], message[1024], mesage[512];
	//struct message nachricht;

	unsigned short int portnummer = 5000;
	char serverIp[] = "127.0.0.1";
	
	socket_nummer=socket(AF_INET,SOCK_STREAM,0);
	
	adressinfo.sin_family=AF_INET;
	inet_pton(AF_INET, serverIp, &adressinfo.sin_addr.s_addr);
	adressinfo.sin_port=htons(portnummer);
	laenge=sizeof(adressinfo);

	printf("\n\nClient connect...");
	ergebnis=connect(socket_nummer, (struct sockaddr *) &adressinfo, laenge);
	if(ergebnis==0){
		printf("\nErfolg: verbinde...\n");
	}
	else{
		perror("\nFehler beim Verbindungsaufbau!\n");
		exit(1);	
	}

	printf("\n\nBitte geben Sie den Namen ein, mit dem Sie sich am Server anmelden möchten: ");
	gets(name);
	write(socket_nummer,name,sizeof(name));

	
while(1){
	com(socket_nummer);
	


	

	}

	printf("Client: close...");
	close(socket_nummer);
	printf("\n\n");
	return 0;
}
