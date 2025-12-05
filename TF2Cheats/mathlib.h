#pragma once
#include <cmath>
#include <vector>

class Matrix3x4
{
public:
	Matrix3x4() : values() {}

	float* operator[](const int i) 
	{
		return values[i];
	}

	const float* operator[](const int i) const 
	{
		return values[i];
	}

	float values[3][4];
};

class Matrix4x4
{
public:
	Matrix4x4() : values() {};

	float* operator[](const int i) {
		return values[i];
	}

	const float* operator[](const int i) const {
		return values[i];
	}

	float values[4][4];
};

class Vector2
{
public:
	float abscissa;
	float ordinate;

	Vector2() 
	{
		this->abscissa = 0.f;
		this->ordinate = 0.f;
	}

	Vector2(float x, float y)
	{
		this->abscissa = x;
		this->ordinate = y;
	}

	float length()
	{
		return sqrt(pow(abscissa, 2.0f) + pow(ordinate, 2.0f));
	}
};

class Vector3
{
public:
	float abscissa;
	float ordinate;
	float applicate;

	Vector3()
	{
		this->abscissa = 0.f;
		this->ordinate = 0.f;
		this->applicate = 0.f;
	}

	Vector3(float x, float y, float z)
	{
		this->abscissa = x;
		this->ordinate = y;
		this->applicate = z;
	}

	float length()
	{
		return sqrt(pow(abscissa, 2.0f) + pow(ordinate, 2.0f) + pow(applicate, 2.0f));
	}

	void subtract(Vector3 subtrahendVector)
	{
		abscissa = abscissa - subtrahendVector.abscissa; 
		ordinate = ordinate - subtrahendVector.ordinate;
		applicate = applicate - subtrahendVector.applicate;
	}
};