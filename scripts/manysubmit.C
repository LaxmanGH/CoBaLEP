void manysubmit()
{
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
  
  for(int i=10;i<100;i++)
    {
      gSystem->Exec(Form("sbatch --export=counter=%i runbuilder.sh",i));
      sleep(1);
    }
}
