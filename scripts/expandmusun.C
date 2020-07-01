void expandmusun(void)
{

  TFile input("musun1000000.root","READ");
  TTree *gTree = (TTree*)input.Get("t1");

  float x = 0;
  float y = 0;
  float z = 0;
  float newx = 0;
  float newy = 0;
  float newz = 0;


  gTree->SetBranchAddress("x",&x);
  gTree->SetBranchAddress("y",&y);
  gTree->SetBranchAddress("z",&z);


  TFile output("musun.root","RECREATE");
  TTree* fTree = (TTree*)gTree->CloneTree(0);

  //fTree->Branch("",&newx);

  for(int i = 0; i < gTree->GetEntries();i++)
    {

      gTree->GetEntry(i);

      x = 1.5*x;
  y = 1.5*y;
  z = 1.5*z;

	fTree->Fill();
    }

  output.cd();
  fTree->Write();
  output.Close();

}
