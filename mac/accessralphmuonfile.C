//weighting with incident muon distribution
//https://escholarship.org/uc/item/6jm8g76d#page-3
double distribution(double Energy, double CosTheta)
{
  double CosThetaStar = sqrt(
	(pow(CosTheta,2) + pow(0.102573,2) -0.068287*pow(CosTheta,0.958633)+0.0407253*pow(CosTheta,0.817285) )/
	(1+pow(0.102573,2)-0.068287+0.0407253));

  double I;
  I = 0.14*pow(Energy*(1+3.64/(Energy*pow(CosThetaStar,1.29))),-2.7)
	  *((1./(1+(1.1*Energy*CosThetaStar)/115.))+(0.054/(1+(1.1*Energy*CosThetaStar)/850.)));

  return (I);
}




void accessralphmuonfile(void)
{

 int max_entries = 0;
 TFile *inputfile= TFile::Open("Muonbackground.root","READ");
 TTree *datatree = (TTree*)inputfile->Get("tree");
 
 max_entries = datatree->GetEntries();


 //data structure

  Int_t            n = 0;
  Double_t        start_energy = 0;      //muon start energy in surface GeV
  Double_t        start_costheta = 0;    //muon start angle
  std::vector<double>  *particle_energy = 0;
  std::vector<double>  *particle_momentumX = 0;  //GeV
  std::vector<double>  *particle_momentumY = 0;  //GeV
  std::vector<double>  *particle_momentumZ = 0;  //GeV
  std::vector<double>  *particle_positionX = 0;  //m 0,0 is yates
  std::vector<double>  *particle_positionY = 0;  //m 0,0 is yates
  std::vector<double>  *particle_positionZ = 0;  //m 0,0 is yates
  std::vector<std::string>  *particle_name = 0;

  TBranch        *b_n;   //!
  TBranch        *b_start_energy;   //!
  TBranch        *b_start_costheta;   //!
  TBranch        *b_particle_energy;   //!
  TBranch        *b_particle_momentumX;   //!
  TBranch        *b_particle_momentumY;   //!
  TBranch        *b_particle_momentumZ;   //!
  TBranch        *b_particle_positionX;   //!
  TBranch        *b_particle_positionY;   //!
  TBranch        *b_particle_positionZ;   //!
  TBranch        *b_particle_name;   //!
  
  datatree->SetBranchAddress("n", &n, &b_n);
  datatree->SetBranchAddress("start_energy", &start_energy, &b_start_energy);
  datatree->SetBranchAddress("start_costheta", &start_costheta, &b_start_costheta);
  datatree->SetBranchAddress("particle_energy", &particle_energy, &b_particle_energy);
  datatree->SetBranchAddress("particle_momentumX", &particle_momentumX, &b_particle_momentumX);
  datatree->SetBranchAddress("particle_momentumY", &particle_momentumY, &b_particle_momentumY);
  datatree->SetBranchAddress("particle_momentumZ", &particle_momentumZ, &b_particle_momentumZ);
  datatree->SetBranchAddress("particle_positionX", &particle_positionX, &b_particle_positionX);
  datatree->SetBranchAddress("particle_positionY", &particle_positionY, &b_particle_positionY);
  datatree->SetBranchAddress("particle_positionZ", &particle_positionZ, &b_particle_positionZ);
  datatree->SetBranchAddress("particle_name", &particle_name, &b_particle_name);

  
  //Let's try to output some info

  //Normalization of positions has been deprecated 8/29/2018
  
  //X positions in the file range from -73.2593 to -52.0251
  //Difference: 21.2342 m... Median: -62.6422
  //X positions in sumulation range from -50 to 50 m
  //To normalize, gotta add 62.6422 and multiply by 100/21.2342

  //Y positions in the file range from 75.0549 to 117.964
  //Difference: 42.9094 m... Median: 96.50945
  //Y positions in sumulation range from -50 to 50 m
  //To normalize, gotta subtract 96.50945 and multiply by 100/42.9094

  
 //Not normalizing z positions. They'll all be the same (how about 11.5?)
 //This is 5 m above the detector geometry (zmax = 6.5)

  TFile *output = new TFile("testSmallMuonFile.root","RECREATE");

  double startenergy = 0;
  double startcostheta = 0;
  double x = 0;
  double y = 0;
  double z = 18;
  double px = 0;
  double py = 0;
  double pz = 0;
  double energy = 0;
  double weight = 0;
  
  TTree *muontree = new TTree("muontree","Muon information only");
  muontree->Branch("startenergy",&startenergy);
  muontree->Branch("startcostheta", &startcostheta);
  muontree->Branch("x", &x);
  muontree->Branch("y", &y);
  muontree->Branch("z", &z);
  muontree->Branch("px", &px);
  muontree->Branch("py", &py);
  muontree->Branch("pz", &pz);
  muontree->Branch("energy", &energy);
  muontree->Branch("weight", &weight);
  
  TH1D *xposition = new TH1D("xposition", "Muon x positions",500, -50, 50);
  TH1D *yposition = new TH1D("yposition", "Muon y positions",500, -50, 50);
  
  char* comparator;
  double highest = -10000;
  double lowest = 1000;
  int highkeeper = 0;
  int lowkeeper = 0;
  double xnorm = 0;
  double ynorm = 0;
  long double weightsum = 0;
  TRandom3 *rng = new TRandom3(10408774);

  
  for(int i = 0; i < max_entries; i++)
    {
      datatree->GetEntry(i);
      comparator = (particle_name->at(0)).data();

      if((int)comparator[0] == 109 && (int)comparator[1] == 117 && (int)comparator[2] == 45) //If comparator = "mu-"; excludes non-muons
     {

       /*Normalize positions and fill
       //Note: this was deprecated due to the x,y, positions of this file and Geant4 being misaligned
       //Now the x and y positions are sampled uniformly from a 1000m^2 square plane
       //located symmetrically above the detector geometry's center
       xnorm = ((particle_positionX->at(0))+62.6422)*(TMath::Sqrt(1000)/21.235);
       xposition->Fill(xnorm);

       ynorm = ((particle_positionY->at(0))-96.50945)*(TMath::Sqrt(1000)/42.91);
       yposition->Fill(ynorm);
       
       x = xnorm;
       y = ynorm;
       */

       x = (rng->Rndm()-0.5)*100; //m 
       y = (rng->Rndm()-0.5)*100; //m
       
       xposition->Fill(x);
       //z is always 16.5 m
       px = particle_momentumX->at(0); //GeV
       py = particle_momentumY->at(0); //GeV
       pz = particle_momentumZ->at(0); //GeV
       startenergy = start_energy; //GeV
       startcostheta = start_costheta;
       energy = particle_energy->at(0);       
       weight = distribution(startenergy, .65)*285589442.329;
       //large number at end of above line is normalization factor
       //if(TMath::ACos(startcostheta)*180/TMath::Pi()>65.5)
       weightsum += weight;
       
       muontree->Fill();
       
       
       if(lowest>particle_positionY->at(0))
	 {
	   //cout << "NEW LOWEST X " << particle_positionX->at(0) << endl;
	   lowkeeper = i;
	   lowest = particle_positionY->at(0);
	 }
       if(highest<particle_positionY->at(0))
	 {
	   //cout << "NEW HIGHEST X " << particle_positionX->at(0) << endl;
	   highkeeper = i;
	   highest = particle_positionY->at(0);
	 }
       
     }
    }

  cout << "Lowest: " << lowest << endl << "Highest: " << highest << endl << "Difference: " << lowest-highest << endl;

  //datatree->GetEntry(lowkeeper);
  //xnorm = ((particle_positionX->at(0))+62.6422)*(TMath::Sqrt(1000)/21.235);
  //ynorm = ((particle_positionY->at(0))-96.50945)*(TMath::Sqrt(1000)/42.91);
  //cout << "LOWEST Y VALUE AND ASSOCIATED X VALUE (in that order)" << endl <<
  //  ynorm << endl << xnorm << endl;

  //datatree->GetEntry(highkeeper);
  //xnorm = ((particle_positionX->at(0))+62.6422)*(TMath::Sqrt(1000)/21.235);
  //ynorm = ((particle_positionY->at(0))-96.50945)*(TMath::Sqrt(1000)/42.91);
  //cout << "HIGHEST Y VALUE AND ASSOCIATED X VALUE (in that order)" << endl <<
  //  ynorm << endl << xnorm << endl;


  muontree->Write();

  cout << "WEIGHT SUM: "  << setprecision(25) << weightsum << endl;
  
  xposition->SetDirectory(gROOT);
  xposition->Draw();




  
}
