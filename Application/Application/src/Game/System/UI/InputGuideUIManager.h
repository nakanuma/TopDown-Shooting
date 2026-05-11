#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Sprite.h>
#include <SpriteCommon.h>

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/System/UI/Mouse/Mouse.h>
#include <src/Game/System/UI/MovementKeys/MovementKeys.h>
#include <src/Game/System/UI/PauseKey/PauseKey.h>

// ---------------------------------------------------------
// Forward Declaration
// ---------------------------------------------------------
class Player;

// =========================================================
// 操作キーのUI管理を行うクラス
// =========================================================
class InputGuideUIManager {
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
	std::unique_ptr<Cygnus::SpriteCommon> spriteCommon_; /* スプライト共通処理 */

	std::unique_ptr<MovementKeys> movementKeys_;   /* 移動キー */
	std::unique_ptr<Mouse> mouse_;                 /* マウス */
	std::unique_ptr<PauseKey> pauseKey_;           /* ポーズキー */
};