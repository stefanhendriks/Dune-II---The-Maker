#pragma once

// Angle stuff for unit facing
float fDegrees(int x1, int y1, int x2, int y2);
float fRadians(int x1, int y1, int x2, int y2);
int faceAngle(float angle, int angles = 8);
float invertDegrees(float degrees);
int convertAngleToDrawIndex(int faceAngle, bool clockWiseBitmap = false, int offset = 2, int maxFacings = 8);

// bullet only
int bullet_face_angle(float angle);

// length calculation
double ABS_length(int x1, int y1, int x2, int y2); // returns only value > -1
