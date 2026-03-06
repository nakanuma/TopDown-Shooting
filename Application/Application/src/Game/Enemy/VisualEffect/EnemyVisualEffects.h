#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Object3D.h>

// =========================================================
// 敵の発光演出クラス
// =========================================================
class EnemyVisualEffects
{
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	/// <param name="targetObject">対象のオブジェクト</param>
	void Initialize(Cygnus::Object3D* targetObject);

	/// <summary>
	/// 更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// 発光演出を開始します。
	/// </summary>
	void TriggerHitBlink();

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr float kHitBlinkDuration = 0.05f;						/* 被弾時の発光時間 */
	static constexpr Cygnus::Float3 kHitBlinkColor = { 1.0f, 0.5f, 0.0f };	/* 被弾時の発光色 */

	// =========================================================
	// Member Variables
	// =========================================================
	Cygnus::Object3D* targetObject_ = nullptr; /* 対象のオブジェクト */

	/// <summary>
	/// 被弾時の発光演出フェーズ
	/// </summary>
	enum class HitBlinkPhase {
		Wait,		// 待機
		BlinkIn,	// 発光
		BlinkOut	// 減光
	} hitBlinkPhase_ = HitBlinkPhase::Wait;
	bool isHitBlink_ = false;		/* 被弾時の発光演出中フラグ */
	float hitBlinkTimer_ = 0.0f;	/* 被弾時の発光演出タイマー */
};

