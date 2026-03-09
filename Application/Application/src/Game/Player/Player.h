#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Animation/AnimatedModelInstance.h>
#include <Collider/Collider.h>
#include <Collider/CollisionManager.h>
#include <Input/Input.h>
#include <Object3D.h>
#include <PostEffectManager.h>
#include <Sprite.h>
#include <SpriteCommon.h>
#include <Util/ParameterSystem.h>

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Bullet/PlayerBullet/PlayerBullet.h>
#include <src/Game/Loader/Loader.h>
#include <src/Game/Player/UI/PlayerUIManager.h>

// =========================================================
// プレイヤークラス
// =========================================================
class Player : public Cygnus::ICollisionCallback, public Cygnus::Configurator {
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
	/// 通常モデルのシャドウマップ描画処理を行います。
	/// </summary>
	void DrawShadow();

	/// <summary>
	/// スキニングモデルのシャドウマップ描画処理を行います。
	/// </summary>
	void DrawShadowSkinning();

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
	void OnCollision(Cygnus::Collider* other) override;

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// プレイヤーの現在位置を取得します。
	/// </summary>
	/// <returns>現在の位置（Float3）</returns>
	const Cygnus::Float3& GetTranslate() const { return objectPlayer_->GetTranslate(); }

	/// <summary>
	/// プレイヤーの位置を設定します。
	/// </summary>
	/// <param name="translate">設定する位置（Float3）</param>
	void SetTranslate(const Cygnus::Float3& translate) { objectPlayer_->GetTranslate() = translate; }

	/// <summary>
	/// プレイヤーの回転を取得します。
	/// </summary>
	/// <returns>回転（Float3）</returns>
	const Cygnus::Float3& GetRotate() const { return objectPlayer_->GetRotate(); }

	/// <summary>
	/// プレイヤーの現在HPを取得します。
	/// </summary>
	/// <returns>現在HP</returns>
	int32_t GetCurrentHP() const { return currentHP_; }

	/// <summary>
	/// プレイヤーの最大HPを取得します。
	/// </summary>
	/// <returns>最大HP</returns>
	int32_t GetMaxHP() const { return maxHP_; }

	/// <summary>
	/// オーバーヒート率を取得します。
	/// </summary>
	/// <returns>オーバーヒート率（0.0f～1.0f）</returns>
	float GetOverheatRatio() const { return overheatTime_ / overheatLimit_; }

	/// <summary>
	/// オーバーヒート中状態を取得します。
	/// </summary>
	/// <returns></returns>
	bool IsOverHeated() const { return isOverheated_; }

	/// <summary>
	/// ダッシュ中状態を取得します。
	/// </summary>
	/// <returns></returns>
	bool IsDashing() const { return isDashing_; }

	/// <summary>
	/// ダッシュクールダウンの割合を取得します。
	/// </summary>
	/// <returns>クールダウン残り時間割合（0.0f～1.0f）</returns>
	float GetDashCooldownRatio() const { return dashCooldownTimer_ / dashCoolDown_; }

	/// <summary>
	/// 死亡フラグを取得します。
	/// </summary>
	/// <returns>死亡フラグ</returns>
	bool IsDead() const { return isDead_; }

	/// <summary>
	/// 無敵状態を設定します。
	/// </summary>
	/// <param name="flag">無敵状態</param>
	void SetInvincible(bool flag) { invincible_ = flag; }

	/// <summary>
	/// このフレームで射撃したかどうかを取得します。
	/// </summary>
	/// <returns>このフレームでの射撃フラグ</returns>
	bool IsShootedThisFrame() const { return isShootedThisFrame_; }

	/// <summary>
	/// ポストエフェクトマネージャーをセットします。
	/// </summary>
	/// <param name="manager">ポストエフェクトマネージャー</param>
	void SetPostEffectManager(Cygnus::PostEffectManager* manager) { postEffectManager_ = manager; }

	/// <summary>
	/// 速度ベクトルを設定します。
	/// </summary>
	/// <param name="velocity"></param>
	void SetVelovity(const Cygnus::Float3& velocity) { velocity_ = velocity; }

