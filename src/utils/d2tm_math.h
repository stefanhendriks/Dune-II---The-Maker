/**
 * @file d2tm_math.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#ifndef M_PI 
// Taken from cmath.h - An easy way to be cross-platform compatible, see also https://stackoverflow.com/a/6563891/214597
#define M_PI        3.14159265358979323846264338327950288   /* pi             */
#endif // M_PI

// Angle stuff for unit facing
float fDegrees(int x1, int y1, int x2, int y2);
float fRadians(int x1, int y1, int x2, int y2);
float wrapDegrees(float value);
bool isAngleBetween(int degrees, int angle1, int angle2);
int faceAngle(float angle, int angles = 8);
float invertDegrees(float degrees);
int convertAngleToDrawIndex(int faceAngle, bool clockWiseBitmap = false, int offset = 2, int maxFacings = 8);

// bullet only
int bullet_face_angle(float angle);

// length calculation
double ABS_length(int x1, int y1, int x2, int y2); // returns only value > -1
