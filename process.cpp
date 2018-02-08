#include "signal.h"
#include "clkUtilities.h"
#include "queueUtilities.h"
#include <unistd.h>

/* Modify this file as needed*/
int remainingtime =0,scdlr_type;
int main(int agrc, char* argv[]) {

    //if you need to use the emulated clock uncomment the following line
    initClk();


    //TODO: it needs to get the remaining time from somewhere
    remainingtime = str_to_num(argv[1],1);
    scdlr_type = str_to_num(argv[2],1);
    int x=0;



    while(remainingtime>0) {
//      printf("==============================================================start loop at  time =%d\n",getClk());

       remainingtime--;
       sleep(1);
       x++;
    //   printf("==================iam process number %d and my remaining time now is %d\n",getpid(),remainingtime);
    //   printf("==============================================================before killing time =%d\n",getClk());
       //kill(getppid(),SIGALRM);
       if(scdlr_type==3 && x%3==0)
       kill(getpid(),SIGSTOP);
  //     printf("==============================================================after killing time =%d\n",getClk());

    }
    //if you need to use the emulated clock uncomment the following line
    destroyClk(false);
    return 0;
}
