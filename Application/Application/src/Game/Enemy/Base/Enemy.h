#pragma once

// Engine
#include <Engine/3D/Object3D.h>
#include <Collider/Collider.h>
#include <Collider/CollisionManager.h>
#include <Sprite.h>
#include <SpriteCommon.h>

/// <summary>
/// 敵の基底クラス
/// </summary>
class Enemy
{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize(const Float3& position, ModelManager::ModelData* model) = 0;

	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// UI描画処理
	/// </summary>
	virtual void DrawUI() = 0;

	/// <summary>
	/// 破棄される際に呼ぶ関数
	/// </summary>
	virtual void OnDestroy() { CollisionManager::GetInstance()->Unregister(collider_.get()); }

	/// <summary>
	/// 死亡フラグの取得
	/// </summary>
	bool IsDead() { return isDead_; }

	/// <summary>
	/// タグの取得（コライダーに設定してあるタグ）
	/// </summary>
	std::string GetTag() const { return collider_->GetTag(); }

	/// <summary>
	/// 現在位置の取得
	/// </summary>
	Float3& GetTranslate() const { return objectEnemy_->transform_.translate; }
		 
	/// <summary>
	/// 残りHPの取得
	/// </summary>
	int32_t GetHP() const { return currentHP_; }

protected:
	// ---------------------------------------------------------
	// 基盤機能
	// ---------------------------------------------------------

	std::unique_ptr<SpriteCommon> spriteCommon_;

	// ---------------------------------------------------------
	// オブジェクト関連
	// ---------------------------------------------------------

	// 敵オブジェクト
	std::unique_ptr<Object3D> objectEnemy_;

	// ---------------------------------------------------------
	// オブジェクト関連
	// ---------------------------------------------------------

	// コライダー
	std::unique_ptr<Collider> collider_;

	// ---------------------------------------------------------
	// スプライト関連
	// ---------------------------------------------------------

	// HPバーの最大サイズ
	const Float2 kHPBarSize = { 100.0f, 25.0f };

	// HPバー（後景）
	std::unique_ptr<Sprite> spriteHPBackground_;
	// HPバー（前景）
	std::unique_ptr<Sprite> spriteHPForeground_;

	// ---------------------------------------------------------
	// パラメーター
	// ---------------------------------------------------------

	// 死亡フラグ
	bool isDead_ = false;

	// 最大HP
	int32_t maxHP_;
	// 現在HP
	int32_t currentHP_;
};