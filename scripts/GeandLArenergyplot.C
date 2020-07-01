void GeandLArenergyplot(void)
{
  int startnumber = 123456789;
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

      TFile *input = new TFile(Form("%s",acceptedname[iwan]),"UPDATE");
      TTree *fTree = (TTree*)input->Get("fTree");

      int entries = fTree->GetEntries();
      cout <<"Number of entries in current file: "<< entries << endl << endl;
      double energydeposition = 0;
      int eventnumber = 0;
      int storedeventnumber = -1;
      double totalargonenergy = 0;
      double totalgeenergy = 0;
      string *material = NULL;

      TH2D *depositionplot = new TH2D("depoplot","Muon depositions in argon and in germanium;Deposition in Ge (MeV);Deposition in liquid argon (MeV)",100000,0,100,1000,0,10000);

     fTree->SetBranchAddress("energydeposition",&energydeposition);
     fTree->SetBranchAddress("eventnumber",&eventnumber);
     fTree->SetBranchAddress("material",&material);

     fTree->GetEntry(0);
     storedeventnumber = eventnumber;
     totalargonenergy++;

     for (int i=i;i<entries;i++)
       {//For all entries
	 fTree->GetEntry(i);

	 if(eventnumber!=storedeventnumber)
	   {//Event is complete, begin energy summing of another event
	     depositionplot->Fill(totalgeenergy/1000,totalargonenergy/1000);
	     totalgeenergy = 0;
	     totalargonenergy = 0;
	     storedeventnumber = eventnumber;
	   }

          if(strstr(material->c_str(),"ArgonLiquid"))
	    totalargonenergy+=energydeposition;
          if(strstr(material->c_str(),"GermaniumEnriched"))
	    totalgeenergy+=energydeposition;

       }//For all entries
     depositionplot->SetDirectory(gROOT);
     //depositionplot->SetMarkerStyle(20);
     depositionplot->Draw();

    }//File by file processing

}//EOF
