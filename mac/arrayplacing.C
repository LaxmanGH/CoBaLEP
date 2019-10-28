void arrayplacing(void)
{//~May make not void eventually

  //An algorithm to place an array of evenly spaced
  //detectors into x number of strings.

  //4 arguments come to mind which are mandatory:
  //Vertical spacing: the uniform spacing between
  //detectors in the same string
  //Horizontal spacing (or: radius of ring)
  //the spacing between strings in the ring
  //Number of strings
  //Total number of detectors(or: number per string)
  //By these powers combined, we should be able to
  //arrange an arbitrary of detectors into an
  //arbitrary number of strings.

  //For now, I'll go with radius as an option
  //and with number of detectors as an option

  const int numberofdetectors = 102;
  const int numberofstrings = 17;
  const int inscribingradius = 500;//mm
  //No idea if that's a big enough radius...
  const int verticalspacing = 50;//mm
  //Detectors are roughly 90mm tall, 100mm wide

  //The easiest way to do this is to just pick
  //n evenly spaces points on the circle via
  //placing the first detector at horizontal
  //And placing the next at 180/n azimuthally

  double azimuth = 0;
  int zcounter = 0;
  int longeststring = 0;

  double x = 0;
  double y = 0;
  double z = 0;

  //The number of detectors in the longest string
  //needs to be determined in order to determine
  //the starting value of z. If the longest string
  //has an odd number of detectors, there will be
  //some row that has z=0. Also, the calculation
  //differs depending on if there's a modulus.

    longeststring = numberofdetectors/numberofstrings;
    if(numberofdetectors%numberofstrings)
      longeststring++;
    cout << longeststring << endl << endl;
  
    z = verticalspacing*(longeststring-1)/2;

  for (int i = 0; i < numberofdetectors; i++)
    {

      if(!(i%numberofstrings)&&(i))
	z = z-verticalspacing;

      azimuth = (2*TMath::Pi()*(i%numberofstrings))/numberofstrings;
    
      x = TMath::Cos(azimuth)*inscribingradius;
      y = TMath::Sin(azimuth)*inscribingradius;

      cout << i << endl << "x: " << x << endl<< "y: " << y << endl << "z: " << z << endl << endl;
    }


}
