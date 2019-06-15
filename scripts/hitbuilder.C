#include <iostream>
#include <algorithm>
#include <iterator>
#include<math.h>
#include <stdlib.h>
#include <stdio.h>
#include<time.h>

void hitbuilder()
{

  //Take a time-sorted output file (with G4events together also
  //and bundle hits within 20us of one another.
  //Input is in ns, so what we're really interested in is
  //events within time of 20,000 of one another.
  //Once bundled, generate a tree and shove the bundled
  //RealEvent into it and name the tree in a way that's
  //consistent with the timing window it covers.

  //I considered also working the G4event# into the naming
  //convention of the RealEvent trees, but then realized a
  //similar way to look at them individually could be accom-
  //plished by creative usage of TChains and the eventnumber
  //variable.


  //Clean mode means that entries with time>timecutoff
  //(presently 10^14 ns a.k.a. 3 hours)
  //will be assigned an artificial 'same' time.
  //ALSO, entries with energy depositions < energycutoff
  //will be stripped from the file. All these discriminators
  //are modifiable right here with these constant values.

  bool cleanmode = true;
  double energycutoff = 0.0001; //keV
  double timecutoff = 100000000000000; //ns
  double t1 = double(clock());
  int maximumstepsallowedinevent = 1000000;
  //Search for the next instance of this variable
  //being called if you're curious how it works

  
  //BLOCK 1: FIND FILES TO PROCESS
  //1111111111111111111111111111111111111111111111
  const char* allfilesbeginningwith = Form("output%i", startnumber);
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
      if(checkedstring.BeginsWith(allfilesbeginningwith))
	{
	  acceptedname[wan] = checkedname;
	  wan++;
	}
    }

  cout << wan << endl << wan << endl;

  //FILE FINDING ENDS HERE
  //1111111111111111111111111111111111111111111111111

  
  for(int iwan = 0;iwan < wan;iwan++)
    {
      cout << acceptedname[iwan] << endl;

      //FILE BY FILE PROCESSING BEGINS HERE

      //Initialize variables

      //IO
      TFile *input = new TFile(Form("%s",acceptedname[iwan]),"read");
      TFile *output = new TFile(Form("bundled%s",acceptedname[iwan]),"recreate");
      input->cd();

      TTree *fTree = (TTree*)input->Get("fTree");
      
      //setup to work with input tree

      int entries = fTree->GetEntries();
       
      double time = 0;
      int eventnumber = 0;
      int detectornumber = 0;
      double energy = 0;
      double x = 0;
      double y = 0;
      double z = 0;

      int hitnumber = 0;

      fTree->SetBranchStatus("*",0);
      fTree->SetBranchStatus("time",1);
      fTree->SetBranchStatus("eventnumber",1);
      fTree->SetBranchStatus("detectornumber",1);
      fTree->SetBranchStatus("energy",1);
      fTree->SetBranchStatus("x",1);
      fTree->SetBranchStatus("y",1);
      fTree->SetBranchStatus("z",1);
      fTree->SetBranchStatus("stepnumber",1);
      fTree->SetBranchStatus("weight",1);

      fTree->SetBranchAddress("time", &time);
      fTree->SetBranchAddress("eventnumber", &eventnumber);
      fTree->SetBranchAddress("detectornumber", &detectornumber);
      fTree->SetBranchAddress("energy", &energy);
      fTree->SetBranchAddress("x", &x);
      fTree->SetBranchAddress("y", &y);
      fTree->SetBranchAddress("z", &z);

      fTree->GetEntry(0);
      cout << "Entries in current input tree: " << entries << endl;
      
     
      /*

	Okay, so here's the general idea for hit building all laid out
	in one big paragraph.

	A 'hit' will be comprised of every step that shares a certain
	G4event#, a detectornumber, and are close together in time. By
	'close together' I mean that every step will be within 20 us
	of at least one other step in the hit. The first step in the
	hit should be the lowest time step of the entire hit, and will
	be the first step the others are compared against,
	time-wise. Once all prospective steps have been compared to it
	and the appropriate ones chosen, and if any were chosen at
	all, the LATEST step which matched will become the new time to
	compare to, and all the remaining prospective steps will be
	checked AGAIN. This will repeat until no matches are found for
	the latest step, i.e., no more steps within 20us of latest
	step.

	A vector called 'counted' that's exactly the size of the
	current G4event will be used to keep track of which steps ARE
	selected, and also which ones have BEEN seected in previous
	loops. Once every step has been selected and written, the
	G4event is done building and we can move one.

      */
      
      //Determine number of steps in each non-empty G4event

      //Initialize variables for this purpose
      int checkevent = eventnumber; //The event# to be compared against
      int numberofstepsinevent = 0; //Obvious name notation
      int G4eventsize[100] = {0}; //each G4event needs its own index size
      //Maximum of 100 events with hits possible
      //but should be much less than this
      int G4eventsizemover = 0;

      int masterincrementer = 0;
      int tempincrementer = 0;
      //These are used to control place in the tree
      //Each G4event will be masterincrementer steps from step 0
      //e.g. if there were events of step size 10, 30, and 77,
      //masterincrementer is always 0 initially, would 'hold' the
      //place of the end of the first step, e.g. would be at 10,
      //then on the next loop would be at 40, and finally would
      //end at 117

      int i = 0; //simple iterator


      while(i<entries) //determines number of hits in each G4event
	{
	  fTree->GetEntry(i);

	  if(eventnumber==checkevent)//Matching event#, same G4event
	    numberofstepsinevent++;

	  else//Non-matching event#, new hit
	    {
	      //Record step size of current G4event	      	      
	      G4eventsize[G4eventsizemover] = numberofstepsinevent;
	      cout <<"Size of G4event: " << numberofstepsinevent <<  endl;
	      //Start recording size of new G4event
	      numberofstepsinevent = 1;
	      checkevent = eventnumber;
	      G4eventsizemover++;
	      
	    }//else
	  
	  i++;
	  
	}//while(i<entries)

      G4eventsize[G4eventsizemover] = numberofstepsinevent; //to catch the last one

      cout << "Size of G4event: " << numberofstepsinevent << endl << "Number of entries determined" << endl;

      
      for (int k = 0; k<G4eventsizemover+1;k++)
	{//Loops over every non-empty G4event

	  cout << "Processing G4event with #steps = " << G4eventsize[k] << endl;


	  //Define boolean array to make sure bundled events aren't double counted
	  std::vector<int>counted;
	  counted.clear();
	  counted.resize(G4eventsize[k],0);
	  
	  /*A lot counts on this array so I'm going to explain it
	    well. A value of zero means the below loops haven't
	    decided that the step entry equal to (tempincrementer+L)
	    exists yet. Once they decide it exists, it needs to be
	    compared to every entry below it. So it's marked with a
	    '2' to indicate 'hey, this entry is the active one'. Once
	    an entry with the same detectornumber and same time window
	    is found, it is also given a '2'. Once all entries have
	    been cycled through, if any of them were marked active,
	    this step repeats with the highest time matching event
	    used as the new comparator. Once no more matches are
	    found, the index is looped over again, and any entry with
	    a '2' is added to a NEW event tree, the '2' is changed to
	    a '1' to signify that it's been added, and at the end of
	    this one time index loop the new tree is written to the
	    output file.*/
       
	  tempincrementer = masterincrementer;
	  //keeps track of my current place in the input file, since
	  //it's all one big tree but is G4event sorted
       

	  //make 2 1D vectors of stepnumbers and times
	  //use these vectors in the choosing loop  in 20 or so lines
	  //This is to reduce processing time by making fewer reads
	  //to the input file in the loops below
	  
	  std::vector<double> timechooser;
	  timechooser.clear();
	  timechooser.resize(G4eventsize[k],0);

	  std::vector<int> detchooser;
	  detchooser.clear();
	  detchooser.resize(G4eventsize[k],0);
       
	  for(int o = 0; o < G4eventsize[k]; o++)
	    {//fill the time and detector comparing vectors above

	      fTree->GetEntry(o + tempincrementer);
	      timechooser[o] = time;
	      detchooser[o] = detectornumber;
	     
	    }


	  if(cleanmode)
	    {//remove very low energy steps
	      for(int j = 0;j<G4eventsize[k];j++)
		{
		  fTree->GetEntry(tempincrementer+j);
		  if(energy<energycutoff)
		    counted[j] = 1;
		  //pretend this step has already been written
		  //so the rest of the program will ignore it
		}
	      cout << "Removing " << std::accumulate(counted.begin(), counted.end(), 0) << " low energy steps in clean mode." << endl;
	    }

	  int numberofpromptstepsremoved = 0;
	  if(G4eventsize[k]>maximumstepsallowedinevent)
	    {
	      cout << "Abnormally large muon event detected. Reducing processing time and file size by cutting all prompt (<20 us) steps." << endl ;
	      for(int L = 0;L<G4eventsize[k];L++)
		{
		  if(timechooser[L]<20000&&!counted[L])
		    {
		      counted[L] = 1;
		      numberofpromptstepsremoved++;
		    }
		}
		      
	      cout << "An additional " << numberofpromptstepsremoved << " prompt steps removed from this G4event because it's too large" << endl;	  
	    }
		  

	  bool donegrabbing = false;
	  //Used for continuous build mode
	  //Once this stays true, no more steps
	  //will need to be added and we can
	  //move on to the writing phase.
		  
	  while(std::accumulate(counted.begin(), counted.end(), 0)!=G4eventsize[k])
	    {//Until every step has been counted, keep going
	      
		cout << "Number of steps filled: " << std::accumulate(counted.begin(), counted.end(), 0) << endl;


	   
	      //Find lowest time value not written

	      //Initialize variables
	      double lowesttime = 0;
	      //Unwritten step with lowest time, to compare to
	      //Will become active step
	      int loweststep = 0;
	      //Entry number of currently active lowest step
	      double timecompare = 0;
	      //time of currently active lowest step
	      int detectororiginal = 0;
	      //detector of currently active lowest step

	      
	      for(int Q = 0;Q<G4eventsize[k];Q++)
		{//time grabbing loop

		  if(counted[Q]) continue;//No need to compare filled values

		  if(!lowesttime || timechooser[Q]<lowesttime)
		    {//Currently lowest time unwritten step
		     
		      lowesttime = timechooser[Q];
		      loweststep = Q;
		    }
		}

	      
	      counted[loweststep]=2;//Marked 'will be written'
	      
	      //Grab comparands	      
	      timecompare = lowesttime;
	      detectororiginal = detchooser[loweststep];

	      //clean mode
	      if(cleanmode && timecompare>timecutoff)
		timecompare=timecutoff;


	      //Compare all non-written values
	      int highesttime = 0;
	      donegrabbing = false;
	      while(!donegrabbing)
		{
		  donegrabbing = true;//until proven false
		  for(int r=0;r<G4eventsize[k];r++)
		    {//Check all steps in G4event (again)

		      if(cleanmode && timechooser[r]>timecutoff)
			timechooser[r] = timecutoff+1;
			
	       
		      if(!counted[r]&&detchooser[r]==detectororiginal && timechooser[r]<=(timecompare+20000) && r!=loweststep)
			{//We have a match!

			      counted[r]=2;
			      if(highesttime<timechooser[r])
				highesttime = timechooser[r];
			      //continuous build

			      donegrabbing = false;//Go another round

			}//We have a match!
		     
		    }//Checking all steps in G4event/for(int r=0;...
		  timecompare = highesttime;

		}//while not done grabbing
	      highesttime = 0; 


	      
	      //Now to go through and write all counted matches

	      //A bunch of tedious garbage to get names right for IO
	      //CONVERTS LOWESTTIME TO A STRING
	      std::ostringstream timestream;
	      if(lowesttime>=1000000000)
		timestream.precision(15);
	      double splo2 = lowesttime;
	      timestream << splo2;
	      std::string timestring = timestream.str();
	   
	      //CONVERTS EVENTNUMBER TO A STRING
	      std::ostringstream eventstream;
	      eventstream << eventnumber;
	      std::string eventstring = eventstream.str();
	   
	      //CONVERTS DETECTORNUMBER TO A STRING
	      std::ostringstream detectorstream;
	      detectorstream << detectororiginal;
	      std::string detectorstring = detectorstream.str();
	   
	   
	      string prefix = "bundledtree";
	      string hyphenator = "__";
	      string suffix = ".root";
	   
	      string treename = prefix + eventstring + hyphenator + timestring + hyphenator + detectorstring + suffix;
	   
	      // declaring character array
	      char tree_array[60]; 
	   
	      // copying the contents of the 
	      // string to char array
	      //Form command will not work with strings
	      strcpy(tree_array, treename.c_str()); 
	   
	   
	      TTree *bundledtree = (TTree*)fTree->CloneTree(0);
	      bundledtree->SetName(Form("%s",tree_array));

	      //Add branch to the bundled tree for hitnumber
	      bundledtree->Branch("hitnumber",&hitnumber);
	      hitnumber++;

	      for(int m=0;m<G4eventsize[k];m++)
		{
		  if(counted[m]==2)
		    {
		      fTree->GetEntry(tempincrementer+m);

			bundledtree->Fill();

			if(energy<energycutoff)
			  cout <<"HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHOW" << endl;
			
		      counted[m]=1;
		    }
		}//Writing loop obviously
		   
	      output->cd();	   

	      if(bundledtree->GetEntries())
		bundledtree->CloneTree()->Write();
	      //The above may seem a weird inclusion, but
	      //with cleanmode on, it's possible to create
	      //a tree but have no entries that pass the
	      //cutoffs to be written to it. So this
	      //prevents empty trees from being added.
	      delete bundledtree;

	      
	      input->cd();
	   	      
	   
	    }//Goes through current G4event until done
       
	  masterincrementer = masterincrementer+G4eventsize[k];
	  //Gotta keep the step placeholder up to date

	}//Switches G4events/for(int k
   
    }//iterates on each file

  t1 = (double(clock())-t1);
  cout <<"Total time taken: " << t1/1000000 << endl;
  
}//EOF

