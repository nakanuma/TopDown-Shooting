#pragma once

// Application
#include <src/Game/Bullet/Base/Bullet.h>

class PlayerBullet : public Bullet, public ICollisionCallback {
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
	/// <param name="other"></param>
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
	float radius_ = 1.0f;

	// 経過時間
	float elapsedTime_ = 0.0f;
	// 生存時間
	const float kMaxLifeTime = 1.0f; // 1秒で消える
};
