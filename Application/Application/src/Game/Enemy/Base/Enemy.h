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

// ---------------------------------------------------------
// Foward Declaration
// ---------------------------------------------------------
class Player;

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
	virtual void Initialize(const Float3& position, ModelManager::ModelData* model, Player* player) = 0;

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
	virtual void OnDestroy() { CollisionManager::GetInstance()->Unregister(collider_.get()); }

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
	const Float3& GetTranslate() const { return objectEnemy_->transform_.translate_; }

	/// <summary>
	/// 敵の回転を取得します。
	/// </summary>
	/// <returns>現在の回転（Float3）</returns>
	const Float3& GetRotate() const { return objectEnemy_->transform_.rotate_; }

	/// <summary>
	/// 敵の現在HPを取得します。
	/// </summary>
	/// <returns>現在のHP</returns>
	int32_t GetHP() const { return currentHP_; }

protected:
	// =========================================================
	// Member Variables
	// =========================================================

	// ----- System -----	
	std::unique_ptr<SpriteCommon> spriteCommon_;			/* スプライト共通処理 */

	// ----- Object -----
	std::unique_ptr<Object3D> objectEnemy_;					/* 敵オブジェクト */

	// ----- Collision -----
	std::unique_ptr<Collider> collider_;					/* コライダー */
	Float3 colliderSize_;									/* コライダーサイズ */

	// ----- Sprite -----
	/// (Todo : 敵用のUIクラスに分ける )
	const Float2 kHPBarSize = {100.0f, 20.0f};				/* HPバーのサイズ */
	std::unique_ptr<Sprite> spriteHPBackground_;			/* HPバー後景スプライト */
	std::unique_ptr<Sprite> spriteHPForeground_;			/* HPバー前景スプライト */

	const Float2 kReloadSize = {100.0f, 10.0f};				/* リロード表示スプライトのサイズ */

	std::unique_ptr<Sprite> spriteReload_;					/* リロード表示スプライト */

	// ----- Parameters -----
	bool isActive_ = false;									/* 有効化フラグ */
	bool isDead_ = false;									/* 死亡フラグ */

	int32_t maxHP_ = 0;										/* 最大HP */
	int32_t currentHP_ = 0;									/* 現在HP */

	Player* targetPlayer_ = nullptr;						/* プレイヤーのポインタ */
};