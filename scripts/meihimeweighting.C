#include<time.h>

void meihimeweighting(void)
{
  //Weight the simulation output with a function
  //based off the Mei Hime paper 
  //https://arxiv.org/abs/astro-ph/0512125

//As of Feb 17 2020, the weighting is in two parts - the energy weighting and the angular (zenith) weighting

//The first is fairly straightforward:
//Define the energy weighting function
//Get the muon energy from the file, use it to get the muon's weighted value
//Assign the weighted value

//The second is slightly complicated by the fact that
//1) the muon's angle isn't directly stored and
//2) the angular sampling wasn't done flatly
//Right now, zenith angle is sampled from a cosine distribution
//This is to guarantee higher statistics for vertical muons
//So first, zenith angle needs to be found from momentum
//Second, the distribution needs to be made flat again
//Third, the mei hime angular weighting needs to be applied



  //BLOCK 1: FIND FILES TO PROCESS
  //1111111111111111111111111111111111111111111111
  const char* allfilesbeginningwith = "output";
  const char* allfilesbeginningwith2 = "nuclearinfo";
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
      if(checkedstring.BeginsWith(allfilesbeginningwith)||checkedstring.BeginsWith(allfilesbeginningwith2))
	{
	  acceptedname[wan] = checkedname;
	  wan++;
	}
    }

  cout << wan << endl << wan << endl;

  //FILE FINDING ENDS HERE
  //1111111111111111111111111111111111111111111111111



      TF1 *meihimeenergy = new TF1("meihimeenergy","[4]*TMath::Exp(-([0]*[3]*([2]-1)))*TMath::Power((x+[1]*(1-TMath::Exp(-([0]*[3])))),-[2])",1,2000);
      //[0]-[2] are constants
      //[3] is lab depth in km.w.e (4.3 at SURF)
      //[4] is a normalization constant when in use
      //set [4] == 1.397115e+10 for integral of 1 if range is 0-20000
      //set [4] == 1.418955e+10 for integral of 1 if range is 0-2000
      //set [4]==1 to not normalize at all

      meihimeenergy->SetParameters(0.4,693,3.77,4.3,1.41895e+10);
     cout << "Integral of energy weighting function (to check normalization): " << meihimeenergy->Integral(1,2000)<<endl;
     //TCanvas *c2 = new TCanvas();
      //c2->SetLogx();
      //c2->SetLogy();
      //meihimeenergy->Draw();

      TF1 *meihimeangle = new TF1("meihimeangle","[5]*([1]*TMath::Exp(-[0]/[2]/TMath::Cos(x*TMath::Pi()/180))+[3]*TMath::Exp(-[0]/[4]/TMath::Cos(x*TMath::Pi()/180)))/TMath::Cos(x*TMath::Pi()/180)",0,70);
      //[0] is vertical depth, which is 4.3 still(?)
      //May have to mess around with [0]
      //[1]-[4] are just fitted parameters from the Mei Hime paper
      //[5] is an optional normalization
      //Somewhere around 3.2854e+6 normalizes max to 1 at... Some depth

      meihimeangle->SetParameters(6.,.0000086,.45,.00000044,.87,8.17418e7);
      cout << "Integral of angular weighting function (to check normalization): " << meihimeangle->Integral(0,70)<<endl;

      /*
Testing scripts, used to verify that the weighting functions look the way they should

      TH1D *testangle = new TH1D("testangle","Angular distribution - Mei Hime parametrization;cos(theta);Arbitrary units",100000,0,1);

      for (int i = 1;i < 10000; i++)
	{
	  double j = i;
	  cout << meihimeangle->Eval(70*j/10000) << endl;

	  testangle->Fill(TMath::Cos(j/10000),(meihimeangle->Eval(70*j/10000)));

	}

      TCanvas *c3 = new TCanvas();
      gStyle->SetOptStat(1111111);
      testangle->Draw("hist");*/
      TCanvas *c3 = new TCanvas();
      //meihimeangle->Draw();
      

  
  for(int iwan = 0;iwan < wan;iwan++)
    {
      cout << acceptedname[iwan] << endl;

      //FILE BY FILE PROCESSING BEGINS HERE

      //Initialize variables

      //IO
      TFile *input = new TFile(Form("%s",acceptedname[iwan]),"UPDATE");
      TTree *fTree;
     
      double muonenergy = 0;
      double px = 0;
      double py = 0;
      double pz = 0;
      double pztransform = 0;
      double pztransformstore = 0;
      double weight = 0;

      if(strstr(acceptedname[iwan],"output"))
	{//Step level file
      fTree = (TTree*)input->Get("fTree");
      fTree->SetBranchAddress("muonenergy",&muonenergy);
	}

      else//Track level file doesn't have muonenergy, need to calculate it ourselves
	{
	        fTree = (TTree*)input->Get("nuclearTree");
		fTree->SetBranchAddress("muonpx",&px);
		fTree->SetBranchAddress("muonpy",&py);
	}

      fTree->SetBranchAddress("muonpz",&pz);

      if(fTree->GetBranch("weight"))
	{//Don't add an extra weight branch, ya doof
	  cout << "Weighting branch already exists. Please remove this existing branch before trying to add another one!" << endl << endl;
	  return;
	}

      TBranch *bweight = fTree->Branch("weight",&weight);

      TH1D *testtransform = new TH1D("testtransform","Zenith angle after momentum deconvolution and flattening",100,0,75);

      for(int i = 0;i < fTree->GetEntries(); i++)
	{
	  fTree->GetEntry(i);

	  if(strstr(acceptedname[iwan],"nuclearinfo"))
	    muonenergy = TMath::Sqrt(px*px+py*py+pz*pz);
	    //Calculate muonenergy ourselves for track level file
	  //For step level file, it's filled automatically

	  //Muon energy is in keV - convert to GeV
	  weight = meihimeenergy->Eval((muonenergy/1000000));
	  //pz is negative, unnormalized, not an angle, and
	  //will be in radians once transformed into an angle. Jeez.
	  pztransform = pz*-1;
	  pztransform = pztransform/muonenergy;
	  pztransform = TMath::ACos(pztransform);
	  //Now it should be ready to work with
	  //Flatten it
	  weight = weight/TMath::Cos(pztransform);//TMath::Cos uses radians
	  //Apply angle weighting function (which is in degrees, thank you very much)
	  pztransform = pztransform*TMath::RadToDeg();
	  cout << pztransform << endl;
	  weight = weight*meihimeangle->Eval(pztransform);
	  //Finally, multiply by the range of each function
	  //The reason why this is necessary is kinda complicated
	  //Ask me about it in person or via email - CJ
	  weight = weight * 2000 * 70;

	  //if(pztransform!=pztransformstore)
	  //{
	  //cout << "Transformed pz: " << pztransform << endl << endl;
	  //pztransformstore=pztransform;
	  //}
	  //testtransform->Fill(pztransform,1/TMath::Cos(pztransform*TMath::DegToRad()));
	  bweight->Fill();
	}//Branch filling

	fTree->Write();

	//testtransform->SetDirectory(gROOT);
	//TCanvas *c2 = new TCanvas;
	//testtransform->Draw("hist");

    }//for each file
}//EOF
