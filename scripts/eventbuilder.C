#include "TSystem.h"
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

void eventbuilder(void)
{
  //Clear up the directory a little bit
  gSystem->Exec("cat slurm*.out > batch2.out");
  sleep(5);
  gSystem->Exec("rm slurm*.out");


  //BLOCK 1: ITERATING ON EACH FILE
  //1111111111111111111111111111111111111111111111
  const char* allfilesbeginningwith = "bundledoutput";
  const char* indirectory = ".";

  char* directory = gSystem->ExpandPathName(indirectory);
  void* directoryP = gSystem->OpenDirectory(directory);

  const char* checkedname;
  const char* acceptedname[100];
  int n = 0;
  TString checkedstring;

  while((checkedname = (char*)gSystem->GetDirEntry(directoryP))) {
    checkedstring = checkedname;
    cout << "Checked string: " << checkedstring << endl;
    if(checkedstring.BeginsWith(allfilesbeginningwith))
      {      
	acceptedname[n] = checkedname;
	n++;
      }
  }

  for(int i = 0;i < n;i++)
    {
    cout << acceptedname[i] << endl;
    //1111111111111111111111111111111111111111111111111



    //Parse number from input file name

    string name = acceptedname[i];
    string numstring = "";

    for(std::string::size_type placer = 0;
	placer < name.size(); ++placer)
      {
	if(isdigit(name[placer]))
	  numstring += name[placer];
      }

    int truenum = 0;

    stringstream preem(numstring);

    preem >> truenum;


    
    
    //BLOCK 2: ITERATING ON EACH TREE
    //2222222222222222222222222222222222222222222222222

    //Variables for iterating on trees
    const char *keyname;
    TFile currentfile(Form("%s",acceptedname[i]));
    currentfile.Print();
    TTree *currenttree;
    TIter nexto(currentfile.GetListOfKeys());
    TKey *key;
    TTree *T;
    //Comparands 
    bool first = true;
    double timeoriginal = 0;
    double timeeventual = 0;
    int hitcounter = 0;
    //Input tree info
    int entries = 0;
    int detectornumber = 0;
    int eventnumber = 0;
    double time = 0;
    int G4event = 0;
    double energy = 0;//for setting branch address
    int hitnumber = 0;

    //Output tree info
    double energysum = 0;
    vector<double> energyvector;
    vector<int> detectorvector;
    vector<int> hitvector;
    TTree *sumtree = new TTree("sumtree","sumtree");
    sumtree->Branch("multiplicity",&hitcounter);
    sumtree->Branch("G4event",&G4event);
    sumtree->Branch("starttime",&timeoriginal);
    sumtree->Branch("endtime",&timeeventual);
    sumtree->Branch("energysum",&energyvector);
    sumtree->Branch("detector",&detectorvector);
    sumtree->Branch("filenumber", &truenum);
    sumtree->Branch("hitnumber",&hitvector);
    
    while ((key=(TKey*)nexto()))
      {
	if (strcmp(key->GetClassName(),"TTree")) continue; //do not use keys that are not trees
	currenttree = (TTree*)currentfile.Get(key->GetName()); //fetch the Tree header in memory
	keyname = key->GetName();
	cout << key->GetName() << endl;
	
	//TREE BY TREE PROCESSING STARTS HERE
	//Each tree is already a bundled tree, ergo, they all share
	//G4eventnumber, time block, and detectornumber
	//Each tree would be a 'hit' in MAJORANA
	//I need to sum each tree's energies to get hit energy
	//To get multiplicity... I need all trees with same g4event#, time cycle
	//To correlate these, I need to make an ntuple tree in the right way
	//This will require pre-filled vectors to do right.
	
	entries = currenttree->GetEntries();
	cout << entries << endl;

	currenttree->SetBranchAddress("time", &time);
	currenttree->SetBranchAddress("energy", &energy);
	currenttree->SetBranchAddress("detectornumber", &detectornumber);
	currenttree->SetBranchAddress("eventnumber", &eventnumber);
	currenttree->SetBranchAddress("hitnumber",&hitnumber);

	currenttree->GetEntry(0);
	double lowesttime = time;
	double highesttime = time;
	for(int gi = 0; gi < entries; gi++)
	  {
	    currenttree->GetEntry(gi);
	    if(time<lowesttime)
	      lowesttime = time;
	    else if(time>highesttime)
	      highesttime = time;

	    //cout << time << endl;
	  }
	//	  cout <<"CHANGEO"<<endl;
	//If [hit] is changing, write to the output tree,
	//flush the vectors and multiplicity, grab new value for times and event#
	//[hit] can change if eventnumber OR time block changes
	//If [hit] is NOT changing, fill the vectors and move on
	//after incrementing multiplicity, of course
	
	if(first==true)//Very first tree comparison, initialize
	  {
	    //cout << "First" << endl;
	    timeoriginal = lowesttime;
	    timeeventual = highesttime;
	    G4event = eventnumber;
	    hitcounter = 1; //all only need one entry
	    first=false;

	    for(int j = 0;j<entries;j++)
	      {
		currenttree->GetEntry(j);
		energysum+=energy;
	      }
	    energyvector.push_back(energysum);
	    detectorvector.push_back(detectornumber);
	    hitvector.push_back(hitnumber);
	    energysum = 0;
	  }//if(first

	else if(time>(timeeventual+20000)||eventnumber!=G4event)
	  {
	    //cout <<"Change"<<endl;
	    //Hit change, write and flush and grab
	    //write
	    sumtree->Fill();
	    //	    cout << timeoriginal << "  " << timeeventual << endl;
	    //flush
	    energyvector.clear();
	    detectorvector.clear();
	    hitvector.clear();
	    //grab
	    hitcounter=1;//The current tree is being included, so it is 1
	    timeoriginal=lowesttime;
	    timeeventual=highesttime;
	    G4event = eventnumber;

	    for(int j = 0;j<entries;j++)
	      {
		currenttree->GetEntry(j);
		energysum+=energy;
	      }
	    energyvector.push_back(energysum);
	    detectorvector.push_back(detectornumber);
	    hitvector.push_back(hitnumber);
	    energysum = 0;
	  }//Hit change


	
	else//same hit, add to the vectors and multiplicity only, change timeeventual to reflect latest time
	  {
	    //cout <<"Continue"<<endl;
	    for(int j = 0;j<entries;j++)
	      {
		currenttree->GetEntry(j);
		energysum+=energy;
	      }
	    energyvector.push_back(energysum);
	    detectorvector.push_back(detectornumber);
	    hitvector.push_back(hitnumber);
	    energysum = 0;
	    hitcounter++;
	    if(timeeventual<highesttime)
	      timeeventual = highesttime;
	  }//Same hit
	
	//cout <<"One less" <<endl;
	//TREE BY TREE PROCESSING ENDS HERE
	//delete T;
      }//while key
    cout <<"New file created" << endl;
    sumtree->Fill();//Not 100% sure this is necessary

    TFile *output = new TFile(Form("sumtree%i.root",truenum),"recreate");
    sumtree->Write();
    output->Close();
    
    }//for int i = 0
  //222222222222222222222222222222222222222222222222222222222



}//EOF


//Store garbage methods down here
  /*  TChain *chain1 = new TChain();
  chain1->Add("/home/CJ.Barton/CoBaLEP/Truon_GUORE/simfiles/fakefiles/output*.root");
  cout << chain1->GetEntries() << endl;
  while(chain1->GetFile())
    {

      TFile *file1 = chain1->GetFile();

      cout << fTree->GetEntries() << endl;
      
    }
  

  TObjArray *allfiles=chain1->GetListOfFiles();
TIter nextfile(allfiles);
TChainElement *link1=0;
while ((link1=(TChainElement*)nextfile()))
  {
    TFile f(link1->GetTitle());
    //    ... do something with f ...
    cout << fTree->GetEntries() << endl;
  }*/



    //    TFile currentfile(Form("%s",acceptedname[i]),"READ");

    //TChain *chain1 = new TChain("fTre*");
    //chain1->Add(Form("%s",acceptedname[i]));
    //cout << chain1->GetEntries() << endl;

/*
class StoreOne{
public:
  float time;
  int multiplicity;
};

class StoreMore{
public:
  float energysum;
  int detectornum;
  };*/
