/* max.cpp -- derived from rook.cpp                             */
/* Max: this program is designed for you to experiment with,    */
/*      to learn how to control a robot in C/C++.               */
/*--------------------------------------------------------------*/
#include "robots.h"
#include <stdio.h>
#include <iostream>
/*
current robot: heads for one of four points based on spawn proximity [(0,400)(0,600)(995,400)(995,600)],
	looking for enemies constantly

future work: 
	head for corner
	optimize looking radius once corner is reached (

*/
using std::cout;
using std::cin;
using std::endl;

int course;
int boundary;
int angle = 0;

/*
void
change ()
{
  if (course == 0)
    {
      boundary = 5;
      course = 180;
    }
  else
    {
      boundary = 995;
      course = 0;
    }

  drive (course, 30);
}
*/
/* look somewhere, and fire cannon repeatedly at in-range target */




void
look (int deg)
{
  static int d;
  int range;
  while ((range = scan (deg, 2)) > 0 && range <= 700)
    {
      //drive (course, 0);
      cannon (deg, range);
      if (damage () >= d + 20)
	{
	  d = damage ();
	//  change ();
	}
    }
}

void
fire ()
{
  if (angle >= 359)
{
	angle = 0;
}

else
{
	look(angle);
	angle = angle + 5;
}
}
int
main ()
{

  /* move to center of board */
  if (loc_y () < 500)
    {
      drive (270, 70);		/* start moving */
      while (500 - loc_y () > 20 && speed () > 0) {	/* stop near center */
	fire ();
      }
    }
  else
    {
      drive (90, 70);		/* start moving */
      while (loc_y () - 500 > 20 && speed () > 0) {	/* stop near center */
	fire ();
      }
    }
  drive (90, 0);



  /* initialize starting parameters */
  course = 0;
  boundary = 995;
  drive (course, 30);

  if (loc_x () < 500)
    {
      drive (180, 70);
      while (500 - loc_x () > 20 && speed () > 0) {
	fire ();
      }
     }
  else
    {
      drive (0, 70);
      while (loc_x () - 500 > 20 && speed () > 0) {
	fire ();
     }
    }
  /* main loop */
  while (1)
{
fire ();
}
      /* look all directions 
      look (0);
      look (90);
      look (180);
      look (270);
      */
      /* if near end of battlefield, change directions */
     /* if (course == 0)
	{
	  if (loc_x () > boundary || speed () == 0)
	    change ();
	}

      else
	{
	  if (loc_x () < boundary || speed () == 0)
	    change ();
	}
*/
    //  drive (course, 0);
}
