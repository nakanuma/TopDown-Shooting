#pragma once

// Engine
#include <SpriteCommon.h>
#include <Sprite.h>

class Player;

/// <summary>
/// HPバー
/// </summary>
class HPBar 
{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(DirectXBase* dxBase, SpriteCommon* spriteCommon);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(const Player* player);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:
	// HPバーの最大サイズ
	const Float2 kHPBarSize = {300.0f, 30.0f};
	// HPバーの位置
	const Float2 kHPBarPosition = {
	    Window::GetWidth() / 2.0f - kHPBarSize.x / 2.0f, // 画面の中央 - サイズ半分で中央揃え
	    (Window::GetHeight() / 8.0f) * 7.0f              // 画面縦サイズの 7/8 の位置へ設定
	};

	// HPバー（後景）
	std::unique_ptr<Sprite> spriteHPBackground_;
	// HPバー（前景）
	std::unique_ptr<Sprite> spriteHPForeground_;
};
