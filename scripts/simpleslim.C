void simpleslim(void)
{

  TFile input("output123456789.root","READ");
  TTree *gTree = (TTree*)input.Get("fTree");

  gTree->SetBranchStatus("*",0);
  /*gTree->SetBranchStatus("PID",1);
  gTree->SetBranchStatus("energydeposition",1);
  gTree->SetBranchStatus("kineticenergy",1);
  gTree->SetBranchStatus("time",1);
  gTree->SetBranchStatus("x",1);
  gTree->SetBranchStatus("y",1);
  gTree->SetBranchStatus("z",1);
  gTree->SetBranchStatus("px",1);
  gTree->SetBranchStatus("py",1);
  gTree->SetBranchStatus("pz",1);
  gTree->SetBranchStatus("eventnumber",1);
  gTree->SetBranchStatus("tracknumber",1);
  gTree->SetBranchStatus("ParentTrackID",1);
  //~gTree->SetBranchStatus("creatorprocess",1);
  gTree->SetBranchStatus("process",1);
  gTree->SetBranchStatus("parentnucleusPID",1);*/

  int PID = 0;
  double kineticenergy = 0;
  string *creatorprocess = NULL;
  double energydeposition = 0;

  gTree->SetBranchAddress("PID",&PID);
  gTree->SetBranchAddress("kineticenergy",&kineticenergy);
  gTree->SetBranchAddress("creatorprocess",&creatorprocess);
  gTree->SetBranchAddress("energydeposition",&energydeposition);

  TFile output("output9.root","RECREATE");
  TTree* fTree = (TTree*)gTree->CloneTree(0);
  string compareo = "eBrem";
  for(int i = 0; i < gTree->GetEntries();i++)
    {

      gTree->GetEntry(i);

      //      if(kineticenergy<1000&&PID==22&&!strncmp(creatorprocess->c_str(),compareo.c_str(),5)) continue;
      if(abs(PID)==12) continue;
      else if(energydeposition==0) continue;
      //      else if(kineticenergy<1000&&PID==11&&!strncmp(creatorprocess->c_str(),"compt",5)) continue;

	fTree->Fill();

    }

  output.cd();
  fTree->Write();
  output.Close();
  return;


}
