// Driver functions to sort a 2D vector by column
//First sorts column two of such a vector; second sorts column three
bool sortcoltwo( const vector<double>& v1, const vector<double>& v2 )
{ 
 return v1[1] < v2[1]; 
} 
bool sortcolthree( const vector<double>& v1, const vector<double>& v2 )
{ 
 return v1[2] < v2[2]; 
} 

void hitbuilder2020(char* startfile)
{

  //It's bigger! It's badder! It's clearer!
  //It's faster and it doesn't leak memory!
  //Search for keyword [intro] for more notes

#include <iostream>
#include <algorithm>
#include <iterator>
#include<math.h>
#include <stdlib.h>
#include <stdio.h>
#include<time.h>


  //Variable declaration
  //Unless otherwise specified, these are the units:
  //Energy - keV
  //Time - nanoseconds
  //Position/length - mm

  //Tracking variables
  double t1 = double(clock());
  int totalstepscleaned = 0;
  int totalhitsmade = 0;

  //Soft variables that may need to be changed depending on analysis needs
  const double timecutoff = 10000000000000000000.;//That's 19 0's, 320 years
  const int timeminimum = 0;
  const double stepenergyminimum = 0.01;//10 eV, seems reasonable
  const double timewindow = 20000.;//20 microseconds, a bit long but this is what we used to do.
  //const int toomanysteps = 100000000;//Honestly, if a single event has 100 million steps, I
                                                                  //don't even want to look at it.

  //Input variables
  TFile input(Form("%s",startfile),"READ");//I don't have to capitalize READ but I want to
  TTree *fTree = (TTree*)input.Get("fTree");
  int entries = fTree->GetEntries();
 
  int hitnumber = 0;//Should never reset, only increment
  bool hitchange = false;
  double time = 0;
  int detectornumber = 0;
  int filenumber = 0;
  //There's no need to work with tracknumber, since it's not sorted and it's not the right
  //level of detail for our purposes
  int eventnumber = 0;
  int stepnumber = 0;
  double energydeposition = 0;
  double x = 0;
  double y = 0;
  double z = 0;
  //~double weight = 0;

  fTree->SetBranchStatus("*",0);

  fTree->SetBranchStatus("time",1);
  fTree->SetBranchStatus("detectornumber",1);
  fTree->SetBranchStatus("randomseed",1);
  fTree->SetBranchStatus("eventnumber",1);
  fTree->SetBranchStatus("stepnumber",1);
  fTree->SetBranchStatus("energydeposition",1);
  fTree->SetBranchStatus("x",1);
  fTree->SetBranchStatus("y",1);
  fTree->SetBranchStatus("z",1);
  //~fTree->SetBranchStatus("weight",1);

  fTree->SetBranchAddress("time",&time);
  fTree->SetBranchAddress("detectornumber",&detectornumber);
  fTree->SetBranchAddress("randomseed",&filenumber);//The file name is based off of the RNG seed
  fTree->SetBranchAddress("eventnumber",&eventnumber);
  fTree->SetBranchAddress("stepnumber",&stepnumber);
  fTree->SetBranchAddress("energydeposition",&energydeposition);
  fTree->SetBranchAddress("x",&x);
  fTree->SetBranchAddress("y",&y);
  fTree->SetBranchAddress("z",&z);
  //~fTree->SetBranchAddress("weight",&weight);
  fTree->GetEntry(0);//Typical 'first fill' call

  TFile output(Form("hbuilt%i.root",filenumber),"RECREATE");
  //The h stands for hit
  //The reason I don't just name it hitbuilt%s is because then every time I want to tab-complete
  //the filename in the terminal, it will wonder whether I want the hitbuilder.C or the hitbuilt.root
  //files. Just a little thing for my own convenience.
  output.cd();
  TTree *gTree = (TTree*)fTree->CloneTree(0);
  gTree->Branch("hitnumber",&hitnumber);//Is what it sounds like
  input.cd();


  //Analysis variables
  //I think I'll try the double sorting/mapping method this time. It's conceptually clean.
  //Hindsight update: it's like one of those puzzles that won't work until you put in the last piece, like tetris
  //But it's working, and it's pretty slick.


  //For work outside of the building loop (mostly event sizing)
  vector<unsigned int> *G4eventsize = new vector<unsigned int>();//Stores number of steps in each event; resets every file
  //Search keyword [eventsizing] for text located under program with additional thoughts
  int eventcomparator = eventnumber;//Used when sizing each event
  int sizeofcurrentevent = 0;
  int eventincrementer = 0;//Cumulative total of G4eventsize
  //Gives us the correct place to start GetEntry for each event


  //For work inside the event loop (sizing, sorting, and display, when necessary
  vector<vector<double>> eventsorter;//Used to double sort an event (no pun intended)
  //That's a mighty big data type though...
  vector<vector<double>> *uneventsorter = new vector<vector<double>> ();//Used to force memory clear from eventsorter
  //A change in eventnumber means a change in event :^)
  int castingnumberone = 0;//Some functions have trouble returning the correct type; this is some encouragement.
  int castingnumbertwo = 0;//Some functions have trouble returning the correct type; this is some encouragement.

  vector<unsigned int> samedetectorsize;//Stores number of steps in each same detector segment
  vector<unsigned int> *unsamedetectorsize = new vector<unsigned int> ();//I know there must be a better way to guarantee vectors are
  //properly removed from memory. But this one is explicit, comprehensible, and most importantly, it WORKS.
  //These are basically the same variables as sizing each event
  //I could reuse the event sizing variables, but this looks better and is less confusing
  int detectorcomparator = -1;
  int sizeofcurrentdetector = 0;
  int detectorincrementer = 0;
  double timecomparator = 0;

  cout << endl<< "Processing file #" << filenumber << " with " << entries << " total entries." << endl << endl;

  //Event sizing
  for (int i = 0; i < entries; i++)
    {
      fTree->GetEntry(i);
      if(eventcomparator==eventnumber)//same event
	  sizeofcurrentevent++;
      else//different event
	{
	  G4eventsize->push_back(sizeofcurrentevent);
	  sizeofcurrentevent = 1;
	  eventcomparator = eventnumber;
	}
    }
  G4eventsize->push_back(sizeofcurrentevent);//To catch the last event
  //Notice the fill only happens during an eventnumber change
  //Well, at the end of the file, there's no eventnumber to change to


  cout << "Number of events in this file: "<< G4eventsize->size() << endl << endl;

  for (int i = 0; i < G4eventsize->size(); i++)
    {//Loop over each event

      fTree->GetEntry(eventincrementer);//Beginning of current event

      cout << endl << endl <<"Processing event #" << eventnumber << " with "  << G4eventsize->at(i) << " entries." << endl;
      cout << eventincrementer << " of " << entries << " total steps processed so far." << endl << endl;

      delete uneventsorter;
      uneventsorter = new vector<vector<double>>();
      eventsorter.swap(*uneventsorter);//Force memory to be both unassigned AND freed back onto the heap

      //Internal loop 1: Filling the vector array to be sorted
      for(int j = eventincrementer; j < (eventincrementer+G4eventsize->at(i)); j++)
	{
	  	  fTree->GetEntry(j);
		  if(time>timeminimum&&time<timecutoff&&energydeposition>stepenergyminimum)//Process
		    eventsorter.push_back({(double)j,(double)detectornumber,time});
	  
	}//Internal loop 1: Filling sorter

      castingnumberone = eventsorter.size();
      castingnumbertwo = G4eventsize->at(i);
      //The number of removed events is the starting total minus the number in the event sorter
      cout << castingnumbertwo-castingnumberone << " steps removed in clean mode." << endl << endl;

      totalstepscleaned = totalstepscleaned+castingnumbertwo-castingnumberone;

      if(!castingnumberone)//Special case
	{
	  cout << "All steps removed by cleaning! Moving to next event..." << endl << endl;
	  eventincrementer += G4eventsize->at(i);
	  continue;
	}

      if(castingnumberone>1)
      sort(eventsorter.begin(),eventsorter.end(),sortcoltwo);//First sort only needs to be done once


      //Internal loop 2: Sizing
      //Search keyword [sizingsorting] for more information
      detectorcomparator = eventsorter[0][1];
      delete unsamedetectorsize;
      unsamedetectorsize = new vector<unsigned int>();
      samedetectorsize.swap(*unsamedetectorsize);
      sizeofcurrentdetector = 0;
      for(int j = 0; j < castingnumberone; j++)
	{//Vectors of size 0 won't process, which is good
	//Detector groups sizing
	if(detectorcomparator==eventsorter[j][1])//same detector
	  sizeofcurrentdetector++;
	else//different detector
	  {
	    samedetectorsize.push_back(sizeofcurrentdetector);
	    sizeofcurrentdetector = 1;
	    detectorcomparator = eventsorter[j][1];
	  }
      }//Internal loop 2: sizing the second sort

      samedetectorsize.push_back(sizeofcurrentdetector);//final fill
      //!cout <<"Number of detectors involved in event: "<< samedetectorsize.size() << endl;


      //Internal loop 3: second sorting
      detectorcomparator = 0;//Naughty reusing of a variable : ^Y
      if(samedetectorsize.size()!=eventsorter.size())
	{//A clever way of not sorting events where every step is in a unique detector
	  //This will mostly catch cleaned events with only one or two steps remaining
	  for(int j = 0;j<samedetectorsize.size();j++)
	    {
	      //cout << "samedetectorsize.at(" << j <<"): " << samedetectorsize.at(j)<< "  detectorcomparator: " << detectorcomparator << endl << endl;	      
	      sort(eventsorter.begin()+detectorcomparator,eventsorter.begin()+detectorcomparator+samedetectorsize.at(j),sortcolthree);
	      //I know this looks a bit complicated but it's just a lot of variable names
	      //This command is saying to sort eventsorter from the following values:
	      //Entry #detectorcomparator to entry #(size of current detector group)
	      detectorcomparator += samedetectorsize.at(j);
	    }
	}//Internal loop 3: sort #2


      //Internal loop 4: hit building
      //With our fully sorted vector array, we can go through and build hits
      //Remember, a hit changes any time a detectornumber changes
      //It can also change for the same detectornumber when the next
      //step dodges the timewindow, which is constantly updating
      //In all the builders where I know there are at least two events,
      //FIRST load comparands (do NOT fill)
      //Then, do your incrementing or not incrementing
      //If you increment, do the right thing

      //Load comparands
	  fTree->GetEntry(eventsorter[0][0]);
	  detectorcomparator = detectornumber;
	  timecomparator = time;


      for(int j = 0; j < castingnumberone; j++)
	{//One full event processing

	  fTree->GetEntry(eventsorter[j][0]);//Index of next step, sorted in detector AND time
	  
	  if(detectorcomparator==detectornumber&&time<(timecomparator+timewindow))//Guaranteed pass on first step
	    gTree->Fill();//Same hit, no further action needed

	  else
	    {
	      //cout << "Hit change" << endl;
	      hitnumber++;
	      gTree->Fill();//You have to increment BEFORE filling, to prevent an off by one
	      detectorcomparator = detectornumber;
	      timecomparator = time;
	    }
	  	  
	  //cout << eventsorter[j][1] << "  " << eventsorter[j][2] << endl;
	}//Internal loop 4: one full event built
      //!cout <<"Last hitnumber " << hitnumber << endl;
      hitnumber++;//Prepares for the next loop
      eventincrementer += G4eventsize->at(i);
    }//Loop over each event


  cout << "Finished processing file #" << filenumber << endl;
  t1 = (double(clock())-t1);
  cout <<"Total number of steps processed: " << entries << endl <<"Of these, " << totalstepscleaned << " were cleaned by the implemented cuts." << endl;
  cout <<"Seconds taken for processing of file #" << filenumber << ": " << t1/1000000 << endl << endl << endl;

  output.cd();
  gTree->Write();

  //End of file cleanup
  output.Close();
  input.Close();
  delete G4eventsize;
 

}//EOF


