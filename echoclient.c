/*
* echoclient.c - An echo client
*/
/* $begin echoclientmain */
#include "csapp.h"

char *inputString(FILE* fp, size_t size) {
	char *str;
	int ch;

	size_t len = 0;
	str = realloc(NULL, sizeof(char)*size);
	if (!str) return str;
	while (EOF != (ch = fgetc(fp)) && ch != '\n') {
		str[len++] = ch;
		if (len == size) {
			str = realloc(str, sizeof(char)*(size += 16));
			if (!str) return str;
		}
	}
	str[len++] = '\0';

	return realloc(str, sizeof(char)*len);
}

int wordcount(char *inputline)
{
	int counted = 0;

	const char* it = inputline;
	int inword = 0;

	do switch (*it) {
	case '\0':
	case ' ': case '\t': case '\n': case '\r':
		if (inword) { inword = 0; counted++; }
		break;
	default: inword = 1;
	} while (*it++);

	return counted;
}

int main(int argc, char **argv)
{
	int clientfd, key, status, filesize;
	char *host, *port, *keyinput, *inputline, *word, data[100000], *filename, buf[MAXLINE];
	rio_t rio;

	//Create structures for commands
	//Structure for "send" command
	struct SendStruct
	{
		int request;
		int secretkey;
		char filename[80];
		int size;
		char data[100000];
	};
	//Structure for "retrieve" command
	struct GetStruct
	{
		int request;
		int secretkey;
		char filename[80];
	};
	//Structure for "delete" command
	struct DelStruct
	{
		int request;
		int secretkey;
		char filename[80];
	};
	//Structure for "list" command
	struct ListStruct
	{
		int request;
		int secretkey;
	};

	//Make sure user used corect program call
	if (argc != 4) {
		fprintf(stderr, "usage: %s <host> <port> <secret key>\n", argv[0]);
		exit(0);
	}

	//Sort arguments where they need to go
	host = argv[1];
	port = argv[2];
	keyinput = argv[3];

	//Convert key to integer
	key = atoi(keyinput);

	clientfd = Open_clientfd(host, port);
	Rio_readinitb(&rio, clientfd);

	//Run until end of file
	while (!feof(stdin)) {
		printf("> ");
		inputline = inputString(stdin, 10);
		if (strcmp(inputline, "\0") != 0) {
			int arguments = wordcount(inputline);
			word = strtok(inputline, " ");

			if (strcmp(word, "cput") == 0 && arguments == 2) {
				struct SendStruct sendrequest;
				sendrequest.request = 1;
				sendrequest.secretkey = key;
				word = strtok(NULL, " ");
				if (strlen(word) + 1 <= 80) {
					strcpy(sendrequest.filename, word);
					//Check if file exists
					FILE * pFile;
					char str[1000];
					char content[100000];
					pFile = fopen(sendrequest.filename, "r");
					if (pFile == NULL) {
						fprintf(stderr, "Error: File opening failed.\n");
						status = -1;
					}
					else {
						while (fgets(str, 1000, pFile) != NULL) {
							strcat(content, str);
						}
						//Send request
						Rio_writen(clientfd, &sendrequest.request, 4);
						Rio_writen(clientfd, &sendrequest.secretkey, 4);
						int size = sizeof(content);
						//Send file name
						Rio_writen(clientfd, &sendrequest.filename, 80);
						//Send file size
						Rio_writen(clientfd, &size, 4);
						//Send content of file
						Rio_writen(clientfd, &content, size);
						fclose(pFile);
					}
					//Recieve request results
					Rio_readn(clientfd, &status, 4);
					if (status == 0) {
						fprintf(stdout, "File Send Successful.\n");
					}
					else if (status == -1) {
						//If get failed, notify user
						fprintf(stderr, "File Send Failed.\n");
					}
					else {
						//If Rio failed, notify user
						fprintf(stderr, "Error: Rio error");
					}
				}
				else {
					fprintf(stderr, "Error: filename is too large.\n");
				}
			}

			else if (strcmp(word, "cget") == 0 && arguments == 2) {
				struct GetStruct getrequest;
				//Set request to "Retreive" (2) and secret key to key
				getrequest.request = 2;
				getrequest.secretkey = key;
				word = strtok(NULL, " ");
				//Make sure filename is not too large
				if (strlen(word) + 1 <= 80) {
					strcpy(getrequest.filename, word);
					//Send message
					Rio_writen(clientfd, &getrequest.request, 4);
					Rio_writen(clientfd, &getrequest.secretkey, 4);
					Rio_writen(clientfd, &getrequest.filename, 80);
					//Read data from server
					Rio_readn(clientfd, &status, 4);
					//Check if request was success
					if (status == 0) {
						//Read file size
						Rio_readn(clientfd, &filesize, 4);
						//Read data sent
						Rio_readn(clientfd, &data, filesize);
						//Store data in filename
						FILE * fp;
						//Open the file for writing
						fp = fopen(getrequest.filename, "w");
						//Write data to file
						fprintf(fp, "%s", data);
						//Close the file
						fclose(fp);
						fprintf(stdout, "File Retrieval Successful.\n");
					}
					else if (status == -1) {
						//If get failed, notify user
						fprintf(stderr, "File Retrieval Failed.\n");
					}
					else {
						//If Rio failed, notify user
						fprintf(stderr, "Error: Rio error");
					}
				}
				else {
					fprintf(stderr, "Error: filename is too large.\n");
				}
			}

			else if (strcmp(word, "cdelete") == 0 && arguments == 2) {
				struct DelStruct delrequest;
				delrequest.request = 3;
				delrequest.secretkey = key;
				word = strtok(NULL, " ");
				//Make sure filename is not too large
				if (strlen(word) + 1 <= 80) {
					strcpy(delrequest.filename, word);
					//Send message
					Rio_writen(clientfd, &delrequest.request, 4);
					Rio_writen(clientfd, &delrequest.secretkey, 4);
					Rio_writen(clientfd, &delrequest.filename, 80);
					//Read data from server
					Rio_readn(clientfd, &status, 4);
					//Check if request was success
					if (status == 0) {
						printf("File Deletion Successful.\n");
					}
					else if (status == -1) {
						//If delete failed, notify user
						fprintf(stderr, "File Deletion Failed.\n");
					}
					else {
						//If Rio failed, notify user
						fprintf(stderr, "Error: Rio error");
					}
				}
				else {
					fprintf(stderr, "Error: filename is too large.\n");
				}
			}

			else if (strcmp(word, "clist") == 0 && arguments == 1) {
				struct SendStruct listrequest;
				listrequest.request = 4;
				listrequest.secretkey = key;
				Rio_writen(clientfd, &listrequest.request, 4);
				Rio_writen(clientfd, &listrequest.secretkey, 4);
				//Read data from server
				Rio_readn(clientfd, &status, 4);
				//Check if request was successful
				if (status == 0) {
					char *content[100000];
					//Get number of files
					Rio_readn(clientfd, &filesize, 4);
					//Print file names
					Rio_readn(clientfd, &content, filesize);
					fprintf(stdout, "File Listing Successful.\nLoading List...\n");
					fprintf(stdout, "%s", content);
				}
				else if (status == -1) {
					//If list failed, notify user
					fprintf(stderr, "File Listing Failed.\n");
				}
				else {
					//If Rio failed, notify user
					fprintf(stderr, "Error: Rio error");
				}
			}

			else if (strcmp(word, "quit") == 0 && arguments == 1) { 
				//end program
				break; 
			}
			else {
				fprintf(stderr, "Error: invalid command.\n");
			}
		}
	}

	Close(clientfd); //line:netp:echoclient:close
	exit(0);
}
/* $end echoclientmain */
