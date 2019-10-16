#ifndef GAME_MATH_H
#define GAME_MATH_H

#include <math.h>

union v2 {
	struct
	{
		float X, Y;
	};
	float E[2];
};

inline v2 operator+(v2 A, v2 B)
{
	v2 Result;

	Result.X = A.X + B.X;
	Result.Y = A.Y + B.Y;

	return Result;
}

inline v2 operator-(v2 A, v2 B)
{
	v2 Result;

	Result.X = A.X - B.X;
	Result.Y = A.Y - B.Y;

	return Result;
}

inline v2
V2(float X, float Y)
{
	v2 Result;

	Result.X = X;
	Result.Y = Y;

	return Result;
}

inline float
Inner(v2 A, v2 B)
{
	float Result = A.X * B.X + A.Y * B.Y;

	return Result;
}

inline float
LengthSq(v2 A)
{
	float Result = Inner(A, A);

	return Result;
}

inline float
Absolute(float a)
{
	float Result = a;
	if (Result < 0)
	{
		Result *= -1;
	}
	
	return Result;
}

#endif // !MATH_H