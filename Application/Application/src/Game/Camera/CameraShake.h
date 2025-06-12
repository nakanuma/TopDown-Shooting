#pragma once

// Engine
#include <Engine/Math/MyMath.h>

/// <summary>
/// カメラシェイク
/// </summary>
class CameraShake {
public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	static CameraShake* GetInstance();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// シェイク開始
	/// </summary>
	void StartShake(float duration, float intensity);

	/// <summary>
	/// シェイクオフセット取得
	/// </summary>
	/// <returns></returns>
	const Float3& GetOffset() const { return offset_; }

private:
	/// <summary>
	/// シェイク適用
	/// </summary>
	void ApplyShake();

	float duration_ = 0.0f;    // 継続時間
	float intensity_ = 0.0f;   // 強度
	float elapsedTime_ = 0.0f; // 経過時間
	const float kDeltaTime = 1.0f / 60.0f;
	bool isShaking_ = false; // シェイク中？
	Float3 offset_;          // カメラに加えるオフセット
};