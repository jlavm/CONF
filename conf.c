/*
audioSimple record|play|duplex [-b(8|16)] [stereo] [-vVOLUME] [-dDURATION] [-fFREQUENCY] fileName

Examples on how the program can be started:
./audioSimple record -b16 audioFile
./audioSimple record audioFile
./audioSimple -b8 stereo -v90 -f8000 audioFile

To compile, execute
	make
from the command line (provided that Makefile and all the .c and .h files required are in the same directory)

Operations:
- store: reads from sound card and writes to a file
- play: the opposite to 'store'
- duplex: sets soundcard to duplex mode,  writes to a file the recorded sound (and also plays it)

-b 8 or 16 bits per sample
VOL volume [0..100]
DUR duration in seconds; 0 or unspecified means that Ctrol-C is the only way to stop
FRE sampling frequency in Hz

default values:  8 bits, vol 30, dur 0, sampling frequency 8000, mono

*/



/* -------------------------------------------------------------------------------- */
/* INCLUDES */

#include "confArgs.h"
#include "conf.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <strings.h>
//#include "configureSndcard.h"



/* -------------------------------------------------------------------------------- */
/* CONSTANTS */

const int BUFFER_SIZE = 4096; /* unit of data to work with: fragment to be used in the soundcard and data to read/write on each request to the soundcard */
const int MaxLongSize = 100; /* max filename size */
const int IpSize = 16; /* max filename size */
const char message[16]= "Sent from host2";

#define MAXBUF 256
/* change the following multicast address if you want to avoid collisions. Note that you must change it here AND ALSO in host1 code */
#define GROUP "225.0.1.1"




/*=====================================================================*/
/* activated by Ctrol-C */
/*void signalHandler (int sigNum)
{
  printf ("audioSimple was requested to finish\n");
  exit (-1);
}*/



/*=====================================================================*/
int main(int argc, char *argv[])
{
 
  //struct sigaction sigInfo; /* signal conf */

  /*Variables para captura de argumentos*/

  int operation;
  char firstIP [IpSize];
  char firstMulticastIP [IpSize];
  int port;
  int vol; 
  int packetDuration; 
  int verbose; 
  int payload; 
  int bufferingTime;

  

  /* we configure the signal */
  //sigInfo.sa_handler = signalHandler;
  //igInfo.sa_flags = 0;  
  //CALL (sigaction (SIGINT, &sigInfo, NULL), "Error installing signal"); 

  /* obtain values from the command line - or default values otherwise */
  captureArguments (argc, argv, &operation,firstIP, firstMulticastIP, &port, &vol, &packetDuration, &verbose, &payload, &bufferingTime);


  /* soundcard configuration */
  //descriptorSnd = 0; /* this is required to request configSnd to open the device for the first time */

  /* create snd descritor and configure soundcard to given format, frequency, number of channels. We also request setting the fragment to BUFFER_SIZE size - when BUFFER_SIZE is set to a power-of-two value, the configured fragment size - returned in 'requestedFragmentSize' should be equal to BUFFER_SIZE */
  //requestedFragmentSize = BUFFER_SIZE;
 /// configSndcard (&descriptorSnd, &datQualitySnd, &requestedFragmentSize); 
  //vol = configVol (datQualitySnd.channels, operation, descriptorSnd, vol);
   

  /* obtained values -may differ slightly - eg. frequency - from requested values */
  //printValues (operation, datQualitySnd, vol, duration, nombFich);
  //printf ("Duration of each packet exchanged with the soundcard :%f\n", (float) requestedFragmentSize / (float) ((datQualitySnd.channels) * (datQualitySnd.format /8) * (datQualitySnd.freq)));


 // numBytes = duration * (datQualitySnd.channels) * (datQualitySnd.format /8) * (datQualitySnd.freq); /* note that this is the actual frequency provided by the device */
  
  printValues (&operation, firstIP,firstMulticastIP, &port, &vol,  &packetDuration, &verbose, &payload, &bufferingTime);


  if (operation ==FIRST){
  printf("se eligio first\n");

  }
  else if (operation == SECOND){
  printf("se eligio second\n");

  }
  else{


  }

  //close (descriptorSnd);

  return 0;
};

