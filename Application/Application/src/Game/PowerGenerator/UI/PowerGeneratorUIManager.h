#pragma once

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/PowerGenerator/UI/Parts/PowerGeneratorHPBar.h>

/// <summary>
/// 発電機のUI表示に使用する情報
/// </summary>
struct PowerGeneratorUIState {
	Cygnus::Float3 worldPos;
	float hpRatio;
};

// =========================================================
// 発電機のUI管理クラス
// =========================================================
class PowerGeneratorUIManager
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
	void Update(const PowerGeneratorUIState& state);

	/// <summary>
	/// 描画処理を行います。
	/// </summary>
	void Draw();

private:
	// =========================================================
	// Member Variables
	// =========================================================
	std::unique_ptr<Cygnus::SpriteCommon> spriteCommon_;	/* スプライト共通処理 */

	std::unique_ptr<PowerGeneratorHPBar> hpBar_;  /* HPバー */
};

