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

private:
	// ---------------------------------------------------------
	// 内部処理
	// ---------------------------------------------------------

	/// <summary>
	/// デバッグ表示
	/// </summary>
	void Debug();

	/// <summary>
	/// カーソル位置のワールド座標を取得
	/// </summary>
	Float3 CalclateCursorPosition();

	/// <summary>
	/// 移動処理
	/// </summary>
	void HandleMove();

	/// <summary>
	/// 弾の発射処理
	/// </summary>
	void HandleShooting();

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
	// スプライト関連
	// ---------------------------------------------------------

	/* レティクル */

	// 仮のレティクル画像
	std::unique_ptr<Sprite> spriteTarget_;

	/* HPバー */

	// HPバーの最大サイズ
	const Float2 kHPBarSize = { 300.0f, 30.0f };

	// HPバー（後景）
	std::unique_ptr<Sprite> spriteHPBackground_;
	// HPバー（前景）
	std::unique_ptr<Sprite> spriteHPForeground_;

	// ---------------------------------------------------------
	// パラメーター
	// ---------------------------------------------------------

	// 速度
	Float3 velocity_ = {0.0f, 0.0f, 0.0f};

	// 速さ
	float speed_ = 0.25f;

	// 最大HP
	int32_t maxHP_;
	// 現在HP
	int32_t currentHP_;

	// ---------------------------------------------------------
	// 未分類
	// ---------------------------------------------------------

	// 弾リスト
	std::vector<std::unique_ptr<Bullet>> bullets_;
};

