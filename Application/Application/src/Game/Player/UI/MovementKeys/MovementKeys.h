#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Sprite.h>
#include <SpriteCommon.h>

// =========================================================
// 移動キーUIクラス
// =========================================================
class MovementKeys {
private:
	/// <summary>
	/// 各キーの情報をまとめる構造体
	/// </summary>
	struct KeyUI {
		std::unique_ptr<Cygnus::Sprite> sprite; /* スプライト */
		Cygnus::Float2 initPos;                 /* 初期位置 */
		Cygnus::Float2 offsetDir;               /* 動く方向（Wなら{0,-1}） */
		float animationT = 0.0f;                /* アニメーションタイマー */
	};

public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// UIの初期化処理を行います。
	/// </summary>
	/// <param name="dxBase">DirectX基盤クラス</param>
	/// <param name="spriteCommon">スプライト共通クラス</param>
	void Initialize(Cygnus::SpriteCommon* spriteCommon);

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// スプライトの描画処理を行います。
	/// </summary>
	void Draw();

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// キーの初期設定を行います。
	/// </summary>
	/// <param name="spriteCommon">スプライト基盤機能</param>
	/// <param name="key">キーUI構造体</param>
	/// <param name="textureHandle">テクスチャ</param>
	/// <param name="initPos">初期位置</param>
	/// <param name="dir">方向</param>
	void SetUpKey(Cygnus::SpriteCommon* spriteCommon, KeyUI& key, uint32_t textureHandle, Cygnus::Float2 initPos, Cygnus::Float2 dir);

	/// <summary>
	/// キーのアニメーション更新を行います。
	/// </summary>
	/// <param name="key">キーUI構造体</param>
	/// <param name="isPressed">入力判定</param>
	void UpdateAnimation(KeyUI& key, bool isPressed);

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr float kAnimSpeed = 10.0f;		/* アニメーション速度 */
	static constexpr float kMoveDistance = 10.0f;	/* 動く距離（ピクセル） */

	static constexpr Cygnus::Float2 kWKeyInitPos = {245.0f, 550.0f};		/* Wキー初期位置 */
	static constexpr Cygnus::Float2 kAKeyInitPos = {165.0f, 630.0f};		/* Aキー初期位置 */
	static constexpr Cygnus::Float2 kSKeyInitPos = {245.0f, 630.0f};		/* Sキー初期位置 */
	static constexpr Cygnus::Float2 kDKeyInitPos = {325.0f, 630.0f};		/* Dキー初期位置 */
	static constexpr Cygnus::Float2 kMoveTextInitPos = {245.0f, 690.0f};	/* MOVEの文字初期位置 */

	static constexpr Cygnus::Float4 kDefaultColor = {1.0f, 1.0f, 1.0f, 1.0f};	/* 通常色 */
	static constexpr Cygnus::Float4 kPushedColor = {1.0f, 0.25f, 0.0f, 1.0f};	/* 押されている場合の色 */

	// =========================================================
	// Member Variables
	// =========================================================
	KeyUI keyW_; /* Wキー */
	KeyUI keyA_; /* Aキー */
	KeyUI keyS_; /* Sキー */
	KeyUI keyD_; /* Dキー */

	std::unique_ptr<Cygnus::Sprite> spriteMoveText_; /* MOVEの文字 */

	float totalTime_; /* 累計時間 */
};
