#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Collider/Collider.h>
#include <Collider/CollisionManager.h>
#include <Input/Input.h>
#include <Object3D.h>
#include <Sprite.h>
#include <SpriteCommon.h>
#include <Util/ParameterSystem.h>
#include <Animation/AnimatedModelInstance.h>

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Bullet/PlayerBullet/PlayerBullet.h>
#include <src/Game/Loader/Loader.h>
#include <src/Game/Player/UI/PlayerUIManager.h>

// =========================================================
// プレイヤークラス
// =========================================================
class Player : public ICollisionCallback, public IConfigurable {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// プレイヤーの初期化処理を行います。
	/// </summary>
	/// <param name="data">初期位置や回転などのTransformデータ</param>
	void Initialize(const Loader::TransformData& data);

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	/// <param name="operable">操作可能フラグ</param>
	void Update(bool operable);

	/// <summary>
	/// モデルの描画処理を行います。
	/// </summary>
	void Draw();

	/// <summary>
	/// シャドウマップ用の描画処理を行います。
	/// </summary>
	void DrawShadow();

	/// <summary>
	/// UIの描画処理を行います。
	/// </summary>
	void DrawUI();

	/// <summary>
	/// デバッグ用の描画処理を行います。
	/// </summary>
	void Debug();

	/// <summary>
	/// 衝突時のコールバック処理を行います。
	/// </summary>
	/// <param name="other">衝突した相手のコライダー</param>
	void OnCollision(Collider* other) override;

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// プレイヤーの現在位置を取得します。
	/// </summary>
	/// <returns>現在の位置（Float3）</returns>
	const Float3& GetTranslate() const { return objectPlayer_->GetTranslate(); }

	/// <summary>
	/// プレイヤーの位置を設定します。
	/// </summary>
	/// <param name="translate">設定する位置（Float3）</param>
	void SetTranslate(const Float3& translate) { objectPlayer_->GetTranslate() = translate; }

	/// <summary>
	/// プレイヤーの現在HPを取得します。
	/// </summary>
	/// <returns>現在HP</returns>
	int32_t GetCurrentHP() const { return currentHP_; }

	/// <summary>
	/// プレイヤーの最大HPを取得します。
	/// </summary>
	/// <returns>最大HP</returns>
	int32_t GetMaxHP() const { return kMaxHP; }

	/// <summary>
	/// オーバーヒート率を取得します。
	/// </summary>
	/// <returns>オーバーヒート率（0.0f～1.0f）</returns>
	float GetOverheatRatio() const { return overheatTime_ / kOverheatLimit; }

	/// <summary>
	/// 死亡フラグを取得します。
	/// </summary>
	/// <returns>死亡フラグ</returns>
	bool IsDead() const { return isDead_; }

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// カーソル方向へプレイヤーを回転させます。
	/// </summary>
	void FaceCursor();

	/// <summary>
	/// 移動およびダッシュ入力の処理を行います。
	/// </summary>
	void HandleMove();

	/// <summary>
	/// 弾の入力発射処理を行います。
	/// </summary>
	void HandleShooting();

	/// <summary>
	/// オーバーヒートの管理処理を行います。
	/// </summary>
	void HandleOverHeat();

	/// <summary>
	/// コライダーの更新処理を行います。
	/// </summary>
	void UpdateCollider();

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// ----- System -----
	Input* input_ = nullptr;								/* 入力管理 */
	std::unique_ptr<SpriteCommon> spriteCommon_;			/* スプライト共通処理 */

	// ----- Object -----
	std::unique_ptr<AnimatedModelInstance> objectPlayer_;	/* プレイヤーオブジェクト */

	// ----- Animation -----
	AnimatedModelInstance::AnimatedModelData walkData_;		/* アニメーションデータ */

	// ----- Collision -----
	std::unique_ptr<Collider> collider_;					/* コライダー */
	const Float3 kColliderSize = { 1.0f, 2.0f, 1.0f };		/* コライダーサイズ */

	// ----- UI -----
	std::unique_ptr<PlayerUIManager> ui_;					/* UIマネージャー */

	// ----- Parameters -----
	Float3 velocity_ = { 0.0f, 0.0f, 0.0f };				/* 速度ベクトル */
	float speed_ = 0.25f;									/* 移動速度 */
	bool isMoving_ = false;									/* 移動中フラグ */

	const int32_t kMaxHP = 100;								/* 最大HP */
	int32_t currentHP_ = 0;									/* 現在HP */

	bool isDead_ = false;									/* 死亡フラグ */

	// ----- Dash -----
	bool isDashing_ = false;								/* ダッシュ中フラグ */
	const float kDashDuration = 0.2f;						/* ダッシュ継続時間 */
	float dashTimer_ = 0.0f;								/* ダッシュタイマー */

	const float kDashCoolDown = 1.5f;						/* ダッシュのクールタイム時間 */
	float dashCooldownTimer_ = 0.0f;						/* ダッシュのクールダウンタイマー */

	const float kDashSpeedMultiplier = 3.0f;				/* ダッシュ速度倍率 */

	// ----- Shooting -----
	bool isFiring_ = false;									/* 射撃中フラグ */
	float fireCooldown_ = 0.15f;							/* 連射速度 */
	float fireTimer_ = 0.0f;								/* 射撃タイマー */
	const float kMaxRandomAngle = 0.02f;					/* 射撃ブレ角 */

	// ----- Overheat -----
	float overheatTime_ = 0.0f;								/* オーバーヒートタイマー */
	const float kOverheatLimit = 3.0f;						/* オーバーヒートになる秒数 */
	float overheatGainPerSecond_ = 1.0f;					/* オーバーヒート加熱速度（1秒あたり） */
	float overheatRecoverySpeed_ = 1.6f;					/* オーバーヒート冷却速度（1秒あたり） */
	bool isOverheated_ = false;								/* オーバーヒート中フラグ */
};