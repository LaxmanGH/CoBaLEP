void heatmap(void)
{

  TFile input("all.root", "read");
  
  TH2D *heatmap = new TH2D("heatmap","Particle starting z vs. creation process",20,0,20,1000,-5000,5000);

  
  fTree->Draw("startz:creatorprocess>>heatmap","PID!=13","colz");

  heatmap->SetContour(100);
  heatmap->GetXaxis()->SetTitle("Creation Process");
  heatmap->GetYaxis()->SetTitle("Starting z position (mm)");
  heatmap->GetXaxis()->CenterTitle();
  heatmap->GetYaxis()->CenterTitle();
  heatmap->GetXaxis()->SetTitleOffset(1.55);
  heatmap->GetXaxis()->SetLabelSize(0.03);
  // heatmap->GetYaxis()->SetTitleOffset(1.3);
  c1->SetLogz();

  heatmap->SetDirectory(gROOT);
  heatmap->Draw("colz");
  
}
