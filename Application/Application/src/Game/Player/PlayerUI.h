#pragma once

// Engine
#include <SpriteCommon.h>
#include <Sprite.h>

class Player;

/// <summary>
/// PlayerUI管理クラス
/// </summary>
class PlayerUI 
{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw(const Player* player);

private:
	// ---------------------------------------------------------
	// システム関連
	// ---------------------------------------------------------

	// スプライト基盤
	std::unique_ptr<SpriteCommon> spriteCommon_;

	// ---------------------------------------------------------
	// スプライト関連
	// ---------------------------------------------------------

	/* レティクル */

	// クロスヘア（十字線）
	std::unique_ptr<Sprite> spriteCrosshair_;

	/* HPバー */

	// HPバーの最大サイズ
	const Float2 kHPBarSize = {300.0f, 30.0f};

	// HPバー（後景）
	std::unique_ptr<Sprite> spriteHPBackground_;
	// HPバー（前景）
	std::unique_ptr<Sprite> spriteHPForeground_;
};
