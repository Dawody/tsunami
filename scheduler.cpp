#include "clkUtilities.h"
#include "queueUtilities.h"

bool new_pros_flag,finish_pros_flag,process_stopped=false;
int pros_id=999999,stat_loc;
int start_time,end_time;

void interrut(int signo){
  printf("SCHEDULER: i will kill my child because  my parent interrupted me !\n");
  kill(pros_id,SIGSTOP);
  end_time=getClk();
  new_pros_flag=true;
}

void quantum_int(int signo){
  kill(pros_id,SIGINT);
  end_time=getClk();
}

void ignore(int signo){
  printf("SCHEDULER: i will ignore the interrupt request now!\n");
  //kill(pros_id,SIGFPE);
}
/*
void general_ignore(int signo){
  printf("i'm the scheduler , reset the actions of the signals!\n");
  //kill(pros_id,SIGFPE);
}
*/
void childreturn(int signo){
  printf("SCHEDULER:working signal interrupted\n");
  end_time=getClk();
  finish_pros_flag=true;
}


int main(int argc, char* argv[]) {
      //TODO: implement the scheduler :)


    initQueue(false);
    initClk();

//    printf("in schedular type %s\n",argv[1] );
    string scdl_algo_str=argv[1];
    int scdl_algo_num = scdl_algo_str[0]-'0';

    if(scdl_algo_num==1)
    {
      signal(SIGALRM,ignore);
      signal(SIGFPE,ignore);
//      signal(SIGCHLD,childreturn);

    }
    if(scdl_algo_num==2)
    {
      signal(SIGALRM,ignore);
      signal(SIGFPE,interrut);
//      signal(SIGCHLD,childreturn);
    }
    if(scdl_algo_num==3)
    {
      signal(SIGFPE,ignore);
      signal(SIGALRM,quantum_int);
    }


    processData pros;
    priority_queue<processData> ready_qu;
    int ret;

    do
    {
      ret=Recmsg(pros);
      //printf("%d\n",ret);




/*this part is useless
      if(ret==-1)
      continue;
*/

      while(ret==0)
      {
        printf("SCHEDULER:receive process number %d\n",pros.id);
      //  show_process(pros);
        pros.scdl_algo=scdl_algo_num;
        ready_qu.push(pros);
        //show_process(ready_qu.top());
        ret=Recmsg(pros);
      }





      if(scdl_algo_num==1 && !ready_qu.empty())
      {
        //HPF :HIGH PRIORITY first scheduler
        //signal(SIGFPE,ignore);
        pros=ready_qu.top();
        pros_id=fork();
        if(pros_id==-1)
        {
          printf("Error in forking the process id %d\n",pros.id);
        }
        if(pros_id==0)
        {
          //child : the process i want to run now
          printf("SCHEDULER:process id %d start now\n",pros.id);
          //at first i force the process to work on single cpu
          char cmd[80]="taskset -p 0x00000001 \0";
          strcat(cmd,(to_string(getpid())).c_str());
          system(cmd);
          //second i exec the process
          char cstr[5] ;
          strcpy(cstr, (num_to_str(pros.remainingtime)).c_str());
          char* argv[]={"process.out",cstr,0};
          execve(argv[0],&argv[0],NULL);
        }
        wait(&stat_loc);
			  if(!(stat_loc & 0x00FF))
				printf("SCHEDULER:process id %d finish now\n",pros.id);
        ready_qu.pop();

      }
      //=================================================================================================
      //=================================================================================================
      //=================================================================================================
      //=================================================================================================


      if(scdl_algo_num==2 && !ready_qu.empty() )
      {
        //printf("in the STRN section!\n");
        //SRTN :SHORTEST REMAINING TIME NEXT scheduler
        //signal(SIGFPE,interrut);
        pros=ready_qu.top();
        start_time=getClk();


        printf("=====================the process id is= %d \n",pros.pros_pid);
        if(pros.pros_pid>0)
        {
          printf("continue the process id = %d\n",pros.pros_pid);
          kill(pros.pros_pid,SIGCONT);
        }
        else
        {
          pros_id=fork();
          if(pros_id==-1)
          {
            printf("Error in forking the process id %d\n",pros.id);
          }
          if(pros_id==0)
          {
            //child : the process i want to run now
            printf("process id %d start now\n",pros.id);
            pros.pros_pid=getpid();
            ready_qu.pop();
            ready_qu.push(pros);
            printf("the child pid=%d and i store in the PCB the value %d\n",getpid(),pros.pros_pid);
            //at first i force the process to work on single cpu
            char cmd[80]="taskset -p 0x00000001 \0";
            strcat(cmd,(to_string(getpid())).c_str());
            system(cmd);
            //second i exec the process
            char cstr[5] ;
            strcpy(cstr, (num_to_str(pros.remainingtime)).c_str());
            char* argv[]={"process.out",cstr,0};
            execve(argv[0],&argv[0],NULL);
          }

        }


        int status;
        pid_t result = waitpid(pros_id, &status, WUNTRACED);
        if (result==0)
        {
          // Child still alive
          printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
        }
        else
        {
          // Child exited
          printf("\n===================================================\n");
          printf("process id %d stoped now with status=%d\n",pros.id,status);
          end_time=getClk();
        }

/*
        wait(&stat_loc);
        if(!(stat_loc & 0x00FF))
        {
          printf("process id %d stoped now with stat_loc=%d\n",pros.id,stat_loc);
          end_time=getClk();
        }
*/
        process_stopped=false;
        pros.remainingtime-=(end_time-start_time);
        printf("start_time=%d **** end_time=%d the remaining time is %d\n",start_time,end_time,pros.remainingtime);
        ready_qu.pop();
        if(pros.remainingtime>0)
        {
          process_stopped=true;
          ready_qu.push(pros);
        }
        else if (pros.remainingtime==0)
        {
          printf("process id %d finish now\n",pros.id);
        }




      }






      //=================================================================================================
      //=================================================================================================
      //=================================================================================================
      //=================================================================================================

/*
      if(scdl_algo_num==3 && !ready_qu.empty() )
      {
        //printf("in the STRN section!\n");
        //SRTN :SHORTEST REMAINING TIME NEXT scheduler
        //signal(SIGFPE,interrut);
        pros=ready_qu.top();
        start_time=getClk();
        pros_id=fork();
        if(pros_id==-1)
        {
          printf("Error in forking the process id %d\n",pros.id);
        }
        if(pros_id==0)
        {
          //child : the process i want to run now
          printf("process id %d start now\n",pros.id);
          //at first i force the process to work on single cpu
          char cmd[80]="taskset -p 0x00000001 \0";
          strcat(cmd,(to_string(getpid())).c_str());
          system(cmd);
          //second i exec the process
          char cstr[5] ;
          strcpy(cstr, (num_to_str(pros.remainingtime)).c_str());
          char* argv[]={"process.out",cstr,0};
          execve(argv[0],&argv[0],NULL);
        }

        wait(&stat_loc);
        if(!(stat_loc & 0x00FF))
        {
          printf("process id %d stoped now with stat_loc=%d\n",pros.id,stat_loc);
          end_time=getClk();
        }


        pros.remainingtime-=(end_time-start_time);
        printf("start_time=%d **** end_time=%d the remaining time is %d\n",start_time,end_time,pros.remainingtime);
        ready_qu.pop();
        if(pros.remainingtime>0)
        {
          ready_qu.push(pros);
        }
        else if (pros.remainingtime==0)
        {
          printf("process id %d finish now\n",pros.id);
        }

      }
*/


//test the last send arrival.
      if(ret==1)
      {
        printf("<<<<<<<<<<<<last send message received>>>>>>>>>>>>\n");
      }


    }
    while(ret!=1);


/*
//test : the ready queue
    printf("==========DISPLAY QUEUE IN SCHEDULER==============\n");
    show_queue(ready_qu);
    printf("=================================================\n");
*/













































printf("SCHEDULER:after last send to finish the scheduler!\n");

    while(!ready_qu.empty())
    {

      if(scdl_algo_num==1 && !ready_qu.empty())
      {
        //HPF :HIGH PRIORITY first scheduler
        pros=ready_qu.top();
        int stat_loc;
        pros_id=fork();
        if(pros_id==-1)
        {
          printf("Error in forking the process id %d\n",pros.id);
        }
        if(pros_id==0)
        {
          //child : the process i want to run now
          printf("SCHEDULER:process id %d start now\n",pros.id);
          //at first i force the process to work on single cpu
          char cmd[80]="taskset -p 0x00000001 \0";
          strcat(cmd,(to_string(getpid())).c_str());
          system(cmd);
          //second i exec the process
          char cstr[5] ;
          strcpy(cstr, (num_to_str(pros.remainingtime)).c_str());
          char* argv[]={"process.out",cstr,0};
          execve(argv[0],&argv[0],NULL);
        }
        wait(&stat_loc);
        if(!(stat_loc & 0x00FF))
        printf("SCHEDULER:process id %d finish now\n",pros.id);
        ready_qu.pop();
      }





      //============================================================================================================
      //============================================================================================================
      //============================================================================================================
      //============================================================================================================




/*
      if(scdl_algo_num==2 && !ready_qu.empty() )
      {
        //printf("in the STRN section!\n");
        //SRTN :SHORTEST REMAINING TIME NEXT scheduler
        //signal(SIGFPE,interrut);
        pros=ready_qu.top();
        start_time=getClk();
        pros_id=fork();
        if(pros_id==-1)
        {
          printf("Error in forking the process id %d\n",pros.id);
        }
        if(pros_id==0)
        {
          //child : the process i want to run now
          printf("process id %d start now\n",pros.id);
          //at first i force the process to work on single cpu
          char cmd[80]="taskset -p 0x00000001 \0";
          strcat(cmd,(to_string(getpid())).c_str());
          system(cmd);
          //second i exec the process
          char cstr[5] ;
          strcpy(cstr, (num_to_str(pros.remainingtime)).c_str());
          char* argv[]={"process.out",cstr,0};
          execve(argv[0],&argv[0],NULL);
        }

        wait(&stat_loc);
        if(!(stat_loc & 0x00FF))
        {
          printf("process id %d stoped now with stat_loc=%d\n",pros.id,stat_loc);
          end_time=getClk();
        }



        printf("start_time=%d **** end_time=%d\n",start_time,end_time);
        pros.remainingtime-=(end_time-start_time);
        ready_qu.pop();
        if(pros.remainingtime>0)
        {
          ready_qu.push(pros);
        }
        else if (pros.remainingtime==0)
        {
          printf("process id %d finish now\n",pros.id);
        }




      }
*/



      if(scdl_algo_num==2 && !ready_qu.empty() )
      {
        //printf("in the STRN section!\n");
        //SRTN :SHORTEST REMAINING TIME NEXT scheduler
        //signal(SIGFPE,interrut);
        pros=ready_qu.top();
        start_time=getClk();


        printf("=====================the process id is= %d \n",ready_qu.top().pros_pid);
        if(ready_qu.top().pros_pid>0)
        {
          printf("continue the process id = %d\n",ready_qu.top().pros_pid);
          kill(ready_qu.top().pros_pid,SIGCONT);
        }
        else
        {
          pros_id=fork();
          if(pros_id==-1)
          {
            printf("Error in forking the process id %d\n",pros.id);
          }
          if(pros_id==0)
          {
            //child : the process i want to run now
            printf("process id %d start now\n",pros.id);
            pros.pros_pid=getpid();
            ready_qu.pop();
            ready_qu.push(pros);

            //at first i force the process to work on single cpu
            char cmd[80]="taskset -p 0x00000001 \0";
            strcat(cmd,(to_string(getpid())).c_str());
            system(cmd);
            //second i exec the process
            char cstr[5] ;
            strcpy(cstr, (num_to_str(pros.remainingtime)).c_str());
            char* argv[]={"process.out",cstr,0};
            execve(argv[0],&argv[0],NULL);
          }

        }


        int status;
        pid_t result = waitpid(pros_id, &status, WUNTRACED);
        if (result==0)
        {
          // Child still alive
          printf("\n$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
        }
        else
        {
          // Child exited
          printf("\n===================================================\n");
          printf("process id %d stoped now with status=%d\n",pros.id,status);
          end_time=getClk();
        }

/*
        wait(&stat_loc);
        if(!(stat_loc & 0x00FF))
        {
          printf("process id %d stoped now with stat_loc=%d\n",pros.id,stat_loc);
          end_time=getClk();
        }
*/
        process_stopped=false;
        pros.remainingtime-=(end_time-start_time);
        printf("start_time=%d **** end_time=%d the remaining time is %d\n",start_time,end_time,pros.remainingtime);
        ready_qu.pop();
        if(pros.remainingtime>0)
        {
          process_stopped=true;
          ready_qu.push(pros);
        }
        else if (pros.remainingtime==0)
        {
          printf("process id %d finish now\n",pros.id);
        }




      }


//============================================================================================================
//============================================================================================================
//============================================================================================================
//============================================================================================================


    }


/*
    //===================================
    //Preimplemented Functions examples

    /////Toget time use the following function
    int x= getClk();
    printf("current time is %d\n",x);

    //////To receive something from the generator, for example  id 2
    struct processData pD;
    Recmsg(pD); //returns -1 on failure or no message available ; 1 on the end of processes, 0 on successfully process receive.
    printf("current received data %d\n",pD.id);
*/


printf("the end of the scheduler\n");

    return 0;

}
