//#include <iostream>
#include <algorithm>
//#include <iterator>
//#include <math.h>
//#include <stdlib.h>
//#include <stdio.h>

vector<string> loadfilelist(void)
{

  //To generate an updated version of this list go to the valid folder and use
  //find /home/xu_lab/shared/simfiles/valid -name "*bund*" > valideventfilelist.txt

 fstream validfiles("valideventfilelist.txt",ios_base::in);
//input text file

 vector<string> validfilearray;
 string fileplaceholder = "";

 while(!validfiles.eof())
   {
     validfiles >> fileplaceholder;
     validfilearray.push_back(fileplaceholder.c_str());
   }

 return validfilearray;

}

void selector(int cutchoice, int comparatorchoice, double numbertocompare)
{//Does the actual work of the program. The main function is just reserved for fancy I/O

  vector<string> validfilearray;
   validfilearray = loadfilelist();
   TChain *chain = new TChain("sumtree");

   for(int i = 0;i<validfilearray.size();i++)
     {   
       chain->Add(Form("%s",validfilearray[i].c_str()));
       cout << validfilearray[i] << endl;
     }
   
   cout << "Entries: " <<chain->GetEntries() << endl;

  //Open input file, initialize
  TFile *input;// = new TFile("chain4189514.root","read");
  TTree *sumtree;// = (TTree*)input->Get("sumtree");
  int entries = chain->GetEntries();
  int subentries = 0;

  //some of chain's branches are vectors
  //So even though chain may have, say, 50 entries,
  //there could be 200 or more vector elements stored.
  //These are what I'm calling 'subentries'
  //These are also known as "event level information" and "hit level information" in other places
  
  chain->GetEntry(0);

  int keepchoice = 0;    //Used for multiple cuts
  bool keepgoing = true; //Used for multiple cuts

  //iterators
  int i = 0;
  int j = 0;
  int k = 0;
  
  //Prepare tree variables
  double starttime = 0;
  double endtime = 0;
  vector<double>* energysum = 0;
  double eventenergysum = 0;
  vector<int>* detector = 0;
  int G4event = 0;
  int multiplicity = 0;
  vector<int>* hitnumber = 0;
  int filenumber;

  chain->SetBranchAddress("starttime",&starttime);
  chain->SetBranchAddress("endtime",&endtime);
  chain->SetBranchAddress("energysum",&energysum); //hit energy sum (hit level information)
  //Note the conspicuous lack of an eventenergysum branch
  chain->SetBranchAddress("detector",&detector);   //hit level information
  chain->SetBranchAddress("G4event",&G4event);
  chain->SetBranchAddress("multiplicity",&multiplicity);
  //Next two not used for cuts - used for tracing back
  chain->SetBranchAddress("hitnumber",&hitnumber);
  chain->SetBranchAddress("filenumber",&filenumber);

  
  //Determine number of subentries, and also
  //Determine how many subentries 'belong' to each entry
  //i.e. the number of vector elements in each tree entry
  //i.e. the number of hits in each event i.e. the multiplicity

  vector<int> subentryplaceholder;
  //The idea is that each value in this vector is the cumulative total of hits
  //Example: suppose event 1 has 3 hits, event 2 has 8 hits, and event 3 has 1 hit
  //The vector will have 3 values, 0, 3, and 11
  //It doesn't matter at all how many hits the last event has,
  //since at that point the algorithm just goes until there's no more hits
  subentryplaceholder.clear();
  subentryplaceholder.resize(entries, 0);
  int subentryfiller = 0; //just used to fill subentryplaceholder
  
  for(i=0;i<entries;i++)
    {
      subentryplaceholder[i] = subentryfiller;
      chain->GetEntry(i);
      subentryfiller +=energysum->size(); 
      
    }//fill subentryplaceholder
  subentries = subentryfiller;


  cout << endl << "Number of entries in these files: " << entries << endl <<
    "Number of subentries in these files: " << subentries << endl << endl;

  
  
  //Populate boolean array to determine which entries pass the cut(s)
  std::vector<int> passed;
  passed.clear();
  passed.resize(entries, 1);
  std::vector<int> subpassed;
  subpassed.clear();
  subpassed.resize(subentries, 1);
  
  //true until proven false
  //this is also to facilitate multiple cuts  
  while(keepgoing)
    {//To compare multiple cuts

      i = 0;
      j = 0;
      k = 0;
      
      switch(cutchoice)
	{
	  
	case 1: //start time
	  
	  switch(comparatorchoice)
	    {
	    case 1://looking for less than
	      for(i=0;i<entries;i++)
		{
		  chain->GetEntry(i);
		  if(starttime>=numbertocompare)
		    {
		      passed[i] = 0;

		      for(j=0;j<energysum->size();j++)
			{
			  subpassed[(subentryplaceholder[i]+j)]=0;
			}
		    }
		}//case 1-1
	      break;
	      
	    case 2://looking for greater than
	      for(i=0;i<entries;i++)
		{
		  chain->GetEntry(i);
		  if(starttime<=numbertocompare)
		    {
		      passed[i] = 0;
		      
		      for(j=0;j<energysum->size();j++)
			{
			  subpassed[(subentryplaceholder[i]+j)]=0;
			}
		    }
		}//case 1-2
	      break;

	    case 3://looking for equal to
	      for(i=0;i<entries;i++)
		{
		  chain->GetEntry(i);
		  if(starttime!=numbertocompare)
		    {
		      passed[i] = 0;
		      for(j=0;j<energysum->size();j++)
			{
			  subpassed[(subentryplaceholder[i]+j)]=0;
			}
		    }
		}//case 1-3
	      break;
	      
	    }	  
	  break;//cutchoice 1

	  
	case 2: //end time
	  
	  switch(comparatorchoice)
	    {
	    case 1://looking for less than
	      for(i=0;i<entries;i++)
		{
		  chain->GetEntry(i);
		  if(endtime>=numbertocompare)
		    {
		      passed[i] = 0;
		      for(j=0;j<energysum->size();j++)
			{
			  subpassed[(subentryplaceholder[i]+j)]=0;
			}
		    }
		}//case 2-1
	      break;
	      
	    case 2://looking for greater than
	      for(i=0;i<entries;i++)
		{
		  chain->GetEntry(i);
		  if(endtime<=numbertocompare)
		    {
		      passed[i] = 0;
		      for(j=0;j<energysum->size();j++)
			{
			  subpassed[(subentryplaceholder[i]+j)]=0;
			}
		    }
		}//case 2-2
	      break;

	    case 3://looking for equal to
	      for(i=0;i<entries;i++)
		{
		  chain->GetEntry(i);
		  if(endtime!=numbertocompare)
		    {
		      passed[i] = 0;
		      for(j=0;j<energysum->size();j++)
			{
			  subpassed[(subentryplaceholder[i]+j)]=0;
			}
		    }
		}//case 2-3
	      break;
	      
	    }	  	  
	  break;//cutchoice 2

	  
	  /*
	    The fact that some of the input tree's branches are vectors, and some
	    are not, complicates things. The vector branches effectively have more
	    entries than the other branches. As mentioned elsewhere, the other branches
	    represent MJevent level information, whereas the vector branches
	    contain hit level information. An event-level cut can be applied to
	    hit-level information, but not the other way around. For example, it
	    makes no sense to ask for the detector number of a multi-site event -
	    this is limited to hit level processing.

	    For that reason, all cuts are performed at the hit level, with event
	    level cuts simply being applied equally to all the hits in an
	    event. The output will all be at the hit level too, of course, as will
	    the second phase of traceback.
	  */

	case 3: //hit energy
	  
	  switch(comparatorchoice)
	    {

	    case 1://looking for less than
	      for(i=0;i<entries;i++)
		{
		  chain->GetEntry(i);
		  
		  //Comb through vectors for hit energies
		  for(j=0;j<energysum->size();j++)
		    {
		      if(energysum->at(j)>=numbertocompare)
			{
			  subpassed[(subentryplaceholder[i]+j)]=0;
			}		      
		    }		  
		}//case 3-1
	      break;

	    case 2://looking for greater than
	      for(i=0;i<entries;i++)
		{
		  chain->GetEntry(i);
		  
		  //Comb through vectors for hit energies
		  for(j=0;j<energysum->size();j++)
		    {
		      if(energysum->at(j)<=numbertocompare)
			{
	
			  subpassed[(subentryplaceholder[i]+j)]=0;
			}
		    }		  
		}//case 3-2
	      break;

	    case 3://looking for equal to (how does that work with a double? Not well...)
	      for(i=0;i<entries;i++)
		{
		  chain->GetEntry(i);
		  
		  //Comb through vectors for hit energies
		  for(j=0;j<energysum->size();j++)
		    {
		      if(energysum->at(j)!=numbertocompare)
			{
			  subpassed[(subentryplaceholder[i]+j)]=0;
			}		      
		    }		  
		}//case 3-3
	      break;


	    }//cutchoice 3
	  break;
	      
	  
	case 4: //event energy
	  
	  eventenergysum = 0;
	  
	  switch(comparatorchoice)
	    {
	    case 1://looking for less than
	      for(i=0;i<entries;i++)
		{
		  chain->GetEntry(i);
		  
		  //Sum hit energies into event energy
		  eventenergysum=0;
		  for(j=0;j<energysum->size();j++)
		    {
		      eventenergysum+=energysum->at(j);
		    }
		  
		  if(eventenergysum>=numbertocompare)		  
		    {
		      passed[i] = 0;
		      for(j=0;j<energysum->size();j++)
			{
			  subpassed[(subentryplaceholder[i]+j)]=0;
			}
		    }
		}//case 4-1
	      break;

	    case 2://looking for greater than
	      for(i=0;i<entries;i++)
		{
		  chain->GetEntry(i);
		  
		  //Sum hit energies into event energy
		  eventenergysum=0;
		  for(j=0;j<energysum->size();j++)
		    {
		      eventenergysum+=energysum->at(j);
		    }
		  
		  if(eventenergysum<=numbertocompare)		  
		    {
		      passed[i] = 0;
		      for(j=0;j<energysum->size();j++)
			{
			  subpassed[(subentryplaceholder[i]+j)]=0;
			}
		    }
		}//case 4-2
	      break;

	    case 3://looking for equal to
	      for(i=0;i<entries;i++)
		{
		  chain->GetEntry(i);
		  
		  //Sum hit energies into event energy
		  eventenergysum=0;
		  for(j=0;j<energysum->size();j++)
		    {
		      eventenergysum+=energysum->at(j);
		    }
		  
		  if(eventenergysum!=numbertocompare)		  
		    {
		      passed[i] = 0;
		      for(j=0;j<energysum->size();j++)
			{
			  subpassed[(subentryplaceholder[i]+j)]=0;
			}
		    }
		}//case 4-3
	      break;

	    }	  
	  break;//cutchoice 4


	case 5: //detector number
	  
	  switch(comparatorchoice)
	    {

	    case 1://looking for less than
	      for(i=0;i<entries;i++)
		{
		  chain->GetEntry(i);
		  
		  //Comb through vectors for detector numbers
		  for(j=0;j<detector->size();j++)
		    {
		      if(detector->at(j)>=numbertocompare)
			{
			  subpassed[(subentryplaceholder[i]+j)]=0;
			}		      
		    }		  
		}//case 5-1
	      break;

	    case 2://looking for greater than
	      for(i=0;i<entries;i++)
		{
		  chain->GetEntry(i);
		  
		  //Comb through vectors for detector numbers
		  for(j=0;j<detector->size();j++)
		    {
		      if(detector->at(j)<=numbertocompare)
			{
	
			  subpassed[(subentryplaceholder[i]+j)]=0;
			}
		    }		  
		}//case 5-2
	      break;

	    case 3://looking for equal to
	      for(i=0;i<entries;i++)
		{
		  chain->GetEntry(i);
		  
		  //Comb through vectors for detector numbers
		  for(j=0;j<detector->size();j++)
		    {
		      if(detector->at(j)!=numbertocompare)
			{
			  subpassed[(subentryplaceholder[i]+j)]=0;
			}		      
		    }		  
		}//case 5-3
	      break;

	    }//cutchoice 5
	  break;


	case 6: //G4event#
	  
	  switch(comparatorchoice)
	    {
	    case 1://looking for less than
	      for(i=0;i<entries;i++)
		{
		  chain->GetEntry(i);
		  if(G4event>=numbertocompare)
		    {
		      passed[i] = 0;
		      for(j=0;j<energysum->size();j++)
			{
			  subpassed[(subentryplaceholder[i]+j)]=0;
			}
		    }
		}//case 6-1
	      break;
	      
	    case 2://looking for greater than
	      for(i=0;i<entries;i++)
		{
		  chain->GetEntry(i);
		  if(G4event<=numbertocompare)
		    {
		      passed[i] = 0;
		      for(j=0;j<energysum->size();j++)
			{
			  subpassed[(subentryplaceholder[i]+j)]=0;
			}
		    }
		}//case 6-2
	      break;

	    case 3://looking for equal to
	      for(i=0;i<entries;i++)
		{
		  chain->GetEntry(i);
		  if(G4event!=numbertocompare)
		    {
		      passed[i] = 0;
		      for(j=0;j<energysum->size();j++)
			{
			  subpassed[(subentryplaceholder[i]+j)]=0;
			}
		    }
		}//case 6-3
	      break;
	      
	    }//comparatorchoice 6
	  	  
	  break;//cutchoice 6


	case 7: //multiplicity
	  
	  switch(comparatorchoice)
	    {
	    case 1://looking for less than
	      for(i=0;i<entries;i++)
		{
		  chain->GetEntry(i);
		  if(multiplicity>=numbertocompare)
		    {
		      passed[i] = 0;
		      for(j=0;j<energysum->size();j++)
			{
			  subpassed[(subentryplaceholder[i]+j)]=0;
			}
		    }
		}//case 7-1
	      break;
	      
	    case 2://looking for greater than
	      for(i=0;i<entries;i++)
		{
		  chain->GetEntry(i);
		  if(multiplicity<=numbertocompare)
		    {
		      passed[i] = 0;
		      for(j=0;j<energysum->size();j++)
			{
			  subpassed[(subentryplaceholder[i]+j)]=0;
			}
		    }
		}//case 7-2
	      break;

	    case 3://looking for equal to
	      for(i=0;i<entries;i++)
		{
		  chain->GetEntry(i);
		  if(multiplicity!=numbertocompare)
		    {
		      passed[i] = 0;
		      for(j=0;j<energysum->size();j++)
			{
			  subpassed[(subentryplaceholder[i]+j)]=0;
			}
		    }
		}//case 7-3
	      break;
	      
	    }	  	  
	  break;//cutchoice 7
	  
	  
	}//cutchoice overall

      cout << endl << "Type '1' if you would like to make another cut. Otherwise, type anything else." << endl;
      keepchoice = 0;

      cin >>keepchoice;

      if(keepchoice!=1)
	break;

      else
	{
	  cutchoice = 0;
	  comparatorchoice = 0;
	  cout << "Select which variable you would like to cut on." << endl << endl <<
	    "1. start time" << endl <<
	    "2. end time" << endl <<
	    "3. hit energy sum" << endl <<
	    "4. event energy sum" << endl <<
	    "5. detector number" << endl <<
	    "6. G4 event number" << endl <<
	    "7. multiplicity" << endl << endl <<
	    "Enter choice numerically: ";

	  cin >> cutchoice;

	  cout << endl << "Select whether to compare less than, greater than, or equal to" << endl << endl <<
	    "1. Less than" << endl <<
	    "2. Greater than" << endl <<
	    "3. Equal to" << endl << endl <<
	    "Enter choice numerically: ";

	  comparatorchoice = 0;
	  cin >> comparatorchoice;

	  if(cutchoice<=0 || cutchoice >=8 ||
	     comparatorchoice<=0 || comparatorchoice>=4)
	    {
	      cout << "Error: invalid choice selected for either variable to cut or for comparator. Terminating program." << endl;
	      return;
	    }

	  cout << endl << "Enter numerical value to compare to: ";
	  numbertocompare = 0;
	  cin >> numbertocompare;

	}//else
    
    }//while keepgoing

  //Final tally
  //It's possible to clear all subentries without necessarily clearing an entry
  //The following loop cycles through all entries real quick and, if an entry's
  //subentries have all been cleared, will clear the entry as well.
  bool totallyempty = true;
  ofstream output("selectdata.txt", ios::out | ios::trunc);
  output.precision(200);
 
  for(i=0;i<entries;i++)
    {
      chain->GetEntry(i);
      totallyempty = true;

      for(j = subentryplaceholder[i];j<(subentryplaceholder[i]+energysum->size());j++)
	{

	  if(subpassed[j])
	    {//If any one of the hits is non-empty, the event is treated as non-empty as well
	      totallyempty = false;
	      //Output the following parameters, in order: filenumber G4eventnumber hitnumber
	      //This is enough information to unambiguously identify any hit in any hit file.
	      output << filenumber << " " << G4event << " " << hitnumber->at(j-subentryplaceholder[i]) << endl;
	      //output << G4event << " " << energysum->at(j-subentryplaceholder[i]) << " " << detector->at(j-subentryplaceholder[i]) << endl;
	    }//This is also where the writing to file is done (still being tested)
	}
      if(totallyempty && passed[i])
	{
	  cout << endl <<"Entry " << i << " had all sub-entries miss the cut, so it has been flagged as cut as well." << endl;
	  passed[i] = 0;
	}
    }

  cout << endl << endl << "Out of a possible " << subentries
       << " hits comprising " << entries << " events, "
       << std::accumulate(subpassed.begin(),subpassed.end(),0)
       << " hits from "
       << std::accumulate(passed.begin(),passed.end(),0)
       << " different events passed the cut(s)." << endl << endl;
  output.close();
  return;
  
}


