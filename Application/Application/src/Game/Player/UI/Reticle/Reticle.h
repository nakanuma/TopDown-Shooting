#pragma once

// Engine
#include <Sprite.h>
#include <SpriteCommon.h>

/// <summary>
/// レティクル
/// </summary>
class Reticle {
public:
	/// <summary>
	/// 更新処理
	/// </summary>
	void Initialize(DirectXBase* dxBase, SpriteCommon* spriteCommon);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:
	// クロスヘア（十字線）
	std::unique_ptr<Sprite> spriteCrosshair_;
};
