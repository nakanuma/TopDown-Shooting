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
	void StartShake(float duration, float intensity);

	/// <summary>
	/// カメラに加算するオフセットを取得します。
	/// </summary>
	/// <returns>オフセット（Float3）</returns>
	const Float3& GetOffset() const { return offset_; }

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
	// Member Variables
	// =========================================================
	float duration_ = 0.0f;              /* カメラシェイク継続時間 */
	float intensity_ = 0.0f;             /* 揺れの強さ */
	float elapsedTime_ = 0.0f;           /* 経過時間 */
	bool isShaking_ = false;             /* シェイク中フラグ */
	Float3 offset_ = {0.0f, 0.0f, 0.0f}; /* カメラに加えるオフセット */
};