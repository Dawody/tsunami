
#include "signal.h"
#include "clkUtilities.h"
#include "queueUtilities.h"
#include <unistd.h>
//#include <stream>
//#include <iostream>

void ClearResources(int);

int main() {


    signal(SIGINT,ClearResources);
    initQueue(true);
    //TODO:
    // 1-Ask the user about the chosen scheduling Algorithm and its parameters if exists.


    char scdlr_type[1];
    while(scdlr_type[0] != '1' && scdlr_type[0] != '2' && scdlr_type[0] != '3')
    {
	    printf("\nPlease choose scheduling algorithm :\n(1):Non-preemptive HPF\n(2):Shortest Remaining Time Next\n(3):Round Robin\n");
	    int ref = scanf("%1s",scdlr_type);
    }


    // 2-Initiate and create Scheduler and Clock processes.


    int scdlr_pid=fork();
    if(scdlr_pid==-1)
    {
      printf("error in forking the scheduler!\n");
    }
    if (scdlr_pid==0)
    {
      //child:scheduler
      //at first i force my schedular to work on single cpu
      char cmd[80]="taskset -p 0x00000001 \0";
      strcat(cmd,(to_string(getpid())).c_str());
      strcat(cmd,(go_to_hell).c_str());
      system(cmd);
      //second i exec the schedular
      char* argv[]={"sch.out",scdlr_type,0};
      execve(argv[0],&argv[0],NULL);
    }



    int clk_pid=fork();
    if(clk_pid==-1)
    {
      printf("\nerror in forking clock!\n");
    }

    if(clk_pid==0)
    {
	    //child:clk
      char cmd[80]="taskset -p 0x00000001 \0";
      strcat(cmd,(to_string(getpid())).c_str());
      strcat(cmd,(go_to_hell).c_str());
      system(cmd);
      char cstr[5] ;
      strcpy(cstr, (num_to_str(scdlr_pid)).c_str());
      char* argv[]={"process.out",cstr,0};
	    execve("clock.out",&argv[0],NULL);
    }






    // 3-use this function AFTER creating clock process to initialize clock, and initialize MsgQueue
    initClk();



    //TODO:  Generation Main Loop
    //4-Creating a data structure for process  and  provide it with its parameters
    processData pros;
    priority_queue<processData> pros_qu;
    string str;
    int pros_info;
    int counter=0;
    FILE *input = fopen("processes.txt","r");

    for(int i =0 ; i<4; i++)
    fscanf(input,"%s",&str);

    while(fscanf(input,"%d",&pros_info)!= EOF)
    {
      pros.id=pros_info;

      fscanf(input,"%d",&pros_info);
      pros.arrivaltime=pros_info;

      fscanf(input,"%d",&pros_info);
      pros.runningtime=pros_info;
      pros.remainingtime=pros_info;

      fscanf(input,"%d",&pros_info);
      pros.priority=pros_info;

      pros.pros_pid=0;
      pros.scdl_algo=0;
      pros.rr_time=counter++;
      pros.wait_time=0;
      pros.stop_time=0;

      pros_qu.push(pros);

    }

//test: show all process that i get from the input file sorted by arrival time

//    show_queue(pros_qu); //:test the input operation and sorting according to arrival time


    //5-Send & Notify the information to  the scheduler at the appropriate time
    //(only when a process arrives) so that it will be put it in its turn.
    int y=getClk();

    while(!pros_qu.empty())
    {
      int x= getClk();
      pros=pros_qu.top();
      if(x==pros.arrivaltime)
      {
        x= Sendmsg(pros);
//        printf("$$$$$$$$$$$$$$$$$$$new process at time %d$$$$$$$$$$$$$$$$$$$$\n",getClk());
        kill(scdlr_pid,SIGFPE);
        pros_qu.pop();
      }
    }

    lastSend();

    int stat_loc;
    wait(&stat_loc);
    if(!(stat_loc & 0x00FF))


    ClearResources(0);




}

void ClearResources(int)
{
    msgctl(qid, IPC_RMID, (struct msqid_ds*)0);
    destroyClk(true);
    exit(0);
}
