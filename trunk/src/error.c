/*
 * error.c
 * 
 * Rafael dos Santos Alves (rafael2710@gmail.com)
 * Viviane de França Oliveira (viviane.oliveira123@gmail.com)
 *
 */
#include "error.h"

void printerror(errortype error, boolean verbose)
{
	char *message = malloc(MAXDATASIZE);
	if(error<1700){
		//errno
		strcpy(message,strerror(error));
	}
	else{
		/*
		#define BadMailAddress 			1700
		#define MissingInputFrom  		1701
		#define MissingInputTo  		1702
		#define InvalidInput		   	1703
		#define ErrorConnectSMTPServer 		1704
		#define ErrorHelloSMTPServer 		1705
		#define ErrorDataMsgSMTPServer		1706
		#define ErrorDataSMTPServer		1707
		#define ErrorQuitSMTPServer		1708
		*/
		if(error==BadMailAddress)
			strcpy(message,"The given e-mail address is invalid");
		else if(error==MissingInputFrom)
			strcpy(message,"The argument \"from\" is mandatory");
		else if(error==MissingInputTo)
			strcpy(message,"The argument \"to\" is mandatory");	
		else if(error==InvalidInput)
			strcpy(message,"Unrecognized argument");
		else if(error==ErrorConnectSMTPServer)
			strcpy(message,"The smtp server returned an error");
		else if(error==ErrorHelloSMTPServer)
			strcpy(message,"The smtp server refused \"HELO\" message");
		else if(error==ErrorDataMsgSMTPServer)
			strcpy(message,"The smtp server refused \"DATA\" message");
		else if(error==ErrorDataSMTPServer)
			strcpy(message,"The smtp server refused Data");
		else if(error==ErrorQuitSMTPServer)
			strcpy(message,"The smtp server refused \"QUIT\" message");			
		else
			strcpy(message,"Unrecognized error");
	}
	printf("An error occurred: %s\n",message);
}
	
