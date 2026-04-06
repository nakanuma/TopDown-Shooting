#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Collider/Collider.h>
#include <Collider/CollisionManager.h>
#include <Engine/3D/Object3D.h>

// =========================================================
// 障害物の基底クラス
// =========================================================
class Obstacle : public Cygnus::ICollisionCallback {
public:
	/// <summary>
	/// 障害物の初期化処理を行います。
	/// </summary>
	/// <param name="position">位置（Float3）</param>
	/// <param name="scale">スケール（Float3）</param>
	/// <param name="rotate">回転（Float3）</param>
	/// <param name="colliderSize">コライダーサイズ（Float3）</param>
	/// <param name="model">モデルデータ</param>
	void Initialize(const Cygnus::Float3& position, const Cygnus::Float3& scale, const Cygnus::Float3& rotate, bool isCollider, const Cygnus::Float3& colliderSize, Cygnus::ModelManager::ModelData* model);

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// モデルの描画処理を行います。
	/// </summary>
	void Draw();

	/// <summary>
	/// シャドウマップ用の描画処理を行います。
	/// </summary>
	void DrawShadow();

	/// <summary>
	/// 破棄を行います。
	/// </summary>
	void OnDestroy() { Cygnus::CollisionManager::GetInstance()->Unregister(collider_.get()); }

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// タグの取得を行います。
	/// </summary>
	/// <returns>コライダーに設定されたタグ（string）</returns>
	std::string GetTag() const { return collider_->GetTag(); }

	/// <summary>
	/// 現在位置の取得を行います。
	/// </summary>
	/// <returns>現在位置（Float3）</returns>
	Cygnus::Float3& GetTranslate() const { return object_->transform_.translate_; }

	/// <summary>
	/// スケールの取得を行います。
	/// </summary>
	/// <returns>スケール（Float3）</returns>
	Cygnus::Float3& GetScale() const { return object_->transform_.scale_; }

	/// <summary>
	/// コライダーの有効化状態を設定します。
	/// </summary>
	/// <param name="active">有効化状態</param>
	void SetActiveCollider(bool active) {
		if (collider_)
			collider_->SetActive(active);
	}

protected:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr float kEnvironmentStrength = 0.1f;        /* 環境マップの強度 */
	static constexpr float kHorizontalRotationAngle = -90.0f;  /* 横向き配置確認用の回転角度（度） */
	static constexpr float kRotationTolerance = 0.01f;         /* 回転角度の判定許容誤差 */
	static constexpr float kHorizontalAdjustmentAngle = 90.0f; /* 横向き時に設定する回転角度（度） */

	// =========================================================
	// Member Variables
	// =========================================================
	std::unique_ptr<Cygnus::Object3D> object_;   /* 障害物オブジェクト */
	std::unique_ptr<Cygnus::Collider> collider_; /* コライダー */
};