int first(int port,char *firstMulticastIP){

/*char * firstMulticastIP, int * port, int *vol, int * packetDuration, int * verbose, int * bufferingTime/*

/*Variables para configurar conexion*/
 
  int s, r; /* s for socket, r for storing results from system calls */
  struct sockaddr_in local, rem; /* to build address/port info for local node and remote node */ 
  struct ip_mreq mreq; /* for multicast configuration */
  char buf[MAXBUF]; /* to receive data from remote node */
  socklen_t  from_len; /* to store the length of the address returned by recvfrom */
 
 /* preparing bind */
  bzero(&local, sizeof(local));
  local.sin_family = AF_INET;
  local.sin_port = htons(port); /* besides filtering, this assures that info is being sent with this PORT as local port */


  if (inet_pton(AF_INET, firstMulticastIP, &local.sin_addr) < 0) {
    perror("inet_pton");
    return 1;
  }
  
  /* creating socket */
  if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket");
    return 1;
  }

  /* binding socket - using mcast local address */
  if (bind(s, (struct sockaddr *)&local, sizeof(local)) < 0) {
    perror("bind");
    return 1;
  }

/* setsockopt configuration for joining to mcast group */
  if (inet_pton(AF_INET, firstMulticastIP, &mreq.imr_multiaddr) < 0) {
    perror("inet_pton");
    return 1;
  }
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);

  if (setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
    perror("setsockopt");
    return 1;
  }

  from_len = sizeof (rem); /* remember always to set the size of the rem variable in from_len */  
  if ((r = recvfrom(s, buf, MAXBUF, 0, (struct sockaddr *) &rem, &from_len)) < 0) {
    perror ("recvfrom");
    } else {
          buf[r] = 0; /* convert to 'string' by appending a 0 value (equal to '\0') after the last received character */
          printf("First: Received message in multicast address. Message is: %s\n", buf); fflush (stdout);
    }
  if ( (r = sendto(s, message, sizeof(message), /* flags */ 0, (struct sockaddr *) &rem, sizeof(rem)))<0) {
    perror ("sendto");
      } else {
      buf[r] = 0;
      printf("First: Sent message to Unicast address\n"); fflush (stdout);
    } 
return 0;
}

int second(int port,char *firstAddress){

/*char * firstAddress, int * port, int *vol, int * packetDuration, int *payload, int * verbose, int * bufferingTime*/

  uint_32 ip; /* for multicast configuration */
  int s, r; /* s for socket, r for storing results from system calls */
  struct sockaddr_in local, remToSend, remToRecv; /* to build address/port info for local node and remote node */
  char buf[MAXBUF]; /* to receive data from remote node */

  socklen_t from_len; /* for recvfrom */   
  
  /* preparing bind */
  bzero(&local, sizeof(local));
  local.sin_family = AF_INET;
  local.sin_port = htons(port);
  local.sin_addr.s_addr = htonl (inet_pton (AF_INET,firestAddress,0));

  
  if ((s = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket");
    return 1; /* failure */
  }

  /* binding socket (to unicast local address) */
  if (bind(s, (struct sockaddr *)&local, sizeof(local)) < 0) {
    perror("bind");
    return 1;
  }

  if (inet_pton(AF_INET, firstMulticastIP, &mreq.imr_multiaddr) < 0) {
    perror("inet_pton");
    return 1;
  }

 
  /* building structure to identify address/port of the remote node in order to send data to it */
  bzero(&remToSend, sizeof(remToSend));

  remToSend.sin_family = AF_INET;
  remToSend.sin_port = htons(port);
  if (inet_pton(AF_INET, GROUP, &remToSend.sin_addr) < 0) {
    perror("inet_pton");
    return 1;
  }


/* Using sendto to send information. Since I've bind the socket, the local (source) port of the packet is fixed. In the rem structure I set the remote (destination) address and port */ 
if ( (r = sendto(s, message, sizeof(message), /* flags */ 0, (struct sockaddr *) &remToSend, sizeof(remToSend)))<0) {
  perror ("sendto");
    } else {
      buf[r] = 0;
      printf("Host1: Using sendto to send data to multicast destination\n"); fflush (stdout);
    }
  

/* we do not need to fill in the 'remToRecv' variable, but this structure appears with the address and port of the remote node from which the packet was received. 
However, we need to provide in advance the maximum amount of memory which recvfrom can use starting from the 'remToRecv' pointer - to allow recvfrom to be sure that it does not exceed the available space. To do so, we need to provide the size of the 'remToRecv' variable */
from_len = sizeof (remToRecv); 

/* receives from any who wishes to send to host1 in this port */  
  if ((r = recvfrom(s, buf, MAXBUF, 0, (struct sockaddr *) &remToRecv, &from_len)) < 0) {
      perror ("recvfrom");
    } else {
      buf[r] = 0;
      printf("Host1: Message received from unicast address. The message is: %s\n", buf); fflush (stdout);
    }
return 0;
}

