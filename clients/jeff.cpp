/* jeff */
/* drive around in a clockwise direction (decreasing degrees) */
/* scan in a counter-clockwise direction (increasing degrees) */

#include "robots.h"
#include <stdlib.h>
// #include <iostream>
// using std::cin;
// using std::cout;
// using std::endl;

int iHeading = 0;
int iSpeed   = 0;

/* cruise moves leisurely around the perimeter of the field  */
void
cruise ()
{
   int x, y;
   // int i = 0;
   int iCurSpeed = speed();
   x = loc_x();
   y = loc_y();
               // We have to start slowing down before we turn, since
               // the robot can only turn when moving less than 50 units/sec.
   if ( 0 == iHeading ) {       // if we're going east
      if ( ( 800 < x ) ) {      //   if we're near the east edge
         if ( iCurSpeed < 50 ) { //     if we're going slow enough to turn
            iHeading = 270;     //       then turn
            iSpeed = 100;
         } else {
            iSpeed = 40;        //       else slow down so we can turn later
         }
      } else {
         iSpeed = 100;          //   else (not near east edge) head east fast
      }
   } else if ( 270 == iHeading ) {  // else if we're going south
     if ( ( y < 200 ) ) {       //   if we're near the south edge
        if ( iCurSpeed < 50 ) { //     if we're going slow enough to turn
           iHeading = 180;      //       then turn
           iSpeed = 100;
        } else {
           iSpeed = 40;         //       else slow down so we can turn later
        }
     } else {
        iSpeed = 100;           //   else (not near south edge) head south fast
     }
  } else if ( 180 == iHeading ) {  // else if we're going west
     if ( ( x < 200 ) ) {       //   if we're near the west edge
        if ( iCurSpeed < 50 ) { //     if we're going slow enough to turn
           iHeading = 90;       //       then turn
           iSpeed = 100;
        } else {
           iSpeed = 40;         //       else slow down so we can turn later
        }
     } else {
        iSpeed = 100;           //   else (not near west edge) head west fast
     }
  } else if (  90 == iHeading ) {  // else if we're going north
     if ( ( 800 < y ) ) {       //   if we're near the north edge
        if ( iCurSpeed < 50 ) { //     if we're going slow enough to turn
           iHeading = 0;        //       then turn
           iSpeed = 100;
        } else {
           iSpeed = 40;         //       else slow down so we can turn later
        }
     } else {
        iSpeed = 100;           //   else (not near north edge) head north fast
     }
  }

  if ( 50 <= iSpeed ) {
    iSpeed = 49;
  }

  drive( iHeading, iSpeed );
}

#define RES                  10
#define DAMAGE_THRESHOLD      1

int
main ()
{
   int angle, range;
   // int d;
   // int i;
   int bestTargetAngle, bestTargetRange;

   while (1)
   {
      cruise();
      bestTargetRange = 701;
      bestTargetAngle = 0;

      for ( angle=0; angle<360; angle+=(2*RES)-1 ) {
         range = scan (angle, RES);
         if ( range < 50 ) {
            continue;                 // too close, find another target
         } else if ( range < 300 ) {
            bestTargetRange = range;
            bestTargetAngle = angle;
            break;
         } else if ( range < 700 ) {
            if ( range < bestTargetRange ) {
               bestTargetRange = range;
               bestTargetAngle = angle;
            }
         }
      }

      if ( ( 50 < bestTargetRange ) && ( bestTargetRange < 700 ) )
      {
         if ( 500 < bestTargetRange ) {              // if it's far away, then
            for ( angle=bestTargetAngle-RES;         // pinpoint the angle
                  angle<bestTargetAngle+RES;
                  angle += 2 ) {
               range = scan( angle, 1 );
               if ( ( 50 < range ) && ( range < 700 ) ) {
                  break;
               }
            }
                  // adjust for our motion
                  //
         } else {
           range = bestTargetRange;
           angle = bestTargetAngle;
         }
         if ( ( 50 < range ) && ( range < 700 ) ) {
            while (cannon (angle, range) != 0) ;  /* fire all shots */
         }  // if there's something precise to shoot at
      }  // if there's something to shoot at
   } // while 1
}
