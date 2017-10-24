#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#define SERVER_PORT 5432
#define MAX_LINE 256

struct course{
  char name[100];
  char id[20];
  int registered;
  int empty;
  char time[20];
};

int main(int argc, char * argv[])
{
	struct course c;
	FILE *fp;
	struct hostent *hp;
	struct sockaddr_in sin;
	char *host;
	char buf[MAX_LINE];
	int s, new_s,n;
	int len;
	char data[1000];
	int choice;
	char search[20];


	if (argc==2) {
		host = argv[1];
	}
	else {
		fprintf(stderr, "usage: simplex-talk host\n");
	exit(1);
	}

	hp = gethostbyname(host);
	if (!hp) {
		fprintf(stderr, "simplex-talk: unknown host: %s\n", host);
		exit(1);
	}

	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
	sin.sin_port = htons(SERVER_PORT);

	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("simplex-talk: socket");
		exit(1);
	}
	if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
		perror("simplex-talk: connect");
		close(s);
		exit(1);
	}

	printf("\n\n*************************************************\n");
    	printf(" 1.List All \n 2.Search \n 3.Add \n 4.Delete \n 9.Quit \n");
	printf("\nPlease enter your choice: \n");
	scanf("%d",&choice);


	do{

		switch(choice)
		{
            case 1: send(s, &choice,sizeof(int), 0);
                printf("\n==============Requesting All Records..=======================\n");
                len = recv(s, &data,sizeof(data), 0);
                fputs(data,stdout);
                break;

            case 2: send(s, &choice,sizeof(int), 0);
                printf("\nPlease Enter Course Name/ID/Sem: ");
                scanf("%s",&search);
                send(s, &search,sizeof(search), 0);
                printf("\n======================Searching for The Record..================\n");
                len = recv(s, &data,sizeof(data), 0);
                fputs(data,stdout);
                break;

            case 3:	send(s, &choice,sizeof(int), 0);
                printf("Please Enter Course ID: ");
                scanf("%s",&c.id);
                printf("Please Enter Course Name: ");

                 //fgets will not work after using scanf
                //to get it work again, read the extra characters and discard
                char ch;
                  while((ch= getchar()) != '\n' && ch != EOF);
                fgets(c.name, 100, stdin);

                printf("No of Students Registered: ");
                scanf("%d",&c.registered);
                printf("No of Vacancies: ");
                scanf("%d",&c.empty);
                printf("Course Time: ");
                scanf("%s",&c.time);
                send(s, &c,sizeof(struct course), 0);
                printf("\nInserting New Record..\n");
                break;

            case 4: send(s, &choice,sizeof(int), 0);
                printf("Please Enter Course Name/ID To Drop: ");

                //fgets will not work after using scanf
                //to get it work again, read the extra characters and discard
                char ch1;
                  while((ch1= getchar()) != '\n' && ch1 != EOF);

                fgets(buf, 256, stdin);
                send(s, buf,sizeof(buf), 0);
                break;

             default: printf("Please enter valid choice...\n");
		}

		printf("\n\n*************************************************\n");
		printf(" 1.List All \n 2.Search \n 3.Add \n 4.Delete \n 9.Quit \n");
		printf("\nPlease enter your choice: \n");
		scanf("%d",&choice);

	}while(choice!=9);
}
