/*------------------------------------------------------------*/
/* jeff.cpp --                                                */
/* drive around in a clockwise direction (decreasing degrees) */
/* scan in a counter-clockwise direction (increasing degrees) */
/*------------------------------------------------------------*/

#include "robots.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <iomanip>

using std::cin;
using std::cout;
using std::endl;
using std::setw;
using std::setfill;
using std::string;

int iHeading = 0;
int iSpeed   = 0;
int iClosestTargetDistance   = 2000;
int iClosestTargetBearing    = 0;

#undef JAG_DEBUG
// #define JAG_DEBUG 1            // uncomment this to print debug output

#define RES                  10

inline void output_time_prompt()
{
    struct timespec stimespec;
    clock_gettime(CLOCK_MONOTONIC, &stimespec);
    cout << stimespec.tv_sec << ".";
    cout << std::setw(9) << std::setfill('0');
    cout << stimespec.tv_nsec;
}

inline void output_debug(int lineno)
{
#ifdef JAG_DEBUG
    output_time_prompt();
    cout << ": line " << lineno << ".";
    cout << endl;
#else
    (void)lineno;
#endif
}

inline void output_debug(int lineno, string const & sString1)
{
#ifdef JAG_DEBUG
   output_time_prompt();
   cout << ": line " << lineno << "; ";
   cout << sString1 << ".";
   cout << endl;
#else
   (void)lineno;
   (void)sString1;
#endif
}

inline void output_debug(int lineno, string const & sString1, int iInt)
{
#ifdef JAG_DEBUG
    output_time_prompt();
    cout << ": line " << lineno << "; ";
    cout << sString1 << "=" << iInt << ".";
    cout << endl;
#else
    (void)lineno;
    (void)sString1;
    (void)iInt;
#endif
}

inline void output_debug(int lineno, string const & sString1, int iInt1, string const & sString2, int iInt2)
{
#ifdef JAG_DEBUG
    output_time_prompt();
    cout << ": line " << lineno << "; ";
    cout << sString1 << "=" << iInt1 << ", ";
    cout << sString2 << "=" << iInt2 << ".";
    cout << endl;
#else
    (void)lineno;
    (void)sString1;
    (void)iInt1;
    (void)sString2;
    (void)iInt2;
#endif
}


/* cruise moves around the perimeter of the field  */
void cruise()
{
    int x, y;
    int iSpeedHigh;
    int iSpeedLow;

    int iCurSpeed = speed();
    int iPrevHeading = iHeading;
    int iPrevSpeed   = iSpeed;

    if ( iClosestTargetDistance <= 450 ) {
        iSpeedHigh = 140;
        iSpeedLow  = 49;
    } else if ( iClosestTargetDistance <= 550 ) {
        iSpeedHigh = 100;
        iSpeedLow  = 49;
    } else if ( iClosestTargetDistance <= 650 ) {
        iSpeedHigh = 89;
        iSpeedLow  = 49;
    } else if ( iClosestTargetDistance <= 700 ) {
        iSpeedHigh = 49;
        iSpeedLow  = 49;
    } else if ( iClosestTargetDistance <= 780 ) {
        iSpeedHigh = 2;
        iSpeedLow  = 2;
    } else if ( iClosestTargetDistance <= 900 ) {
        iSpeedHigh = 10;
        iSpeedLow  = 10;
    } else {
        iSpeedHigh = 19;
        iSpeedLow  = 19;
    }

   // We have to start slowing down before we turn, since
   // the robot can only turn when moving less than 50 units/sec.
   if ( iHeading <= 45 || 315 <= iHeading ) {       // if we're going east
      x = loc_x();
      if ( ( 800 < x ) ) {      //   if we're near the east edge
         if ( iCurSpeed < 50 ) { //     if we're going slow enough to turn
            iHeading = 268;     //       then turn
            iSpeed = iSpeedHigh;
         } else {
            iSpeed = iSpeedLow; //       else slow down so we can turn later
         }
      } else if ( 720 < x ) {
         iSpeed = iSpeedLow;    //    else it's time to start slowing down
      } else {
         iSpeed = iSpeedHigh;   //   else (not near east edge) head east fast
      }
   } else if ( 225 <= iHeading && iHeading <= 315 ) { // if we're going south
      y = loc_y();
      if ( ( y < 200 ) ) {       //   if we're near the south edge
         if ( iCurSpeed < 50 ) { //     if we're going slow enough to turn
            iHeading = 178;      //       then turn
            iSpeed = iSpeedHigh;
         } else {
            iSpeed = iSpeedLow;  //       else slow down so we can turn later
         }
      } else if ( y < 280 ) {
         iSpeed = iSpeedLow;     //    else it's time to start slowing down
      } else {
         iSpeed = iSpeedHigh;    //   else (not near south edge) head south fast
      }
   } else if ( 135 <= iHeading && iHeading <= 225 ) {  // if we're going west
      x = loc_x();
      if ( ( x < 200 ) ) {       //   if we're near the west edge
         if ( iCurSpeed < 50 ) { //     if we're going slow enough to turn
            iHeading = 88;       //       then turn
            iSpeed = iSpeedHigh;
         } else {
            iSpeed = iSpeedLow;  //       else slow down so we can turn later
         }
      } else if ( x < 280 ) {
         iSpeed = iSpeedLow;     //    else it's time to start slowing down
      } else {
         iSpeed = iSpeedHigh;    //   else (not near west edge) head west fast
      }
   } else if ( 45 <= iHeading && iHeading <= 135 ) {  // if we're going north
      y = loc_y();
      if ( ( 800 < y ) ) {       //   if we're near the north edge
         if ( iCurSpeed < 50 ) { //     if we're going slow enough to turn
            iHeading = 358;      //       then turn
            iSpeed = iSpeedHigh;
         } else {
            iSpeed = iSpeedLow;  //       else slow down so we can turn later
         }
      } else if ( 720 < y ) {
         iSpeed = iSpeedLow;     //    else it's time to start slowing down
      } else {
         iSpeed = iSpeedHigh;    //   else (not near north edge) head north fast
      }
   }

#ifdef JAG_NOTDEFINED        // change to "ifndef" to try always moving slowly
   if ( 50 <= iSpeed ) {
      iSpeed = 49;
   }
#endif

   if ( ( iPrevHeading != iHeading ) ||
        ( iPrevSpeed   != iSpeed )   ||
        ( iCurSpeed    != iSpeed )   ||
        ( 0            == iCurSpeed )   ) {
      drive( iHeading, iSpeed );
   }
}

