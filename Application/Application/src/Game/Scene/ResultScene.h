#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <BaseScene.h>
#include <Camera.h>
#include <Input.h>
#include <LightManager.h>
#include <ModelManager.h>
#include <Object3D.h>
#include <SoundManager.h>
#include <Sprite.h>
#include <SpriteCommon.h>
#include <TextureManager.h>
#include <Engine/Collider/CollisionManager.h>
#include <Engine/Texture/PostEffectManager.h>
#include <Engine/Util/TimeManager.h>

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Utility/NumberSprite.h>

// =========================================================
// リザルトシーンシーンクラス
// =========================================================
class ResultScene : public BaseScene {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// リザルトシーンの初期化処理を行います。
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// リザルトシーンの終了処理を行います。
	/// </summary>
	void Finalize() override;

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	void Update() override;

	/// <summary>
	/// シーンの描画処理を行います。
	/// </summary>
	void Draw() override;

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// ----- System -----
	std::unique_ptr<Camera> camera = nullptr;				/* 3Dカメラクラス */
	std::unique_ptr<SpriteCommon> spriteCommon = nullptr;	/* スプライト共通処理クラス */
	std::unique_ptr<SoundManager> soundManager = nullptr;	/* サウンド管理クラス */
	Input* input = nullptr;									/* 入力管理クラス */
	LightManager* lightManager = nullptr;					/* 各ライト管理クラス */

	// ----- Sprite -----
	/// Todo : 適切なクラスへの整理
	std::unique_ptr<Sprite> spriteBackGround_;				/* 背景スプライト */
	std::unique_ptr<Sprite> spriteTitleButton_;				/* タイトルボタンスプライト */
	std::unique_ptr<Sprite> spriteRecord_;					/* 戦績スプライト */

	std::unique_ptr<NumberSprite> spriteTotalDamage_;		/* 合計ダメージ数字スプライト */
	std::unique_ptr<NumberSprite> spriteDefeated_;			/* 撃破数数字スプライト */
	std::unique_ptr<NumberSprite> spriteHitRate_;			/* 命中率数字スプライト */
	std::unique_ptr<NumberSprite> spriteClearTime_;			/* クリアタイム数字スプライト */

	// ----- Others -----
	int32_t shadowMapHandle_;								/* シャドウマップテクスチャ */
};
