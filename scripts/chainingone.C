#include<time.h>

void chainingone(void)
{
  TChain *chain = new TChain("fTree");
  chain->Add("output*.root");
  chain->Merge("allout.root");
  gSystem->Exec("cat slurm*.out > batch1.out");
  gSystem->Exec("rm slurm*.out");
  sleep(3);
  TChain *chain2 = new TChain("nuclearTree");
  chain2->Add("nuclear*.root");
  chain2->Merge("allnuclear.root");
}
