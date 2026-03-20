#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Engine/Math/MyMath.h>

// =========================================================
// カメラシェイクの制御を行うクラス
// =========================================================
class CameraShake {
public:
	/// <summary>
	/// シェイクモード
	/// </summary>
	enum class ShakeMode {
		Random,	// 通常のランダム揺れ
		SineY   // 上下揺れ（プレイヤー射撃時用）
	};

	/// <summary>
	/// シェイク優先度（モード上書き防止用）
	/// </summary>
	enum class ShakePriority {
		Low,
		High,
	};

	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// カメラシェイク制御クラスのシングルトンインスタンスを返します。
	/// </summary>
	/// <returns>インスタンス</returns>
	static CameraShake* GetInstance();

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// カメラシェイクを行います。
	/// </summary>
	/// <param name="duration">継続時間</param>
	/// <param name="intensity">揺れの強さ</param>
	void StartShake(float duration, float intensity, ShakePriority priority = ShakePriority::Low);

	/// <summary>
	/// 上下揺れを行います（プレイヤー射撃時用）
	/// </summary>
	/// <param name="duration"></param>
	/// <param name="intensity"></param>
	void StartShootShake();

	/// <summary>
	/// カメラに加算するオフセットを取得します。
	/// </summary>
	/// <returns>オフセット（Float3）</returns>
	const Cygnus::Float3& GetOffset() const { return offset_; }

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// カメラシェイクを適用し、オフセットの更新を行います。
	/// </summary>
	void ApplyShake();

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr float shootShakeDuration = 0.075f; // 射撃揺れ時間
	static constexpr float shootShakeIntensity = 0.15f; // 射撃揺れ強度

	// =========================================================
	// Member Variables
	// =========================================================
	float duration_ = 0.0f;              /* カメラシェイク継続時間 */
	float intensity_ = 0.0f;             /* 揺れの強さ */
	float elapsedTime_ = 0.0f;           /* 経過時間 */
	bool isShaking_ = false;             /* シェイク中フラグ */
	Cygnus::Float3 offset_ = {0.0f, 0.0f, 0.0f}; /* カメラに加えるオフセット */

	ShakeMode mode_ = ShakeMode::Random;					/* 現在のシェイクモード */
	ShakePriority currentPriority_ = ShakePriority::Low;	/* シェイク優先度 */
};