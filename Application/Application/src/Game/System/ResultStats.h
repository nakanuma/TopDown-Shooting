#pragma once

// C++
#include <stdint.h>

// Engine
#include <TimeManager.h>

/// <summary>
/// リザルト表示戦績の管理クラス
/// </summary>
class ResultStats {
public:
	/// <summary>
	/// インスタンス取得
	/// </summary>
	static ResultStats* GetInstance();

	/// <summary>
	/// 戦績をクリア
	/// </summary>
	void Clear();

	// 記録用
	void AddShot() { totalShots_++; }
	void AddHit() { hitShots_++; }
	void AddDamage(uint32_t dmg) { totalDamage_ += dmg; }
	void AddDefeated() { defeated_++; }
	void AddTime() { clearTime_ += TimeManager::GetInstance()->GetDeltaTime(); }

	// 取得用
	uint32_t GetTotalDamage() const { return totalDamage_; }
	uint32_t GetDefeated() const { return defeated_; }
	float GetHitRate() const; // 命中率
	float GetClearTime() const { return clearTime_; }

	/// <summary>
	/// デバッグ表示
	/// </summary>
	void Debug();

private:
	uint32_t totalShots_ = 0;
	uint32_t hitShots_ = 0;
	uint32_t totalDamage_ = 0;
	uint32_t defeated_ = 0;
	float clearTime_ = 0.0f;
};
