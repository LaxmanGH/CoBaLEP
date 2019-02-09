#include <typeinfo>

void specificcreatorprocess(void)
{

  TFile input("all.root", "read");

  string* creatorprocess;
  double startz;
  double pz;
  int PID;

    
  fTree->SetBranchAddress("creatorprocess", &creatorprocess);
  fTree->SetBranchAddress("startz", &startz);
  fTree->SetBranchAddress("pz", &pz);
  fTree->SetBranchAddress("PID", &PID);


  char* comparator;

  TH1D *pidhisto = new TH1D("pid histo", "PID of bottom entering particles created by MuIoni",50,-25,25);
  TH1D *pzhisto = new TH1D("pz histo", "pz of bottom entering particles created by MuIoni",100,0,1);
  
  for(int i=0;i<(fTree->GetEntries()-1);i++)
    {
      fTree->GetEntry(i);

       comparator = creatorprocess->data();
       if((int)comparator[0] == 109 && (int)comparator[1] == 117 && (int)comparator[2] == 73 && pz>0)

	 pzhisto->Fill(pz);
	 //	 pidhisto->Fill(PID);
	 
    }
  pzhisto->SetDirectory(gROOT);
  pzhisto->Draw();
  
  //  fTree->Draw("creatorprocess");
}
