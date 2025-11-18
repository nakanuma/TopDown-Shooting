#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <MyMath.h>

// =========================================================
// 追従カメラの管理を行うクラス
// =========================================================
class FollowCamera {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 追従カメラの初期化処理を行います。
	/// </summary>
	/// <param name="offset">オフセット（Float3）</param>
	void Initialize(const Float3& offset);

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	void Update();

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// 追従目標のセットを行います。
	/// </summary>
	/// <param name="translate">現在位置（Float3）</param>
	void SetTarget(const Float3* translate);

	/// <summary>
	/// 現在位置を取得します。
	/// </summary>
	/// <returns></returns>
	Float3 GetCameraPosition() const { return currentPos_; }

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr float kCursorInfluence = 0.2f; /* カーソル補正の影響度 */
	static constexpr float kFollowSpeed = 0.75f;    /* カメラの追従速度 */

	// =========================================================
	// Member Variables
	// =========================================================
	const Float3* targetTranslate_ = nullptr;  /* 追従対象の座標 */
	Float3 offset_ = {0.0f, 0.0f, 0.0f};       /* カメラに加えるオフセット */
	Float3 currentPos_ = {0.0f, 0.0f, 0.0f};   /* 現在のカメラ位置 */
	Float3 cursorOffset_ = {0.0f, 0.0f, 0.0f}; /* カーソル補正分のオフセット */
};
