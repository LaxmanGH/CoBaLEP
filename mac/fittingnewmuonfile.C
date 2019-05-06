void BinLogX(TH1*h) 
{//Script for setting variable binning on a log axis

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





void fittingnewmuonfile(void)
{

  //Dr. Xu: change this to your current working directory
  const char* muon_path = "/home/CJ.Barton/CoBaLEP/Truon_GUORE/mac";
  inputfile = new TFile(Form("%s/SmallMuonFile.root", muon_path) ,"READ");
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
   
   
  TH1D *energyhisto = new TH1D("energyhisto","Muon surface energy distribution (cut muon file);Energy(GeV) (20 GeV bins);Normalized probability",10000,0,200000);
  energyhisto->GetXaxis()->CenterTitle();
  energyhisto->GetYaxis()->CenterTitle();

  //BinLogX(energyhisto); //For variable log binning


  //TH1D *realphi = new TH1D("realphi","Azimuth angular distribution (cut muon file);Angle (degrees);Normalized probability",100,0,360);
  //TH1D *realtheta = new TH1D("realtheta","Zenith angular distribution (cut muon file);Angle (degrees);Normalized probability",75,0,75);

  TF1 *dednfit = new TF1("dednfit","[4]*TMath::Exp(-([0]*[3]*([2]-1)))*TMath::Power((x+[1]*(1-TMath::Exp(-([0]*[3])))),-[2])",1,2500);
  //Fitting function. Parameters 0-2 constant and from the Mei-Hime paper
  //Parameter 3 is depth of lab in km.w.e.
  //Parameter 4 is normalization constant
  
  dednfit->SetParameters(0.4,693,3.77,4.3,1.41035e+10);
  dednfit->SetParLimits(0,0.4,0.4);
  dednfit->SetParLimits(1,693,693);
  dednfit->SetParLimits(2,3.77,3.77);
  dednfit->SetParLimits(3,0,100);  


  double weightcheck = 0; //To make sure weighting was done correctly
  for(int i=0;i<max_entries;i++)
    {
      datatree->GetEntry(i);
      weightcheck+=weight;

      //phicalc = TMath::ATan(py/px)*360/TMath::Pi()+180;
      //realphi->Fill(phicalc, weight);
      
      //thetacalc = -1*TMath::ACos(pz/TMath::Sqrt(px*px+py*py+pz*pz))*180/TMath::Pi()+180;
      //thetacalc = TMath::Cos(thetacalc*TMath::Pi()/180);
      //realtheta->Fill(thetacalc, weight);

      // if(energy<2500)
      energyhisto->Fill(startenergy, weight);
      

    }


  TH1::AddDirectory(kFALSE);
  cout << "WEIGHT " << weightcheck << endl;

  cout << "bintegral " << energyhisto->Integral() << endl;
  
  //realphi->Draw();
  //realtheta->Draw();
  energyhisto->Draw();

  //energyhisto->Fit("dednfit","","",1,1000);
  //dednfit->Draw("same");
  //c1->SetLogy();
  //c1->SetLogx();
}
