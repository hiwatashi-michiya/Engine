#pragma once
#include <cstdlib>
#include <ctime>
#include <random>
#include "Vector3.h"

void SetRandom();

float RandomFloat(float min, float max);

int RandomInt(int min, int max);

Vector3 RandomVector3(float min, float max);

Vector3 RandomVector3(float minX, float maxX, float minY, float maxY, float minZ, float maxZ);

Vector3 RandomVector3(Vector3 min, Vector3 max);

