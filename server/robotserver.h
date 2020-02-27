#ifndef ROBOTSERVER_H
#define ROBOTSERVER_H

#include "net_utils.h"

#define RELOAD_RATIO 50
#define SPEED_RATIO 0.04
#define BREAK_DISTANCE (0.7 / SPEED_RATIO)

struct Cannon {
    int timeToReload;
    int x;
    int y;
};

struct Robot {
  char name[STD_BUF];   // this size must match size of progname in client
  double x;
  double y;
  int damage;
  int speed;
  int break_distance;
  int target_speed;
  int degree;
  int cannon_degree;
  int radar_degree;
  Cannon cannon[2];
  float color[3]; 	

  /* info on what was done... */
};

extern Robot **all_robots;
extern int max_robots;

/* Interface from networking code to game logic. */

int scan(Robot *r, int degree, int resolution);
int cannon(Robot *r, int degree, int range);
void drive(Robot *r, int degree, int speed);
void cycle(void);
int loc_x(Robot *r);
int loc_y(Robot *r);
int speed(Robot *r);
int damage(Robot *r);
void kill_robot(Robot *r);

#endif
