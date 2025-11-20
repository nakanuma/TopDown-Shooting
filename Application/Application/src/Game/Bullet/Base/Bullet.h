#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Collider/Collider.h>
#include <Collider/CollisionManager.h>
#include <Engine/3D/Object3D.h>

// =========================================================
// 弾の基底クラス
// =========================================================
class Bullet {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 弾の初期化処理を行います。
	/// </summary>
	/// <param name="position">初期位置</param>
	/// <param name="direciton">方向</param>
	/// <param name="model">モデルデータ</param>
	virtual void Initialize(const Float3& position, const Float3& direction, ModelManager::ModelData* model) = 0;

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// モデルの描画処理を行います。
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 弾を死亡させます。
	/// </summary>
	void Dead() { isDead_ = true; }

	/// <summary>
	/// 破棄を行います。
	/// </summary>
	void OnDestroy() {
		if (collider_)
			CollisionManager::GetInstance()->Unregister(collider_.get());
	}

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// 弾の攻撃力を取得します。
	/// </summary>
	/// <returns>攻撃力（int）</returns>
	int32_t GetDamage() const { return damage_; }

	/// <summary>
	/// 弾の現在位置を取得します。
	/// </summary>
	/// <returns>現在の位置（Float3）</returns>
	const Float3& GetTranslate() const { return objectBullet_->transform_.translate_; }

	/// <summary>
	/// 死亡したかどうかを取得します。
	/// </summary>
	/// <returns>死亡フラグ</returns>
	bool IsDead() const { return isDead_; }

protected:
	// =========================================================
	// Member Variables
	// =========================================================

	// ----- Object -----
	std::unique_ptr<Object3D> objectBullet_; /* 弾オブジェクト */

	// ----- Collision -----
	std::unique_ptr<Collider> collider_; /* コライダー */

	// ----- Parameters -----
	int32_t damage_ = 0;                   /* 弾の攻撃力 */
	float speed_ = 0.0f;                   /* 移動速度 */
	Float3 velocity_ = {0.0f, 0.0f, 0.0f}; /* 速度ベクトル */
	bool isDead_ = false;                  /* 死亡フラグ */
};
