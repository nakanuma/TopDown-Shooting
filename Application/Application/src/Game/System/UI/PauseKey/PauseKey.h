#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Sprite.h>
#include <SpriteCommon.h>

// =========================================================
// ポーズキーUIクラス
// =========================================================
class PauseKey
{
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	/// <param name="spriteCommon"></param>
	void Initialize(Cygnus::SpriteCommon* spriteCommon);

	/// <summary>
	/// 更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理を行います。
	/// </summary>
	void Draw();

private:
	// =========================================================
	// Constants
	// =========================================================

	static constexpr Cygnus::Float2 kEscKeyInitPos = { 50.0f, 50.0f };
	static constexpr Cygnus::Float2 kPauseTextInitPos = { 50.0f, 95.0f };

	// =========================================================
	// Member Variables
	// =========================================================

	// ----- Sprite -----
	std::unique_ptr<Cygnus::Sprite> spriteEscapeKey_;	/* ESCキー */
	std::unique_ptr<Cygnus::Sprite> spritePauseText_;	/* PAUSEの文字 */
};

