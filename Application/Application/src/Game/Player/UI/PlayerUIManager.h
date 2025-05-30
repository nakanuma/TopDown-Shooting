#pragma once

// Engine
#include <SpriteCommon.h>
#include <Sprite.h>

// Application
#include <src/Game/Player/UI/Reticle/Reticle.h>
#include <src/Game/Player/UI/HPBar/HPBar.h>
#include <src/Game/Player/UI/RemainingBullets/RemainingBullets.h>

class Player;

/// <summary>
/// PlayerUI管理クラス
/// </summary>
class PlayerUIManager 
{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(const Player* player);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:
	// ---------------------------------------------------------
	// システム関連
	// ---------------------------------------------------------

	// スプライト基盤
	std::unique_ptr<SpriteCommon> spriteCommon_;

	// ---------------------------------------------------------
	// UI
	// ---------------------------------------------------------

	// レティクル
	std::unique_ptr<Reticle> reticle_;
	// HPバー
	std::unique_ptr<HPBar> hpBar_;
	// 残弾表示
	std::unique_ptr<RemainingBullets> remainingBullets_;
};
