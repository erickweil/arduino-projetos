/*
  Morse.h - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/
#ifndef Vector3_h
#define Vector3_h

#include "Arduino.h"

class Vector3
{
  public:
    Vector3();
    Vector3(float x, float y, float z);
	void set(float x, float y, float z);
	void set(Vector3* other);
    float lengthSquared();
    float length();
    void normalize();
    void add(Vector3* other);
    void subtract(Vector3* other);
	void reflect(Vector3* other);
    void scale(float scalar);
    void divide(float scalar);
	void absolute();
    float dot(Vector3* other);
    void cross(Vector3* other);
    float x;
	float y;
	float z;
};

#endif