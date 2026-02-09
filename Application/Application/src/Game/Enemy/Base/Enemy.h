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

// ---------------------------------------------------------
// Forward Declaration
// ---------------------------------------------------------
class Player;
namespace Cygnus { enum class BehaviorStatus; }

// =========================================================
// 敵の基底クラス
// =========================================================
class Enemy {
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

	/// <summary>
	/// ノードの状態を記録します。
	/// </summary>
	/// <param name="nodePtr"></param>
	/// <param name="status"></param>
	void SetNodeStatus(const void* nodePtr, Cygnus::BehaviorStatus status);

	/// <summary>
	/// ノードの状態を取得します。
	/// </summary>
	/// <param name="nodePtr"></param>
	/// <returns></returns>
	Cygnus::BehaviorStatus GetNodeStatus(const void* nodePtr) const;

protected:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// 被弾時の発光処理を行います。
	/// </summary>
	void HandleHitBlink();

protected:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr float kHitBlinkDuration = 0.05f;						/* 被弾時の発光時間 */
	static constexpr Cygnus::Float3 kHitBlinkColor = { 1.0f, 0.5f, 0.0f };	/* 被弾時の発光色 */

	// =========================================================
	// Member Variables
	// =========================================================
	
	// ----- Object -----
	std::unique_ptr<Cygnus::Object3D> objectEnemy_; /* 敵オブジェクト */

	// ----- Collision -----
	std::unique_ptr<Cygnus::Collider> collider_; /* コライダー */

	// ----- UI -----
	std::unique_ptr<EnemyUIManager> ui_;

	// ----- Parameters -----
	bool isActive_ = false; /* 有効化フラグ */
	bool isDead_ = false;   /* 死亡フラグ */

	int32_t maxHP_ = 0;     /* 最大HP */
	int32_t currentHP_ = 0; /* 現在HP */

	Player* targetPlayer_ = nullptr; /* プレイヤーのポインタ */

	// ----- HitBlink -----
	/// <summary>
	/// 被弾時の発光演出フェーズ
	/// </summary>
	enum class HitBlinkPhase {
		Wait,		// 待機
		BlinkIn,	// 発光
		BlinkOut	// 減光
	} hitBlinkPhase_ = HitBlinkPhase::Wait;
	bool isHitBlink_ = false;		/* 被弾時の発光演出中フラグ */
	float hitBlinkTimer_ = 0.0f;	/* 被弾時の発光演出タイマー */

	// ----- Others -----
	std::unordered_map<const void*, Cygnus::BehaviorStatus> nodeStatusMap_; /* この個体でのノード実行状態を保存するマップ */
};