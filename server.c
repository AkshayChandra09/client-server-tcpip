#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define SERVER_PORT 5432
#define MAX_PENDING 5
#define MAX_LINE 256


struct course{
   char name[100];
  char id[20];
  int registered;
  int empty;
  char time[20];
};

int s, new_s;



void read_file(char filename[20]);
void search_record(char *str);
void appendFile(char fileName[20],  struct course c);
void newDel(char *, char*);



int main()
{
	struct course c;
	struct sockaddr_in sin;

	int len, value;

	char buf[MAX_LINE];
	char search[50];

	/* build address data structure */
	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(SERVER_PORT);
	/* setup passive open */
	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		perror("simplex-talk: socket");
		exit(1);
	}
	if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
		perror("simplex-talk: bind");
		exit(1);
	}
	listen(s, MAX_PENDING);
	/* wait for connection, then receive and print text */

	while(1){

		if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0) {
            perror("simplex-talk: accept");
            exit(1);
        }

        while (len = recv(new_s, &value,sizeof(int), 0))

		switch(value)
		{

			case 1: printf("View All Option Selected:  %d\n",value);
			read_file("records.txt");
			break;

			case 2: printf("Search Option Selected:  %d\n",value);
			len = recv(new_s, &search,sizeof(search), 0);
			search_record(search);
			break;

			case 3: printf("Add New Course Option Selected:  %d\n",value);
			len = recv(new_s, &c,sizeof(struct course), 0);
			appendFile("records.txt",c);
			break;

			case 4: printf("Delete Course Option Selected: %d\n",value);
			len = recv(new_s, &buf,sizeof(buf), 0);
			newDel("records.txt", buf);
			break;

		  case 9: printf("Client Wants To Terminate..."); break;
		}
		close(new_s);
	}

}


//Function To Read Records From File
void read_file(char filename[20])
{
	char courses[100];
	char records[1000]="";
	FILE *fptr;
	fptr = fopen(filename, "r");

	if (fptr == NULL){
	    printf("Cannot open file \n");
	    exit(0);
	}

	while(fgets(&courses,sizeof(courses),fptr)!=NULL){
	    strcat(records,courses);
	    strcat(records,"\n");
	}

	send(new_s, &records,sizeof(records), 0);
	printf("\n\n");
	fclose(fptr);
}


//Function For Searching the Records
void search_record(char *str) {

    char temp[512], search_result[150]="";
	int find_result = 0;

	FILE *fp;
	fp = fopen("records.txt", "r");

    if (fp == NULL){
	   printf("Cannot open file \n");
	   exit(0);
	}

	while(fgets(temp, 512, fp) != NULL) {
		if((strstr(temp, str)) != NULL) {
			strcat(search_result,temp);
			strcat(search_result,"\n");
			find_result++;
		}
	}

	if(find_result == 0) {
		printf("\nSorry, couldn't find a match.\n");
	}

	send(new_s, &search_result,sizeof(search_result), 0);

	if(fp) {
		fclose(fp);
	}

}

//Function To Add a New Record
void appendFile(char fileName[20],  struct course c)
{
    //If you use gets at the client side it will add '\n' at the end
    //Remove Trailing '\n' of Previous Field
    char *p = strchr(c.name, '\n');
	if (p)
    	   *p = '\0';

    FILE *fp;

    fp = fopen(fileName, "a+b");
    if(fp==NULL){
        printf("File Not Found");
        exit(0);
    }

    fprintf(fp, "\n%s\t\t%s\t\t\t%d\t%d\t%s", c.id, c.name,c.registered, c.empty, c.time);
    printf("\nData Written\n");
    fclose(fp);
}


//Function To Delete a Record
void newDel(char *filename, char *searchname)
{
    //If you use gets at the client side it will add '\n' at the end
    //Remove Trailing '\n' of Previous Field
	char *p = strchr(searchname, '\n');
	if (p)
    	   *p = '\0';

	FILE *fp, *fp_tmp;
	char temp[512];

	fp = fopen("records.txt", "r");
	if (fp == NULL){
        printf("Cannot open file \n");
	    exit(0);
	}

	fp_tmp=fopen("tmp.txt", "w+");
	if (!fp_tmp) {
		printf("Unable to open file temp file.");
		exit(0);
	}

	while(fgets(temp, 512, fp) != NULL){
	   if(strstr(temp,searchname)!=NULL){  }
	   else{fprintf(fp_tmp, "%s",temp);}
	}

	fclose(fp);
	fclose(fp_tmp);
	remove(filename);
	rename("tmp.txt", filename);
}
