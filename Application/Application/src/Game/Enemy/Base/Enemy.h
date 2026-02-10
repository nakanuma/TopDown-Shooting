#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Collider/Collider.h>
#include <Collider/CollisionManager.h>
#include <Engine/3D/Object3D.h>
#include <Sprite.h>
#include <SpriteCommon.h>

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Bullet/Base/Bullet.h>
#include <src/Game/Enemy/UI/EnemyUIManager.h>
#include <src/Game/Enemy/VisualEffect/EnemyVisualEffects.h>

// ---------------------------------------------------------
// Forward Declaration
// ---------------------------------------------------------
class Player;

// =========================================================
// 敵の基底クラス
// =========================================================
class Enemy : public Cygnus::ICollisionCallback{
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 敵の初期化処理を行います。
	/// </summary>
	/// <param name="position">初期位置</param>
	/// <param name="model">モデルデータ</param>
	/// <param name="player">プレイヤーのポインタ</param>
	virtual void Initialize(const Cygnus::Float3& position, Player* player) = 0;

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// モデルの描画処理を行います。
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// シャドウマップ用の描画処理を行います。
	/// </summary>
	virtual void DrawShadow() = 0;

	/// <summary>
	/// UIの描画処理を行います。
	/// </summary>
	virtual void DrawUI() = 0;

	/// <summary>
	/// 衝突時のコールバック処理を行います。
	/// </summary>
	/// <param name="other">衝突した相手のコライダー</param>
	virtual void OnCollision(Cygnus::Collider* other) override;

	/// <summary>
	/// 破棄を行います。
	/// </summary>
	virtual void OnDestroy() { Cygnus::CollisionManager::GetInstance()->Unregister(collider_.get()); }

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// 死亡フラグの取得を行います。
	/// </summary>
	/// <returns>死亡フラグ</returns>
	bool IsDead() { return isDead_; }

	/// <summary>
	/// コライダーのタグを取得します。
	/// </summary>
	/// <returns>コライダータグ（string）</returns>
	std::string GetTag() const { return collider_->GetTag(); }

	/// <summary>
	/// 敵の現在位置を取得します。
	/// </summary>
	/// <returns>現在の位置（Float3）</returns>
	const Cygnus::Float3& GetTranslate() const { return objectEnemy_->transform_.translate_; }

	/// <summary>
	/// 敵の回転を取得します。
	/// </summary>
	/// <returns>現在の回転（Float3）</returns>
	const Cygnus::Float3& GetRotate() const { return objectEnemy_->transform_.rotate_; }

	/// <summary>
	/// 敵の現在HPを取得します。
	/// </summary>
	/// <returns>現在のHP</returns>
	int32_t GetHP() const { return currentHP_; }

protected:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// プレイヤー弾衝突時のダメージ適用処理（パーティクル・SE・死亡判定も含む）
	/// </summary>
	/// <param name="damage"></param>
	void ApplyDamage(int32_t damage);

	/// <summary>
	/// 障害物との押し戻し処理
	/// </summary>
	/// <param name="other"></param>
	void ResolveObstacleCollision(Cygnus::Collider* other);

	/// <summary>
	/// 発見アイコンのアニメーション開始（プレイヤー発見時に呼び出し）
	/// </summary>
	void ToggleDetectUI();

protected:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr int32_t kDeathCrossCount = 3;     /* 死亡時クロスパーティクルの発生数 */
	static constexpr float kDeathCrossAngle1 = 45.0f;  /* 死亡時クロスパーティクル1の角度（度） */
	static constexpr float kDeathCrossAngle2 = 135.0f; /* 死亡時クロスパーティクル2の角度（度） */

	// =========================================================
	// Member Variables
	// =========================================================
	std::unique_ptr<Cygnus::Object3D> objectEnemy_;		/* 敵オブジェクト */
	std::unique_ptr<Cygnus::Collider> collider_;		/* コライダー */
	std::unique_ptr<EnemyUIManager> ui_;				/* UI */
	std::unique_ptr<EnemyVisualEffects> visualEffect_;	/* 発光演出管理 */

	// ----- Parameters -----
	bool isActive_ = false;			/* 有効化フラグ */
	bool isDead_ = false;			/* 死亡フラグ */
	bool isDetectedPlayer_ = false;	/* プレイヤー発見フラグ */
	int32_t maxHP_ = 0;				/* 最大HP */
	int32_t currentHP_ = 0;			/* 現在HP */

	Player* targetPlayer_ = nullptr; /* プレイヤーのポインタ */
};