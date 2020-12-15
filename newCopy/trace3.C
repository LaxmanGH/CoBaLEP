void trace3(void)
{

  //Search keyword [intro] to jump to the
  //explanation of this script

  //Declare variables

  //First number from input: File number
  //Second number from input:Step number

  //Processing variables
  int filecheck = 0;
  int stepcheck = 0;
  int filenumber = 0;
  int stepnumber = 0;

  //Flags for efficiency or tracking
  int lastprocessed = 0;
  bool wasprocessed = false;
  int failedtimesaves = 0;
  double t1 = double(clock());
  int totalprocessed = 0;


  //I/O
  fstream validsteps("simdata.txt",ios_base::in);//input text file
  validsteps >> filecheck >> stepcheck;//first read , used to initialize

  TFile *input = new TFile(Form("output%i.root",filecheck),"read");
  TTree *currenttree;//tree used from input file
  currenttree = (TTree*)input->Get("fTree"); //fetch the Tree header in memory
  currenttree->SetBranchAddress("stepnumber",&stepnumber);
  currenttree->SetBranchAddress("randomseed",&filenumber);
  currenttree->GetEntry(0);
  cout <<filenumber<<endl << endl;
  TFile *output = new TFile(Form("cutsfromtracer%i.root",filenumber),"recreate");
  output->cd();
TTree* outputtree = (TTree*)(currenttree->CloneTree(0));
  input->cd();

  while(!validsteps.eof())
    {
      if(filecheck!=filenumber)
	{//Change of file; close current input and open new one
	  totalprocessed+=outputtree->GetEntries();
	  output->cd();
	  outputtree->Write();
	  output->Write();
	  delete outputtree;
	  output->Close();

          delete currenttree;
	  input->Close();
	  input = new TFile(Form("output%i.root",filecheck),"read");
	  currenttree = (TTree*)input->Get("fTree"); //fetch the Tree header in memory
	  currenttree->SetBranchAddress("stepnumber",&stepnumber);
	  currenttree->SetBranchAddress("randomseed",&filenumber);
	  currenttree->GetEntry(0);
	  cout <<filenumber<<endl << endl;
	  output = new TFile(Form("cutsfromtracer%i.root",filenumber),"recreate");
	  outputtree = (TTree*)currenttree->CloneTree(0);
	  input->cd();
	}

      wasprocessed = false;
      for(int j = (int)floor(lastprocessed*0.9); j < currenttree->GetEntries();j++)
	{//Only look a little behind, but always ahead
	  currenttree->GetEntry(j);
	  if(stepcheck == stepnumber)//Target step found, add it to the output tree
	    {
	      outputtree->Fill();
	      wasprocessed = true;
	      break;//No need to look at the rest of the entries
	    }

	}//Time saving attempt

      if(!wasprocessed)//Time saving attempt failed; start from 0
	{
	  failedtimesaves++;
	  for(int j = 0; j < currenttree->GetEntries();j++)
	    {
	      currenttree->GetEntry(j);
	      if(stepcheck == stepnumber)//Target step found, add it to the output tree
		{
		  outputtree->Fill();
		  wasprocessed = true;
		  break;//No need to look at the rest of the entries
		}
	    }//Full processing
	}

      if(!wasprocessed)
	cout <<"ERROR: stepnumber corresponding to input not located. Stepcheck: " << stepcheck << "  " << " Entries: " <<currenttree->GetEntries() << " File number: "<< filenumber << endl << endl;

      
      validsteps >> filecheck >> stepcheck;
    }//Process all target steps

  totalprocessed+=outputtree->GetEntries();
  output->cd();
  outputtree->Write();
  output->Write();
  output->Close();

  gSystem->Exec("hadd -f cutsfromtraceback.root cutsfromtracer*");
  gSystem->Exec("rm cutsfromtracer*");

  t1 = (double(clock()) - t1)/1000000;
  cout << "Processing complete." << endl << "Time taken: " << t1 << endl << "Steps written: " << totalprocessed << endl << "Failed time saves: " << failedtimesaves << endl << endl;


}//EOF



/* Keyword [intro]

The final tracing script has been in desperate need of remodeling.
I tried to do something fancy with vectors of vectors in the pre-
vious iteration, but it failed spectacularly. So I resorted to
one of the stupidest, easiest, least efficient ways of process-
ing the data, and I paid for it dealy with severely bloated
finaltrace runtimes. Picking out just 100 steps from a data set
took upwards of 15 minutes, which is completely unacceptable.

Our savior is the newish stepnumber parameter, which assigns
a unique identifier to every entry in the processed data.
No more looking for ultra-precise time and energy signatures
in order to make sure we're grabbing the correct entry.
The other savior is me being a little better at coding than
I was two years ago.

The input file has a file number and a step number.
Neither parameter is ordered; however, every entry belonging to a
single file number is written sequentially, so we at least don't 
have to worry about grouping entries together. Just reading them in
order should process a whole file before moving on to the next one.


Assuming we have the correct file opened and ready for processing,
we can grab each target step number. Then we just have to find it.
Two methods jump out at me for processing the steps:

r is #entries in the raw data, t is number of target entries

1) Compare every step to the target number until the right one is
found, and then move on to the next target number
Processing time: (r*t)/2
r is #entries in the raw data, t is number of target entries
This one's super simple, but becomes inefficient for large t.
t is <=r, so worst case, its processing time is (r^2)/2
But, t should in principle never be anywhere close to r.

2) Sort the target steps in each file, then process sequentially
Processing time: tlog(t)+r + unknown vector allocating/dealloc
This one's way faster for large t - depending on the vector
operations processing times, which I don't have estimates for.
But, it'd be as much of a bitch to code as part of a builder...


I'm leaning for an iteration on 1) with a little optimization.
While it's true that the stepnumbers in each file are unsorted,
most steps aren't incredibly unsorted. If your current stepnum-
ber is 23000, the next stepnumber is probably either 23005,
22990, or 65006 - or something like that. I'm thinking that,
instead of starting from entry 0 each time a target entry is
found, I'll start from (target)*0.9. Odds are good that this
will catch the next target, even if it's lower than the current
target. This is NOT guaranteed, though, so if the correct entry
isn't located, I have to assume that the next target is signifi-
cantly lower than the current target and start from 0 anyway.
Processing time:(r*t*(1-f))/20 + (r*t*f)/2
Where f is the failure rate of my clever method.
I'd tentatively estimate f at like 0.02. If that's true, the
processing time will be (r*t)/17 or so, still much better.

That's about all I have to say. Did you even read all that?
Did I even read all that? ¯\_(ツ)_/¯

 */
