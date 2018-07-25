/* ---------------------------
 * Project: EG-PMS2-LAN Control over TCP
 * Filename: egpms.c
 * Function:
 * Comment:
 * Requires:
 * Parameters:
 * Return values:
 * Name: Andreas Dolp
 * Date: 05/01/2015
 * Version: 0.1
 * --------------------------- */

#include <stdio.h>	/* printf */
#include <sys/socket.h>	/* socket-functions */
#include <arpa/inet.h>	/* AF_INET, htons */
#include <string.h>	/* strlen */
#include <errno.h>	/* errno */

#define SOCK_DEFAULT_SERVER_ADDR "192.168.101.41"
#define SOCK_DEFAULT_SERVER_PORT 5000

#define MAXLEN 32

extern int errno;

int main (int argc, char *argv[]) {
	char rcv_buffer[MAXLEN];
	char snd_buffer[MAXLEN];

	int sockfd, len, status_code, i, ret;
	int STUPID_number = -1;
	struct sockaddr_in dest;

	printf("Server IP-address: %s\nServer Port: %d\n", SOCK_DEFAULT_SERVER_ADDR, SOCK_DEFAULT_SERVER_PORT);
	printf("\nstarting:\n");

/* initialize socket */
	printf("Initializing socket...\n");
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1) {
		perror("[ERR] Error creating socket descriptor");
		ret = errno;
		goto EXIT_ERROR;
	}
	memset(&dest, 0, sizeof(dest));	/* zero the struct */

	dest.sin_family = AF_INET;
	dest.sin_port = htons(SOCK_DEFAULT_SERVER_PORT);
	dest.sin_addr.s_addr = inet_addr(SOCK_DEFAULT_SERVER_ADDR);
	if(dest.sin_addr.s_addr == INADDR_NONE) {
		perror("[ERR] Unable to set server address");
		ret = errno;
		goto EXIT_ERROR;
	}
	printf("[OK] Socket initialized\n");

/* connect */
	printf("Connecting...\n");
	if(connect(sockfd, (struct sockaddr *)&dest, sizeof(struct sockaddr)) == -1) {
		perror("[ERR] Unable to connect socket");
		ret = errno;
		goto EXIT_ERROR;
	}
	printf("[OK] Connection established\n");

/* zero the buffers */
	for(i; i<MAXLEN; i++) {
		snd_buffer[i] = '\0';
		rcv_buffer[i] = '\0';
	}
printf("Sending: %1x\n", (int)snd_buffer);

/* send INIT data */
	snd_buffer[0] = 0x11;
	printf("Sending: %#1x\n", snd_buffer);
	if(send(sockfd, snd_buffer, strlen(snd_buffer),0) == -1) {
		perror("[ERR] Error sending INIT data");
		ret = errno;
		goto EXIT_ERROR;
	}
	printf("[OK] Sending successfull\n");

/* receive data */
	printf("receiving data...\n");
	len = recv(sockfd, rcv_buffer, MAXLEN, 0);
	if(len < 0) {
		perror("[ERR] Error receiving data");
		ret = errno;
		goto EXIT_ERROR;
	}
	rcv_buffer[len] = '\n';
	printf("[OK] received %d byte:\n%x)\n",len, rcv_buffer);

/* parse received data */
	printf("Calculating auth code...\n");
	char key[8];
	key[0] = 0x20;
        key[1] = 0x20;
        key[2] = 0x20;
        key[3] = 0x20;
        key[4] = 0x20;
        key[5] = 0x20;
        key[6] = 0x20;
        key[7] = 0x31;
/*
        key[0] = 'P';
        key[1] = '0';
        key[2] = 'w';
        key[3] = '3';
        key[4] = 'r';
        key[5] = 0x31;
        key[6] = 0x20;
        key[7] = 0x31;
*/

	char task[4];
	task[0] = rcv_buffer[0];
	task[1] = rcv_buffer[1];
	task[2] = rcv_buffer[2];
	task[3] = rcv_buffer[3];

	char res[4];
	res[0] = (( task[0] ^ key[2] ) * key[0]) ^ (key[6] | (key[4] << 8)) ^ task[2];
	res[2] = (( task[1] ^ key[3] ) * key[1]) ^ (key[7] | (key[5] << 8)) ^ task[3];

	snd_buffer[0] = res[0];
        snd_buffer[1] = res[1];
        snd_buffer[2] = res[2];
        snd_buffer[3] = res[3];

        printf("Sending: %x\n", snd_buffer);
        if(send(sockfd, snd_buffer, strlen(snd_buffer),0) == -1) {
                perror("[ERR] Error sending INIT data");
                ret = errno;
                goto EXIT_ERROR;
        }
        printf("[OK] Sending successfull\n");

/* zero the buffers */
        for(i; i<MAXLEN; i++) {
                snd_buffer[i] = '\0';
                rcv_buffer[i] = '\0';
        }



/* receive data */
        printf("receiving data...\n");
        len = recv(sockfd, rcv_buffer, MAXLEN, 0);
        if(len < 0) {
                perror("[ERR] Error receiving data");
                ret = errno;
                goto EXIT_ERROR;
        }
        rcv_buffer[len] = '\n';
        printf("[OK] received %d byte:\n%x)\n",len, rcv_buffer);



EXIT_ERROR:
	close(sockfd);
	return ret;
}