/*Keyword [intro]
  //The previous CoBaLEP builder was painstakingly written by myself
  //when I wasn't as experienced with C++ programming as I am now.
  //It's finally time for a replacement to fix many issues which have
  //been bothering me for quite some time.

  //The problem is like this: we have a bunch of TTree branches.
  //The entries in these branches contain Geant4 events, which
  //we'll refer to as G4events from now on. Each Geant4 event is
  //one high-energy muon being simulated.
  //Each secondary particle created has a track, and each track is
  //propagated in a number of linear trajectories known as steps.

  //Four  levels of granularity:
  //File (many events per file)
  //Event (many tracks per event)
  //Track (many steps per track)
  //Step (no smaller unit in Geant4)

  //Each entry in a TTree is one step.
  //The task at hand is to build these steps/entries into 'hits', as defined by
  //the Majorana Demonstrator collaboration. A MJD hit is the term for all
  //energy depositions that occur in ONE detector, close together in time.
  //For our purposes, 'close together' is defined by the variable timewindow
  //and is on the order of a few microseconds. The hit is continuously built.

  //Imagine that entry A is at t=0, entry B is at t=10 and entry C is at t=28.
  //Building A and B together will redefine the timewindow to
  //timewindow+(A or B, whichever is higher). So, if we pick a timewindow
  //of 20, building A and B together will increase the timewindow to
  //20+10=30. Then, C is eligible to be included in the hit.
  //Including C will extend the timewindow to 20+28=48, and so on.
  //This is continuous building.

  //As long as we're talking about time... Some hits will occur at times
  //greater than the age of the earth. Clearly these hits will not occur
  //in any great quantities during our experiment's life cycle, so we
  //define an upper limit to the builder called timeupperlimit.
  //All hits occuring in an event at times greater than timeupperlimit
  //get discarded. Get out of here ya damn fossils!
  //I'll include an option for a minimum time too, in case we want to
  //skip processing the prompt signal, making things MUCH faster.

  //We can also define a lower limit on energy depositions.
  //Some steps leave a few eV in a detector, which is not something
  //our experiment can record. Towards this end, two cutoffs are
  //defined: a step cutoff and a hit cutoff.
  //The step cutoff, on the order of eV, simply doesn't process
  //entries with extremely low energy *at all*. The hit cutoff, on the order
  //of our detectors' expected thresholds, will neglect to record a hit if
  //its total energy is below this threshold.

  //The input data is sorted by G4eventnumber, but it is NOT sorted by
  //track number or time. It is by definition sorted by step number, but
  //this doesn't help us in any way from a processing perspective.
  //So, we can trust the program to read each event in order, and when
  //a new event number is found, we can be confident that no steps later
  //in the file are from the old event.
  //As for the steps in each event, we have to organize them ourselves.
  //The obvious solution is to sort them in time. That way, we can read
  //them one by one, and once a time is found that's at least timewindow
  //microseconds away, we can be confident that no later steps belong
  //in the hit we're building. An alternative approach is to loop over the
  //entries once to find the lowest time, then loop and add all events
  //within timewindow, then adjust timewindow, and then loop again.
  //The alternative solution is clumsy to implement, but might be faster
  //for events with extremely large numbers of steps. I'll think on it.

  //To add additional complexity, each hit is for only ONE detector.
  //So then one can sort an event by detectornumber, then sort
  //each portion with the same detectornumber by time.
  //What a pain!
  //The other solution is to keep an index of which steps have been
  //built already, and once the index is 'full' of built hits, consider
  //the event fully processed and move on.

  //A final problem. I can't actually sort the TTree events, per se.
  //One must not touch the input file! Read only! READ! ONLY!
  //So then, I have to map each entry and its time and detector
  //onto a matrix, and sort that matrix. Then, I can read the steps
  //from the tree in whichever order is necessary.

  //Regardless of implementation method chosen, memory leaks
  //have been a problem in the past. I'm cracking down on them.

  //Well that's about it. We have the sorting and the indexing options.
  //Each with their own challenges. We have all the requirements
  //and features. I'll keep explaining as we go along. Let's go!
*/