int adjustForOurMotion(int *angle, int *range, int numCycles)
{
                                                  /* Correct for our motion */
   double dTargetDiffX, dTargetDiffY;
   double dOurMotionX,  dOurMotionY;

   dTargetDiffX = cos( ((float)*angle+0.4) * M_PI/180.0 ) * *range;
   dTargetDiffY = sin( ((float)*angle+0.4) * M_PI/180.0 ) * *range;
   output_debug( __LINE__, "adj: dOrigX", (int)dTargetDiffX,
                               " dOrigY", (int)dTargetDiffY );
                                   // how far we've travelled
                                   // 0.04 is distance/cycle/unit of speed
   dOurMotionX = cos( ((float)iHeading+0.4) * M_PI/180.0 ) *
                       iSpeed * numCycles * 0.04;
   dOurMotionY = sin( ((float)iHeading+0.4) * M_PI/180.0 ) *
                       iSpeed * numCycles * 0.04;
                                   // adjust target differences in X and Y
   dTargetDiffX = dTargetDiffX - dOurMotionX;
   dTargetDiffY = dTargetDiffY - dOurMotionY;
   output_debug( __LINE__, "adj: dOX", (int)dOurMotionX,
                                "dOY", (int)dOurMotionY );
   output_debug( __LINE__, "adj: dX", (int)dTargetDiffX,
                               " dY", (int)dTargetDiffY );
                                   // calculate new range and bearing
   *angle = (int)(round( atan2( dTargetDiffY, dTargetDiffX ) * 180.0 / M_PI ) );
   *range = (int)(round( sqrt( dTargetDiffX * dTargetDiffX +
                               dTargetDiffY * dTargetDiffY   )));
   *angle = ( *angle + 360 ) % 360;

   return 0;
}

int lockOnTarget(int angle, int beamwidth)
{
   int range = 0;
   output_debug( __LINE__, "lockOn call", angle );
   output_debug( __LINE__, "lockOn call", angle, "beamwidth ", beamwidth );
   if ( 1 == beamwidth ) {
      if ( 0 < (range = scan( angle, beamwidth )) ) {
         if ( 0 <= angle && 100 < range && range < 2000 ) {
            adjustForOurMotion( &angle, &range, 1 );
         }
         angle = ( 360 + angle ) % 360;
      } else {
         angle = -360;
      }
   } else {
      if ( 0 < ( range = scan( angle, beamwidth/2 ) ) ) {
         angle = lockOnTarget( angle, beamwidth/2 );
      } else if ( 0 < ( range = scan( (angle-beamwidth/2 + 360 - 1)%360,
                                      beamwidth/2 ) ) ) {
         angle = lockOnTarget( angle-beamwidth/2, beamwidth/2 );
      } else if ( 0 < ( range = scan( (angle+beamwidth/2 + 1)%360,
                                      beamwidth/2 ) ) ) {
         angle = lockOnTarget( angle+beamwidth/2, beamwidth/2 );
      }
   }
   output_debug( __LINE__, "lockOn returning", angle,
                           "beamwidth ", beamwidth    );
   return ( angle );
}

