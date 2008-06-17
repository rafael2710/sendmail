/*
 * network.c
 * 
 * Rafael dos Santos Alves (rafael2710@gmail.com)
 * Viviane de França Oliveira (viviane.oliveira123@gmail.com)
 *
 */

#include "network.h"

errortype sendTCPData(int sockfd, byte *message, unsigned long size, boolean verbose){
	if (send(sockfd, message, size, 0) == -1){
		if(verbose)
			perror("send");
		return errno;
	}
	if(verbose)
		printf ("Sent:%s", message);
	return 0;
}

///////////////////////////////////////////////////////////////////////////

errortype receiveTCPData(int sockfd, byte *data, unsigned long * size, boolean verbose){
	bzero(data,MAXDATASIZE);
	if ((*size = recv(sockfd, data, MAXDATASIZE, 0) == -1)){
		if(verbose)
			perror("recv");
		return errno; //exit (-1);
	}
	if(verbose){
		printf ("Received: %s", data);
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////

int LoadDefaultParameters(char *server,int *port,char *from,char *to,char *body,char *subject, boolean verbose){
	FILE *Pfile = fopen("config.txt","r");
	char *buf=malloc(MAXDATASIZE);
	char *buf2;
	if(Pfile==NULL)
		return errno;
	//read the file ....
	while(fgets(buf,MAXDATASIZE,Pfile)){
		buf=strtok(buf,":");
		buf2=strtok(NULL,"\n");
#ifdef _MY_DEBUG_	
		printf("%s, %s\n",buf,buf2);
#endif
		if(strcmp(buf,"server")==0)
			strcpy(server,buf2);
		else
		if(strcmp(buf,"body")==0)
			strcpy(body,buf2);
		else
		if(strcmp(buf,"from")==0)
			strcpy(from,buf2);
		else
		if(strcmp(buf,"to")==0)
			strcpy(to,buf2);
		else
		if(strcmp(buf,"port")==0)
			*port=atoi(buf2);
		else
		if(strcmp(buf,"subject")==0)
			strcpy(subject,buf2);
	}
#ifdef _MY_DEBUG_
	printf("### Default Parameters: ####\nserver: %s, body: %s, from: %s, to: %s, port: %d, subject: %s\n #############\n",server,body,from,to,*port,subject);
#endif
	free(buf);
	buf=NULL;
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////

errortype sendMail(char *server, unsigned short port, char *from, char *to, char *cc, char *bcc, 
		char *subject, char *body,char *attachment,boolean verbose){
	int i=0;
	int to_num=0,cc_num=0,bcc_num=0;
	byte *message = malloc(MAXDATASIZE);
	byte *buf = malloc(MAXDATASIZE);
	int sockfd;
	errortype error;
	unsigned long size;
	char * to_buf[MAXNAMESIZE];
	char * cc_buf[MAXNAMESIZE];
	char * bcc_buf[MAXNAMESIZE];
	char attachment_name[MAXNAMESIZE];
	char attachment_name_w_path[MAXNAMESIZE];

	int attachment_flag=strcmp(attachment,"");

	if(attachment_flag>0 && verbose)
		printf("Have attachment");

	if(attachment_flag>0){
		char *attachment_64=strrchr(attachment,'/');
		if(attachment_64 != NULL)
			attachment_64++;
		else
			strcpy(attachment_64,attachment);
#ifdef _MY_DEBUG_
		printf("\nattachment: %s, 64: %s\n",attachment,attachment_64);
#endif
		strcpy(attachment_name_w_path,"tmp/");
		strcat(attachment_name_w_path,attachment_64);
		strcpy(attachment_name,attachment_64);
		encode64(attachment,attachment_name_w_path,verbose);
	}

	if(strlen(to)>0){
		i=0;
		to_buf[i]=strtok(to,",");
		i++;
		while(1){
			to_buf[i]=strtok(NULL,",");
			if(to_buf[i]==NULL)
				break;
			i++;
		}
		to_num=i;
	}

	if(strlen(cc)>0){
		i=0;
		cc_buf[i]=strtok(cc,",");
		i++;
		while(1){
			cc_buf[i]=strtok(NULL,",");
			if(cc_buf[i]==NULL)
				break;
			i++;
		}
		cc_num=i;
	}

	if(strlen(bcc)>0){
		i=0;
		bcc_buf[i]=strtok(bcc,",");
		i++;
		while(1){
			bcc_buf[i]=strtok(NULL,",");
			if(bcc_buf[i]==NULL)
				break;
			i++;
		}
		bcc_num=i;
	}
#ifdef _MY_DEBUG_
	printf("DEBUG: to_num=%d, cc_num=%d, bcc_num=%d\n",to_num,cc_num,bcc_num);
#endif
	error = connectToTCPServer(server, port, verbose, &sockfd);
	if(error!=0){
		return error;
	}
#ifdef _MY_DEBUG_
	printf("ok\n");
#endif
	error = receiveTCPData(sockfd, message, &size, verbose);
	if(error!=0){
		return error;
	}
	if(message[0] != '2')
		return ErrorConnectSMTPServer;

#ifdef _MY_DEBUG_
	printf("ok\n");
#endif
	bzero(message,MAXDATASIZE);
	strcpy(message,"HELO del.ufrj.br\n");
	size = strlen(message);
	error = sendTCPData(sockfd, message, size, verbose);
	if(error!=0){
		return error;
	}
#ifdef _MY_DEBUG_
	printf("ok\n");
#endif
	error = receiveTCPData(sockfd, message, &size, verbose);
	if(error!=0){
		return error;
	}
	if(message[0] != '2')
		return ErrorHelloSMTPServer;
	bzero(message,MAXDATASIZE);
	strcpy(message,"MAIL FROM:<");
	strcat(message,from);
	strcat(message,">\n");
	size = strlen(message);
	error = sendTCPData(sockfd, message, size, verbose);
	if(error!=0){
		return error;
	}
	error = receiveTCPData(sockfd, message, &size, verbose);
	if(error!=0){
		return error;
	}
	if(message[0] != '2')
		return BadMailAddress;
	// to, cc e bcc	

	for(i=0;i<to_num;i++){
		//SendMessage(to,sockfd,'t',verbose);//SendMessage(to[i],sockfd,'t');
		bzero(message,MAXDATASIZE);
		strcpy(message,"RCPT TO:<");
		strcat(message,to_buf[i]);
		strcat(message,">\n");
		size = strlen(message);
		error = sendTCPData(sockfd, message, size, verbose);
		if(error!=0){
			return error;
		}
		error = receiveTCPData(sockfd, message, &size, verbose);
		if(error!=0){
			return error;
		}
		if(message[0] != '2')
			return BadMailAddress;
	}
	for(i=0;i<cc_num;i++){
		//SendMessage(cc,sockfd,'t',verbose);//SendMessage(cc[i],sockfd,'t');
		bzero(message,MAXDATASIZE);
		strcpy(message,"RCPT TO:<");
		strcat(message,cc_buf[i]);
		strcat(message,">\n");
		size = strlen(message);
		error = sendTCPData(sockfd, message, size, verbose);
		if(error!=0){
			return error;
		}	
		error = receiveTCPData(sockfd, message, &size, verbose);
		if(error!=0){
			return error;
		}
		if(message[0] != '2')
			return BadMailAddress;

	}
	for(i=0;i<bcc_num;i++){
		//SendMessage(bcc,sockfd,'t',verbose);//SendMessage(bcc[i],sockfd,'t');
		bzero(message,MAXDATASIZE);
		strcpy(message,"RCPT TO:<");
		strcat(message,bcc_buf[i]);
		strcat(message,">\n");
		size = strlen(message);
		error = sendTCPData(sockfd, message, size, verbose);
		if(error!=0){
			return error;
		}
		error = receiveTCPData(sockfd, message, &size, verbose);
		if(error!=0){
			return error;
		}
		if(message[0] != '2')
			return BadMailAddress;
	}
	bzero(message,MAXDATASIZE);
	strcpy(message,"DATA\n");
	size = strlen(message);
	error = sendTCPData(sockfd, message, size, verbose);
	if(error!=0){
		return error;
	}
	error = receiveTCPData(sockfd, message, &size, verbose);
	if(error!=0){
		return error;
	}
	if(message[0] != '3')
		return ErrorDataMsgSMTPServer;

	bzero(message,MAXDATASIZE);	
	strcpy(message,"From: ");
	strcat(message,from);

	// to, cc e bcc	

	for(i=0;i<to_num;i++){
		strcat(message,"\nTo: ");
		strcat(message,to_buf[i]);//strcat(buf,to[i]);
	}
	for(i=0;i<cc_num;i++){
		strcat(message,"\nCc: ");
		strcat(message,cc_buf[i]);//strcat(buf,cc[i]);
	}
	for(i=0;i<bcc_num;i++){
		strcat(message,"\nBcc: ");
		strcat(message,bcc_buf[i]);//strcat(buf,bcc[i]);
	}

	strcat(message,"\nSubject: ");
	strcat(message,subject);
	if(attachment_flag>0){
		strcat(message,"\nContent-Type: multipart/mixed; boundary=\"abcdefghijkl\"");
		strcat(message,"\n\n--abcdefghijkl\n");
		strcat(message,"Content-Type: text/plain; charset=US-ASCII\n\n");
	}
	if(attachment_flag==0)
		strcat(message,"\n\n");
	strcat(message,body);
	
	if(attachment_flag>0){
		strcat(message,"\n\n--abcdefghijkl\n");
		strcat(message,"Content-Type:application/octet-stream;\n");
		strcat(message,"Content-Transfer-Encoding: base64\n");
		strcat(message,"Content-Disposition: attachment;\n");
		strcat(message," filename= \"");
		strcat(message,attachment_name);
		strcat(message,"\"\n\n");
		size = strlen(message);
		error = sendTCPData(sockfd, message, size, verbose);	
		if(error!=0){
			return error;
		}
		bzero(message,MAXDATASIZE);	
		FILE *attach = fopen(attachment_name_w_path,"r");
		while(fgets(buf,MAXDATASIZE,attach)){
			size = strlen(buf);
			error = sendTCPData(sockfd, buf, size, verbose);	
			if(error!=0){
				return error;
			}
		}
		strcpy(message,"\n--abcdefghijkl--\n");
		fclose(attach);
		char cmd[MAXNAMESIZE];
		strcpy(cmd,"rm ");
		strcat(cmd,attachment_name_w_path);
		system(cmd);
	}
	
	strcat(message,"\n.\n");
	size = strlen(message);
	error = sendTCPData(sockfd, message, size, verbose);
	if(error!=0){
		return error;
	}
	error = receiveTCPData(sockfd, message, &size, verbose);
	if(error!=0){
		return error;
	}
	if(message[0] != '2')
		return ErrorDataSMTPServer;
	bzero(message,MAXDATASIZE);
	strcpy(message,"quit\n");
	size = strlen(message);
	error = sendTCPData(sockfd, message, size, verbose);
	if(error!=0){
		return error;
	}
	error = receiveTCPData(sockfd, message, &size, verbose);
	if(error!=0){
		return error;
	}
	if(message[0] != '2')
		return ErrorQuitSMTPServer;
	free(message);
	message=NULL;
	error = disconnectToTCPServer(sockfd, verbose);
	if(error!=0){
		return error;
	}
	return 0;
}

/////////////////////////////////////////////////////////////

errortype connectToTCPServer(char * server, unsigned short port, boolean verbose, int * sockfd){
	struct sockaddr_in their_addr;	// connector's address inormation
	struct hostent *he;
	if ((he = gethostbyname (server)) == NULL){// get the host info
		if(verbose)
			perror ("gethostbyname");
		return errno;
	}
#ifdef _MY_DEBUG_
	printf("Host Name: %s, Address Type: %d, Address Length: %d\n ",he->h_name, he->h_addrtype, he->h_length);
#endif
	if ((*sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1){
		if(verbose)
			perror ("socket");
		return errno;
	}
#ifdef _MY_DEBUG_
	printf("ok\n");
#endif
	their_addr.sin_family = AF_INET;	// host byte order
	their_addr.sin_port = htons(port);	// short, network byte order
	their_addr.sin_addr = *((struct in_addr *) he->h_addr);
	memset (their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);
#ifdef _MY_DEBUG_
	printf("ok\n");
#endif
	if (connect(*sockfd,(struct sockaddr *)&their_addr,sizeof(their_addr)) == -1){
		if(verbose)
			perror ("connect");
		return errno; 
	}
	return 0;
}

/////////////////////////////////////////////////////////////

errortype disconnectToTCPServer(int sockfd, boolean verbose){
	if(close(sockfd)==-1){
		if(verbose)	
			perror("close");
		return errno;
	}
	return 0;
}

///////////////////////////////////////////////////////////////
errortype encodeBlock64(char textbuf[], char codebuf[],boolean verbose){
	char codeTable [64] = {
		'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
		'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
		'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
		'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
	};

	codebuf[0] = codeTable[(textbuf[0] & 0xFC) >> 2];
	codebuf[1] = codeTable[((textbuf[0] & 0x03) << 4) | ((textbuf[1] & 0xF0) >> 4)];
	codebuf[2] = codeTable[((textbuf[1] & 0x0F) << 2) | ((textbuf[2] & 0xC0) >> 6)];
	codebuf[3] = codeTable[textbuf[2] & 0x3F];

	return 0;	
}
///////////////////////////////////////////////////////////////
errortype encodeLastBlock64(char textbuf[], char codebuf[], int missingBytes,boolean verbose){
	if(missingBytes==1){textbuf[2]=0;}
	if(missingBytes==2){textbuf[2]=0;textbuf[1]=0;}
	encodeBlock64(textbuf,codebuf,verbose);
	if(missingBytes==1){
		codebuf[3]='=';
	}
	if(missingBytes==2){
		codebuf[3]='=';
		codebuf[2]='=';
	}
	return 0;
}

///////////////////////////////////////////////////////////////
errortype encode64(char *inFilename, char *outFilename, boolean verbose){ 
	FILE *textFile = fopen(inFilename,"r");
	FILE *codeFile = fopen(outFilename,"w");
	char textbuf[3], codebuf[4];
	int count=0;
	if(textFile==NULL)
		return errno;
	if(codeFile==NULL)
		return errno;
	int k=0;
	int read;
	while((read=fscanf(textFile,"%c%c%c",&textbuf[0],&textbuf[1],&textbuf[2]))){
#ifdef _MY_DEBUG_	
		printf("DEBUG: %d, %c:%d, %c:%d, %c:%d\n",read,textbuf[0],textbuf[0],textbuf[1],textbuf[1],textbuf[2],textbuf[2]);
#endif
		if(read==-1)break;
		encodeBlock64(textbuf,codebuf,verbose);
		if(read<3)
			encodeLastBlock64(textbuf,codebuf,3-read,verbose);
		for(k=0;k<4;k++)
			fprintf(codeFile,"%c",codebuf[k]);
		count++;
#ifdef _MY_DEBUG_				
		printf("Text: %s, Code: %s\n",textbuf,codebuf);
#endif
		if(count>=LINELENGTH/4){
			fprintf(codeFile,"\r\n");
			count=0;
		}
	}
	fclose(textFile);
	fclose(codeFile);
	return 0;
}

///////////////////////////////////////////////////////////////
errortype decodeBlock64(char codebuf[], char textbuf[],boolean verbose){
	char codeTable [64] = {
		'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
		'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
		'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
		'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
	};
	int i,j,test;	
	j=0;
	while(j<4){
		test=0;
		for(i=0;i<64;i++){
			if(codeTable[i]==codebuf[j]){
				textbuf[j] = i;
				test=1;
				i=64;
			}
		}
		if(test==0)textbuf[j]=64;	
		j++;
	}	
	textbuf[0] = ((textbuf[0] & 0x03f) << 2) | ((textbuf[1] & 0x030) >> 4);
	if(codebuf[2]==64){}
	else {textbuf[1] = ((textbuf[1] & 0x00f) << 4) | ((textbuf[2] & 0x03c) >> 2);}
	if(codebuf[3]==64){}
	else {textbuf[2] = ((textbuf[2] & 0x003) << 6) | (textbuf[3] & 0x03f);}
#ifdef _MY_DEBUG_
	printf("\nOut: %s\n",textbuf);
#endif
	return 0;
}

///////////////////////////////////////////////////////////////
errortype decode64(char inFilename[], char outFilename[],boolean verbose){
	FILE *codeFile = fopen(inFilename,"r");
	FILE *textFile = fopen(outFilename,"w");
	if(codeFile==NULL)
		return errno;
	if(textFile==NULL)
		return errno;

	int k,lineread=0;
	char textbuf[3],codebuf[4],dum[2];
	int read;
	while((read=fscanf(codeFile,"%c%c%c%c",&codebuf[0],&codebuf[1],&codebuf[2],&codebuf[3]))){
#ifdef _MY_DEBUG_	
		printf("DEBUG: read: %d,lineread: %d %c:%d, %c:%d, %c:%d, %c:%d\n",read,lineread,codebuf[0],codebuf[0],codebuf[1],codebuf[1],codebuf[2],codebuf[2],codebuf[3],codebuf[3]);
#endif
		if(read<0)
			break;
		decodeBlock64(codebuf, textbuf,verbose);
		for(k=0;k<3;k++)
			fprintf(textFile,"%c",textbuf[k]);
		lineread+=read;
		if(lineread==LINELENGTH){
			fscanf(codeFile,"%c%c",&dum[0],&dum[1]);//to read \r \n
			printf("HHH\n");
			lineread=0;
		}
	}
	fclose(textFile);
	fclose(codeFile);
	return 0;
}

