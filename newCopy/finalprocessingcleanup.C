void finalprocessingcleanup(void)
{//A quality of life script to make things a little easier and more proceduralized
#include <time.h>
  gSystem->Exec("cat slurm*>logforeventbuilder.out");
  sleep(1);
  gSystem->Exec("rm slurm*");
  TChain *chain = new TChain("sumtree");
  chain->Add("ebuilt*.root");
  chain->Merge("allebuilt.root");
  gSystem->Exec("ls ebuilt*>valideventfilelist.txt");


}
