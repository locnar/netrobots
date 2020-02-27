#include "robotserver.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

Robot **all_robots;   // jag; 15nov2018; added here
int max_robots;              // jag; 15nov2018; added here

template<typename T>
static T clamp(T value, T min, T max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

void kill_robot(Robot *r)
{
    r->x = -1000;
    r->y = -1000;
    r->damage = 100;
    r->cannon[0].timeToReload = 0;
    r->cannon[1].timeToReload = 0;
}

int loc_x(Robot *r)
{
    /* Convert 0.5 ... 999.5 to 0 ... 1000 */
    int const x = (r->x - 0.5) * 1000 / 999;
    return clamp(x, 0, 999);
}

int loc_y(Robot *r)
{
    /* Convert 0.5 ... 999.5 to 0 ... 1000 */
    int const y = (r->y - 0.5) * 1000 / 999;
    return clamp(y, 0, 999);
}

int speed(Robot *r)
{
    return r->speed;
}

int damage(Robot *r)
{
    return r->damage;
}

int standardizeDegree(int degree)
{	
    int result = degree;
    if (degree < 0) {
        result =  360 - (-degree % 360);
    }
    return result % 360;
}

int getDistance(int x1, int y1, int x2, int y2)
{
    int const x = x2 - x1;
    int const y = y2 - y1;
    return sqrt(x * x + y * y);
}

int compute_angle(int x1, int y1, int x2, int y2)
{
    return standardizeDegree(atan2(y2 - y1,x2 - x1) * 180.0 / M_PI);
}

int scan(Robot *r, int degree, int resolution)
{
    if (resolution < 0 || resolution > 10) {
        return -1;
    }
    
    degree = standardizeDegree(degree);
    r->radar_degree = degree;
    
    int const upper_angle = degree + resolution;
    int const bottom_angle = degree - resolution;

    int const posx = r->x;
    int const posy = r->y;

    int min_distance = 1500;
    for (int i = 0; i < max_robots; i++) {
        if (all_robots[i]->damage < 100) {
            int const angle_between_robots = compute_angle(posx, posy, all_robots[i]->x, all_robots[i]->y);
            if (angle_between_robots <= upper_angle && angle_between_robots >= bottom_angle) {
                int const distance = getDistance(posx, posy, all_robots[i]->x, all_robots[i]->y);

                if (distance < min_distance && distance != 0) {
                    min_distance = distance;
                }
            }
        }
    }
    if (min_distance == 1500) {
        return 0;
    }
    return min_distance;
}

int cannon(Robot *r, int degree, int range)
{
	int i;
        int freeSlot;
	int distance_from_center;

	/* If the cannon is not reloading, meaning it's ready the robottino shoots otherwise break */
	for (freeSlot = 0; freeSlot < 2; ++freeSlot) {
            if (r->cannon[freeSlot].timeToReload == 0) {
                break;
            }
        }
	
	if (freeSlot == 2) {
            return 0;
	}
	
	/* If we reach that point the missile could be shot */
	if (range > 700) {
            range = 700;
        }
	
	degree = standardizeDegree(degree);
	
	r->cannon_degree = degree;
	
	int const x = cos(degree * M_PI / 180.0) * range + r->x;
	int const y = sin(degree * M_PI / 180.0) * range + r->y;
	
	for (i = 0; i < max_robots; i++) {
		if (all_robots[i]->damage < 100) {
			distance_from_center = getDistance(all_robots[i]->x, all_robots[i]->y, x, y);
			if (distance_from_center <= 5)
				all_robots[i]->damage += 10;
			else if (distance_from_center <= 20)
				all_robots[i]->damage += 5;
			else if (distance_from_center <= 40)
				all_robots[i]->damage += 3;
		}
		if (all_robots[i]->damage >= 100)
			kill_robot(all_robots[i]);
	}
	
	r->cannon[freeSlot].timeToReload = RELOAD_RATIO;
	r->cannon[freeSlot].x = x;
	r->cannon[freeSlot].y = y;

	return 1;
}

void drive(Robot *r, int degree, int speed)
{
    degree = standardizeDegree(degree);
    if (r->speed >= 50) {
        degree = r->degree;
    }

    if (speed > r->target_speed) {
        r->speed = speed;
    }

    r->target_speed = speed;
    r->degree = degree;
    r->break_distance = BREAK_DISTANCE;
}

constexpr float TOL = sin(M_PI / 360.0);

static void cycle_robot(Robot *r)
{
    if (r->x >= 1000 + TOL || r->x <= -TOL || r->y >= 1000 + TOL || r->y <= -TOL) {
        r->damage += 2;
        r->speed = 0;
        r->break_distance = 0;
        r->target_speed = 0;
    }

    if (r->damage < 100) {
        r->x = clamp(r->x, 0.0, 1000.0);
        r->y = clamp(r->y, 0.0, 1000.0);
    }
		
    for (int i = 0; i < max_robots; ++i) {
        if (all_robots[i] != r) {
            if ((int)r->x == (int)all_robots[i]->x && (int)r->y == (int)all_robots[i]->y) {
                r->damage += 2;
                r->speed = 0;
                r->break_distance = 0;
                r->target_speed = 0;
                all_robots[i]->damage += 2;
                all_robots[i]->speed = 0;
                all_robots[i]->break_distance = 0;
                all_robots[i]->target_speed = 0;
                if (r->x > 0.0) {
                    r->x -= 1.0;
                }
                else {
                    r->x += 1.0;
                }
                if (r->y > 0.0) {
                    r->y -= 1.0;
                } 
                else {
                    r->y += 1.0;
                }
            }
        }
    }
    
    r->x += cos(r->degree * M_PI / 180.0) * r->speed * SPEED_RATIO;
    r->y += sin(r->degree * M_PI / 180.0) * r->speed * SPEED_RATIO;
    
    if (r->break_distance == 0) {
        r->speed = r->target_speed;
    }
    else if (r->target_speed < r->speed) {
        r->speed += (r->target_speed - r->speed) / r->break_distance;
        r->break_distance--;
    }
    
    /* Decreasing the time to reload the missiles */
    for (int i = 0; i < 2; i++) {
        if (r->cannon[i].timeToReload > 0) {
            r->cannon[i].timeToReload--;
        }
    }
}

void cycle()
{
    for (int i = 0; i < max_robots; i++) {
        if (all_robots[i]->damage < 100) {
            cycle_robot(all_robots[i]);
        }
    }
}