	/// <summary>
	/// 回転を設定します。
	/// </summary>
	/// <param name="rotate"></param>
	void SetRotate(const Cygnus::Float3& rotate) { objectPlayer_->GetRotate() = rotate; }

	/// <summary>
	/// 移動速度を取得します
	/// </summary>
	/// <returns></returns>
	float GetMoveSpeed() const { return moveSpeed_; }

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
	/// 被弾時の発光処理を行います。
	/// </summary>
	void HandleHitBlink();

	/// <summary>
	/// 被弾時のダメージ演出処理を行います。
	/// </summary>
	void HandleDamageEffect();

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float3 kColliderSize = {1.0f, 2.0f, 1.0f}; /* コライダーサイズ */

	static constexpr float kAnimationPlaybackSpeed = 1.5f;              /* アニメーション再生速度 */

	static constexpr Cygnus::Float4 kGunColor = {0.0f, 0.0f, 0.0f, 1.0f};	/* 銃の色（黒） */
	static constexpr float kGunEnvironmentStrength = 0.2f;					/* 銃の環境マップ強度 */
	static constexpr float kGunForwardOffset = 1.1f;						/* 銃の前方位置オフセット */
	static constexpr float kGunRightOffset = 0.3f;							/* 銃の右方向位置オフセット */
	static constexpr Cygnus::Float3 kGunEmissiveColor = {1.0f, 0.0f, 0.0f}; /* 銃のオーバーヒート時発光色（赤） */
	static constexpr float kGunIntensityFactor = 3.0f;                      /* 銃のオーバーヒート時の発光強度係数（倍率） */
	static constexpr float kOverheatSmokeInterval = 0.12f;                  /* オーバーヒート時煙パーティクルの頻度 */

	static constexpr float kVelocityNormalizeAdditive = 1.0f; /* 速度正規化用の加算値 */
	static constexpr float kVelocityThreshold = 0.01f;        /* 速度判定のしきい値 */

	static constexpr int32_t kShellEjectionCount = 1;        /* 薬莢排出パーティクル発生数 */
	static constexpr int32_t kBloodSplatterCount = 30;       /* 血飛沫パーティクル発生数 */
	static constexpr int32_t kMuzzleFlashCount = 6;          /* マズルフラッシュパーティクル発生数 */
	static constexpr float kMuzzleFlashForwardOffset = 1.4f; /* マズルフラッシュ発生位置の前方オフセット */

	static constexpr int32_t kDeathCrossCount = 3;     /* 死亡時クロスパーティクルの発生数 */
	static constexpr float kDeathCrossAngle1 = 45.0f;  /* 死亡時クロスパーティクル1の角度（度） */
	static constexpr float kDeathCrossAngle2 = 135.0f; /* 死亡時クロスパーティクル2の角度（度） */

	static constexpr float kHitBlinkDuration = 0.05f;                    /* 被弾時の発光時間 */
	static constexpr Cygnus::Float3 kHitBlinkColor = {1.0f, 0.5f, 0.0f}; /* 被弾時の発光色 */

	static constexpr float kDamageEffectDurationIn = 0.1f;         /* ダメージ演出増加の時間 */
	static constexpr float kDamageEffectDurationHold = 0.2f;       /* ダメージ演出維持の時間 */
	static constexpr float kDamageEffectDurationOut = 0.8f;        /* ダメージ演出減少の時間 */
	static constexpr float kDamageEffectIntensityThreshold = 0.5f; /* ダメージ演出開始のための閾値 */

	// =========================================================
	// Member Variables
	// =========================================================

	// ----- System -----
	Cygnus::Input* input_ = nullptr;                     /* 入力管理 */
	std::unique_ptr<Cygnus::SpriteCommon> spriteCommon_; /* スプライト共通処理 */
	Cygnus::PostEffectManager* postEffectManager_;       /* ポストエフェクトマネージャーへの参照 */

	// ----- Object -----
	std::unique_ptr<Cygnus::AnimatedModelInstance> objectPlayer_; /* プレイヤーオブジェクト */
	std::unique_ptr<Cygnus::Object3D> objectGun_;                 /* 銃オブジェクト */

