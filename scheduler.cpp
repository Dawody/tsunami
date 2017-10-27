#include "clkUtilities.h"
#include "queueUtilities.h"

int main(int argc, char* argv[]) {
      //TODO: implement the scheduler :)

    initQueue(false);
    initClk();

    printf("in schedular type %s\n",argv[1] );
    string scdl_algo_str=argv[1];
    int scdl_algo_num = scdl_algo_str[0]-'0';

    processData pros;
    priority_queue<processData> ready_qu;
    int ret;

    do
    {
      ret=Recmsg(pros);

      if(ret==1)
      {
        printf("last send message received\n");
      }

      if(ret==-1)
      continue;

      if(ret==0)
      {
        printf("receive process number %d\n",pros.id);
        //show_process(pros);
        pros.scdl_algo=scdl_algo_num;
        ready_qu.push(pros);
        //show_process(ready_qu.top());
      }

      



    }
    while(ret!=1);

    printf("==========DISPLAY QUEUE IN SCHEDULER==============\n");
    show_queue(ready_qu);
    printf("=================================================\n");

    printf("after first while\n");




    while(!ready_qu.empty())
    {
      printf("after last send to finish the scheduler!\n");
      break;
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
