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
class ResultScene : public Cygnus::BaseScene {
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
	// Constants
	// =========================================================
	static constexpr Cygnus::Float3 kInitialCameraPosition = { 0.0f, 30.0f, -50.0f };	/* 初期カメラ位置 */
	static constexpr Cygnus::Float3 kInitialCameraRotation = { 0.5f, 0.0f, 0.0f };		/* 初期カメラ回転角 */
	static constexpr float kCameraFovY = 0.45f;											/* カメラの視野角 */

	static constexpr Cygnus::Float4 kBackgroundColor = { 0.5f, 0.5f, 0.5f, 1.0f };	/* 背景の色 */
	static constexpr Cygnus::Float2 kBackgroundSize = { 1280.0f, 720.0f };			/* 背景サイズ */
	static constexpr Cygnus::Float2 kTitleButtonPosition = { 640.0f, 620.0f };		/* タイトルボタン位置 */
	static constexpr Cygnus::Float2 kRecordPosition = { 640.0f, 260.0f };			/* 戦績位置 */
	static constexpr Cygnus::Float2 kAnchorPoint = { 0.5f, 0.5f };					/* アンカーポイント（中心） */
	static constexpr uint32_t kDecimalPlaces = 2;									/* 戦績表示での桁数 */
	static constexpr Cygnus::Float2 kTotalDamatePosition = { 640.0f, 130.0f };		/* 合計ダメージの位置 */
	static constexpr Cygnus::Float2 kDefeatedPosition = { 640.0f, 260.0f };			/* 撃破数の位置 */
	static constexpr Cygnus::Float2 kHitRatePosition = { 650.0f, 390.0f };			/* 命中率の位置 */
	static constexpr Cygnus::Float2 kClearTimePosition = { 650.0f, 520.0f };		/* クリア時間の位置 */

	static constexpr float kFadeInDuration = 1.0f;	/* フェードインの演出時間 */
	static constexpr float kFadeOutDuration = 1.0f;	/* フェードアウトの演出時間 */
	static constexpr float kFadeOutDelay = 0.2f;	/* フェードアウトの遅延時間 */

	// =========================================================
	// Member Variables
	// =========================================================

	// ----- System -----
	std::unique_ptr<Cygnus::Camera> camera_ = nullptr;						/* 3Dカメラクラス */
	std::unique_ptr<Cygnus::SpriteCommon> spriteCommon_ = nullptr;			/* スプライト共通処理クラス */
	std::unique_ptr<Cygnus::SoundManager> soundManager_ = nullptr;			/* サウンド管理クラス */
	Cygnus::Input* input_ = nullptr;										/* 入力管理クラス */
	Cygnus::LightManager* lightManager_ = nullptr;							/* 各ライト管理クラス */

	// ----- Sprite -----
	/// Todo : 適切なクラスへの整理
	std::unique_ptr<Cygnus::Sprite> spriteBackGround_;				/* 背景スプライト */
	std::unique_ptr<Cygnus::Sprite> spriteTitleButton_;				/* タイトルボタンスプライト */
	std::unique_ptr<Cygnus::Sprite> spriteRecord_;					/* 戦績スプライト */

	std::unique_ptr<NumberSprite> spriteTotalDamage_;		/* 合計ダメージ数字スプライト */
	std::unique_ptr<NumberSprite> spriteDefeated_;			/* 撃破数数字スプライト */
	std::unique_ptr<NumberSprite> spriteHitRate_;			/* 命中率数字スプライト */
	std::unique_ptr<NumberSprite> spriteClearTime_;			/* クリアタイム数字スプライト */

	// ----- Others -----
	int32_t shadowMapHandle_;								/* シャドウマップテクスチャ */
};
