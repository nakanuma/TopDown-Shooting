#pragma once

// Engine
#include <Object3D.h>
#include <Input/Input.h>
#include <SpriteCommon.h>
#include <Sprite.h>
#include <Collider/Collider.h>
#include <Collider/CollisionManager.h>

// Application
#include <src/Game/Loader/Loader.h>
#include <src/Game/Bullet/PlayerBullet/PlayerBullet.h>
#include <src/Game/Player/UI/PlayerUIManager.h>

/// <summary>
/// プレイヤー
/// </summary>
class Player : public ICollisionCallback
{
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
	/// リロード中かの取得
	/// </summary>
	bool IsReloading() const { return isReloading_; }

	/// <summary>
	/// リロードタイマーの取得
	/// </summary>
	float GetReloadTimer() const { return reloadTimer_; } 

	/// <summary>
	/// 最大リロード時間の取得
	/// </summary>
	float GetMaxReloadTime() const { return kMaxReloadTime; }

	/// <summary>
	/// 残弾数の取得
	/// </summary>
	int32_t GetCurrentAmmo() const { return currentAmmo_; }

private:
	// ---------------------------------------------------------
	// 内部処理
	// ---------------------------------------------------------

	/// <summary>
	/// デバッグ表示
	/// </summary>
	void Debug();

	/// <summary>
	/// 移動処理
	/// </summary>
	void HandleMove();

	/// <summary>
	/// 弾の発射処理
	/// </summary>
	void HandleShooting();

	/// <summary>
	/// 弾のリロード処理
	/// </summary>
	void HandleReloading();

	/// <summary>
	/// 弾の更新処理
	/// </summary>
	void UpdateBullets();

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
	// 弾関連
	// ---------------------------------------------------------

	// 弾リスト
	std::vector<std::unique_ptr<Bullet>> bullets_;

	// 最大弾数
	const int32_t kMaxAmmo = 30;
	// 現在の弾数
	int32_t currentAmmo_;
	// リロード中フラグ
	bool isReloading_ = false;
	// リロードにかかる時間
	const float kMaxReloadTime = 1.0f;
	// リロード時間計測タイマー
	float reloadTimer_ = 0.0f;

	// ブレ幅の最大角度（ラジアン）
	const float kMaxRandomAngle = 0.087f;
};