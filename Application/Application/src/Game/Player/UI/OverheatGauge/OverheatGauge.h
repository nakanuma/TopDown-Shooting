#pragma once

// Engine
#include <Sprite.h>
#include <SpriteCommon.h>

class Player;

/// <summary>
/// オーバーヒートの円ゲージ
/// </summary>
class OverheatGauge
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
	std::unique_ptr<Sprite> sprite_;
};

