void trace2(void)
{
  /*
    A program that takes the output of traceback.C and uses it to find
    the appropriate step level information from the correlated hitfile.

    With the implementation of hitnumber, this has become much easier.
    
    First things first. Let's make sure we can reliably read the
    information from the input file.    
  */

  //Declare variables

  //First number: File number
  //Second number:G4event#
  //Third number: Hit number

  int filecheck = 0;
  int filenumber = 0;
  int G4event = 0;
  int hitnumber = 0;

  double energy = 0;
  int skippedtrees = 1;
  double time = 0;
  int stepnumber = 0;

  //I/O
  fstream validhits("selectdata.txt",ios_base::in);//input text file
  validhits >> filenumber >> G4event >> hitnumber;//first read , used to initialize
  filecheck = filenumber;
  TFile *input = new TFile(Form("bundledoutput%i.root",filenumber),"read");
  TTree *currenttree;//tree used from input file

  ofstream output("simdata.txt", ios::out | ios::trunc);//output text file
  output.precision(200);

  //Iterate over the trees until we find the one we want
  TIter *nexto = new TIter(input->GetListOfKeys());
  TKey *key;
  const char* keyname;


  //And now we iterate
  while(!validhits.eof())
    {  

      if(filecheck!=filenumber)
	{
	  filecheck = filenumber;
	  input->Close();
	  input = new TFile(Form("bundledoutput%i.root",filenumber),"read");
	  nexto = new TIter(input->GetListOfKeys());
	  cout <<filenumber<<endl << endl;
	  skippedtrees = 1;
	}

      //Well first of all, we may have to pass by hundreds of useless trees
      //Combing through all of them will take too long, which is where we first use hitnumber
      //We can skip a number of trees equal to the hitnumber-1 (hitnumber does not start at 0)
      
      while(skippedtrees<hitnumber)//skip to the tree we want
	{
	  nexto->Next();
	  skippedtrees++;
	}
      cout <<key<<endl;      
      key=(TKey*)nexto->Next();
      skippedtrees++;
      currenttree = (TTree*)input->Get(key->GetName()); //fetch the Tree header in memory
      keyname = key->GetName();
      currenttree->SetBranchAddress("energy",&energy);
      currenttree->SetBranchAddress("time",&time);
      currenttree->SetBranchAddress("stepnumber",&stepnumber);

      //Now that we have the tree we want, we simply output certain key step information
      //in order to trace back one last time to the sim file.
      
      for(int gogo = 0;gogo<currenttree->GetEntries();gogo++)
	{
	  currenttree->GetEntry(gogo);
	  	  output << filenumber << " " << time << " " << stepnumber << endl;
	  //output << filenumber << " " << stepnumber << endl;
	}

      //Do processing BEFORE grabbing next values to make sure no EOF and
      //since I cheaped out by doing an initial grab outside the loop.
      
      //cout << filenumber << " " << G4event << " " << hitnumber << endl;


      validhits >> filenumber >> G4event >> hitnumber;
    }//while !eof
  cout <<"Trace2 has finished running." << endl;
}//EOF for realsies
