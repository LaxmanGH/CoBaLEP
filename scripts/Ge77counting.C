void Ge77counting(int numberofmuons,string option)
{
  //A script to calculate the Ge77 Ge77 production rate in a track-level output file

  //Import usual file-finding algorithm

  //BLOCK 1: FIND FILES TO PROCESS
  //1111111111111111111111111111111111111111111111
  const char* allfilesbeginningwith = "nuclearinfo";
  const char* indirectory = ".";

  char* directory = gSystem->ExpandPathName(indirectory);
  void* directoryP = gSystem->OpenDirectory(directory);

  const char* checkedname;
  const char* acceptedname[100];
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

  //End of file finding


  for(int iwan = 0;iwan < wan;iwan++)
    {
      cout << acceptedname[iwan] << endl;
      
      //Begin file by file processing
      
      //Typical beginning of script stuff
      
      TFile input(Form("%s",acceptedname[iwan]),"READ");
      TTree *nuclearTree = (TTree*)input.Get("nuclearTree");
      int entries = nuclearTree->GetEntries();
      cout <<"Entries in file: " << entries << endl << endl;
      int PID = 0;
      double muonpx = 0;
      double muonpy = 0;
      double muonpz = 0;
      double muonenergy = 0;
      double weight = 0;

      vector<int> Ge77entrynumber;

  nuclearTree->SetBranchAddress("PID",&PID);
  nuclearTree->SetBranchAddress("weight",&weight);
  nuclearTree->SetBranchAddress("muonpx",&muonpx);
  nuclearTree->SetBranchAddress("muonpy",&muonpy);
  nuclearTree->SetBranchAddress("muonpz",&muonpz);
  
	  //Extract Ge77 information to a separate construct
	  
	  const int PIDtocompare = 1000320770;	  


	  for(int k = 0;k < entries;k++)
	    {
	      nuclearTree->GetEntry(k);
	      if(PID==PIDtocompare)
		  Ge77entrynumber.push_back(k);
	    }

	  //This is information collected at the track level
	  //Every Ge77 (ground state) is by definition unique
	  cout <<"Number of Ge77s produced: " << Ge77entrynumber.size() <<endl;
	  if(Ge77entrynumber.size()==0) continue;


	  const double minimumenergy = 0;//GeV
	  const double maximumenergy = 2000;//GeV
	  const double numberofbins = 2000;
	  
	  TH1D *uniqueGe77plot = new TH1D(Form("uniqueGe77plot"),"Kinetic energy of parent muons;Kinetic Energy (GeV); Counts / (kg yr)",numberofbins, minimumenergy, maximumenergy);
	  gStyle->SetOptStat("oemi");
	  uniqueGe77plot->SetDirectory(gROOT);
	  uniqueGe77plot->SetFillColor(kOrange+2);  
	  uniqueGe77plot->SetLineColor(kOrange+2);  
	  uniqueGe77plot->GetXaxis()->CenterTitle();
	  uniqueGe77plot->GetYaxis()->CenterTitle();


	  //Let the normalization begin!
	  //const double trigefficiency = 11.99;//For this geometry
	  //Dimensionless, calculated with 100,000 trial runs
	  
	  //The pCDR geometries don't have any excess rock
	  //Therefore, for now the trig efficiency cut is unused and unneeded

	  double muonflux = 0;

	  if(option=="SURF")
	    muonflux = 5.31 * 1e-5;//In muons/m^2/s source: https://arxiv.org/pdf/1602.07742.pdf
	  else if (option=="SNO")
	    muonflux = 3.31 * 1e-6;//In muons/m^2/s source: https://arxiv.org/pdf/0902.2776.pdf
	  else if (option=="LNGS")
	    muonflux = 3.472 * 1e-4;//In muons/m^2/s source: https://arxiv.org/pdf/1802.05040.pdf
	  else
	    {
	      cout << "Invalid site option. Valid options at present are: 'SURF, 'SNO', 'LNGS'" << endl << endl << endl;
	      return;
	    }


	  const double trigefficiency = 1;

	  const double samplingarea = 99.9 * 99.9; //In m^2
		  const double secondstoyears = (60 * 60 * 24 * 365.2422);//In s/y
	  const double detectormass = (2.52 * 6 * 19 * 4)*0.883;//in kg
	  //Remove mass of Ge74, for the Ge77 study (88.3% Ge76 fraction)
	  
	  //Divide by the bin width (in GeV) (is currently just 1)((do I really need to do this?))
	  double binwidth = 1;//(maximumenergy - minimumenergy)/numberofbins;
	  
	  double expectedmuonsperyear = samplingarea * muonflux * secondstoyears;//Expected muons per year through our sampling plane
	  //Unit of muons/y
	  double yearsofdata = (numberofmuons*trigefficiency)/expectedmuonsperyear; //unit of y
	  //cout << "YEARS" << yearsofdata << endl;      

	  double newweight = 0;

	  for(int k = 0; k < Ge77entrynumber.size();k++)
	    {
	      nuclearTree->GetEntry(Ge77entrynumber[k]);	      

	      muonenergy = TMath::Sqrt(muonpx*muonpx+muonpy*muonpy+muonpz*muonpz);
	      newweight = weight;	      
	      newweight = newweight/yearsofdata; //unit of nuclei/y
	      newweight = newweight/detectormass;//unit of nuclei/(kg*y)

	      uniqueGe77plot->Fill(muonenergy/1000000, newweight);
	    }


	  TCanvas *uniq = new TCanvas;
	  //uniq->SetLogx();
	  uniq->SetLogy();
	  uniqueGe77plot->Draw("hist");
	  
	  TFile *output = new TFile(Form("wh-%s",acceptedname[iwan]),"RECREATE");
	  uniqueGe77plot->Write();
	  output->Close();
    }//File by file processing

}//EOF




  /*I designed this as though the track numbers were arbitrarily sorted, but in reality, they're grouped together...
  while(Ge77energy.size()>1)
    {
      tracknumbertocompare = Ge77tracknumber.at(0);
      energytocompare = Ge77energy.at(0);
      cout << tracknumbertocompare << " COMP" <<endl;
      vectorplaceholder = 0;
      cout <<Ge77energy.size()<<endl;
      for(int i = 1; i < Ge77energy.size();i++)
	{
	  cout << Ge77tracknumber.at(i) << " neut i " << i << endl;
	  if(i>20000)
	    return;
	  if(Ge77tracknumber.at(i) == tracknumbertocompare)
	    {//If it's the same Ge77
	      cout << "i " << i << endl;
	      if(Ge77energy.at(i) < energytocompare)
		{//New lowest energy, save this entry, discard the other entry
		  tracknumbertocompare = Ge77tracknumber.at(i);
		  energytocompare = Ge77energy.at(i);
		  Ge77tracknumber.erase(Ge77tracknumber.begin()+vectorplaceholder);
		  Ge77energy.erase(Ge77energy.begin()+vectorplaceholder);
		  vectorplaceholder = i;
		}
	      else
		{//Not new lowest energy, delete this entry
		  Ge77tracknumber.erase(Ge77tracknumber.begin()+i);
		  Ge77energy.erase(Ge77energy.begin()+i);
		}
	      i--;
	    }//if(Ge77tracknumber
	}//for(int i = 1

      //Now the comparators should have a unique Ge77 ID and (low) kinetic energy
      //Add it to the final vector
      filteredGe77energy.push_back(energytocompare);

    }//while(Ge77energy

  if(Ge77energy.size()==1)
    filteredGe77energy.push_back(Ge77energy.at(0));

  cout << filteredGe77energy.size() << endl;
  */
