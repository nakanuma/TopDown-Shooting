#pragma once

// Engine
#include <SpriteCommon.h>
#include <Sprite.h>

class Player;

/// <summary>
/// 残弾表示
/// </summary>
class RemainingBullets 
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
	// 残弾表示の最大サイズ
	const Float2 kRemainingBulletsSize = {300.0f, 60.0f};
	// 残弾表示の位置
	const Float2 kRemainingBulletsPosition = {
	    Window::GetWidth() / 2.0f - kRemainingBulletsSize.x / 2.0f, // 画面の中央 - サイズ半分で中央揃え
	    (Window::GetHeight() / 8.0f) * 6.0f                         // 画面縦サイズの 6/8 の位置へ設定
	};

	// 残弾表示（後景）
	std::unique_ptr<Sprite> spriteRemainingBulletsBackground_;
	// 残弾表示（前景）
	std::unique_ptr<Sprite> spriteRemainingBulletsForeground_;
	// 残弾表示（アイコン）
	std::unique_ptr<Sprite> spriteRemainingBulletsIcon_;
};
