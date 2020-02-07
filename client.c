#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#define DEFAULT_PROTOCOL 0
#define MAXLINE 100

readLine(int fd, char* str);
void sign(int sfd, char* inmsg, char* outmsg);
void signup(int sfd, char* inmsg, char* outmsg);
void login(int sfd, char* inmsg, char* outmsg);
void study(int sfd, char* inmsg, char* outmsg);
void learn(int sfd, char* inmsg, char* outmsg);
void test(int sfd, char* inmsg, char* outmsg);
void end(int sfd, char* outmsg);

int main(int argc, char* argv[]){
	int sfd, port, result;
	char *host, inmsg[MAXLINE], outmsg[MAXLINE];
	struct sockaddr_in serverAddr;
	struct hostent *hp;

	if(argc != 3){
		fprintf(stderr, "사용법:%s <host><port>\n", argv[0]);
		exit(0);
	}

	host = argv[1];
	port = atoi(argv[2]);

	sfd = socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);

	if((hp = gethostbyname(host)) == NULL)
		perror("gethostbyname error");
	bzero((char *) &serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	bcopy((char *)hp->h_addr_list[0],(char *)&serverAddr.sin_addr.s_addr, hp->h_length);
	serverAddr.sin_port = htons(port);

	do{
		result = connect(sfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
		if(result == -1)sleep(1);
	}while(result == -1);
	printf("접속 완료\n");

	while(1){
		readLine(sfd,outmsg);
		if(strcmp(outmsg,"100") == 0)
			sign(sfd, inmsg, outmsg);
		else if(strcmp(outmsg,"101") == 0)
			signup(sfd, inmsg, outmsg);
		else if(strcmp(outmsg, "102") == 0)
			login(sfd, inmsg, outmsg);
		else if(strcmp(outmsg,"103") == 0)
			study(sfd, inmsg, outmsg);	
		else if(strcmp(outmsg, "104") == 0)
			learn(sfd, inmsg, outmsg);
		else if(strcmp(outmsg, "105") == 0)
			test(sfd, inmsg, outmsg);
		else if(strcmp(outmsg, "001") == 0)
			end(sfd, outmsg);
		else if(strcmp(outmsg, "000") == 0)
			break;
		
	}
	
	close(sfd);
	exit(0);
}
readLine(int fd, char* str){
	int n;
	do{
		n = read(fd, str, 1);
	}while(n > 0 && *str++ != NULL);
	return(n > 0);
}

void sign(int sfd, char* inmsg, char* outmsg){
	readLine(sfd,outmsg);
	printf("%s", outmsg);
	scanf("%s", inmsg);
	write(sfd, inmsg, strlen(inmsg)+1);
}

void signup(int sfd, char* inmsg, char* outmsg){
	readLine(sfd,outmsg);
	printf("\n%s", outmsg);
	scanf("%s", inmsg);
	write(sfd, inmsg, strlen(inmsg)+1);

	readLine(sfd,outmsg);
	printf("%s", outmsg);
	scanf("%s", inmsg);
	write(sfd, inmsg, strlen(inmsg)+1);
	printf("\n");

	readLine(sfd,outmsg);
	printf("%s\n", outmsg);		
}

void login(int sfd, char* inmsg, char* outmsg){
	readLine(sfd,outmsg);
	printf("\n%s", outmsg);
	scanf("%s", inmsg);
	write(sfd, inmsg, strlen(inmsg)+1);

	readLine(sfd,outmsg);
	printf("%s", outmsg);
	scanf("%s", inmsg);
	write(sfd, inmsg, strlen(inmsg)+1);
	printf("\n");

	readLine(sfd,outmsg);
	printf("%s\n", outmsg);
}

void study(int sfd, char* inmsg, char* outmsg){
	readLine(sfd,outmsg);
	printf("\n%s", outmsg);
	scanf("%s", inmsg);
	write(sfd, inmsg, strlen(inmsg)+1);
}

void learn(int sfd, char* inmsg, char* outmsg){
	printf("\n");
	for(int i = 0; i < 10; i++){
		readLine(sfd,outmsg);
		if(strcmp(outmsg, "111") == 0){
			readLine(sfd,outmsg);
			printf("%s", outmsg);
			scanf("%s", inmsg);
			write(sfd, inmsg, strlen(inmsg)+1);
			printf("\n");
			break;
		}
		printf("%s\n", outmsg);
		readLine(sfd,outmsg);
		printf("%s\n", outmsg);
		readLine(sfd,outmsg);
		printf("%s", outmsg);
		scanf("%s", inmsg);
		write(sfd, inmsg, strlen(inmsg)+1);
		printf("\n");
	}
	
}

void test(int sfd, char* inmsg, char* outmsg){
	printf("\n");
	for(int i = 0; i < 10; i++){
		readLine(sfd, outmsg);
		if(strcmp(outmsg, "111") == 0)
			break;
		printf("%s\n", outmsg);
		scanf("%s", inmsg);
		write(sfd, inmsg, strlen(inmsg)+1);

		readLine(sfd, outmsg);
		printf("%s\n", outmsg);
	}
	readLine(sfd, outmsg);
	printf("%s\n", outmsg);
}

void end(int sfd, char* outmsg){
	readLine(sfd, outmsg);
	printf("%s\n", outmsg);
}
