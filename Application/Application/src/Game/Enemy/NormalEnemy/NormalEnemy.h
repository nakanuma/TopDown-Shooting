#pragma once

// Application
#include <src/Game/Enemy/Base/Enemy.h>

/// <summary>
/// 通常敵
/// </summary>
class NormalEnemy : public Enemy, public ICollisionCallback {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(const Float3& position, ModelManager::ModelData* model, Player* player) override;

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw() override;

	/// <summary>
	/// UI描画処理
	/// </summary>
	void DrawUI() override;

	/// <summary>
	/// 衝突時コールバック
	/// </summary>
	void OnCollision(Collider* other) override;

	/// <summary>
	/// 弾モデルのセット
	/// </summary>
	void SetBulletModel(ModelManager::ModelData* model) { modelEnemyBullet_ = model; }

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
	// モデル
	// ---------------------------------------------------------

	ModelManager::ModelData* modelEnemyBullet_;

	// ---------------------------------------------------------
	// パラメーター
	// ---------------------------------------------------------

};
