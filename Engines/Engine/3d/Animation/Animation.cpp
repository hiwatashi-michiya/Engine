#include "Animation.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <cassert>
#include <windows.h>
#include "Convert.h"

Animation LoadAnimationFile(const std::string& fileName) {

	Animation animation; //今回作るアニメーション
	Assimp::Importer importer;
	std::string filePath = fileName;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);

	//アニメーションが無い場合、assert
	if (scene->mNumAnimations == 0) {

		std::string text = "There is no animation in the file \n";

		text += filePath;

		MessageBox(NULL, ConvertString(text).c_str(), L"Animation - LoadAnimationFile", MB_OK);

		return animation;

		assert(false);

	}

	aiAnimation* animationAssimp = scene->mAnimations[0]; //最初のアニメーションだけ採用。今後複数対応するのもあり
	animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond); //時間の単位を秒に変換

	//assimpでは個々のAnimationをchannelと呼んでいるのでchannelを回してNodeAnimationの情報を取ってくる
	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex) {

		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation = animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];

		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); //ここも秒に変換
			keyframe.value = { -keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z }; //右手->左手
			nodeAnimation.translate.keyFrames.push_back(keyframe);
		}

		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex) {
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			KeyframeQuaternion keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); //ここも秒に変換
			keyframe.value = { keyAssimp.mValue.x, -keyAssimp.mValue.y, -keyAssimp.mValue.z, keyAssimp.mValue.w }; //右手->左手
			nodeAnimation.rotate.keyFrames.push_back(keyframe);
		}

		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond); //ここも秒に変換
			keyframe.value = { keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z }; //そのまま
			nodeAnimation.scale.keyFrames.push_back(keyframe);
		}

	}

	return animation;

}

Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time) {

	//キーが無いものは返す値が不明なのでダメ
	if (keyframes.empty()) {

		MessageBox(NULL, L"keyframe is empty", L"Animation - CalculateValue(Vector3)", MB_OK);

		assert(false);

	}

	//キーが1つか、時刻がキーフレーム前なら最初の値とする
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}

	//先頭から時刻を順番に調べ、指定した時刻が範囲内であれば、補間を行い値を返す
	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		//indexとnextIndexの2つのkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			//範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Lerp(keyframes[index].value, keyframes[nextIndex].value, t);

		}

	}

	//ここまできた場合は一番後の時刻よりも後ろなので最後の値を返すことにする
	return (*keyframes.rbegin()).value;

}

Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time) {

	//キーが無いものは返す値が不明なのでダメ
	if (keyframes.empty()) {

		MessageBox(NULL, L"keyframe is empty", L"Animation - CalculateValue(Quaternion)", MB_OK);

		assert(false);

	}

	//キーが1つか、時刻がキーフレーム前なら最初の値とする
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}

	//先頭から時刻を順番に調べ、指定した時刻が範囲内であれば、補間を行い値を返す
	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		//indexとnextIndexの2つのkeyframeを取得して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			//範囲内を補間する
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Slerp(keyframes[index].value, keyframes[nextIndex].value, t);

		}

	}

	//ここまできた場合は一番後の時刻よりも後ろなので最後の値を返すことにする
	return (*keyframes.rbegin()).value;

}
