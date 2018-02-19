Grant Sturgill
CS485G
Project 5 "Cloud File Service"
12.6.17

This program runs a simple cloud file service

To run the program, follow this process:
	1. Open two seperate connections. Be sure that all files are in both connection locations
	2. On Connection A, use the command "make server" to compile the server code
	3. On Connection A, run the server code by using the command "./myserver <port> <secretkey>"
	4. On Connection B, use the command "make client" to compile the client code
	5. On Connection B, use the command "./myclient <hostname> <port> <secretkey>" to run the client code
		>To run the code on the same machine, use "localhost" in place of <hostname>
	!!IMPORTANT!! For the program to function correctly, both port and secretkey must be the same for both commands

The program comes with the following options to manipulate files
	1. cput <filename>
		This command moves the designated file to the server
	2. cget <filename> 
		This command retrieves the target file and stores it in the client
	3. cdelete <filename>
		This command deletes the target file from the server
	4. clist
		This command lists all files in the present working directory of the server
	5. quit
		This command exits the program and terminates the connection

Two sample files to manipulate with this code, "myfile.txt" and "testfile.txt", are included

