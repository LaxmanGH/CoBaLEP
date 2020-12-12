void trace2(void)
{
  /*
    A program that takes the output of traceback.C and uses it to find
    the appropriate step level information from the correlated hitfile.

    With the implementation of hitnumber, this has become much easier.
  */

  //Declare variables

  //First number from input: File number
  //Second number from input: Hit number

  double t1 = (double)clock();
  int filecheck = 0;
  int hitcheck = 0;
  int filenumber = 0;
  int hitnumber = 0;
  int stepnumber = 0;

  //I/O
  fstream validhits("selectdata.txt",ios_base::in);//input text file
  validhits >> filecheck >> hitcheck;//first read , used to initialize
  TFile *input = new TFile(Form("hbuilt%i.root",filecheck),"read");
  TTree *currenttree;//tree used from input file
  currenttree = (TTree*)input->Get("fTree"); //fetch the Tree header in memory
  currenttree->SetBranchAddress("stepnumber",&stepnumber);
  currenttree->SetBranchAddress("hitnumber", &hitnumber);
  currenttree->SetBranchAddress("randomseed",&filenumber);
  currenttree->GetEntry(0);
  cout << filenumber << endl;
  ofstream output("simdata.txt", ios::out | ios::trunc);//output text file
  cout << "Initial input and output successfully opened." << endl;
  //And now we iterate
  while(!validhits.eof())
    {  

      if(filecheck!=filenumber)
	{//Change of file; close current input and open new one
	  delete currenttree;
	  input->Close();
	  input = new TFile(Form("hbuilt%i.root",filecheck),"read");
	  currenttree = (TTree*)input->Get("fTree"); //fetch the Tree header in memory
	  currenttree->SetBranchAddress("stepnumber",&stepnumber);
	  currenttree->SetBranchAddress("hitnumber", &hitnumber);
	  currenttree->SetBranchAddress("randomseed",&filenumber);
	  currenttree->GetEntry(0);
	  cout <<filenumber << endl;
	}
      

      for(int gogo = 0;gogo<currenttree->GetEntries();gogo++)
	{
	  currenttree->GetEntry(gogo);
	  if(hitnumber==hitcheck)
	  output << filenumber << " " << stepnumber << endl;
	}

      //Do processing BEFORE grabbing next values to make sure no EOF and
      //since I cheaped out by doing an initial grab outside the loop.
      
      validhits >> filecheck >> hitcheck;
    }//while !eof

  cout <<"Trace2 has finished running." << endl;
  t1 = (double(clock()) - t1)/1000000;
  cout << "Time taken: " << t1 << endl << endl;
}//EOF for realsies
