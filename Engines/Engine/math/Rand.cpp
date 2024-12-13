#include "Rand.h"

void SetRandom() {

	srand(unsigned int(time(nullptr)));

}

float RandomFloat(float min, float max) {

	//ランダムエンジン
	static std::random_device seed;
	static std::default_random_engine engine(seed());

	//最小値と最大値が逆の場合、入れ替える
	if (min > max) {
		std::swap(min, max);
	}

	std::uniform_real_distribution<float> dist(min, max);

	float result = dist(engine);

	return result;

}

int RandomInt(int min, int max) {

	//ランダムエンジン
	static std::random_device seed;
	static std::default_random_engine engine(seed());

	//最小値と最大値が逆の場合、入れ替える
	if (min > max) {
		std::swap(min, max);
	}

	std::uniform_int_distribution<int> dist(min, max);

	int result = dist(engine);

	return result;

}

Vector3 RandomVector3(float min, float max) {

	//ランダムエンジン
	static std::random_device seed;
	static std::default_random_engine engine(seed());

	//最小値と最大値が逆の場合、入れ替える
	if (min > max) {
		std::swap(min, max);
	}

	std::uniform_real_distribution<float> dist(min, max);

	Vector3 result{};

	result.x = dist(engine);
	result.y = dist(engine);
	result.z = dist(engine);

	return result;

}

Vector3 RandomVector3(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) {

	//ランダムエンジン
	static std::random_device seed;
	static std::default_random_engine engine(seed());

	//最小値と最大値が逆の場合、入れ替える
	if (minX > maxX) {
		std::swap(minX, maxX);
	}

	if (minY > maxY) {
		std::swap(minY, maxY);
	}

	if (minZ > maxZ) {
		std::swap(minZ, maxZ);
	}

	std::uniform_real_distribution<float> distX(minX, maxX);
	std::uniform_real_distribution<float> distY(minY, maxY);
	std::uniform_real_distribution<float> distZ(minZ, maxZ);

	Vector3 result{};

	result.x = distX(engine);
	result.y = distY(engine);
	result.z = distZ(engine);

	return result;

}

Vector3 RandomVector3(Vector3 min, Vector3 max) {

	//ランダムエンジン
	static std::random_device seed;
	static std::default_random_engine engine(seed());

	//最小値と最大値が逆の場合、入れ替える
	if (min.x > max.x) {
		std::swap(min.x, max.x);
	}

	if (min.y > max.y) {
		std::swap(min.y, max.y);
	}

	if (min.z > max.z) {
		std::swap(min.z, max.z);
	}

	std::uniform_real_distribution<float> distX(min.x, max.x);
	std::uniform_real_distribution<float> distY(min.y, max.y);
	std::uniform_real_distribution<float> distZ(min.z, max.z);

	Vector3 result{};

	result.x = distX(engine);
	result.y = distY(engine);
	result.z = distZ(engine);

	return result;

}
