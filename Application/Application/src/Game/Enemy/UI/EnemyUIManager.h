#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <SpriteCommon.h>

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Enemy/UI/Parts/EnemyHPBar.h>
#include <src/Game/Enemy/UI/Parts/EnemyReloadBar.h>
#include <src/Game/Enemy/UI/Parts/EnemyDetectionIcon.h>
#include <src/Game/Enemy/UI/Parts/EnemyReloadIcon.h>

/// <summary>
/// 敵のUI表示に使用する情報
/// </summary>
struct EnemyUIState {
	Cygnus::Float3 worldPos;
	float hpRatio;
	float reloadRatio;
	bool isReloading = false;
};

// =========================================================
// 敵のUI管理クラス
// =========================================================
class EnemyUIManager
{
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理を行います。
	/// </summary>
	void Update(const EnemyUIState& state);

	/// <summary>
	/// 描画処理を行います。
	/// </summary>
	void Draw();

	/// <summary>
	/// 発見アイコンのアニメーションを開始します。
	/// </summary>
	void PlayDetectAnimation() { detectionIcon_->Play(); }

private:
	// =========================================================
	// Member Variables
	// =========================================================
	std::unique_ptr<Cygnus::SpriteCommon> spriteCommon_;	/* スプライト共通処理 */

	std::unique_ptr<EnemyHPBar> hpBar_;					/* HPバー */
	std::unique_ptr<EnemyReloadBar> reloadBar_;			/* リロードバー */
	std::unique_ptr<EnemyDetectionIcon> detectionIcon_;	/* 発見アイコン */
	std::unique_ptr<EnemyReloadIcon> reloadIcon_;		/* リロードアイコン */
};

