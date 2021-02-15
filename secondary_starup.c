#include "mpcore.h"
#include "taskcpu.h"
void secondary_startup(unsigned int cpu){
  //tell cpu0 cpux boot success
  pen_release=-1;

  switch(cpu){
    case 1:
      task_ledblink(0);
      break;
    case 2:
      task_ledblink(1);
      break;
    case 3:
      task_print("info from cpu3\n\r");
      break;
    default:
      break;
  }
   while(1){} 
}
