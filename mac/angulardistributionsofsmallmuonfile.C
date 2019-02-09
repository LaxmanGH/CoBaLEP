void BinLogX(TH1*h) 
{

   TAxis *axis = h->GetXaxis(); 
   int bins = axis->GetNbins();

   Axis_t from = axis->GetXmin();
   Axis_t to = axis->GetXmax();
   Axis_t width = (to - from) / bins;
   Axis_t *new_bins = new Axis_t[bins + 1];
   cout << "From " << from << " to " << to << " width " << width << " bins " << bins << endl;
   for (int i = 0; i <= bins; i++) {
     new_bins[i] = TMath::Power(10, from + i * width);
     cout << "new bins " << i << " " << new_bins[i] << endl;
   } 
   axis->Set(bins, new_bins); 
   delete new_bins; 
}

void angulardistributionsofsmallmuonfile(void)
{

  const char* muon_path = "/home/CJ.Barton/Muon_GUORE/Truon_GUORE/mac";
  inputfile = new TFile(Form("%s/newSmallMuonFile.root", muon_path) ,"READ");
  datatree = (TTree*)inputfile->Get("muontree");

  int max_entries = datatree->GetEntries();
  
  //  double start_costheta = 0;    //muon start angle
  double weight = 0;  

  double px = 0;  //GeV
  double py = 0;  //GeV
  double pz = 0;  //GeV
  double energy = 0; //GeV
  double startenergy = 0; //GeV
  
  double thetacalc = 0;
  double phicalc = 0;
  
  datatree->SetBranchAddress("px", &px);
  datatree->SetBranchAddress("py", &py);
  datatree->SetBranchAddress("pz", &pz);
  datatree->SetBranchAddress("weight", &weight);
  datatree->SetBranchAddress("energy", &energy);
  datatree->SetBranchAddress("startenergy", &startenergy);
   
   
  TH1D *energyhisto = new TH1D("energyhisto","At-lab energies of muons with surface energy E<20 TeV;Energy(GeV) (50 GeV bins);Normalized probability",400,0,20000);
  energyhisto->GetXaxis()->CenterTitle();
  energyhisto->GetYaxis()->CenterTitle();
  //  energyhisto->GetYaxis()->SetRangeUser(10e-11,.1);
  //  energyhisto->Draw();
  //BinLogX(energyhisto);
  
  TH1D *realphi = new TH1D("realphi","Azimuthal angular distribution;Angle (degrees);Normalized probability",36,0,360);

  //TH1D *realtheta = new TH1D("realtheta","Zenith angular distribution (cut muon file);Angle (degrees);Normalized probability",75,0,75);

  TF1 *dednfit = new TF1("dednfit","[4]*TMath::Exp(-([0]*[3]*([2]-1)))*TMath::Power((x+[1]*(1-TMath::Exp(-([0]*[3])))),-[2])",1,2500);

  dednfit->SetParameters(0.4,693,3.77,4.3,1.41035e+10);
  dednfit->SetParLimits(0,0.4,0.4);
  dednfit->SetParLimits(1,693,693);
  dednfit->SetParLimits(2,3.77,3.77);
  dednfit->SetParLimits(3,0,100);  


  double weightcheck = 0;
  for(int i=0;i<max_entries;i++)
    {
      datatree->GetEntry(i);
      weightcheck+=weight;

      phicalc = TMath::ATan(py/px)*360/TMath::Pi()+180;
      realphi->Fill(phicalc, weight);
      
      thetacalc = -1*TMath::ACos(pz/TMath::Sqrt(px*px+py*py+pz*pz))*180/TMath::Pi()+180;
      //thetacalc = TMath::Cos(thetacalc*TMath::Pi()/180);
      //realtheta->Fill(thetacalc, weight);

      
      //if(thetacalc>60&&thetacalc<70)
      //if(startenergy<15000&&startenergy>10000&&thetacalc<35)//LE muons only
	  //energyhisto->Fill(energy, weight);
      

    }

  //realtheta->SetDirectory(gROOT);
  TH1::AddDirectory(kFALSE);
  cout << "WEIGHT " << weightcheck << endl;
  //cout << "bintegral " << energyhisto->Integral() << endl;
  realphi->Draw();
  //realtheta->Draw();
  //energyhisto->Draw();
  //energyhisto->Fit("dednfit","","",10,1000);
  //dednfit->Draw("same");
  //c1->SetLogy();
  //c1->SetLogx();
}
