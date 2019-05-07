void chainingtwo(void)
{

  TChain *chain = new TChain("sumtree");
  chain->Add("sumtree*.root");
  chain->Merge("allsum.root");
  gSystem->Exec("ls sumtree*>valideventfilelist.txt");

}
