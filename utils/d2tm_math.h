#pragma once

// Angle stuff for unit facing
float fDegrees(int x1, int y1, int x2, int y2);
float fRadians(int x1, int y1, int x2, int y2);
int face_angle(float angle, int angles = 8);
float invertDegrees(float degrees);
int convert_angle(int face_angle);

// bullet only
int bullet_face_angle(float angle);

int rnd(int max);

// length calculation
double ABS_length(int x1, int y1, int x2, int y2); // returns only value > -1
