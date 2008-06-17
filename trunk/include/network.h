/*
 * network.h
 * 
 * Rafael dos Santos Alves (rafael2710@gmail.com)
 * Viviane de França Oliveira (viviane.oliveira123@gmail.com)
 *
 */
#ifndef __NETWORK_H___
#define __NETWORK_H___

#include  <stdio.h>
#include  <stdlib.h>
#include  <unistd.h>
#include  <errno.h>
#include  <string.h>
#include  <netdb.h>
#include  <sys/types.h>
#include  <netinet/in.h>
#include  <sys/socket.h>
#include  <getopt.h>

#include "types.h"
#include "error.h"
#include "const.h"

int LoadDefaultParameters(char *server,int *port,char *from,char *to,char *body,char *subject, boolean verbose);

errortype connectToTCPServer(char * server, unsigned short port, boolean verbose, int * sockfd);
errortype disconnectToTCPServer(int sockfd, boolean verbose);
errortype sendTCPData(int sockfd, byte *message, unsigned long size, boolean verbose);
errortype receiveTCPData(int sockfd, byte *data, unsigned long * size, boolean verbose);
errortype sendMail(char *server, unsigned short port, char *from, char *to, char *cc, char *bcc, 
		char *subject, char *body,char *attachment,boolean verbose);

errortype encodeBlock64(char textbuf[], char codebuf[],boolean verbose);
errortype encodeLastBlock64(char textbuf[], char codebuf[], int missingBytes,boolean verbose);
errortype encode64(char *inFilename, char *outFilename,boolean verbose);
errortype decodeBlock64(char codebuf[], char textbuf[],boolean verbose);
errortype decode64(char inFilename[], char outFilename[],boolean verbose);

#endif
