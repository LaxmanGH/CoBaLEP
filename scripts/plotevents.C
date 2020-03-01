void plotevents(void)
{

  TFile input("allsum.root","READ");
  TTree *sumtree = (TTree*)input.Get("sumtree");
  TH1F *lowhit = new TH1F("lowhit","Summed hit energies (low energy)", 200, 0, 6000);
  TH1F *lowhitone = new TH1F("lowhitone","Summed hit energies(low energy)", 200, 0, 6000);
  
  TH1F *lowevent = new TH1F("lowevent", "Summed event energies (low energy)", 6000, 0, 6000);

  TH1I *multiplot = new TH1I("multiplot", "Multiplicity (all energies)",1080,0,1080);
  int entries = sumtree->GetEntries();
  int lowentries = 0;
  vector<double> *energysum = 0;
  int multiplicity;
  int j = 0;
  double realsum = 0;
  
  cout <<"Number of events: " << entries << endl;

  sumtree->SetBranchAddress("energysum",&energysum);
  sumtree->SetBranchAddress("multiplicity",&multiplicity);

  
  for (int i=0;i<entries;i++)
    {
      sumtree->GetEntry(i);
      j = 0;
      realsum = 0;
      
      while(j<energysum->size())
	{
	  if(energysum->at(j) <= 6000)
	    {
	      lowentries++;
	      lowhit->Fill(energysum->at(j));
	      if(multiplicity==1)
		lowhitone->Fill(energysum->at(j));
	    }
	  

	  realsum+=energysum->at(j);
	  j++;
	}//loop over one entry of energysum

      lowevent->Fill(realsum);
      multiplot->Fill(multiplicity);         
    }


  
  TCanvas *c2 = new TCanvas("c2");
  c2->cd();
  c2->SetLogy();

  lowevent->GetXaxis()->SetTitle("Energy (keV)");
  lowevent->GetYaxis()->SetTitle("Counts");
  lowevent->GetXaxis()->CenterTitle();
  lowevent->GetYaxis()->CenterTitle();
  lowevent->GetYaxis()->SetTitleOffset(1.15);
  lowevent->SetFillColor(kGreen);

  
  lowevent->SetDirectory(gROOT);
  lowevent->Draw();

TCanvas *c3 = new TCanvas("c3");
  c3->cd();
  c3->SetLogy();
  multiplot->GetXaxis()->SetTitle("Multiplicity");
  multiplot->GetYaxis()->SetTitle("Counts");
  multiplot->GetXaxis()->CenterTitle();
  multiplot->GetYaxis()->CenterTitle();
  multiplot->GetYaxis()->SetTitleOffset(1.15);
  multiplot->SetFillColor(kOrange);
  multiplot->SetLineColor(kOrange);
  multiplot->SetDirectory(gROOT);
  multiplot->Draw();

  TCanvas *c1 = new TCanvas("c1");
  c1->cd();
  c1->SetLogy();
  
  lowhit->GetXaxis()->SetTitle("Energy (keV)");
  lowhit->GetYaxis()->SetTitle("Counts");
  lowhit->GetXaxis()->CenterTitle();
  lowhit->GetYaxis()->CenterTitle();
  lowhit->GetYaxis()->SetTitleOffset(1.15);
  lowhit->SetFillColor(kRed);
  lowhit->SetDirectory(gROOT);
  lowhitone->SetFillColor(7);
  lowhitone->SetDirectory(gROOT);
  lowhit->Draw();
  lowhitone->Draw("same");
  
}
