#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Sprite.h>
#include <SpriteCommon.h>

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Player/UI/HPBar/HPBar.h>
#include <src/Game/Player/UI/OverheatGauge/OverheatGauge.h>
#include <src/Game/Player/UI/Reticle/Reticle.h>

// ---------------------------------------------------------
// Foward Declaration
// ---------------------------------------------------------
class Player;

// =========================================================
// プレイヤーの全てのUI管理を行うクラス
// =========================================================
class PlayerUIManager {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 全てのUIの初期化処理を行います。
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	void Update(const Player* player);

	/// <summary>
	/// スプライトの描画処理を行います。
	/// </summary>
	void Draw();

private:
	// =========================================================
	// Member Variables
	// =========================================================
	std::unique_ptr<SpriteCommon> spriteCommon_;			/* スプライト共通処理 */

	std::unique_ptr<Reticle> reticle_;						/* レティクル */
	std::unique_ptr<HPBar> hpBar_;							/* HPバー */
	std::unique_ptr<OverheatGauge> overheatGauge_;			/* オーバーヒートゲージ */
};
