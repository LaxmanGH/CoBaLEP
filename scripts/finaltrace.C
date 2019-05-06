void finaltrace(void)
{
  gROOT->ProcessLine("#include <vector>");
  using std::vector;
  //The final step in a 3-part program
  //Part 1 (traceback.C) outputs hit level information
  //from event-built files, after performing some cuts
  //Part 2 (trace2.C) uses part 1's output to find the
  //hit-built files and trees associated with the
  //initial cuts in part 1 and outputs their step-level
  //information. Finally, part 3 (this program) will use
  //that step-level information to retrieve other step-
  //level information from the original sim file.

  //Variables
  int filenumber = 0;
  double eventnumber = 0;
  int stepnumber = 0;
  int compareeventnumber = 0;
  int comparestepnumber = 0;
  int comparefilenumber = 0;
  vector<int> filenumbervector;
  vector<double> eventnumbervector;
  vector<int> stepnumbervector;
  vector<int> counted;
  //~vector<int> *nuclearPID = 0;
  //~vector<int> *nuclearPIDfinder = 0;
  //~vector<vector<int>> PIDcopyvectors;
  fstream validsteps("simdata.txt",ios_base::in);//input text file  
  validsteps >> filenumber >> eventnumber >> stepnumber;//initialiation read  
  TFile *output = new TFile("cutsoutput.root","recreate");
  TTree* outputtree;
  TFile *input = new TFile("allout.root","read");
  TTree *currenttree;
  int size = 0;

  while(!validsteps.eof())
    {
      //I'm gonna read the pertinent information
      // into a vector to make less calls to the
      //disk. This should keep runtime at a minimum.

      filenumbervector.push_back(filenumber);
      eventnumbervector.push_back(eventnumber);
      stepnumbervector.push_back(stepnumber);

      cout << filenumber << "  " << eventnumber << "  " << stepnumber << endl;
      validsteps >> filenumber >> eventnumber >> stepnumber;
    }//while validsteps

  counted.resize(filenumbervector.size(),0);
  currenttree = (TTree*)input->Get("fTree");
  currenttree->SetBranchAddress("eventnumber",&compareeventnumber);
  currenttree->SetBranchAddress("stepnumber",&comparestepnumber);
  currenttree->SetBranchAddress("randomseed",&comparefilenumber);
  //currenttree->SetBranchStatus("nuclear*",0);
  //I'm going to do this is the dumbest possible fashion in the hopes that ROOT can understand first grade level coding protocol. If it doesn't work so help me I am gutting all vectors from this piece of shit program.
  //Update: it didn't work. I'll save this final attempt at a solution here so it can mock me every time I open this script.
  /*currenttree->SetBranchAddress("nuclearPID",&nuclearPID);
  PIDcopyvectors.resize(currenttree->GetEntries());
  for(int i = 0;i<currenttree->GetEntries();i++)
    {//Copy all this file's vectors into a vector of vectors
     currenttree->GetEntry(i);
      size = nuclearPID->size();
      cout << size << endl;
      PIDcopyvectors[i].reserve(size);
      for(int j = 0;j<size;j++)
	PIDcopyvectors[i].push_back(nuclearPID->at(j));
    }
    
    currenttree->SetBranchStatus("nuclear*",0);*/


  outputtree = (TTree*)currenttree->CloneTree(0);
  //outputtree->Branch("nuclearPID",&nuclearPIDfinder);

  //Loop FIRST through the text file input entries
  //For each text file input, find the corresponding sim tree entry
  //Then, move onto the next text file input
  //Once the text file input corresponds to a different file,
  //change the sim tree accordingly.

  for(int i = 0;i<filenumbervector.size();i++)
    {
      filenumber = filenumbervector[i];
      eventnumber = eventnumbervector[i];
      stepnumber = stepnumbervector[i];

      /*if(i)
	{
	  if(filenumbervector[i]!=filenumbervector[i-1])
	    {
	      //Change of file, open new input, get new vectors
	      input = new TFile(Form("output%i.root",filenumber),"read");
	      currenttree = (TTree*)input->Get("fTree");
	      currenttree->SetBranchAddress("eventnumber",&compareeventnumber);
	      currenttree->SetBranchAddress("stepnumber",&comparestepnumber);
	      currenttree->SetBranchAddress("randomseed",&comparefilenumber);
	      //currenttree->SetBranchAddress("nuclearPID",&nuclearPID);
	      //Are these necessary?^^^
	      /*PIDcopyvectors.clear();
	      PIDcopyvectors.resize(currenttree->GetEntries());
	      for(int k = 0;k<currenttree->GetEntries();k++)
		{//Copy all this file's vectors into a vector of vectors
		  currenttree->GetEntry(k);
		  cout << "Size 2:" << size << endl;
		  size = nuclearPID->size();
		  PIDcopyvectors[k].reserve(size);
		  for(int m = 0;m<size;m++)
		    PIDcopyvectors[k].push_back(nuclearPID->at(m));
		}
	      
		currenttree->SetBranchStatus("nuclear*",0);STARASTERISK	      
	    }
	    }//Check to see if file changed (if(i))*/

      for(int j = 0;j<currenttree->GetEntries();j++)
	{
	  currenttree->GetEntry(j);

	  if(stepnumber==comparestepnumber&&filenumber==comparefilenumber&&!counted[i])
	    {//We have a match
	      /*cout << "Size 3: " << PIDcopyvectors[j].size() << endl;
	      nuclearPIDfinder->clear();
	      nuclearPIDfinder->reserve(PIDcopyvectors[j].size());
	      for(int k = 0;k<PIDcopyvectors[j].size();k++)
	      nuclearPIDfinder->push_back(PIDcopyvectors[j][k]);*/
	      outputtree->Fill();
	      counted[i]=1;
	      cout << "Filled " << i << endl;
	    }
	}//int j
    }//int i

  
  /*  for(int j = 0;j<currenttree->GetEntries();j++)
    {
      currenttree->GetEntry(j);

      for(int i = 0;i<filenumbervector.size();i++)
	{
	  filenumber = filenumbervector[i];
	  eventnumber = eventnumbervector[i];
	  stepnumber = stepnumbervector[i];
	  if(stepnumber==comparestepnumber&&filenumber==comparefilenumber&&!counted[i])
	    {
	      outputtree->Fill();
	      counted[i]=1;
	    }
	}//int i
    }//int j*/



      for(int i = 0;i<filenumbervector.size();i++)
	{
	  if(!counted[i])
	    {
	      cout << "Error: uncounted entry at " << i << endl;
	      cout <<"eventnumber: " << eventnumbervector[i] <<endl << "Stepnumber: " << stepnumbervector[i] << endl;
	    }
	}

  output->cd();
  outputtree->Write();

}//EOF
