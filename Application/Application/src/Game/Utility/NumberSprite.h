#pragma once

// Engine
#include <Sprite.h>
#include <SpriteCommon.h>

/// <summary>
/// 数字をスプライトで表示
/// </summary>
class NumberSprite
{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(float value, uint32_t decimalPlaces = 0);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(Float2 position);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:
	// スプライト基盤
	std::unique_ptr<SpriteCommon> spriteCommon_;

	// 1文字のサイズ
	Float2 digitSize_{ 32.0f, 32.0f };
	// 桁数
	uint32_t digitCount_ = 0;
	// 各桁の数字
	std::vector<uint32_t> digitValues_;
	// スプライト
	std::vector<std::unique_ptr<Sprite>> digits_;
};

