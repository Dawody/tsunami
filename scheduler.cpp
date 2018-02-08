#include <vector>
#include "clkUtilities.h"
#include "queueUtilities.h"

vector<int> v;
double avg_wta=0,avg_wating=0,std_wta=0,cpu_utilization=0;
int pros_id=999999,stat_loc,status;
int start_time,end_time,total_run_time=0;
processData pros;
priority_queue<processData> ready_qu;
int scdl_algo_num,ret,pros_counter=0,rr_counter=1000;//,past_pros_pid=0;



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



//i use this interrupt in SRTN scheduler to stop the running process and calculate the time the process stopped at
void interrut(int signo){
  if(!ready_qu.empty() && ready_qu.top().pros_pid>0)
  {
  //printf("ok i w'll stop process number %d\n",ready_qu.top().pros_pid);
  //  printf("at time %d i send kill stop to process %d\n",getClk(),ready_qu.top().pros_pid);
  //  printf("==========new process come in time %d =====\n",getClk());
    kill(ready_qu.top().pros_pid,SIGSTOP);
  }
}

//i use this interrupt in RR scheduler to stop the running process and calculate the time the process stopped at
void quantum_int(int signo){
  if(!ready_qu.empty() && ready_qu.top().pros_pid>0)
  {
    //printf("ok i w'll stop process number %d\n",ready_qu.top().pros_pid);
    kill(ready_qu.top().pros_pid,SIGSTOP);
  }
}

