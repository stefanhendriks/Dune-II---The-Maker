#pragma once

// Angle stuff for unit facing
float fDegrees(int x1, int y1, int x2, int y2);
float fRadians(int x1, int y1, int x2, int y2);
int face_angle(float angle);
int convert_angle(int face_angle);

// bullet only
int bullet_face_angle(float angle);

// randomizing timer(s)
int rnd(int max);

// lenght calculation
double length(int x1, int y1, int x2, int y2);
double ABS_length(int x1, int y1, int x2, int y2); // returns only value > -1
