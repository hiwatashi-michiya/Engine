#pragma once
#include <cstdlib>
#include <ctime>
#include <random>
#include "Vector3.h"

//乱数初期化
void SetRandom();
//float型で乱数を返す
float RandomFloat(float min, float max);
//int型で乱数を返す
int RandomInt(int min, int max);
//Vector3型で乱数を返す
Vector3 RandomVector3(float min, float max);
//Vector3型で乱数を返す
Vector3 RandomVector3(float minX, float maxX, float minY, float maxY, float minZ, float maxZ);
//Vector3型で乱数を返す
Vector3 RandomVector3(Vector3 min, Vector3 max);

