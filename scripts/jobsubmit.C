void jobsubmit()
{
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
  
  for(int i=1;i<101;i++)
    {
      gSystem->Exec("sbatch runsims.sh");
      sleep(1);
    }
}
