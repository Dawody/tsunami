
#include <stdio.h>      //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>     //i added this
#include <sys/resource.h> //i added this
#include <queue>
#include <string>
#include <cstring>

using namespace std;

#define QUEUEKEY 777
#define ENDTYPE 19L

///==============================
//don't mess with this variable//
int qid;
//===============================


struct processData {
   //TODO: add the process data details
   int id;
   int arrivaltime;
   int runningtime;
   int priority;
   int remainingtime;
   int scdl_algo;  //All processes whitch in the same container MUST have the same scdl_algo . otherwise , unpredictable behaviour will say welcome!
   int pros_pid;

    bool operator<(const processData& x)const
    {
      if(scdl_algo==0)
      return arrivaltime > x.arrivaltime;
      if(scdl_algo==1)
      return priority < x.priority;
      if(scdl_algo==2)
      return remainingtime > x.remainingtime;
      if(scdl_algo==3)
      return arrivaltime > x.arrivaltime;


    }

};

struct messagebuffer
{
 long mtype;
 struct processData data;

};


void initQueue(bool flag) {
  if(flag) {
    qid= msgget(QUEUEKEY,IPC_CREAT  |   0644);
    if(qid == -1)
    {
        perror("queue not created");
    }
  }

  else
  {
     qid = msgget(QUEUEKEY, 0444);
    if(qid == -1)
      {
            //Make sure that the Clock exists
          printf("Error, queue not initialized yet\n");
          kill(getpgrp(),SIGINT);
      }
  }

}



int Sendmsg(struct processData pData) {
  struct messagebuffer msg;
  msg.data = pData;
  msg.mtype = 1L;
  return msgsnd(qid, &msg, sizeof(msg)-sizeof(long), !IPC_NOWAIT);
}

int Recmsg( processData &pData) {
  struct messagebuffer msg;
  msg.mtype = 1L;
  int ret=msgrcv(qid,&msg,sizeof(msg)-sizeof(long),0,IPC_NOWAIT);
  pData=msg.data;
  if (ret == -1)
    return -1;
  if(msg.mtype == ENDTYPE)
      return  1;
  return 0;

}

void lastSend() {
  struct messagebuffer msg;
  msg.mtype = ENDTYPE;
  msgsnd(qid, &msg, sizeof(msg)-sizeof(long), !IPC_NOWAIT);
}

void destroyQueueAndExit(int)
{
    msgctl(qid, IPC_RMID, (struct msqid_ds*)0);
    exit(0);
}


void show_process(processData pros){
  processData pd=pros;
  printf("\n=====all process info====\n");
    printf("Process id = %d\n",pd.id );
    printf("Process arrivaltime = %d\n",pd.arrivaltime );
    printf("Process runningtime = %d\n",pd.runningtime );
    printf("Process priority = %d\n",pd.priority );
    printf("Process remainingtime = %d\n",pd.remainingtime );
    printf("Process scdl_algo = %d\n",pd.scdl_algo );
//    if(pd.scdl_algo=="0")
//    printf("its ok\n");

    printf("==================================\n" );

}

void show_queue(priority_queue<processData> pros_qu){
  processData pd;
  printf("\n=====all process in the simulation====\n");
  while(!pros_qu.empty())
  {
    pd=pros_qu.top();
    show_process(pd);

    pros_qu.pop();
  }
}


int str_to_num(string s,int mul){
if(!s.size())return 0;
return (s[s.size()-1]-'0')*mul+str_to_num(s.substr(0,s.size()-1),mul*10);
}

string num_to_str(int x){
string s=to_string(x);
return s;
}
