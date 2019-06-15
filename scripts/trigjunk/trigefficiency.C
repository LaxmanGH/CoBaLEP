void trigefficiency(void)
{
  //The output from the trig efficiency study will
  //be just a bunch of numbers. This script should
  //read in those numbers, populate a histogram with
  //them, and plot it. Simple as that.

  fstream input("efficiencytextfile.txt",ios_base::in);
  //input text file, obviously
  int currentfill = 0;//The number to be plotted
  TH1I* trigefficiencyplot = new TH1I("trigefficiencyplot","Trig cut efficiency;Number of attempts to sample a valid muon;Counts",130,0,130);
  int counter = 0;
  while(!input.eof())
    {
      input >> currentfill;
      trigefficiencyplot->Fill(currentfill);
      counter++;
      if(counter%10000==0)
	cout << counter << endl;
    }

  gStyle->SetOptStat("nemou");

  gStyle->SetStatX(.9);
  gStyle->SetStatY(.9);

  trigefficiencyplot->SetFillColor(kGray+2);
  trigefficiencyplot->SetLineColor(kGray+2);

  trigefficiencyplot->GetXaxis()->CenterTitle();
  trigefficiencyplot->GetYaxis()->CenterTitle();
  trigefficiencyplot->SetDirectory(gROOT);

  TCanvas *can = new TCanvas();
  can->SetLogy();

  trigefficiencyplot->Draw();

}
