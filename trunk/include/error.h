/*
 * error.h
 * 
 * Rafael dos Santos Alves (rafael2710@gmail.com)
 * Viviane de França Oliveira (viviane.oliveira123@gmail.com)
 *
 */
#ifndef __ERROR_H____
#define __ERROR_H____

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//#include "network.h"
#include "const.h"
#include "types.h"

#define BadMailAddress 				1700
#define MissingInputFrom  			1701
#define MissingInputTo  			1702
#define InvalidInput		   		1703
#define ErrorConnectSMTPServer 	1704
#define ErrorHelloSMTPServer 		1705
#define ErrorDataMsgSMTPServer	1706
#define ErrorDataSMTPServer		1707
#define ErrorQuitSMTPServer		1708
#define FileNotOpened				1709

void printerror(errortype error, boolean verbose);

#endif
