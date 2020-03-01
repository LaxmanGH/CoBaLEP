void isotopecounting(int numberofmuons)
{
  //A script to retrieve the energy spectrum of neutrons in sim data

  //Typical beginning of script stuff

  TFile input("LB.root","READ");
  TTree *nuclearTree = (TTree*)input.Get("nuclearTree");
  int entries = nuclearTree->GetEntries();
  cout <<"Entries in file: " << entries << endl << endl;
  int PID = 0;
  double muonpx = 0;
  double muonpy = 0;
  double muonpz = 0;
  double muonenergy = 0;
  double weight = 0;
  int numberofmuons = 0;

  cout << "Please enter the number of muons being processed, for normalization purposes (enter a numerical value only): " <<endl;
  cin >> numberofmuons;

  nuclearTree->SetBranchAddress("PID",&PID);
  nuclearTree->SetBranchAddress("weight",&weight);
  nuclearTree->SetBranchAddress("muonpx",&muonpx);
  nuclearTree->SetBranchAddress("muonpy",&muonpy);
  nuclearTree->SetBranchAddress("muonpz",&muonpz);
  

  //Loop all over existing isotopes (start smaller)

  for(int i = 1; i < 21;i++)//Ar is 18
    {
      for(int j = 1; j < 50; j++)
	{
	  //Extract isotope information to a separate construct
	  
	  vector<int> isotopeentrynumber;
	  int PIDtocompare = 1000000000+(i*10000)+(j*10);	  
	  cout << "PID to compare: " << PIDtocompare << endl;


	  for(int k = 0;k < entries;k++)
	    {
	      nuclearTree->GetEntry(k);
	      if(PID==PIDtocompare)
		isotopeentrynumber.push_back(k);
	    }

	  //This is information collected at the track level
	  //Every isotope (ground state) is by definition unique
	  cout <<"Number of isotopes produced: " << isotopeentrynumber.size() <<endl;
	  if(isotopeentrynumber.size()==0) continue;


	  const double minimumenergy = 0;//GeV
	  const double maximumenergy = 2000;//GeV
	  const double numberofbins = 2000;
	  
	  TH1D *uniqueisotopeplot = new TH1D(Form("uniqueisotopeplot%i-%i",i,j),Form("Kinetic energy of parent muons (Z=%i,A=%i);Kinetic Energy (GeV); Counts / (10kton day)",i,j),numberofbins, minimumenergy, maximumenergy);
	  gStyle->SetOptStat("oemi");
	  uniqueisotopeplot->SetDirectory(gROOT);
	  uniqueisotopeplot->SetFillColor(kOrange+2);  
	  uniqueisotopeplot->SetLineColor(kOrange+2);  
	  uniqueisotopeplot->GetXaxis()->CenterTitle();
	  uniqueisotopeplot->GetYaxis()->CenterTitle();


	  //Let the normalization begin!
	  const double trigefficiency = 11.99;//For this geometry
	  //Dimensionless, calculated with 100,000 trial runs
	  
	  const double samplingarea = 99.9 * 99.9; //In m^2
	  const double muonfluxatSURF = 5.31 * 1e-5;//In muons/m^2/s, from MJD paper
	  const double secondstodays = (60 * 60 * 24);//In s/d
	  const double detectormass = 0.0203;//in  units of 10 kilotons, e.g. 10,000,000 kg
	  //Direct transcript of the output from the Diagnostic geometry option:
	  //All in units of kg
	  //pCDR2020 diagnostics:
	  //Mass of one inner liquid argon before adding detectors: 4522.93
	  //After adding detectors (and displacing some LAr): 4447.68
	  //Mass of outer liquid argon: 198555
	  
	  //Previous implementation, for comparison
	  //Mass of outer LAr shield before stuff is added:: 372056 kg
	  //Mass of upper LAr shield before stuff is added:: 12734.9kg
	  //Total mass before stuff is added: 384790.9 kg
	  //384.8 tons, .3848 ktons, 0.03848 10 ktons
	  //We'll round down to 0.0384 to account for space taken by detectors and cryostats
	  
	  
	  //Divide by the bin width (in GeV) (is currently just 1) ((is this really necessary?))
	  double binwidth = (maximumenergy - minimumenergy)/numberofbins;
	  
	  double expectedmuonsperday = samplingarea * muonfluxatSURF * secondstodays;//Expected muons per day through our sampling plane
	  //Unit of muons/d
	  double daysofdata = (numberofmuons*trigefficiency)/expectedmuonsperday; //unit of d
	  //cout << "DAYS " << daysofdata << endl;      	  
	  
	  for(int k = 0; k < isotopeentrynumber.size();k++)
	    {
	      nuclearTree->GetEntry(isotopeentrynumber[k]);	      

	      muonenergy = TMath::Sqrt(muonpx*muonpx+muonpy*muonpy+muonpz*muonpz);

	      newweight = weight;	      
	      newweight = newweight/daysofdata; //unit of 1/d
	      newweight = newweight/detectormass;//unit of 1/(10kton*d)

	      //newweight = (weight)/(0.18*detectormass);
	      //If you want newweight to have units of muons/y/kg/GeV
	      	      
	      uniqueisotopeplot->Fill(muonenergy/1000000, newweight);
	    }


	  TCanvas *uniq = new TCanvas;
	  //uniq->SetLogx();
	  uniq->SetLogy();
	  uniqueisotopeplot->Draw("hist");
	  
	  TFile *output = new TFile(Form("weighted%i-%ihisto.root",i,j),"RECREATE");
	  uniqueisotopeplot->Write();
	  output->Close();

	}//for int j
    }//for int i    
}//EOF




/*I designed this as though the track numbers were arbitrarily sorted, but in reality, they're grouped together...
  while(isotopeenergy.size()>1)
  {
  tracknumbertocompare = isotopetracknumber.at(0);
  energytocompare = isotopeenergy.at(0);
  cout << tracknumbertocompare << " COMP" <<endl;
  vectorplaceholder = 0;
  cout <<isotopeenergy.size()<<endl;
  for(int i = 1; i < isotopeenergy.size();i++)
  {
  cout << isotopetracknumber.at(i) << " neut i " << i << endl;
  if(i>20000)
  return;
  if(isotopetracknumber.at(i) == tracknumbertocompare)
  {//If it's the same isotope
  cout << "i " << i << endl;
  if(isotopeenergy.at(i) < energytocompare)
  {//New lowest energy, save this entry, discard the other entry
  tracknumbertocompare = isotopetracknumber.at(i);
  energytocompare = isotopeenergy.at(i);
  isotopetracknumber.erase(isotopetracknumber.begin()+vectorplaceholder);
  isotopeenergy.erase(isotopeenergy.begin()+vectorplaceholder);
  vectorplaceholder = i;
  }
  else
  {//Not new lowest energy, delete this entry
  isotopetracknumber.erase(isotopetracknumber.begin()+i);
  isotopeenergy.erase(isotopeenergy.begin()+i);
  }
  i--;
  }//if(isotopetracknumber
  }//for(int i = 1

  //Now the comparators should have a unique isotope ID and (low) kinetic energy
  //Add it to the final vector
  filteredisotopeenergy.push_back(energytocompare);

  }//while(isotopeenergy

  if(isotopeenergy.size()==1)
  filteredisotopeenergy.push_back(isotopeenergy.at(0));

  cout << filteredisotopeenergy.size() << endl;
  */
