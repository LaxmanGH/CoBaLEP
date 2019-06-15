void neutroncounting(void)
{
  //A script to retrieve the energy spectrum of neutrons in sim data

  //Typical beginning of script stuff

  TFile input("overout.root","READ");
  TTree *fTree = (TTree*)input.Get("fTree");
  int entries = fTree->GetEntries();

  double kineticenergy = 0;
  int tracknumber = 0;
  int PID = 0;
  fTree->SetBranchAddress("kineticenergy",&kineticenergy);
  fTree->SetBranchAddress("tracknumber",&tracknumber);
  fTree->SetBranchAddress("PID",&PID);

  //Extract neutron information to a separate construct
  vector<double> neutronenergy;
  vector<int> neutrontracknumber;

  for(int i = 0;i<entries;i++)
    {
      fTree->GetEntry(i);
      if(PID==2112)
	{
	  neutronenergy.push_back(kineticenergy);
	  neutrontracknumber.push_back(tracknumber);
	}
    }

}