/* Keyword [eventsizing]
Currently, this vector uses the push_back function for each new event added to its list.
Using push_back like this is great, because we don't have to know how many events
there are to begin with. But push_back is slow, and might be called thousands of times.
This is because it has to allocate a little chunk of memory every single time it's called.
Not to mention the memory check it does on the object to see if it already has room.
We could allocate the space ahead of time and use array-style filling to speed it up.
But we don't know how much capacity to assign! Chicken and egg, am I right?

   fTree->GetEntry(entries-1);
  G4eventsize->resize(eventnumber);

This is a cute way to get the maximum possible size for this vector.
But (at least for the way I do my simulations) the vast majority of G4events
don't have entries. So this might allocate 100,000 spaces but only need 8,000.
The 8000 will fill fine, but leave 92000 zero values in the array.
I could then write a loop to check how many zero values there are, and snip
that much space off of the vector, but that would be ugly and memory leaky.
So basically, I would trade clunkiness on the backend for clunkiness in the code.
I'll circle back to this if the new builder isn't as fast as I would like.
*/

/*Keyword [howsort]

This is where things get difficult.

I have a vector of an array of three elements

[A B C]
[A B C]
. . .
[A B C]

A is an index to be used with GetEntry(), B is detectornumber, and C is time.
I want to sort by B, and then within each same B block, sort by C.
I think I'll want two loops after all.
for(i, eventsize whatever)
if(passcuts) pushback (lazybones)

sort(arraything[1].begin(),arraything[1].end())

OK cool, first part was easy enough. For the next part I'm going to need another sizer array? I guess so : ^/
Also there's no built-in way to sort by a certain column in a vector of vectors, so I
had to implement my own sorting rules. Loops in loops in loops in loops

*/

/*Keyword [sizingsorting]
      
Now that we have our G4event sorted by detectornumber, we need to sort each set of entries that
have the same detectornumber by time (the third column in eventsorter). Once we do this, we will
have a clean, straight shot to a build process.
We have the same problem we started with - we don't know how many steps share each detectornumber.
We can just use the same sizing algorithm that we implemented to make G4eventsize.
Normally I'd make this algorithm a function and call it in the main() program, but since the first call
requires access to the TTree and its associated variables, you can imagine how tedious this would be.

*/
