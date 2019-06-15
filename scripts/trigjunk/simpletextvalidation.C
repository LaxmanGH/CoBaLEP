void simpletextvalidation(void)
{

  //June 10 2019
  //This script was created as part of the
  //trigonometric efficiency study.
  //The whole point of the trig cut is to reduce
  //runtime by not simulating high energy muons
  //which totally miss the detector.

  //But how can we be sure the trig cut isn't cut-
  //ting out valid muons, or accepting invalid mu-
  //ons? The idea is to draw a physical target vol-
  //ume in GEANT4 that's the sie of the trig accept-
  //ance area, see whether the trig cut passes or
  //fails particles generated above it, and then
  //simulate the particles and see if the cut was
  //correct. This is being done with geantinos in-
  //stead of muons to reduce runtime, but the
  //results will apply equally to both particles.

  //I had the shower generator report a "1" if the
  //particle passed the trig cut, or a "2" if the
  //particle failed the trig cut. Similarly, I had
  //the main program report a "3" if the particle
  //actually passed through the target volume.

  //Ideally, then, every single "1" should be foll-
  //owed by a "3", and not a single "2" should be
  //followed by a "3". This program just scans the
  //results to make sure this is true.

  fstream input("numbertextfile.txt",ios_base::in);
  //input text file, obviously

  int firstnumber = 0;
  int secondnumber = 0;

  int validpasscounter = 0;
  int validfailcounter = 0;
  int invalidpasscounter = 0;
  int invalidfailcounter = 0;
  int wrong = 0;
  input >> firstnumber;
  input >> secondnumber;

  while(!input.eof())
    {
      //cout << firstnumber << " " << secondnumber << endl;

      if(firstnumber==2 && secondnumber!=3)
	{//need to shift one forward
	  validfailcounter++;
	  firstnumber = secondnumber;
	  if(!input.eof())
	    input >> secondnumber;
	}      
      else if (firstnumber == 2 && secondnumber == 3)
	{//bad, clear
	  invalidfailcounter++;
	  cout <<"Invalid fail at "<< invalidpasscounter+invalidfailcounter+wrong+validpasscounter+validfailcounter << endl;
	  if(!input.eof())
	    input >> firstnumber;
	  if(!input.eof())
	    input >> secondnumber;
	}
      else if(firstnumber == 1 && secondnumber==3)
	{
	validpasscounter++;//1 always followed by 3
	//Need to shift two forward
	  if(!input.eof())
	    input >> firstnumber;
	  if(!input.eof())
	    input >> secondnumber;
	}
      else if(firstnumber==1 && secondnumber!=3)
	{//bad
	  invalidpasscounter++;
	  cout <<"Invalid pass at "<< invalidpasscounter+invalidfailcounter+wrong+validpasscounter+validfailcounter << endl;
	  firstnumber = secondnumber;
	  if(!input.eof())
	    input >> secondnumber;
	}
      else
	{
	cout <<"WRONG" << endl;
	wrong++;
	}
    }//while(!input

  cout <<"Valid pass: " << validpasscounter << endl;
  cout <<"Valid fail: " << validfailcounter << endl;
  if(invalidpasscounter)  
cout <<"Invalid pass: " << invalidpasscounter << endl;
  if(invalidfailcounter)
  cout <<"Invalid fail: " << invalidfailcounter << endl;
  if(wrong)
  cout << "Wrong: " << wrong << endl;
  cout << "Total: " << invalidpasscounter+invalidfailcounter+wrong+validpasscounter+validfailcounter << endl << endl;

}//EOF
