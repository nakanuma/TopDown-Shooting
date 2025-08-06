#pragma once

// Engine
#include <Collider/Collider.h>
#include <Collider/CollisionManager.h>
#include <Input/Input.h>
#include <Object3D.h>
#include <Sprite.h>
#include <SpriteCommon.h>
#include <Util/ParameterSystem.h>

// Application
#include <src/Game/Bullet/PlayerBullet/PlayerBullet.h>
#include <src/Game/Loader/Loader.h>
#include <src/Game/Player/UI/PlayerUIManager.h>

/// <summary>
/// プレイヤー
/// </summary>
class Player : public ICollisionCallback, public IConfigurable {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(const Loader::TransformData& data);

	/// <summary>
	/// 毎フレーム更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// UI描画処理
	/// </summary>
	void DrawUI();

	/// <summary>
	/// デバッグ表示
	/// </summary>
	void Debug();

	/// <summary>
	/// 衝突時コールバック
	/// </summary>
	void OnCollision(Collider* other) override;

	/// <summary>
	/// 位置の取得
	/// </summary>
	Float3& GetTranslate() const { return objectPlayer_->transform_.translate; }

	/// <summary>
	/// 現在HPの取得
	/// </summary>
	int32_t GetCurrentHP() const { return currentHP_; }

	/// <summary>
	/// 最大HPの取得
	/// </summary>
	int32_t GetMaxHP() const { return kMaxHP; }

	/// <summary>
	/// オーバーヒート率の取得（円ゲージのUIに使用）
	/// </summary>
	float GetOverheatRatio() const { return overheatTime_ / kOverheatLimit; }

private:
	// ---------------------------------------------------------
	// 内部処理
	// ---------------------------------------------------------

	/// <summary>
	/// カーソル方向へ向くよう回転させる
	/// </summary>
	void FaceCursor();

	/// <summary>
	/// 移動処理 & ダッシュ入力
	/// </summary>
	void HandleMove();

	/// <summary>
	/// 弾の発射処理
	/// </summary>
	void HandleShooting();

	/// <summary>
	/// オーバーヒートの管理処理
	/// </summary>
	void HandleOverHeat();

	/// <summary>
	/// コライダー更新処理
	/// </summary>
	void UpdateCollider();

private:
	// ---------------------------------------------------------
	// システム関連
	// ---------------------------------------------------------

	// 入力
	Input* input_ = nullptr;

	// スプライト基盤
	std::unique_ptr<SpriteCommon> spriteCommon_;

	// ---------------------------------------------------------
	// オブジェクト関連
	// ---------------------------------------------------------

	// プレイヤーモデル
	ModelManager::ModelData modelPlayer_;
	// プレイヤーオブジェクト
	std::unique_ptr<Object3D> objectPlayer_;

	// プレイヤー弾モデル
	ModelManager::ModelData modelBullet_;

	// ---------------------------------------------------------
	// コライダー
	// ---------------------------------------------------------

	std::unique_ptr<Collider> collider_;

	// コライダーのサイズ
	const Float3 kColliderSize = { 1.0f, 2.0f, 1.0f };

	// ---------------------------------------------------------
	// UI
	// ---------------------------------------------------------

	std::unique_ptr<PlayerUIManager> ui_;

	// ---------------------------------------------------------
	// パラメーター
	// ---------------------------------------------------------

	// 速度
	Float3 velocity_ = {0.0f, 0.0f, 0.0f};
	// 速さ
	float speed_ = 0.25f;

	// 最大HP
	const int32_t kMaxHP = 100;
	// 現在HP
	int32_t currentHP_;

	// ---------------------------------------------------------
	// ダッシュ関連
	// ---------------------------------------------------------

	// ダッシュ中か
	bool isDashing_ = false;
	// ダッシュ継続時間
	const float kDashDuration = 0.2f;
	// ダッシュ中タイマー
	float dashTimer_ = 0.0f;

	// ダッシュのクールタイム時間
	const float kDashCoolDown = 1.5f;
	// ダッシュのクールタイムのタイマー
	float dashCooldownTimer_ = 0.0f;

	// ダッシュ時のスピード倍率
	float kDashSpeedMultiplier = 3.0f;

	// ---------------------------------------------------------
	// オーバーヒート・弾関連
	// ---------------------------------------------------------

	// 現在のオーバーヒート時間
	float overheatTime_ = 0.0f;
	// オーバーヒートまでの限界時間
	const float kOverheatLimit = 3.0f;
	// 1秒あたりどれだけゲージが増えるか（撃っている間）
	float overheatGainPerSecond_ = 1.0f;
	// 回復速度（秒あたりに減る量）
	float overheatRecoverySpeed_ = 1.6f;
	// オーバーヒート中かどうか
	bool isOverheated_ = false;
	// 射撃中かどうか
	bool isFiring_ = false;

	// 長押し中の連射間隔
	float fireCooldown_ = 0.15f;
	// タイマー
	float fireTimer_ = 0.0f;

	// ブレ幅の最大角度（ラジアン）
	const float kMaxRandomAngle = 0.02f;
};