void traceback(void)
{
  cout << "Select which variable you would like to cut on." << endl << endl <<
    "1. start time" << endl <<
    "2. end time" << endl <<
    "3. hit energy sum" << endl <<
    "4. event energy sum" << endl <<
    "5. detector number" << endl <<
    "6. G4 event number" << endl <<
    "7. multiplicity" << endl << endl <<
    "Enter choice numerically: ";

  int cutchoice;
  cin >> cutchoice;

  cout << endl << "Select whether to compare less than, greater than, or equal to" << endl << endl <<
    "1. Less than" << endl <<
    "2. Greater than" << endl <<
    "3. Equal to" << endl << endl <<
    "Enter choice numerically: ";

  int comparatorchoice = 0;
  cin >> comparatorchoice;

  // cout << cutchoice << "  " << comparatorchoice << endl;

  if(cutchoice<=0 || cutchoice >=8 ||
     comparatorchoice<=0 || comparatorchoice>=4)
    {
      cout << "Error: invalid choice selected for either variable to cut or for comparator. Terminating program." << endl;
      return;
    }

  cout << endl << "Enter numerical value to compare to: ";
  double numbertocompare = 0;

  cin >> numbertocompare;

  string niceoutputone;
  string niceoutputtwo;

  switch(cutchoice)
    {
    case 1:
      niceoutputone = "start times";
      break;
    case 2:
      niceoutputone = "end times";
      break;
    case 3:
      niceoutputone = "hit energy sums";
      break;
    case 4:
      niceoutputone = "event energy sums";
      break;
    case 5:
      niceoutputone = "detector numbers";
      break;
    case 6:
      niceoutputone = "G4 event numbers";
      break;
    case 7:
      niceoutputone = "multiplicities";
      break;      
    }

  switch(comparatorchoice)
    {
    case 1:
      niceoutputtwo = "less than";
      break;
    case 2:
      niceoutputtwo = "greater than";
      break;
    case 3:
      niceoutputtwo = "equal to";
      break;
    }

  cout << endl
       <<  "Comparing all entries in every available chain to determine if their "
       << niceoutputone << " are " << niceoutputtwo
       << " " << numbertocompare << endl; 


  selector(cutchoice, comparatorchoice, numbertocompare);
  
  

  return;
}
