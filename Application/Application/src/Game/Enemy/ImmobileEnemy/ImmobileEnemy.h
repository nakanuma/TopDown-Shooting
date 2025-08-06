#pragma once

// Application
#include <src/Game/Enemy/Base/Enemy.h>

// Engine
#include <Engine/BehaviourTree/BehaviorTree.h>

class Player;

/// <summary>
/// 固定敵
/// </summary>
class ImmobileEnemy : public Enemy, public ICollisionCallback {
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
	/// <param name="other"></param>
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

	/// <summary>
	/// プレイヤーとの距離・遮蔽チェック
	/// </summary>
	bool IsPlayerInSight();

	/// <summary>
	/// 索敵モーション : 回転->待機->回転->待機・・・
	/// </summary>
	void SearchMotion();

	/// <summary>
	/// プレイヤーの方向を向く
	/// </summary>
	void FaceToPlayer();

	/// <summary>
	/// 弾発射処理
	/// </summary>
	void Shoot();

private:
	// ---------------------------------------------------------
	// モデル
	// ---------------------------------------------------------

	ModelManager::ModelData* modelEnemyBullet_;

	// ---------------------------------------------------------
	// パラメーター
	// ---------------------------------------------------------

	///
	///	プレイヤーとの距離・遮蔽チェック用
	/// 
	
	// プレイヤー索敵距離
	float searchRange_ = 20.0f;
	// プレイヤーを発見したかどうか
	bool isPlayerVisible_ = false;

	///
	/// 索敵モーション用
	///

	// 索敵時ステート
	enum class SearchState {
		Rotate,
		Wait,
	};
	SearchState searchState_ = SearchState::Wait;
	// 索敵時タイマー
	float searchStateTimer_ = 0.0f;
	// 回転後の待機時間（ランダム）
	float waitDuration_ = 0.0f;
	const float kMinWaitTime = 1.5f;
	const float kMaxWaitTime = 2.0f;
	// 回転時間（ランダム）
	float searchRotateDuration_ = 0.0f;
	const float kMinRotateTime = 0.8f;
	const float kMaxRotateTime = 2.0f;
	// 回転速度
	float rotationSpeed_ = 0.01f;
	// 回転方向 : true（右回り）false（左回り）
	float isRotatingRight_ = true;

	///
	///	攻撃用
	/// 

	// 弾の拡散角
	float bulletSpreadAngle_ = 0.1f;
	// 次の発射までの待ち時間
	float nextShotInterval_ = 0.0f;
	const float kShotInterval = 0.3f;
	// 残弾数
	int32_t bulletRemaining_ = 5;
	// 弾の上限
	const int32_t kMaxBullet = 5;
	// リロード中フラグ
	bool isReloading_ = false;
	// リロードにかかる時間
	const float kReloadTime = 1.5f;
	//　リロード中の経過時間
	float reloadTimer_ = 0.0f;


private:
	std::unique_ptr<BehaviorTree<ImmobileEnemy>> behaviorTree_;

public:
	void BuildBehaviorTree();
};