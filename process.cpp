
#include "headers.h"

/* Modify this file as needed*/
int remainingtime =0;


int main(int agrc, char* argv[]) {

  printf("%s\n",argv[1] );

    //if you need to use the emulated clock uncomment the following line
    //initClk();

    //TODO: it needs to get the remaining time from somewhere
    //remainingtime = ??;

    while(remainingtime>0) {
       sleep(1);
       remainingtime--;
    }
    //if you need to use the emulated clock uncomment the following line
    //destroyClk(false);
    return 0;
}
