void nucleardecayinfo(void)
{

  TFile *input = new TFile("all.root","read");

  int PID;
  fTree->SetBranchAddress("PID", &PID);
  double time;
  fTree->SetBranchAddress("time", &time);
  int modPID = 0;

  int j = 0;
  int seventyfive = 0;
  int seventyone = 0;
  int seventy = 0;
  int seventyseven = 0;
  int sixtysix = 0;
  int sixtynine = 0;

  int gallium = 0;
  int germanium = 0;
  int arsenic = 0;
  int zinc = 0;
  int selenium = 0;

  int z = 0;
  int a = 0;
  TH1I *anumber = new TH1I("anumber", "A Number", 15,65,80);
  TH1I *znumber = new TH1I("znumber", "Z Number", 5 ,30,35);
  TH2I *heatmap = new TH2I("heatmap","A number vs Z number for delayed time nuclear decays",15,65,80,5,30,35);
  
  for(double i=0;i<fTree->GetEntries();i++)
    {
      fTree->GetEntry(i);
      
      if(PID>1000000&&time>1000000000&&time<TMath::Power(10,18))
	{
	  modPID = PID-1000000000;

	  //	  if(modPID < 310000)
	  //  zinc++;
	  // else if(modPID <320000)
	  //  gallium++;
	  // else if(modPID < 330000)
	  //  germanium++;
	  //else if(modPID < 340000)
	  //  arsenic++;
	  //else
	  //  selenium++;

	  z = floor(modPID/10000);

	  modPID = modPID%10000;
	  a = floor(modPID/10);

	  /*	  if(modPID==700 || modPID==701)
	    seventy++;
	  if(modPID==710 || modPID==711)
	    seventyone++;
	  if(modPID==750 || modPID==751)
	    seventyfive++;
	  if(modPID==770 || modPID==771)
	    seventyseven++;
	  if(modPID==660 || modPID==661)
	    sixtysix++;
	  if(modPID==690 || modPID==691)
	  sixtynine++;*/

	  anumber->Fill(a);
	  znumber->Fill(z);
	  heatmap->Fill(a,z);
	  j++;
	}
      
    }

  heatmap->SetDirectory(gROOT);
  heatmap->Draw("colz");
  

  
  
  }
