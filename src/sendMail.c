/*
 * sendMail.c
 * 
 * Rafael dos Santos Alves (rafael2710@gmail.com)
 * Viviane de França Oliveira (viviane.oliveira123@gmail.com)
 *
 */
#include "network.h"
#include "error.h"
#include "const.h"
#include "types.h"

int main (int argc, char *argv[])
{
	int port=0;
	char server[MAXNAMESIZE];
	char from[MAXNAMESIZE];
	char to[MAXDATASIZE];
	char cc[MAXDATASIZE];
	char bcc[MAXDATASIZE];
	char subject[MAXDATASIZE];
	char body[MAXDATASIZE];
	char attachment[MAXNAMESIZE];
	
	bzero(server,MAXNAMESIZE);
	bzero(from,MAXNAMESIZE);	
	bzero(to,MAXDATASIZE);
	bzero(cc,MAXDATASIZE);
	bzero(bcc,MAXDATASIZE);
	bzero(subject,MAXDATASIZE);
	bzero(body,MAXDATASIZE);
	bzero(attachment,MAXNAMESIZE);
	
	errortype error;
	int option;
	int idx=0;
	int to_flag=0,from_flag=0,subject_flag=0,body_flag=0;

	int verbose=0;
	opterr = 0;
	static char opts[] = "f:t:A:s:P:S:b:B:c:hvV";
	struct option longopts[]={
		{"from", required_argument, 0, 'f'},
		{"to", required_argument, 0, 't'},
		{"cc", required_argument, 0, 'c'},
		{"bcc", required_argument, 0, 'b'},
		{"Server", required_argument, 0, 'S'},
		{"Port", required_argument, 0, 'P'},
		{"Body", required_argument, 0, 'B'},
		{"subject", required_argument, 0 ,'s'},
		{"Attachment",required_argument, 0,'A'},
		{"help", no_argument, 0, 'h'},
		{"verbose", no_argument, 0, 'v'},
		{"version", no_argument, 0, 'V'},
		{0,0,0,0},
	};
	error=LoadDefaultParameters(server,&port,from,to,body,subject,verbose);
	if(error!=0){
		printerror(error,0);
		exit(-1);
	}
#ifdef _MY_DEBUG_	
	printf("%s, %d, %s, %s, %s\n",server, port, from, body, subject);
#endif
	while((option=getopt_long(argc,argv,opts,longopts,&idx))!=-1){
		switch(option){
		case 0:
		{
			printf("missing option\n");
			break;
		}
		case 'h':
		{
			printf("\nUsage: %s [-h|--help]\n",argv[0]);
			printf("       %s [-f|--from <endereço de origem>] [-t|--to <endereço do destinatário>] [Options]\n",argv[0]);
			printf("\nOptions:\n[-S|--Server <endereço do servidor>]\n[-P|--Port <porta>]\n[-c|--cc <endereço de cópia]\n[-b|--bcc <endereço de cópia oculta>]\n[-s|--subject <assunto>]\n[-B|--Body <corpo da mensagem>]\n[-A|--Attachment <anexos>]\n[-v|--verbose]\n\n");
			exit(-1);
			break;
		}
		case 'v':
		{
			verbose=1;
			printf("\nVerbose Mode\n\n");
			break;
		}
		case 'f':
		{
			bzero(from,MAXNAMESIZE);
			strcpy(from,optarg);
#ifdef _MY_DEBUG_
			printf("Mail from: %s.\n",from);
#endif
			from_flag++;
			break;
		}
		case 't':
		{
			bzero(to,MAXNAMESIZE);
			strcpy(to,optarg);
#ifdef _MY_DEBUG_
			printf("Mail to: %s.\n",to);
#endif
			to_flag++;
			break;
		}
		case 'c':
		{
			bzero(cc,MAXNAMESIZE);
			strcpy(cc,optarg);
#ifdef _MY_DEBUG_
			printf("Mail cc: %s.\n",cc);
#endif
			break;
		}
		case 'b':
		{
			bzero(bcc,MAXNAMESIZE);
			strcpy(bcc,optarg);
#ifdef _MY_DEBUG_
			printf("Mail bcc: %s.\n",bcc);
#endif
			
			break;
		}
		case 'A':
		{
			bzero(attachment,MAXNAMESIZE);
			strcpy(attachment,optarg);
#ifdef _MY_DEBUG_
			printf("Attachment: %s\n",optarg);
#endif
			break;
		}
		case 'S':
		{
			bzero(server,MAXNAMESIZE);
			strcpy(server,optarg);
#ifdef _MY_DEBUG_
			printf("Mail server: %s.\n",server);
#endif
			break;
		}
		case 's':
		{
			bzero(subject,MAXDATASIZE);
			strcpy(subject,optarg);
#ifdef _MY_DEBUG_
			printf("Mail subject: %s.\n",subject);
#endif
			subject_flag++;
			break;
		}
		case 'P':
		{
			port=atoi(optarg);
#ifdef _MY_DEBUG_
			printf("Port used: %d.\n",port);
#endif
			break;
		}
		case 'B':
		{
			bzero(body,MAXDATASIZE);
			strcpy(body,optarg);
#ifdef _MY_DEBUG_
			printf("Mail body: %s\n",body);
#endif
			body_flag++;
			break;
		}
		default:
		{
			printf("Opção inválida!\n");
			exit(-1);
			break;
		}
		}
	}
	if(!from_flag){
		printerror(MissingInputFrom,verbose);
		exit(-1);
	}
	if(!to_flag){
		printerror(MissingInputTo,verbose);
		exit(-1);
	}
	if(!subject_flag){	
		if(verbose)
			printf("Missing optional argument: \'Subject\'\n");//printerror(...)
//		exit(-1);
	}
	if(!body_flag){
		if(verbose)
			printf("Missing optional argument: \'body\'\n");//printerror(...)
//		exit(-1);
	}

	error = sendMail(server, port, from, to, cc, bcc, subject, body, attachment,verbose);
	if(error!=0){
		printerror(error,verbose);
		exit(-1);
	}
		
	return 0;
}
