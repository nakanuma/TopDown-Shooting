#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <stdint.h>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <TimeManager.h>

// =========================================================
// リザルト表示戦績の管理クラス
// =========================================================
class ResultStats {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// リザルト表示戦績クラスのシングルトンインスタンスを返します。
	/// </summary>
	/// <returns>インスタンス</returns>
	static ResultStats* GetInstance();

	/// <summary>
	/// 戦績のリセットを行います。
	/// </summary>
	void Clear();

	/// <summary>
	/// 射撃数を加算します。
	/// </summary>
	void AddShot() { totalShots_++; }

	/// <summary>
	/// ヒット数を加算します。
	/// </summary>
	void AddHit() { hitShots_++; }

	/// <summary>
	/// 合計ダメージを加算します。
	/// </summary>
	void AddDamage(uint32_t dmg) { totalDamage_ += dmg; }

	/// <summary>
	/// 敵の撃破数を加算します。
	/// </summary>
	void AddDefeated() { defeated_++; }

	/// <summary>
	/// クリアタイムを加算します。
	/// </summary>
	void AddTime() { clearTime_ += TimeManager::GetInstance()->GetDeltaTime(); }

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// 合計与ダメージを取得します。
	/// </summary>
	/// <returns>合計与ダメージ（uint32_t）</returns>
	uint32_t GetTotalDamage() const { return totalDamage_; }

	/// <summary>
	/// 敵の撃破数を取得します。
	/// </summary>
	/// <returns>撃破数（uint32_t）</returns>
	uint32_t GetDefeated() const { return defeated_; }

	/// <summary>
	/// 射撃した弾の命中率を取得します。
	/// </summary>
	/// <returns>命中率（0.0f～100.0f）</returns>
	float GetHitRate() const;

	/// <summary>
	/// クリアタイムを取得します。
	/// </summary>
	/// <returns>クリアタイム（float）</returns>
	float GetClearTime() const { return clearTime_; }

	/// <summary>
	/// デバッグ用の描画処理を行います。
	/// </summary>
	void Debug();

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// ----- Parameters -----
	uint32_t totalShots_ = 0;		/* 合計射撃数 */
	uint32_t hitShots_ = 0;			/* 命中した弾数 */
	uint32_t totalDamage_ = 0;		/* 合計与ダメージ */
	uint32_t defeated_ = 0;			/* 敵の撃破数 */
	float clearTime_ = 0.0f;		/* クリア時間 */
};
