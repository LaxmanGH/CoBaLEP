// Driver function to sort a 2D vector by its THIRD column
bool sortcoltwo( const vector<double>& v1, const vector<double>& v2 )
{ 
 return v1[1] < v2[1]; 
} 

//
//Don't forget to move the cat command from the old eventbuilder to the new submission script!
//Same with the validfilelist.txt or whatever
//

void eventbuilder2020(char* startfile)
{

//Search keyword [intro] for the explanation at the end of this script

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
  int totalhitscleaned = 0;

  //Soft variables that may need to be changed based on analysis needs
  const double hitenergyminimum = 10.;//Just a guess, but a good one
  const double timewindow = 20000.;
  //By the way, I make timewindow a double because it's added to time,
  //which is a double. Don't want the processor to get confused!


  //Open the file to be processed in the usual way
  TFile input(Form("%s",startfile),"READ");//I don't have to capitalize READ, but I want to
  TTree *fTree = (TTree*)input.Get("fTree");
  int entries = fTree->GetEntries();

  //Reading variables
  int hitnumber = 0;
  double time = 0;
  int detectornumber = 0;
  int filenumber = 0;
  int eventnumber = 0;
  int stepnumber = 0;
  double energydeposition = 0;
  double x = 0;
  double y = 0;
  double z = 0;
  //~double weight = 0;

  fTree->SetBranchAddress("hitnumber",&hitnumber);
  fTree->SetBranchAddress("time",&time);
  fTree->SetBranchAddress("detectornumber",&detectornumber);
  fTree->SetBranchAddress("randomseed",&filenumber);
  fTree->SetBranchAddress("eventnumber",&eventnumber);
  fTree->SetBranchAddress("stepnumber",&stepnumber);
  fTree->SetBranchAddress("energydeposition",&energydeposition);
  fTree->SetBranchAddress("x",&x);
  fTree->SetBranchAddress("y",&y);
  fTree->SetBranchAddress("z",&z);
  //~fTree->SetBranchAddress("weight",&weight);
  fTree->GetEntry(0);//Typical 'first fill' call


  //Writing variables
  TFile output(Form("ebuilt%i.root",filenumber),"RECREATE");
  vector<int> hitnumbervector;
  vector<int> detectornumbervector;
  vector<double> energydepositionvector;
  vector<double> R90writer;
  vector<int>* unintvector = new vector<int> ();//For deleting
  vector<double>* undoublevector = new vector<double> ();//For deleting
  double MJDenergy = 0;
  double starttime = 0;
  double endtime = 0;
  int multiplicity = 0;

  output.cd();
  TTree *sumtree = new TTree("sumtree","sumtree");
  //Search keyword [leaves] for more information here
  //Hit level parameters
  sumtree->Branch("hitnumber",&hitnumbervector);
  sumtree->Branch("detectornumber",&detectornumbervector);
  sumtree->Branch("hitenergy",&energydepositionvector);
  sumtree->Branch("R90",&R90writer);
  //Event level parameters
  sumtree->Branch("MJDeventenergy",&MJDenergy);
  sumtree->Branch("filenumber",&filenumber);
  sumtree->Branch("multiplicity",&multiplicity);
  //~sumtree->Branch("weight",&weight);//Eventually
  //Event level parameters that need to be processed like hits
  sumtree->Branch("starttime",&starttime);
  sumtree->Branch("endtime",&endtime);

  //No need for eventnumber, to be honest
  //8 branches, but 1 is filled automatically
  //We can trace back using nothing but filenumber and hitnumber
  input.cd();

  //After 110 lines of setup we can finally begin the processing, uh, process


  //Processing variables

  //Sizing variables
  vector<unsigned int> *G4eventsize = new vector<unsigned int>();//Stores number of steps in each event; resets every file
  //Search keyword [MJDevent?] for an important note on G4events vs MJDevents
  int eventcomparator = eventnumber;//Used when sizing each event
  int sizeofcurrentevent = 0;
  int eventincrementer = 0;//Cumulative total of G4eventsize
  //Gives us the correct place to start GetEntry for each event

  //Variables used for hit building
  int hitnumbercomparator = hitnumber;
  starttime = time;
  endtime = time;
  double energyaccumulator = 0;
  double timecomparator = 0;
  bool hasfilled = false;
  int hitcounter = 0;
  int previousdetectornumber = 0;

  //Variables used for event building
  vector<vector<double>> eventsorter;
  vector<vector<double>> *uneventsorter = new vector<vector<double>> ();

  //Variables used for R90
  //Search keyword [R90process] for more information
  vector<vector<double>> R90sorter;
  double Rvalue = 0;
  double R90 = 0;
  double R90energy = 0;
  int R90placeholder = 0;
  double centerx = 0;
  double centery = 0;
  double centerz = 0;

  cout << endl<< "Processing file #" << filenumber << " with " << entries << " total entries." << endl << endl;

  //G4event sizing
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
  //Well, at the end of the loop, there's no eventnumber to change to


  cout << "Number of G4events in this file: "<< G4eventsize->size() << endl << endl;

  for (int i = 0; i < G4eventsize->size(); i++)
    {//Loop over each G4event

      fTree->GetEntry(eventincrementer);//Beginning of current event

      cout << endl << endl <<"Processing event #" << eventnumber << " with "  << G4eventsize->at(i) << " entries." << endl;
      cout << eventincrementer << " total steps processed so far." << endl << endl;

      //Cleanup from previous G4event
      delete uneventsorter;
      uneventsorter = new vector<vector<double>> ();
      eventsorter.swap(*uneventsorter);
      delete uneventsorter;
      uneventsorter = new vector<vector<double>> ();
      R90sorter.swap(*uneventsorter);
      delete unintvector;
      unintvector = new vector<int> ();
      hitnumbervector.swap(*unintvector);
      delete unintvector;
      unintvector = new vector<int> ();
      detectornumbervector.swap(*unintvector);
      delete undoublevector;
      undoublevector = new vector<double> ();
      energydepositionvector.swap(*undoublevector);
      delete undoublevector;
      undoublevector = new vector<double> ();
      R90writer.swap(*undoublevector);

      starttime = time;
      endtime = time;
      energyaccumulator = 0;
      hitnumbercomparator=hitnumber;//Load comparands for internal loop 1
      multiplicity = 0;
      MJDenergy = 0;
      timecomparator = 0;
      hasfilled = false;
      hitcounter = 0;
      previousdetectornumber = detectornumber;
      Rvalue = 0;
      R90 = 0;
      R90energy = 0;
      R90placeholder = eventincrementer;
      centerx = 0;
      centery = 0;
      centerz = 0;


      if(G4eventsize->at(i)==1)
	{//Special case, only one entry in the hit
	  energyaccumulator = energydeposition;
	  R90 = 0;
	  if(energyaccumulator>hitenergyminimum)
	    eventsorter.push_back({(double)hitnumber,starttime,endtime,energyaccumulator,(double)detectornumber,R90});
	  else
	    totalhitscleaned++;
	  ////!cout <<"Singular hit number going in to the array: "<< hitnumber <<endl;
	  hitcounter++;
	}//Hit has one entry

      else
	{//Hit has multiple entries
	  for(int j = eventincrementer;j<(eventincrementer+G4eventsize->at(i));j++)
	    {//Internal loop 1

	      fTree->GetEntry(j);

	      if(hitnumbercomparator==hitnumber)//Step belongs to same hit
		{//First step always gets a free pass
		  ////!cout <<"Same"<< hitnumber <<endl;
		  hitcounter++;
		  energyaccumulator+=energydeposition;
		  centerx +=x*energydeposition;
		  centery +=y*energydeposition;
		  centerz +=z*energydeposition;
		  /*cout <<"This step's x value:     " << setprecision(10) << x <<endl;
		  cout <<"This step's energy:      " << energydeposition << endl;
		  cout <<"Stepnumber:              " << stepnumber << endl;
		  cout <<"Energy running total:    " << energyaccumulator << endl;
		  cout <<"Centerx running average: " << centerx/energyaccumulator << endl;
		  */  if(time<starttime)
		    starttime = time;
		  else if(time>endtime)
		    endtime = time;
		}//Same hit	      
	 
	      else
		{//New hit
		  //The for loop has already moved onto the next step, but we want the parameters from the previous step
		  ////!cout <<"Normal hit number going in to the array: "<< hitnumbercomparator <<endl;
		  centerx = centerx/energyaccumulator;
		  centery = centery/energyaccumulator;
		  centerz = centerz/energyaccumulator;
		  //cout << "Centerx end value: " << centerx << endl << endl << endl;
		  //cout <<"centerx final average:   " << centerx << endl << endl;


		  //R90 processing, to be done from placeholder up to (currentstep-1) with currentstep being j

		  for(int k = R90placeholder; k < j; k++)
		    {
		      fTree->GetEntry(k);
		      Rvalue = TMath::Sqrt(TMath::Power((centerx-x),2)+TMath::Power((centery-y),2)+TMath::Power((centerz-z),2));
		      R90sorter.push_back({energydeposition,Rvalue});
		    }
		  fTree->GetEntry(j);

		  if(R90sorter.size()>1)
		    sort(R90sorter.begin(),R90sorter.end(),sortcoltwo);//Sort by RValue

		  R90energy = 0;
		  R90placeholder = 0;
		  while(R90energy<(0.9*energyaccumulator))
		    {
		      R90energy+=R90sorter[R90placeholder][0];
		      R90placeholder++;
		    }
		  
		  R90 = R90sorter[R90placeholder-1][1];
		  if(R90<0.000000001)
		    R90=0;

		  if(energyaccumulator>hitenergyminimum)//Cleaning cuts go here
		    eventsorter.push_back({(double)hitnumbercomparator,starttime,endtime,energyaccumulator,(double)previousdetectornumber,R90});
		  else//!cout << "Low energy hit removed " << energyaccumulator << endl;
		    totalhitscleaned++;
		  
		  //Reset comparands and other parameters as needed

		  delete uneventsorter;
		  uneventsorter = new vector<vector<double>> ();
		  R90sorter.swap(*uneventsorter);
		  R90energy = 0;
		  Rvalue = 0;
		  R90 = 0;
		  R90placeholder = j;
		  centerx = x*energydeposition;
		  centery = y*energydeposition;
		  centerz = z*energydeposition;
		  hitnumbercomparator = hitnumber;
		  energyaccumulator = energydeposition;
		  starttime = time;
		  endtime = time;
		  previousdetectornumber = detectornumber;
		  /*cout <<"this step's x value:     " << setprecision(10) << x <<endl;
                  cout <<"this step's energy:      " << energydeposition << endl;
		  cout <<"stepnumber:              " << stepnumber << endl;
                  cout <<"energy running total:    " << energyaccumulator << endl;
                  cout <<"centerx running average: " << centerx/energyaccumulator << endl << endl;
		  *///No need for an index, since we aren't really reading from the TTree anymore
		}//New hit
	      
	    }//Internal loop 1
	  
      //Final fill - my looping philosophy usually requires one of these
	  centerx = centerx/energyaccumulator;
	  centery = centery/energyaccumulator;
	  centerz = centerz/energyaccumulator;
	  //cout <<"Final step's x value:    " << setprecision(10) << x <<endl;
	  //cout <<"centerx final average:   " << centerx << endl << endl;
	  
	  //R90 processing, to be done from placeholder up to (currentstep-1) with currentstep being j
	  
	  for(int k = R90placeholder; k < (eventincrementer+G4eventsize->at(i)); k++)
	    {
	      fTree->GetEntry(k);
	      Rvalue = TMath::Sqrt(TMath::Power((centerx-x),2)+TMath::Power((centery-y),2)+TMath::Power((centerz-z),2));
	      R90sorter.push_back({energydeposition,Rvalue});
	    }
	  fTree->GetEntry((eventincrementer+G4eventsize->at(i)));
	  
	  if(R90sorter.size()>1)
	    sort(R90sorter.begin(),R90sorter.end(),sortcoltwo);//Sort by RValue
	  
	  R90energy = 0;
	  R90placeholder = 0;
	  while(R90energy<(0.9*energyaccumulator))
	    {
	      R90energy+=R90sorter[R90placeholder][0];
	      R90placeholder++;
	    }

	  R90 = R90sorter[R90placeholder-1][1];
	  if(R90<0.000000001)
	    R90=0;
	  
	  
	  //If the world is a just place, no final cleanup is required from the hit building part 2

		  
	  if(energyaccumulator>hitenergyminimum&&G4eventsize->at(i)>1)//Cleaning cuts go here
	    {
	      eventsorter.push_back({(double)hitnumbercomparator,starttime,endtime,energyaccumulator,(double)previousdetectornumber,R90});
	      ////!cout <<"Last hit number going in to the array: " << hitnumbercomparator <<endl;
	      cout << endl << "Final R90: " << R90 << endl;
	      hitcounter++;
	    }
	  else if(energyaccumulator<hitenergyminimum)////!cout << "Low energy hit removed " << energyaccumulator << endl;
	    totalhitscleaned++;
	  
	}//When there's more than one entry

      
      cout << "Number of hits after cleaning: " << eventsorter.size() << endl;
      //!cout <<"Hit counter: " << hitcounter <<endl;
      if(!eventsorter.size())//Empty event
	{
	  cout <<"All hits removed by cleaning! Moving to next event..." << endl << endl;
	  eventincrementer+=G4eventsize->at(i);
	  continue;//Be wary of proper variable cleanup...
	}
      


      //The previous section prepares the hit-level parameters
      //This section prepares the event level parameters and writes to the output tree


      if(eventsorter.size()==1)//A multiplicity 1 event! We're already done : ^)
	{
	  multiplicity = 1;
	  hitnumbervector.push_back((int)eventsorter[0][0]);
	  starttime = eventsorter[0][1];
	  endtime = eventsorter[0][2];
	  energydepositionvector.push_back(eventsorter[0][3]);
	  detectornumbervector.push_back((int)eventsorter[0][4]);
	  R90writer.push_back(eventsorter[0][5]);
	  MJDenergy = eventsorter[0][3];
	  sumtree->Fill();
	  eventincrementer+=G4eventsize->at(i);
	  ////!cout << "Multiplicity 1 event" << endl << endl;
	  continue;
	}//Multiplicity 1 event

      //If eventsorter size isn't 0 or 1, we have to actually sort and build...
      if(eventsorter.size()>1)
	{
	  sort(eventsorter.begin(),eventsorter.end(),sortcoltwo);//Sort hits by starttime
	  
	  //Set up comparands but DO NOT do an initial fill
	  starttime = eventsorter[0][1];
	  timecomparator = eventsorter[0][2]+timewindow;
	  MJDenergy = 0;
	  multiplicity = 0;
	  //Internal loop 2: event building for multiplicity>1 MJDevents
	  for(int j = 0;j<eventsorter.size();j++)
	    {//The hits are time sorted so can be time built easily
	      //!cout << eventsorter[j][0] << endl;
	      	      
	      //If this hit's starttime is within the timewindow of the time comparand
	      if(eventsorter[j][1]<timecomparator)
		{//Notice how nicely the time building wraps itself up
		  MJDenergy+=eventsorter[j][3];
		  multiplicity++;
		  timecomparator = eventsorter[j][2]+timewindow;//Continuous building
		}//Remember, eventsorter[j][1] is starttime and [j][2] is endtime
	      else//Change in hit
		{//Gotta reset the leaves and the branches
		  endtime = timecomparator - timewindow;//Last hit to be included in event
		  sumtree->Fill();
		  ////!cout <<"Hitnumber: " << eventsorter[j][0] << endl;
		  //I think this is where I should call R90? Maybe??
		  starttime = eventsorter[j][1];//New event needs a new starttime
		  timecomparator = eventsorter[j][2]+timewindow;//Continuous building
		  MJDenergy = eventsorter[j][3];
		  multiplicity = 1;
		  //New vectors, carefully freeing memory
		  delete unintvector;
		  unintvector = new vector<int> ();
		  hitnumbervector.swap(*unintvector);
		  delete unintvector;
		  unintvector = new vector<int> ();
		  detectornumbervector.swap(*unintvector);
		  delete undoublevector;
		  undoublevector = new vector<double> ();
		  R90writer.swap(*undoublevector);
		  delete undoublevector;
		  undoublevector = new vector<double> ();
		  energydepositionvector.swap(*undoublevector);
		}
	      
	      //These happen no matter what
	      hitnumbervector.push_back((int)eventsorter[j][0]);
	      energydepositionvector.push_back(eventsorter[j][3]);
	      detectornumbervector.push_back((int)eventsorter[j][4]);
	      R90writer.push_back(eventsorter[j][5]);	      
	    }//Internal loop 2: event building
	  endtime = timecomparator - timewindow;//Last hit to be included in event
	  sumtree->Fill();
	  //To catch the last hit
	  //Again, hits were only filled in the previous loop upon an event change
	  //The last hit has nothing to compare with!
       	}//For events with more than one hit
      
      //Cleanup from last MJDevent
      starttime = time;
      endtime = time;
      multiplicity = 0;
      MJDenergy = 0;
      timecomparator = 0;
      delete unintvector;
      unintvector = new vector<int> ();
      hitnumbervector.swap(*unintvector);
      delete undoublevector;
      undoublevector = new vector<double> ();
      energydepositionvector.swap(*undoublevector);
      delete undoublevector;
      undoublevector = new vector<double> ();
      R90writer.swap(*undoublevector);
      delete unintvector;
      unintvector = new vector<int> ();
      detectornumbervector.swap(*unintvector);
      
      eventincrementer+=G4eventsize->at(i);
    }//Loop over each G4event

  cout << endl << "Number of low energy hits removed: " << totalhitscleaned << endl;

  delete G4eventsize;
  output.cd();
  sumtree->Write();
  output.Close();
  input.Close();
  
  cout << "Total processing time: " << (double(clock())-t1)/1000000 << endl << endl << endl;

}//EOF


  /* keyword [intro]
The new eventbuilder operates on similar principles to the 
new hitbuilder, but is decidedly less complicated.
Because of the two levels of granularity (see next note),
neither of which is step-level like the input file, we must
accumulate step information into hit information, and we
must accumulate hit information into MJDevent info.
I think the best way to do this is in order.
We'll build the step varibles into hit variables, then
we'll use these hit variables to event build, and then
finally we can slap the event variables onto the end.
  */

 /* keyword [leaves]
It behooves us to include two levels of information
granularity directly in our branches, which in ROOT means
including leaves in the form of vector entries.
Entries which are filled with other data types will represent
event-level information and entries filled with vectors
will store hit-level information.
One could take this same approach in the hitbuilder with
hit-level and step-level parameters, but it would only be
saving about 40% of the total space occupied by branches.
And besides, can you imagine the I/O nightmares?
hitbuilder is complicated enough as it is (for now...).

Event level information:
Multiplicity
start time and end time*
Total event energy (labelled MJDenergy in the code)
File number (technically file level information)

Hit level information:
Detector number (an MJD event contains one or more)
Hit number (da-doy)
Total hit energy

 */

 /* Keyword [MJDevent?]
There are two different types of events being discussed now:
G4events, and MJD events. Unfortunately, we can't escape
this confusing nomenclature without some effort.
We've already discussed what a G4event is back in
hitbuilder.C. An MJD event is a data structure containing
all time associated hits, just in the way a hit is a structure
containing all time associated steps in one detector.
One G4event can contain many MJD events; the reverse
is not true. This is an MJD event builder.
We're doing continuous time building just like in
hitbuilder.C, although the timewindows may end up
being different eventually.
 */

/* Keyword [R90process]
R90 is a hit-level parameter.
Imagine all the steps in one hit in 3D space,
e.g. all energy depositions in one detector.
You could determine the 'center of energy' for such a collec-
tion of energy depositions.
The R90 value is the radius of a sphere, centered on the
hit's center of energy, which spatially encompasses the
steps responsible for 90% of the hit's total energy.

By definition, a hit with just one energy deposition
will have a R90 of 0, since the center of energy is
located at the site of the only hit.

Rvalue is a step-level parameter.
It represents that individual step's distance from the
center of energy for the hit.
Every step with an Rvalue < R90 is inside the sphere.
Every step with an Rvalue > R90 is outside the sphere.

centerx,y,z should be pretty self-explanatory.
It's the location of the hit center of energy and is used
for calculating each step's Rvalue.
 */
