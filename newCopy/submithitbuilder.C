void submithitbuilder(void)
{
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

  //Cleanup from simulation job submission
  //gSystem->Exec("cat slurm*>logforcobalep.out");
  //sleep(3);
  //gSystem->Exec("rm slurm*");  

  const char* allfilesbeginningwith = "output";
  const char* indirectory = ".";

  char* directory = gSystem->ExpandPathName(indirectory);
  void* directoryP = gSystem->OpenDirectory(directory);

  const char* checkedname;
  const char* acceptedname[1000];//I hope I never process 1000 files in one directory
  int wan = 0;
  TString checkedstring;

  while((checkedname = (char*)gSystem->GetDirEntry(directoryP)))
    {
      checkedstring = checkedname;
      cout << "Checked string: " << checkedstring << endl;
      if(checkedstring.BeginsWith(allfilesbeginningwith))
        {
          acceptedname[wan] = checkedname;
          wan++;
        }
    }

  cout << wan << endl << wan << endl;


  for(int iwan = 0;iwan < wan;iwan++)
    {
      cout << acceptedname[iwan] << endl;

      //FILE BY FILE PROCESSING BEGINS HERE
       gSystem->Exec(Form("sbatch --export=counter=%s runhitbuilder.sh",acceptedname[iwan]));
      sleep(1);
    }
}
