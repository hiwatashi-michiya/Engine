#pragma once
#include "Camera.h"
#include "Drawing/LineDrawer.h"
#include "Drawing/RenderingData.h"
#include "Keyframe/Keyframe.h"
#include "Quaternion.h"
#include "Vector3.h"
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

/// <summary>
/// アニメーションカーブ
/// </summary>
/// <typeparam name="tValue"></typeparam>
template<typename tValue>
struct AnimationCurve {
	std::vector<Keyframe<tValue>> keyFrames;
};
/// <summary>
/// ノードアニメーション
/// </summary>
struct NodeAnimation {
	AnimationCurve<Vector3> translate;
	AnimationCurve<Quaternion> rotate;
	AnimationCurve<Vector3> scale;
};

/// <summary>
/// アニメーション構造体
/// </summary>
struct Animation {
	float duration; //アニメーション全体の尺(単位は秒)
	//NodeAnimationの集合。Node名でひけるようにしておく
	std::map<std::string, NodeAnimation> nodeAnimations;
};

/// <summary>
/// ジョイント
/// </summary>
struct Joint {
	QuaternionTransform transform; //transform情報
	Matrix4x4 localMatrix; //localMatrix
	Matrix4x4 skeletonSpaceMatrix; //skeletonSpaceでの変換行列
	std::string name; //名前
	std::vector<int32_t> children; //子JointのIndexのリスト。いなければ空
	int32_t index; //自身のIndex
	std::optional<int32_t> parent; //親JointのIndex。いなければnull
	std::shared_ptr<Line> line; //描画用ライン
};

/// <summary>
/// スケルトン
/// </summary>
struct Skeleton {
	int32_t root; //RootJointのIndex
	std::map<std::string, int32_t> jointMap; //Joint名とIndexとの辞書
	std::vector<Joint> joints; //所属しているジョイント
};

//アニメーションファイル読み込み
Animation LoadAnimationFile(const std::string& fileName);
//値計算
Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);
//値計算
Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);
//ジョイント作成
int32_t CreateJoint(const Node& node,
	const std::optional<int32_t>& parent,
	std::vector<Joint>& joints);
//スケルトン作成
Skeleton CreateSkeleton(const Node& rootNode);
//スケルトン更新
void UpdateSkeleton(Skeleton& skeleton);
//スケルトンライン描画
void DrawSkeletonLine(Skeleton& skeleton, Camera* camera, const Matrix4x4& matrix);

//アニメーションの適用
void ApplyAnimation(Skeleton& skeleton, const Animation& animation, float animationTime);
