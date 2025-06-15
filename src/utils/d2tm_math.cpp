/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefan@fundynamic.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2022 (c) code by Stefan Hendriks

  Purpose of this file:

    math.cpp should deliver all mathematical functions needed in the game.


  */
#include "d2tm_math.h"

#include <fmt/format.h>

#include "definitions.h"
#include "common.h"

#include <cassert>
#include <cmath>
#include <cstdlib>

// returns in fRadians
float fRadians(int x1, int y1, int x2, int y2)
{
  float delta_x = (x2-x1);
  float delta_y = (y2-y1);

  // calculate fRadians
  float r = (std::atan2(delta_y, delta_x));

  // return in fRadians
  return r;
}


/**
 * Returns the amount of degrees (0-360) between two x,y positions.
 *
 * x1,y1 can be seen as 'from' and x2,y2 as 'to' (destination).
 * Given those terms, the amount degrees returned is:
 * When 'to' is perfectly left of 'from': 90.
 * When 'to' is straight above 'from': 360 (0).
 * When 'to' is perfectly right of 'from': 270.
 * When 'to' is straight below 'from': 180.
 *
 * This basically means, compared to an arrow of the clock. When a handle points upwards, we are at 360 degrees.
 * When the handle is below we are at 360 (or 0) degrees.
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @return
 */
float fDegrees(int x1, int y1, int x2, int y2) {
    /***
     calculation between two 2D positions, returning the angle in degrees (1-360 degrees).

     Method used:

     x1,y1 is always center of our 'fictional triangle'.
     Example:


      A           B
     x1,y1--------
     \           |
      ----       |
          \------+x2,y2
                  C


     Using tan to calculate the angle.

    tan = AB/BC


    the distances between A and B , B and C are calculated by delta_x and delta_y. Those will always
    be > 0 due the ABS command.
    ***/

    float delta_x = (x2 - x1);
    float delta_y = (y2 - y1);

    // this makes the 'circle' start at the top. If we use delta_y, delta_x as you would expect, the 'circle' (360 degrees)
    // starts at the left
    float angle = (std::atan2(delta_x, delta_y));

    // convert to fDegrees
    angle = angle * (180 / M_PI);

    angle += 180;

    // angle is now as following:
    // 360 is UP, (0 is also UP)
    // 270 = RIGHT,
    // 180 = DOWN,
    // 90 = LEFT
    return std::fabs(angle - 360);
}

// for bullets; bullets have twice as many angles (facings) than units. (16)
// also, bullets have a clock-wise rotation on the drawing bitmap
int bullet_face_angle(float angle) {
    int facingAngle = faceAngle(angle, 16);
    return convertAngleToDrawIndex(facingAngle, true, 0, 16);
}

float wrapDegrees(float value) {
    if (value < 0) {
        return value + 360;
    }
    if (value > 360) {
        return value - 360;
    }
    return value;
}

/**
 * Is degrees(angle) within angle1 and angle2? (taking care of wrapping around)
 * Taken from: https://stackoverflow.com/a/11412077/214597
 * @param degrees
 * @param angle1
 * @param angle2
 * @return
 */
bool isAngleBetween(int degrees, int angle1, int angle2) {
    // make the angle from angle1 to angle2 to be <= 180 degrees
    int rAngle = ((angle2 - angle1) % 360 + 360) % 360;
    if (rAngle >= 180)
        std::swap(angle1, angle2);

    // check if it passes through zero
    if (angle1 <= angle2)
        return degrees >= angle1 && degrees <= angle2;
    else
        return degrees >= angle1 || degrees <= angle2;
}

/**
 * This is an intermediate function that returns a "face angle" based on
 * the fDegrees() result.
 *
 * The "face angle" is a clock-wise rotation angle. In order
 * to calculate the correct 'facing angle' we require the degrees to
 * be inverted (?).
 * @param angle
 * @return
 */
int faceAngle(float angle, int angles) {
  int degreesPerFacing = (360 / angles);

  // face angles are determined
  // by having boundaries of degrees
  // ie, face angle of 0, (UP) is when degrees is
  // between 330 and 30 (or something)
  // we need to build up that table, depending
  // on the amount of degreesPerFacing we have

  float halfDegreesFacing = degreesPerFacing / 2;

  // 0 = UP
  float startDegreesFacing = wrapDegrees(0 - halfDegreesFacing); // -22 becomes (360 - 22) = 338
  int facingIndex = 0;

  // 360 - 22 =
  float start = wrapDegrees(startDegreesFacing); // ie 338
  float end = wrapDegrees(start + degreesPerFacing); // ie 338 + 45 = 383 - 360 = 23

  while (facingIndex < angles) {
      if (isAngleBetween(angle, start, end)) {
          // found it!
          break;
      }
      start = wrapDegrees(start + degreesPerFacing);
      end = wrapDegrees(end + degreesPerFacing);

      facingIndex++;
  }
    return facingIndex;
}

// Converts the faceAngle produced with the function above, into a correct number for drawing
// correctly.
int convertAngleToDrawIndex(int faceAngle, bool clockWiseBitmap, int offset, int maxFacings) {
    if (clockWiseBitmap) {
        // assume drawing bitmap has a clockwise direction (from left to right).
        // in that case, the offset is *added* to the faceAngle
        int angle = offset + faceAngle;

        // and finally make sure we wrap around
        if (angle > (maxFacings - 1)) {
            angle -= maxFacings;
        }
        return angle;
    } else {
        // assume drawing bitmap has a counter-clockwise direction (going from left to right).
        // we need to substract the faceAngle from the offset here.

        int angle = offset - faceAngle;

        // and finally make sure we wrap around
        if (angle < 0) {
            angle += maxFacings;
        }
        return angle;
    }
}

// return random number between 0 and 'max'
int rnd(int max) {
  if (max < 1) return 0;
  return std::rand() % max;
}

/**
 * returns length between 2 points, always > 0. If x and y match, distance is 1.
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @return
 */
double ABS_length(int x1, int y1, int x2, int y2) {
  if (x1 == x2 && y1 == y2) return 1; // when all the same, distance is 1 ...

  int A = std::abs(x2 - x1) * std::abs(x2 - x1);
  int B = std::abs(y2 - y1) * std::abs(y2 - y1);
  return sqrt(static_cast<double>(A + B)); // A2 + B2 = C2 :)
}
