void chainingone(void)
{

  TChain *chain = new TChain("fTree");
  chain->Add("output*.root");
  chain->Merge("allout.root");
  gSystem->Exec("cat slurm*.out > batch1.out");
  gSystem->Exec("rm slurm*.out");

}
