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
#define MAXLINE 50

struct Info{
	char Id[10];
	char Pw[10];
};

struct Dic{
	char word[10];
	char mean[MAXLINE];
};
struct Pro{
	char word[10];
	char mean[MAXLINE];
};
struct Com{
	char word[10];
	char mean[MAXLINE];
};

readLine(int fd, char* str);
void fileread(struct Dic* D, struct Info* In);
void sign(int cfd, char* inmsg, char* ID, char* PASSWORD, struct Info* In, struct Dic* D, struct Pro* P, struct Com* C);
void signup(int cfd, char* inmsg, char* ID, char* PASSWORD, struct Info* In, struct Dic* D, struct Pro* P, struct Com* C);
void login(int cfd, char*inmsg, char* ID, char* PASSWORD, struct Info* In, struct Dic* D, struct Pro* P, struct Com* C);
void filecom(int cfd, char* inmsg, char* ID, struct Dic* D, struct Pro* P, struct Com* C);
void study(int cfd, char* inmsg, char* ID, struct Pro* P);
void learn(int cfd, char* inmsg, char* ID, struct Pro* P);
void test(int cfd, char* inmsg, char* ID, struct Pro* P);

int main(int argc, char* argv[]){
	int sfd, cfd, port, clientlen;
	struct Info In[MAXLINE];
	struct Dic D[MAXLINE];
	struct Pro P[MAXLINE];
	struct Com C[MAXLINE];
	char inmsg[MAXLINE], outmsg[MAXLINE], ID[MAXLINE], PASSWORD[MAXLINE];
	struct sockaddr_in serveraddr, clientaddr;
	struct hostent *hp;
	char *haddrp;

	signal(SIGCHLD, SIG_IGN);

	if(argc != 2){
		fprintf(stderr, "사용법:%s <port>\n", argv[0]);
		exit(0);
	}

	port = atoi(argv[1]);
	sfd = socket(AF_INET, SOCK_STREAM, DEFAULT_PROTOCOL);

	bzero((char *) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons((unsigned short)port);
	bind(sfd,(struct sockaddr *) &serveraddr, sizeof(serveraddr));
	listen(sfd, 5);
	printf("클라이언트 연결 대기중\n");

	while(1){
		clientlen = sizeof(clientaddr);
		cfd = accept(sfd, (struct sockaddr *)&clientaddr, &clientlen);
		haddrp = inet_ntoa(clientaddr.sin_addr);

		printf("클라이언트 연결 감지\n");

		fileread(D, In);

		sign(cfd, inmsg, ID, PASSWORD, In, D, P, C);

		write(cfd, "000", 4);
		
		close(cfd);
		exit(0);


	}
}
readLine(int fd, char* str){
	int n;
	do{
		n = read(fd, str, 1);
	}while(n > 0 && *str++ != NULL);
	return(n > 0);
}

void fileread(struct Dic* D, struct Info* In){
	FILE* fp;
	fp = fopen("순우리말", "r");
	for(int i = 0; i < MAXLINE; i++)
		fscanf(fp,"%s %[^\n]s", &D[i].word,&D[i].mean);
	fclose(fp);

	fp = fopen("INFO", "r");
	for(int i = 0; i < MAXLINE; i++)
		fscanf(fp, "%s %[^\n]s", &In[i].Id, &In[i].Pw);
	fclose(fp);
}


void sign(int cfd, char* inmsg, char* ID, char* PASSWORD, struct Info* In, struct Dic* D, struct Pro* P, struct Com* C){

	write(cfd, "100", 4);
	write(cfd, "1. 회원등록\n2. 기존계정\n3. 종료\n", 45);
	readLine(cfd, inmsg);

	if(strcmp(inmsg, "1") == 0)
		signup(cfd, inmsg, ID, PASSWORD, In, D, P, C);
	else if(strcmp(inmsg, "2") == 0)
		login(cfd, inmsg, ID, PASSWORD, In, D, P, C);
	else
		write(cfd, "000", 4);
}

void signup(int cfd, char* inmsg, char* ID, char* PASSWORD, struct Info* In, struct Dic* D, struct Pro* P, struct Com* C){
	FILE*fp;
	write(cfd, "101", 4);
	write(cfd, "ID: ", 5);
	readLine(cfd, ID);
	write(cfd, "PASSWORD: ", 11);
	readLine(cfd, PASSWORD);

	for(int i = 0; i < MAXLINE; i++){
		if(strcmp(In[i].Id, ID) == 0){
			write(cfd, "중복된 ID입니다.\n", 24);
			sign(cfd, inmsg, ID, PASSWORD, In, D, P, C);
		}else if(strcmp(In[i].Id, ID) != 0 && i == MAXLINE-1){
			write(cfd, "등록되었습니다.\n", 24);
			fp = fopen("INFO", "a");
			fprintf(fp, "%s %s\n", ID, PASSWORD);
			fclose(fp);
 			fp = fopen(ID, "a");
			filecom(cfd, inmsg, ID, D, P, C);
			study(cfd, inmsg, ID, P);
		}
	}
}

void login(int cfd, char*inmsg, char* ID, char* PASSWORD, struct Info* In, struct Dic* D, struct Pro* P, struct Com* C){
	FILE *fp;
	write(cfd, "102", 4);
	write(cfd, "ID: ", 5);
	readLine(cfd, ID);
	write(cfd, "PASSWORD: ", 11);
	readLine(cfd, PASSWORD);

	for(int i = 0; i < MAXLINE; i++){
		if(strcmp(In[i].Id, ID) == 0 && strcmp(In[i].Pw, PASSWORD) == 0){
			write(cfd, "로그인 성공\n", 18);
			fp = fopen(ID, "a");
			filecom(cfd, inmsg, ID, D, P, C);
			if(strcmp(P[0].word, "학습") == 0)
				end(cfd, inmsg);
			else
				study(cfd, inmsg, ID, P);
		}else if(i == MAXLINE-1 && strcmp(In[i].Id, ID) != 0 && strcmp(In[i].Pw, PASSWORD) != 0){
			write(cfd, "로그인 실패\n", 18);
			sign(cfd, inmsg, ID, PASSWORD, In, D, P, C);
		}
	}
}

void filecom(int cfd, char* inmsg, char* ID, struct Dic* D, struct Pro* P, struct Com* C){
	FILE* fp;
	fp = fopen(ID, "r");
	for(int i = 0; i < MAXLINE; i++)
		fscanf(fp, "%s %[^\n]s", &C[i].word, &C[i].mean);
	fclose(fp);
	if(strcmp(C[20].word, "학습") == 0)
		end(cfd, inmsg);

	fp = fopen(ID, "a");

	int cnt = 0;
	while(cnt < 10){
		for(int i = 0; i < MAXLINE; i++){
			for(int j = 0; j < MAXLINE; j++){
				if(strcmp(D[i].word, C[j].word) == 0)
					break;
				if(strcmp(D[i].word, C[j].word) != 0 && j == MAXLINE -1){
					strcpy(P[cnt].word, D[i].word);
					strcpy(P[cnt].mean, D[i].mean);
					if(strcmp(P[cnt].word, "학습") == 0)
						cnt = 10;
					cnt++;
				}
			}
		}
	}
	fclose(fp);
	
}


void study(int cfd, char* inmsg, char* ID, struct Pro* P){
	write(cfd, "103", 4);
	write(cfd, "1. 학습하기\n2. 종료하기\n" ,33);
	readLine(cfd, inmsg);
	
	if(strcmp(inmsg, "1") == 0)
		learn(cfd, inmsg, ID, P);
	else
		write(cfd, "000", 4);	
}

void learn(int cfd, char* inmsg, char* ID, struct Pro* P){
	write(cfd, "104", 4);
	int i = 0;		
	while(i < 10){			
		write(cfd, "단어: ", 8);
		write(cfd, &P[i].word, strlen(&P[i].word)+1);
		write(cfd, "의미: ", 8);
		write(cfd, &P[i].mean, strlen(&P[i].mean)+1);
		if(i == 9){
			write(cfd, "1. 다시학습\n2. 문제풀기\n" ,33);
			readLine(cfd, inmsg);
			if(strcmp(inmsg, "1") == 0)
				learn(cfd, inmsg, ID, P);
			else if(strcmp(inmsg, "2") == 0)
				test(cfd, inmsg, ID, P);
		}else{
			write(cfd, "다음 단어(Y/y): ", 21);
			readLine(cfd, inmsg);
			if(strcmp(inmsg, "Y") == 0 || strcmp(inmsg, "y") == 0)
				i++;
		}
		if(strcmp(P[i].word, "학습") == 0){
			write(cfd, "111", 4);
			write(cfd, "1. 다시학습\n2. 문제풀기\n" ,33);
			readLine(cfd, inmsg);
			if(strcmp(inmsg, "1") == 0)
				learn(cfd, inmsg, ID, P);
			else if(strcmp(inmsg, "2") == 0)
				test(cfd, inmsg, ID, P);	
		}
	}
	
}

void test(int cfd, char* inmsg, char* ID, struct Pro* P){
	FILE *fp;
	write(cfd, "105", 4);
	
	for(int i = 0; i < 10; i++){
		if(strcmp(P[i].word, "학습") == 0){
			write(cfd, "111", 4);
			break;
		}
		write(cfd, "문제: ", 8);
		write(cfd, &P[i].mean, strlen(&P[i].mean)+1);
		readLine(cfd, inmsg);
		if(strcmp(inmsg, &P[i].word) == 0){
			write(cfd, "정답\n", 8);
			fp = fopen(ID, "a");
			fprintf(fp, "%s %s\n", &P[i].word, &P[i].mean);
			fclose(fp);
		}else
			write(cfd, "오답\n", 8);
	}
	write(cfd, "금일 학습 종료 되었습니다.\n", 39);
	write(cfd, "000", 4);
}


void end(int cfd, char* inmsg){
	write(cfd, "001", 4);
	write(cfd, "학습 완료된 계정입니다.\n", 35);
	write(cfd, "000", 4);
}

