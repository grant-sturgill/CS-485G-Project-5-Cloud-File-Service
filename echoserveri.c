/*
* echoserveri.c - An iterative echo server
*/
/* $begin echoserverimain */
#include "csapp.h"
#include <dirent.h>

void echo(int connfd);

int main(int argc, char **argv)
{
	int listenfd, connfd, request, key, secretkey, status;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;  /* Enough space for any address */  //line:netp:echoserveri:sockaddrstorage
	char client_hostname[MAXLINE], client_port[MAXLINE], filename[80], *keyinput;

	if (argc != 3) {
		fprintf(stderr, "usage: %s <port> <secretkey>\n", argv[0]);
		exit(0);
	}

	keyinput = argv[2];

	//Convert key to integer
	key = atoi(keyinput);

	listenfd = Open_listenfd(argv[1]);
	while (1) {
		clientlen = sizeof(struct sockaddr_storage);
		connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
		Getnameinfo((SA *)&clientaddr, clientlen, client_hostname, MAXLINE,
			client_port, MAXLINE, 0);
		printf("Connected to (%s, %s)\n", client_hostname, client_port);
		
		size_t n;
		char buf[MAXLINE];
		rio_t rio;

		Rio_readinitb(&rio, connfd);
		//Run server until user exits program
		while (request != 5) {
			//Read request from user to determine what server does
			Rio_readn(connfd, &request, 4);
			if (request == 1) {
				//Read in secret key and file name
				Rio_readn(connfd, &secretkey, 4);
				Rio_readn(connfd, &filename, 80);
				int size;
				//Read in size of file and content
				Rio_readn(connfd, &size, 4);
				char *filecontent[100000];
				Rio_readn(connfd, &filecontent, size);
				//Make sure key matches
				if (key == secretkey) {
					FILE * pFile;
					pFile = fopen(filename, "w");
					if (pFile == NULL) {
						status = -1;
						Rio_writen(connfd, &status, 4);
					}
					//Write content to file
					else {
						fprintf(pFile, "%s", filecontent);
						status = 0;
						Rio_writen(connfd, &status, 4);
						fclose(pFile);
					}
				}
				else {
					status = -1;
					Rio_writen(connfd, &status, 4);
				}
				//Output request
				fprintf(stdout, "%d\tSend\t%s\t%d\n", key, filename, status);
			}

			//Handle request to retrieve file
			else if (request == 2) {
				Rio_readn(connfd, &secretkey, 4);
				Rio_readn(connfd, &filename, 80);
				if (key == secretkey){
					FILE * pFile;
					char str[1000];
					char content[100000];
					pFile = fopen(filename, "r");
					if (pFile == NULL) {
						status = -1;
						Rio_writen(connfd, &status, 4);
					}
					else {
						while (fgets(str, 1000, pFile) != NULL) {
							strcat(content, str);
						}
						status = 0;
						int size = sizeof(content);
						//Send status
						Rio_writen(connfd, &status, 4);
						//Send file size
						Rio_writen(connfd, &size, 4);
						//Send content of file
						Rio_writen(connfd, &content, size);
						fclose(pFile);
					}
				}
				else {
					status = -1;
					Rio_writen(connfd, &status, 4);
				}
				fprintf(stdout, "%d\tRetrieve\t%s\t%d\n", key, filename, status);
			}

			//Handle request to delete file
			else if (request == 3) {
				Rio_readn(connfd, &secretkey, 4);
				Rio_readn(connfd, &filename, 80);
				if (key == secretkey) {
					//Attempt to remove file
					if (remove(filename) == 0) {
						status = 0;
						Rio_writen(connfd, &status, 4);
					}
					//If file doesn't exist, return status of -1
					else {
						status = -1;
						Rio_writen(connfd, &status, 4);
					}
				}
				//If secret key is wrong, return status of -1
				else {
					status = -1;
					Rio_writen(connfd, &status, 4);
				}
				//output command code
				fprintf(stdout, "%d\tDelete\t%s\t%d\n", key, filename, status);
			}

			else if (request == 4) {
				Rio_readn(connfd, &secretkey, 4);
				if (key == secretkey) {
					//Open directory
					DIR *d;
					struct dirent *dir;
					d = opendir(".");
					if (d)
					{
						//Turn list of files into string
						char content[100000];
						while ((dir = readdir(d)) != NULL)
						{
							strcat(content, dir->d_name);
							strcat(content, "\n");
						}
						strcat(content, "\0");
						closedir(d);
						//Send string to client
						int size = sizeof(content);
						status = 0;
						Rio_writen(connfd, &status, 4);
						Rio_writen(connfd, &size, 4);
						Rio_writen(connfd, &content, size);
					}
					else {
						status = -1;
						Rio_writen(connfd, &status, 4);
					}
				}
				else {
					status = -1;
					Rio_writen(connfd, &status, 4);
				}
				fprintf(stdout, "%d\tList\tNONE\t%d\n", key, status);
			}
		}
		Close(connfd);
	}
	exit(0);
}
/* $end echoserverimain */
