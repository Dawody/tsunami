#include "signal.h"
#include "clkUtilities.h"
#include "queueUtilities.h"


void ClearResources(int);

int main() {
    signal(SIGINT,ClearResources);	//Dawod: clearing resources if user tried to terminate before schedular starts.
    initQueue(true);
    //TODO: 
    // 1-Ask the user about the chosen scheduling Algorithm and its parameters if exists.
    int scdlr_flg=0;
    while(scdlr_flg != 1 && scdlr_flg != 2 && scdlr_flg != 3)
    {
	    printf("\nPlease choose scheduling algorithm :\n(1):Non-preemptive HPF\n(2):Shortest Remaining Time Next\n(3):Round Robin\n");
	    scanf("%d",&scdlr_flg);
    }


    // 2-Initiate and create Scheduler and Clock processes.
    

    // 3-use this function AFTER creating clock process to initialize clock, and initialize MsgQueue
    initClk();
    

    
    //TODO:  Generation Main Loop
    //4-Creating a data structure for process  and  provide it with its parameters 
    //5-Send & Notify the information to  the scheduler at the appropriate time 
    //(only when a process arrives) so that it will be put it in its turn.

    //===================================
    //Preimplemented Functions examples

    /////Toget time use the following function
    int x= getClk();
    printf("current time is %d\n",x);

    //////Tosend something to the scheduler, for example send id 2
    struct processData pD;
    pD.id = 2;
    Sendmsg(pD); //returns -1 on failure;
    //no more processes, send end of transmission message
    lastSend();
    //////////To clear all resources
    ClearResources(0);
    //======================================
    
}

void ClearResources(int)
{
    msgctl(qid, IPC_RMID, (struct msqid_ds*)0);
    destroyClk(true);
	printf("Clock terminating \n"); 
    exit(0);
}




