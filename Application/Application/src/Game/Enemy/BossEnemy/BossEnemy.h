#pragma once

// Application
#include <src/Game/Enemy/Base/Enemy.h>

class BossEnemy : public Enemy, public ICollisionCallback
{
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
	/// デバッグ表示
	/// </summary>
	void Debug();

	/// <summary>
	/// 衝突時コールバック
	/// </summary>
	void OnCollision(Collider* other) override;

	/// <summary>
	/// ミサイルモデルのセット
	/// </summary>
	void SetMissileModel(ModelManager::ModelData* model) { modelMissile_ = model; }

	/// <summary>
	/// 地面警告モデルのセット
	/// </summary>
	void SetGroundWarningModel(ModelManager::ModelData* model) { modelGroundWarning_ = model; }

private:
	// ---------------------------------------------------------
	// 内部処理
	// ---------------------------------------------------------

	/// <summary>
	/// コライダー更新処理
	/// </summary>
	void UpdateCollider();

	/// <summary>
	/// 追尾ミサイルの発射
	/// </summary>
	void FireHomingMissile();

	/// <summary>
	/// 地面警告攻撃
	/// </summary>
	void GroundWarningAttack();

private:
	// ---------------------------------------------------------
	// モデル
	// ---------------------------------------------------------

	// ミサイルモデル
	ModelManager::ModelData* modelMissile_;
	
	// 球
	ModelManager::ModelData* modelGroundWarning_;
};

