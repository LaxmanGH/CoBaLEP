double baselineremove(int centerbin, TH1D* histotocalc)
{
  //Average the baseline by averaging the bins +-5 to the side of the center bin

  double baseline = 0;

  for(int j = (centerbin-5);j<(centerbin+6);j++)
    {

      if(j!=centerbin)
	baseline+=histotocalc->GetBinContent(j);

    }

  baseline = baseline/10;//Baseline average

  baseline = histotocalc->GetBinContent(centerbin)-baseline;//Peak with baseline subtracted

  return baseline;
}

void plotR90(void)
{

  //A simple script for demonstrating the effectiveness of the R90 cut on event built data

  TFile input("allebuilt.root","READ");
  TTree *sumtree = (TTree*)input.Get("sumtree");
  int entries = sumtree->GetEntries();

  vector<double> *R90 = 0;
  double MJDeventenergy = 0;
  int multiplicity = 0;
  double avgnumerator = 0;
  double avgdenominator = 0;

  sumtree->SetBranchAddress("R90",&R90);
  sumtree->SetBranchAddress("MJDeventenergy",&MJDeventenergy);
  sumtree->SetBranchAddress("multiplicity",&multiplicity);

  TH1D *Ge77plot = new TH1D("Ge77plot","Ge77gs spectra in L200-like geometry;Energy(keV);Counts (no units yet)",2700,0,2700);
  Ge77plot->SetFillColor(kWhite);
  Ge77plot->SetLineColor(kBlack);

  TH1D *Ge77plotAC = new TH1D("Ge77plotAC","2614 keV gamma spectra in L200-like geometry;Energy(keV);Counts (no units yet)",2700,0,2700);
  Ge77plotAC->SetFillColor(kWhite);
  Ge77plotAC->SetLineColor(kRed);

  TH1D *Ge77plotACR90 = new TH1D("Ge77plotACR90","2614 keV gamma spectra in L200-like geometry;Energy(keV);Counts (no units yet)",2700,0,2700);
  Ge77plotACR90->SetFillColor(kWhite);
  Ge77plotACR90->SetLineColor(kMagenta);

  for(int i = 0; i < entries;i++)
    {
      sumtree->GetEntry(i);
      if(i%100000==0)
	cout << "Processing completion status: " << i*100/entries << "%" << endl;

      Ge77plot->Fill(MJDeventenergy);

      if(multiplicity==1)
	{
	  Ge77plotAC->Fill(MJDeventenergy);

	  if(R90->at(0)<2)
	    Ge77plotACR90->Fill(MJDeventenergy);
	}

    }//Process all entries

  Ge77plot->SetDirectory(gROOT);
  Ge77plotAC->SetDirectory(gROOT);
  Ge77plotACR90->SetDirectory(gROOT);

  Ge77plot->Draw();
  Ge77plotAC->Draw("same");
  Ge77plotACR90->Draw("same");

  TLegend *leggo = new TLegend(0.6,0.7,0.8,0.8);
  leggo->AddEntry(Ge77plot,"No cuts");
  leggo->AddEntry(Ge77plotAC,"AC cut");
  leggo->AddEntry(Ge77plotACR90,"AC + R90<2mm cut");
  leggo->Draw("same");

  cout << endl <<"All efficiencies are calculated AFTER AC cut has been applied." << endl;
  cout <<"Now calculating with baseline subtracted from both peaks." << endl << endl;

  avgnumerator = baselineremove(171,Ge77plotAC);
  avgdenominator = baselineremove(171,Ge77plot);
  cout << "AC efficiency at 511 keV line: " << avgnumerator/avgdenominator << endl;

  avgnumerator = baselineremove(341,Ge77plotAC);
  avgdenominator = baselineremove(341,Ge77plot);
  cout << "AC efficiency at 1022 keV line: " << avgnumerator/avgdenominator << endl;

  avgnumerator = baselineremove(531,Ge77plotAC);
  avgdenominator = baselineremove(531,Ge77plot);
  cout << "AC efficiency at 1592 keV line (DEP): " << avgnumerator/avgdenominator << endl;

  avgnumerator = baselineremove(702,Ge77plotAC);
  avgdenominator = baselineremove(702,Ge77plot);
  cout << "AC efficiency at 2103 keV line (SEP): " << avgnumerator/avgdenominator << endl;

  avgnumerator = baselineremove(872,Ge77plotAC);
  avgdenominator = baselineremove(872,Ge77plot);
  cout << "AC efficiency at 2614 keV line (FEP): " << avgnumerator/avgdenominator << endl;

  cout << "AC efficiency at all energies: " <<  (Ge77plotAC->GetEntries()/Ge77plot->GetEntries()) << endl << endl;

  avgnumerator = baselineremove(171,Ge77plotACR90);
  avgdenominator = baselineremove(171,Ge77plot);
  cout << "AC+R90 efficiency at 511 keV line: " << avgnumerator/avgdenominator << endl;

  avgnumerator = baselineremove(341,Ge77plotACR90);
  avgdenominator = baselineremove(341,Ge77plot);
  cout << "AC+R90 efficiency at 1022 keV line: " << avgnumerator/avgdenominator << endl;

  avgnumerator = baselineremove(531,Ge77plotACR90);
  avgdenominator = baselineremove(531,Ge77plot);
  cout << "AC+R90 efficiency at 1592 keV line (DEP): " << avgnumerator/avgdenominator << endl;

  avgnumerator = baselineremove(702,Ge77plotACR90);
  avgdenominator = baselineremove(702,Ge77plot);
  cout << "AC+R90 efficiency at 2103 keV line (SEP): " << avgnumerator/avgdenominator << endl;

  avgnumerator = baselineremove(872,Ge77plotACR90);
  avgdenominator = baselineremove(872,Ge77plot);
  cout << "AC+R90 efficiency at 2614 keV line (FEP): " << avgnumerator/avgdenominator << endl;


  cout << "AC+R90 efficiency at all energies: " <<  (Ge77plotACR90->GetEntries()/Ge77plot->GetEntries()) << endl << endl;

}//EOF
