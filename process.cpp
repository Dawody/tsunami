#include "signal.h"
#include "clkUtilities.h"
#include "queueUtilities.h"
#include <unistd.h>

/* Modify this file as needed*/
int remainingtime =0;

void interrut(int signo){
  printf("i'm the process my parent killed me beacuse my grand father interrupted him !\n");
  exit(0);
}

int main(int agrc, char* argv[]) {
        signal(SIGFPE,interrut);
        printf("process with remianing time = %s\n",argv[1] );

    //if you need to use the emulated clock uncomment the following line
    //initClk();

    //TODO: it needs to get the remaining time from somewhere
    remainingtime = str_to_num(argv[1],1);

    while(remainingtime>0) {

       remainingtime--;
       printf("i worked in running time =%d\n",remainingtime+1);
       sleep(1);
    }
    //if you need to use the emulated clock uncomment the following line
    //destroyClk(false);
    return 0;
}