	// ----- Animation -----
	Cygnus::AnimatedModelInstance::AnimatedModelData walkData_; /* アニメーションデータ */

	// ----- Collision -----
	std::unique_ptr<Cygnus::Collider> collider_; /* コライダー */

	// ----- UI -----
	std::unique_ptr<PlayerUIManager> ui_; /* UIマネージャー */

	// ----- Parameters -----
	Cygnus::Float3 velocity_ = {0.0f, 0.0f, 0.0f}; /* 速度ベクトル */
	bool isMoving_ = false;                        /* 移動中フラグ */
	int32_t currentHP_ = 0;                        /* 現在HP */

	bool invincible_ = false; /* 無敵フラグ（ボス撃破時に有効化） */
	bool isDead_ = false;     /* 死亡フラグ */

	// ----- Dash -----
	bool isDashing_ = false;         /* ダッシュ中フラグ */
	float dashTimer_ = 0.0f;         /* ダッシュタイマー */
	float dashCooldownTimer_ = 0.0f; /* ダッシュのクールダウンタイマー */

	// ----- Shooting -----
	bool isFiring_ = false;				/* 射撃中フラグ */
	bool isShootedThisFrame_ = false;	/* このフレームで弾を発射したか */
	float fireTimer_ = 0.0f;			/* 射撃タイマー */

	// ----- Overheat -----
	float overheatTime_ = 0.0f; /* オーバーヒートタイマー */
	bool isOverheated_ = false; /* オーバーヒート中フラグ */

	float gunEmissiveIntensity_ = 0.0f; /* 銃の発光強度（オーバーヒート時） */
	float overheatSmokeTimer_ = 0.0f;   /* オーバーヒート時の煙パーティクル用タイマー */

	// ----- HitBlink -----
	/// <summary>
	/// 被弾時の発光演出フェーズ
	/// </summary>
	enum class HitBlinkPhase {
		Wait,    // 待機
		BlinkIn, // 発光
		BlinkOut // 減光
	} hitBlinkPhase_ = HitBlinkPhase::Wait;
	bool isHitBlink_ = false;    /* 被弾時の発光演出中フラグ */
	float hitBlinkTimer_ = 0.0f; /* 被弾時の発光演出タイマー */

	// ----- DamageEffect -----
	enum class DamageEffectPhase {
		Wait, // 待機
		In,   // 増加
		Hold, // 維持
		Out   // 減少
	} damageEffectPhase_ = DamageEffectPhase::Wait;
	bool isReceiveDamage_ = false;   /* 被ダメージフラグ */
	float damageEffectTimer_ = 0.0f; /* ダメージ演出タイマー */

	// ----- Config -----
	int32_t maxHP_ = 100; /* 最大HP */

	float moveSpeed_ = 0.25f;          /* 移動速度 */
	float dashDuration_ = 0.2f;        /* ダッシュ継続時間 */
	float dashCoolDown_ = 1.5f;        /* ダッシュのクールタイム時間 */
	float dashSpeedMultiplier_ = 3.0f; /* ダッシュ速度倍率 */

	float fireCooldown_ = 0.15f;          /* 連射速度 */
	float overheatLimit_ = 3.0f;          /* オーバーヒートになるまでの秒数 */
	float overheatGainPerSecond_ = 1.0f;  /* オーバーヒート加熱速度（1秒あたり） */
	float overheatRecoverySpeed_ = 1.6f;  /* オーバーヒート冷却速度（1秒あたり） */

	float overheatRecoveryThreshold_ = 3.0f; /* オーバーヒートした際、冷却が開始するまでの時間（秒） */

	float recoveryDelayTimer_ = 0.0f;		/* 現在の待機カウント */
	float recoveryDelayThreshould_ = 0.5f;	/* 射撃をやめてから冷却が始まるまでの時間（秒） */

	float coolingAccelerationTimer_ = 0.0f; /* 冷却が始まってからの経過時間 */
	float coolingAccelerationRate_ = 1.5f;  /* 冷却時間の加速倍率*/

	float maxRandomAngle_ = 0.02f;        /* 射撃ブレ角 */
	float shootingBlurMultiplier_ = 3.0f; /* 移動時の射撃ブレ倍率 */
};