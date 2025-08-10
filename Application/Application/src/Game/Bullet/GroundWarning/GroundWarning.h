#pragma once

// Application
#include <src/Game/Bullet/Base/Bullet.h>

class Player;

/// <summary>
/// 地面警告攻撃（ボスの第一形態が使用・弾として管理）
/// </summary>
class GroundWarning : public Bullet, public ICollisionCallback {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(const Float3& position, const Float3& direciton, ModelManager::ModelData* model) override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 衝突時コールバック
	/// </summary>
	void OnCollision(Collider* other) override;

private:
	// ---------------------------------------------------------
	// 内部処理
	// ---------------------------------------------------------

	/// <summary>
	/// コライダー更新処理
	/// </summary>
	void UpdateCollider();

private:
	// ---------------------------------------------------------
	// パラメーター
	// ---------------------------------------------------------

	// 半径
	float radius_ = 5.0f;

	// この秒数経過後から当たり判定を有効にする
	float hitDelay_ = 0.8f;
	// コライダー有効化
	bool colliderEnabled_ = false;

	// 経過時間
	float elapsedTime_ = 0.0f;
	// 生存時間
	const float kMaxLifeTime = 1.0f; // 1秒で消える
};