//i use this handler to make any scheduler ignore the signals of other schedulers
void ignore(int signo){
//DO NO THING
  }

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void HPF(){
  if(scdl_algo_num==1 && !ready_qu.empty())
  {
    //HPF :HIGH PRIORITY first scheduler
    pros=ready_qu.top();
    pros_counter++;
    total_run_time+=pros.runningtime;
    pros_id=fork();
    if(pros_id==-1)
    {
      printf("Error in forking the process id %d\n",pros.id);
    }
    if(pros_id==0)
    {
      //child : the process i want to run now
      //at first i force the process to work on single cpu
      char cmd[80]="taskset -p 0x00000001 \0";
      strcat(cmd,(to_string(getpid())).c_str());
      strcat(cmd,(go_to_hell).c_str());
      system(cmd);
      //second i exec the process
      char cstr[5] ;
      strcpy(cstr, (num_to_str(pros.remainingtime)).c_str());
      char* argv[]={"process.out",cstr,"1",0};
      execve(argv[0],&argv[0],NULL);
    }
    else
    {
      pros.start_time=getClk();
      printf("At time %d process %d started arr %d total %d remain %d wait %d\n",getClk(),pros.id,pros.arrivaltime,pros.runningtime,pros.remainingtime,pros.wait_time);
    }
    wait(&stat_loc);
    if(!(stat_loc & 0x00FF))
    {
      pros.WTA=double(double(getClk()-pros.arrivaltime)/double(pros.runningtime));
      printf("At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",getClk(),pros.id,pros.arrivaltime,pros.runningtime,0,pros.wait_time,getClk()-pros.arrivaltime,pros.WTA);
      avg_wating+=pros.wait_time;
      avg_wta+=pros.WTA;
      v.push_back(pros.WTA);
    }

    ready_qu.pop();

  }

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SRT(){
  if(scdl_algo_num==2 && !ready_qu.empty() )
  {
    //SRTN :SHORTEST REMAINING TIME NEXT scheduler
    pros=ready_qu.top();
    start_time=getClk();
    //printf("the top now is %d = %d\n",ready_qu.top().pros_pid,pros.pros_pid);

    if(pros.pros_pid>0)
    {
      //the process is actually worked before so it's stopped now . lets make it continue working
      pros.wait_time+=(getClk()-pros.stop_time);
      printf("At time %d process %d resumed arr %d total %d remain %d wait %d\n",getClk(),pros.id,pros.arrivaltime,pros.runningtime,pros.remainingtime,pros.wait_time);
      ready_qu.pop();
      ready_qu.push(pros);
      kill(pros.pros_pid,SIGCONT);
    }
    else
    {
      //it's new process . so i will fork it to start running
      pros_counter++;
      total_run_time+=pros.runningtime;
      pros_id=fork();
      if(pros_id==-1)
      {
        printf("Error in forking the process id %d\n",pros.id);
      }
      if(pros_id==0)
      {
        //child : the process i want to run now
        //at first i force the process to work on single cpu
        char cmd[80]="taskset -p 0x00000001 \0";
        strcat(cmd,(to_string(getpid())).c_str());
        strcat(cmd,(go_to_hell).c_str());
        system(cmd);
        //second i exec the process
        char cstr[5] ;
        strcpy(cstr, (num_to_str(pros.remainingtime)).c_str());
        char* argv[]={"process.out",cstr,"2",0};
        execve(argv[0],&argv[0],NULL);
      }
      else
      {
        //parent
        pros.start_time=getClk();
        //ready_qu.top().start_time=getClk();
        printf("At time %d process %d started arr %d total %d remain %d wait %d\n",getClk(),pros.id,pros.arrivaltime,pros.runningtime,pros.remainingtime,pros.wait_time);
        pros.pros_pid=pros_id;
      //  ready_qu.top().pros_pid=pros_id;

        ready_qu.pop();
        ready_qu.push(pros);

      }

    }




    pid_t result = waitpid(pros.pros_pid, &status, WUNTRACED);
    if (result!=0)
    {
      // Child is stoped or terminated
      end_time=getClk();
    }


    pros.remainingtime-=(end_time-start_time);
    //printf("the currect process id is %d = %d\n",ready_qu.top().id ,pros.id);
    ready_qu.pop();
    if(status!=0)
    {
      //process stoped
      pros.stop_time=getClk();
      printf("At time %d process %d stopped arr %d total %d remain %d wait %d\n",getClk(),pros.id,pros.arrivaltime,pros.runningtime,pros.remainingtime,pros.wait_time);
      ready_qu.push(pros);
    }
    else if (status==0)
    {
      //process terminated
      pros.WTA=double(double(getClk()-pros.arrivaltime)/double(pros.runningtime));
      printf("At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",getClk(),pros.id,pros.arrivaltime,pros.runningtime,pros.remainingtime,pros.wait_time,getClk()-pros.arrivaltime,pros.WTA);
      avg_wating+=pros.wait_time;
      avg_wta+=pros.WTA;
      v.push_back(pros.WTA);
    }




  }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void RR(){
  if(scdl_algo_num==3 && !ready_qu.empty() )
  {
    //SRTN :SHORTEST REMAINING TIME NEXT scheduler
    pros=ready_qu.top();
    start_time=getClk();
    rr_counter++;

    if(pros.pros_pid>0)
    {
      //the process is actually worked before so it's stopped now . lets make it continue working
      pros.wait_time+=(start_time-pros.stop_time);
      printf("At time %d process %d resumed arr %d total %d remain %d wait %d\n",start_time,pros.id,pros.arrivaltime,pros.runningtime,pros.remainingtime,pros.wait_time);
      ready_qu.pop();
      ready_qu.push(pros);
      pros_id=pros.pros_pid;
      kill(pros.pros_pid,SIGCONT);
    }
    else
    {
      //it's new process . so i will fork it to start running
      pros_counter++;
      total_run_time+=pros.runningtime;
      pros_id=fork();
      if(pros_id==-1)
      {
        printf("Error in forking the process id %d\n",pros.id);
      }
      if(pros_id==0)
      {
        //child : the process i want to run now
        //at first i force the process to work on single cpu
        char cmd[80]="taskset -p 0x00000001 \0";
        strcat(cmd,(to_string(getpid())).c_str());
        strcat(cmd,(go_to_hell).c_str());
        system(cmd);
        //second i exec the process
        char cstr[5] ;
        strcpy(cstr, (num_to_str(pros.remainingtime)).c_str());
        char* argv[]={"process.out",cstr,"3",0};
        execve(argv[0],&argv[0],NULL);
      }
      else
      {
        //parent
        pros.start_time=start_time;
        pros.pros_pid=pros_id;
        printf("At time %d process %d started arr %d total %d remain %d wait %d\n",start_time,pros.id,pros.arrivaltime,pros.runningtime,pros.remainingtime,pros.wait_time);
        ready_qu.pop();
        ready_qu.push(pros);
      }

    }




    pid_t result = waitpid(pros.pros_pid, &status, WUNTRACED);


    end_time=getClk();
//    printf("start_time=%d end_time=%d remainingtime=%d\n",start_time,end_time,pros.remainingtime);

//    printf("time ending calculation \n");


    pros.remainingtime-=(end_time-start_time);

//    if(past_pros_pid!=pros.pros_pid)
//    {
      pros.rr_time=rr_counter;
//    }
//    past_pros_pid=pros.pros_pid;

    ready_qu.pop(); //you MUST pop the current process from the ready queue to update the changed  info for it before pushing it again if it didn't finish
    if(status!=0)
    {
      //process stopped
      pros.stop_time=end_time;
//      printf("before stoping me my rr vaile is %d\n",pros.rr_time);
      printf("At time %d process %d stopped arr %d total %d remain %d wait %d\n",end_time,pros.id,pros.arrivaltime,pros.runningtime,pros.remainingtime,pros.wait_time);
      ready_qu.push(pros);
    }
    else if (status==0)
    {
      //process terminated
      pros.WTA=double(double(end_time-pros.arrivaltime)/double(pros.runningtime));
      printf("At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",end_time,pros.id,pros.arrivaltime,pros.runningtime,pros.remainingtime,pros.wait_time,getClk()-pros.arrivaltime,pros.WTA);
      avg_wating+=pros.wait_time;
      avg_wta+=pros.WTA;
      v.push_back(pros.WTA);

    }


  }

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
      //TODO: implement the scheduler :)
      //ME : OK xD

    initQueue(false);
    initClk();

    //extract the type of chosen scheduler to work with it
    string scdl_algo_str=argv[1];
    scdl_algo_num = scdl_algo_str[0]-'0';


    //signals handlers for each scheduler type

    if(scdl_algo_num==1)
    {
      //handlers for HPF
      signal(SIGALRM,ignore);
      signal(SIGFPE,ignore);

    }
    if(scdl_algo_num==2)
    {
      //handlers for SRTN
      signal(SIGALRM,ignore);
      signal(SIGFPE,interrut);
    }
    if(scdl_algo_num==3)
    {
      //handlers for RR
      signal(SIGFPE,ignore);
      signal(SIGALRM,quantum_int);
    }


    //let's start :)
    freopen("scheduler.log","w",stdout);
    do
    {
      ret=Recmsg(pros);

      while(ret==0)
      {
        //catch all msges in the queue in this moment
        pros.scdl_algo=scdl_algo_num;
        ready_qu.push(pros);
//        printf("============================================%d==================================%d\n",pros.id,ready_qu.top().id);
        ret=Recmsg(pros);
      }


      HPF();
      SRT();
      RR();
    }
    while(ret!=1);  //work till seeing last msg


/*
//test : the ready queue
    printf("==========DISPLAY QUEUE IN SCHEDULER==============\n");
    show_queue(ready_qu);
    printf("=================================================\n");
*/

    while(!ready_qu.empty()) //work till you finish all tasks
    {
      HPF();
      SRT();
      RR();
    }

    avg_wta/=pros_counter;
    avg_wating/=pros_counter;
    cpu_utilization=double(total_run_time)/double(getClk());
    for(int i =0 ; i<pros_counter ; i++)
    {
      std_wta+=sqrt(((v[i]-avg_wta)*(v[i]-avg_wta))/(pros_counter));
    }

    if(pros_counter==0)
    {
      avg_wta=avg_wating=std_wta=0;
    }
//    printf("===========================================\n");
    freopen("scheduler.perf","w",stdout);
    printf("CPU utilization=%.0f%\n",cpu_utilization*100);
    printf("Avg WTA=%.2f\n",avg_wta);
    printf("Avg Waiting=%.2f\n",avg_wating);
    printf("Std WTA=%.2f\n",std_wta);

    return 0;

}
