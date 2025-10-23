/*
  Morse.cpp - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.
*/

#include "Arduino.h"
#include "Vector3.h"


Vector3::Vector3()
{
x = 0.0f;
y = 0.0f;
z = 0.0f;
}
Vector3::Vector3(float _x, float _y, float _z)
{
x = _x;
y = _y;
z = _z;
}

void Vector3::set(float _x, float _y, float _z)
{
x = _x;
y = _y;
z = _z;
}

void Vector3::set(Vector3* other)
{
x = other->x;
y = other->y;
z = other->z;
}

float Vector3::lengthSquared()
{
return x*x + y*y + z*z;
}

float Vector3::length()
{
return sqrt(lengthSquared());
}

void Vector3::normalize()
{
float Mag = length();
x /= Mag;
y /= Mag;
z /= Mag;
}

void Vector3::add(Vector3* other)
{
x += other->x;
y += other->y;
z += other->z;
}

void Vector3::subtract(Vector3* other)
{
x -= other->x;
y -= other->y;
z -= other->z;
}

void Vector3::reflect(Vector3* normal)
{
// I is the original vector
// N is the normal of the incident plane
// R = I - (2 * N * ( DotProduct[ I,N] ))

// == this - ( 2 * normal *  dot(this,normal) )
// == (-2 * normal *  ( this dot normal ) ) + this;

float thisX =x;
float thisY =y;
float thisZ =z;

float tdotn = dot(normal);
set(normal);
scale(-2.0f);
scale(tdotn);

x+= thisX;
y+= thisY;
z+= thisZ;
}

void Vector3::scale(float scalar)
{
x *= scalar;
y *= scalar;
z *= scalar;
}

void Vector3::divide(float scalar)
{
x /= scalar;
y /= scalar;
z /= scalar;
}

void Vector3::absolute()
{
x = abs(x);
y = abs(y);
z = abs(z);
}

float Vector3::dot(Vector3* other)
{
return x * other->x + y * other->y + z * other->z;
}

void Vector3::cross(Vector3* other)
{
x = y * other->z - z * other->y;
y = z * other->x - x * other->z;
z = x * other->y - y * other->x;
}