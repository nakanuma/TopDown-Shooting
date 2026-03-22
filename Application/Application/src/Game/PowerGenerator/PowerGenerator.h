#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Engine/3D/Object3D.h>
#include <Engine/Collider/Collider.h>
#include <Engine/Collider/CollisionManager.h>

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/PowerGenerator/UI/PowerGeneratorUIManager.h>

// =========================================================
// 発電機クラス
// ステージクリア条件用の破壊可オブジェクト
// =========================================================
class PowerGenerator : public Cygnus::ICollisionCallback  {
public:
	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	/// <param name="position">位置</param>
	/// <param name="model">モデルデータ</param>
	void Initialize(const Cygnus::Float3& position, Cygnus::ModelManager::ModelData* model);

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// モデルの描画処理を行います。
	/// </summary>
	void Draw();

	/// <summary>
	/// UI描画処理を行います。
	/// </summary>
	void DrawUI();

	/// <summary>
	/// シャドウマップ用の描画処理を行います。
	/// </summary>
	void DrawShadow();

	/// <summary>
	/// 衝突時のコールバック処理を行います。
	/// </summary>
	/// <param name="other">衝突した相手のコライダー</param>
	void OnCollision(Cygnus::Collider* other) override;

	/// <summary>
	/// 破棄を行います。
	/// </summary>
	void OnDestroy() { Cygnus::CollisionManager::GetInstance()->Unregister(collider_.get()); }

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// 位置を取得します。
	/// </summary>
	/// <returns></returns>
	const Cygnus::Float3& GetTranslate() const { return object_->transform_.translate_; }

	/// <summary>
	/// 死亡フラグを取得します。
	/// </summary>
	/// <returns></returns>
	bool IsDead() const { return isDead_; }

private:
	// =========================================================
	// Internal Methods
	// =========================================================

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Cygnus::Float3 kColliderSize = {2.0f, 3.0f, 2.0f};	/* コライダーサイズ */
	static constexpr int32_t kMaxHP = 100;	/* 最大HP */

	// =========================================================
	// Member Variables
	// =========================================================
	std::unique_ptr<Cygnus::Object3D> object_;	 /* オブジェクト */
	std::unique_ptr<Cygnus::Collider> collider_; /* コライダー */

	std::unique_ptr<PowerGeneratorUIManager> ui_;	/* UI */

	int32_t currentHP_ = kMaxHP;	/* 現在HP */
	bool isDead_ = false;			/* 死亡フラグ */
};

