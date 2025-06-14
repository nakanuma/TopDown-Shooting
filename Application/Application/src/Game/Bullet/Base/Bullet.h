﻿#pragma once

// Engine
#include <Engine/3D/Object3D.h>
#include <Collider/Collider.h>
#include <Collider/CollisionManager.h>

/// <summary>
/// 弾の基底クラス
/// </summary>
class Bullet {
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize(const Float3& position, const Float3& direciton, ModelManager::ModelData* model) = 0;

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 死亡フラグの取得
	/// </summary>
	virtual bool IsDead() const { return isDead_; }

	/// <summary>
	/// 攻撃力の取得
	/// </summary>
	virtual int32_t GetDamage() const { return damage_; }

	/// <summary>
	/// 位置の取得
	/// </summary>
	virtual Float3 GetTranslate() const { return objectBullet_->transform_.translate; }

	/// <summary>
	/// 破棄される際に呼ぶ関数
	/// </summary>
	virtual void OnDestroy() { CollisionManager::GetInstance()->Unregister(collider_.get()); }

protected:
	// ---------------------------------------------------------
	// オブジェクト関連
	// ---------------------------------------------------------

	// 弾オブジェクト
	std::unique_ptr<Object3D> objectBullet_;

	// ---------------------------------------------------------
	// コライダー
	// ---------------------------------------------------------

	// コライダー
	std::unique_ptr<Collider> collider_;

	// ---------------------------------------------------------
	// パラメーター
	// ---------------------------------------------------------

	// 攻撃力
	int32_t damage_;

	// 速さ
	float speed_;

	// 速度
	Float3 velocity_;

	// 死亡フラグ
	bool isDead_ = false;
};
