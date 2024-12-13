#pragma once
#include "Quaternion.h"
#include "Vector3.h"

/// <summary>
/// キーフレーム構造体
/// </summary>
/// <typeparam name="tValue"></typeparam>
template <typename tValue>
struct Keyframe {
	float time;
	tValue value;
};

//Vector3とQuaternionのキーフレーム定義
using KeyframeVector3 = Keyframe<Vector3>;
using KeyframeQuaternion = Keyframe<Quaternion>;
