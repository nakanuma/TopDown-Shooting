#pragma once

// Engine
#include <MyMath.h>

/// <summary>
/// カメラ追従
/// </summary>
class FollowCamera
{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(const Float3& offset);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 追従目標をセット
	/// </summary>
	void SetTarget(const Float3* translate);

	/// <summary>
	/// 現在位置の取得
	/// </summary>
	Float3 GetCameraPosition() const { return currentPos_; }

private:
	// 追従対象の座標
	const Float3* targetTranslate_ = nullptr;

	// オフセット
	Float3 offset_;

	// 現在の位置
	Float3 currentPos_;
};

