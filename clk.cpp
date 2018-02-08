#include "clkUtilities.h"
#include <string>

using namespace std;
/* This file is done for you,
Probably you will not need to change anything
This file represents an emulated clock for the simulation purpose only
it is not a real part of operating system */

int shmid;
/*clear the resources before exit*/
void cleanup(int x)
{
  shmctl( shmid,IPC_RMID,NULL);
  printf("=====================================================\nClock terminating \n");
  raise(9);
}

int str_to_num(string s,int mul){
if(!s.size())return 0;
return (s[s.size()-1]-'0')*mul+str_to_num(s.substr(0,s.size()-1),mul*10);
}

/* this file represents the system clock for ease of calculations*/
int main(int agrc, char* argv[]) {
  printf("Clock Starting\n========================================================\n");
  signal(SIGINT,cleanup);
  int clk=0;

 //Create shared memory for one integer variable 4 bytes
 shmid = shmget(SHKEY, 4, IPC_CREAT|0644);
 if((long)shmid == -1)
  	{
  	  perror("Error in create shm");
  	  exit(-1);
  	}

 int * shmaddr = (int*) shmat(shmid, (void *)0, 0);
  if((long)shmaddr == -1)
  {
  	perror("Error in attach in parent");
  	exit(-1);
  }
  else
  {
   	*shmaddr = clk;		/* initialize shared memory */
  }
  int scdlr_pid=str_to_num(argv[1],1);
   while(1)
   {
       sleep(1.5);
       (*shmaddr)++;
       //i send signal to the sdlr to send it to the running process ; iuse this signal actually in RR schedler
      // kill(scdlr_pid,SIGALRM);

   }

}
