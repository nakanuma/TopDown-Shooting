#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <string>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Engine/3D/Object3D.h>
#include <Engine/Collider/Collider.h>
#include <Engine/Collider/CollisionManager.h>

// =========================================================
// イベントトリガークラス
// 触れたらイベント発生
// =========================================================
class EventTrigger : public Cygnus::ICollisionCallback {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 初期化処理を行います
	/// </summary>
	/// <param name="translate">中心位置</param>
	/// <param name="colliderSize">サイズ</param>
	void Initialize(const Cygnus::Float3& translate, const Cygnus::Float3& colliderSize);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 衝突時のコールバック処理を行います。
	/// </summary>
	/// <param name="other"></param>
	void OnCollision(Cygnus::Collider* other) override;

	// =========================================================
	// Accessor
	// =========================================================

	/// <summary>
	/// イベント識別用IDのセットを行います。
	/// </summary>
	/// <param name="id"></param>
	void SetEventID(std::string id) { eventID_ = id; }

	/// <summary>
	/// イベント識別用IDの取得を行います。
	/// </summary>
	/// <returns></returns>
	const std::string& GetEventID() const { return eventID_; }

	/// <summary>
	/// イベントトリガーが引かれたかを取得します。
	/// </summary>
	/// <returns></returns>
	bool IsTriggered() const { return isTriggered_; }

private:
	// =========================================================
	// Constants
	// =========================================================

	// =========================================================
	// Member Variables
	// =========================================================
	std::unique_ptr<Cygnus::Collider> collider_;     /* コライダー */
	std::function<void()> onGoalCallback_ = nullptr; /* イベント用コールバック関数 */

	std::string eventID_; /* イベント識別用ID */
	bool isTriggered_ = false; /* トリガーフラグ */
	bool isActive_ = true;     /* 二重発火防止用 */
};