int main()
{
   int angle, range;
   int bestTargetAngle, bestTargetRange;

#ifdef JAG_NOTDEFINED     // change to "ifndef" to activate this test code
   int deg = 0;
   while ( 1 ) {
      output_time_prompt();
      if (cannon (deg, 500) != 0) {   // if shot was successful
         output_debug( __LINE__, "cannon FIRE", deg );
      } else {
         output_debug( __LINE__, "cannon wait", deg );
      }
      deg = (deg+1)%360;
   }
#endif

   output_debug( __LINE__, "START: " );

   while (1)
   {
      int iCyclesSinceScan;

      cruise();
      bestTargetRange = 751;
      iClosestTargetDistance = 2000;
      bestTargetAngle = 0;
      iCyclesSinceScan = 0;

      for ( angle=10; angle<360; angle+=(2*RES)-4 ) {
         range = scan( angle, RES );
         iCyclesSinceScan++;
         if ( range <= 0 ) {
            continue;           // nothing seen on this pie-wedge, try another
         } else if ( range < 300 ) {
            bestTargetRange = range;
            iClosestTargetDistance = range;
            iClosestTargetBearing  = angle;
            bestTargetAngle = angle;
            iCyclesSinceScan = 1;
            break;
         } else if ( range < 750 ) {
            if ( range < bestTargetRange ) {
               bestTargetRange = range;
               iClosestTargetDistance = range;
               iClosestTargetBearing  = angle;
               bestTargetAngle = angle;
               iCyclesSinceScan = 1;
            }
         } else {
            iClosestTargetDistance = range;
            iClosestTargetBearing  = angle;
         }
      }
      output_debug( __LINE__, "best target angle", bestTargetAngle,
                                          "range", bestTargetRange );

      if ( ( 0 < bestTargetRange ) && ( bestTargetRange < 750 ) )
      {
         int iNewAngle;

         iCyclesSinceScan += 3;     // allow for cycles to be used in cruise()
                              // adjust first, before calling cruise, since
                              // cruise may change movement speed or heading
         adjustForOurMotion( &bestTargetAngle,
                             &bestTargetRange,
                             iCyclesSinceScan );
         cruise();            // check if it's time to change speed or heading

         if ( 100 < bestTargetRange ) {              // if it's far away, then
            iNewAngle = lockOnTarget( bestTargetAngle, 16 );
            if ( 0 <= iNewAngle ) {
               angle = iNewAngle;
               range = scan( angle, 10 );
            }
            output_debug( __LINE__, "lock target angle", iNewAngle,
                                    "angle", angle                 );
            output_debug( __LINE__, "prevBest", bestTargetAngle,
                                    "range", range );
                  // adjust for our motion
                  //
         } else {
            range = bestTargetRange;
            angle = bestTargetAngle;
         }
         if ( ( 0 < range ) && ( range < 750 ) ) {
                                                  /* Correct for our motion */
            adjustForOurMotion( &angle, &range, 1 );

            if ( range < 40 ) {
               range = 40;
            }
            while ( ( 0 < range ) &&                  /* keep trying to fire */
                    ( 0 == cannon (angle, range) ) ) {  /* until successful  */
               int iSavedRange;
               output_debug( __LINE__, "cannon wait, angle", angle,
                                    "range", range                 );
               adjustForOurMotion( &angle, &range, 1 );

               iSavedRange = range;
               range = scan( (angle+360-4)%360, 4 );      // re-find the target
               if ( range <= 0 ) {
                  adjustForOurMotion( &angle, &iSavedRange, 1 );
                  range = scan( (angle+4)%360, 4 );
                  if ( range <= 0 ) {
                     adjustForOurMotion( &angle, &iSavedRange, 1 );
                     range = scan( (angle+360-12)%360, 8 );
                     if ( range <= 0 ) {
                        adjustForOurMotion( &angle, &iSavedRange, 1 );
                        range = scan( (angle+12)%360, 8 );
                        if ( range <= 0 ) {
                           adjustForOurMotion( &angle, &iSavedRange, 1 );
                           range = 0;
                        } else {
                           angle = ( angle+12 ) % 360;
                        }
                     } else {
                        angle = ( angle+360-12 ) % 360;
                     }
                  } else {
                     angle = ( angle+1 ) % 360;
                  }
               } else {
                  angle = (angle+360-1)%360;
               }
               if ( range <= 0 ) {
                  output_debug( __LINE__, "lost target" );
               } else {
                  adjustForOurMotion( &angle, &range, 1 );
               }
            }
            if ( 0 < range ) {
               output_debug( __LINE__, "cannon FIRE1: angle", angle,
                                       "range", range                );
               adjustForOurMotion( &angle, &range, 1 );
            }

            while ( ( 0 < range ) &&                  /* fire all shots */
                    ( 0 != cannon( angle, range ) ) ) {
               output_debug( __LINE__, "cannon FIRE2: angle", angle,
                                       "range", range               );
               adjustForOurMotion( &angle, &range, 1 );
            }
            output_debug( __LINE__, "cannon reloading: angle", angle,
                                    "range", range                    );
         }  // if there's something precise to shoot at
      }  // if there's something to shoot at
   } // while 1
}
