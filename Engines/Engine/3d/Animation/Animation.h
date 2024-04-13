#pragma once
#include "Vector3.h"
#include "Quaternion.h"
#include "Keyframe/Keyframe.h"
#include <vector>
#include <map>
#include <string>

template<typename tValue>
struct AnimationCurve {
	std::vector<Keyframe<tValue>> keyFrames;
};

struct NodeAnimation {
	AnimationCurve<Vector3> translate;
	AnimationCurve<Quaternion> rotate;
	AnimationCurve<Vector3> scale;
};

struct Animation {
	float duration; //アニメーション全体の尺(単位は秒)
	//NodeAnimationの集合。Node名でひけるようにしておく
	std::map<std::string, NodeAnimation> nodeAnimations;
};

Animation LoadAnimationFile(const std::string& fileName);

Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);

Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);
