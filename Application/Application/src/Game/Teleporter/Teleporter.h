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
// テレポータークラス
// プレイヤーのみが使用する
// =========================================================
class Teleporter : public ICollisionCallback {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// テレポーターの初期化処理を行います。
	/// </summary>
	/// <param name="position">位置</param>
	/// <param name="model">モデルデータ</param>
	void Initialize(const Float3& position, ModelManager::ModelData* model);

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// モデルの描画処理を行います。
	/// </summary>
	void Draw();

	/// <summary>
	/// 衝突時のコールバック処理を行います。
	/// </summary>
	/// <param name="other">衝突した相手のコライダー</param>
	void OnCollision(Collider* other) override;

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// ペアIDのセットを行います。
	/// </summary>
	/// <param name="id">ペアID（string）</param>
	void SetPairID(std::string id) { pairID_ = id; }

	/// <summary>
	/// ペアIDを取得します。
	/// </summary>
	/// <returns>ペアID（string）</returns>
	const std::string& GetPairID() const { return pairID_; }

	/// <summary>
	/// ペアのテレポーターをセットします。
	/// </summary>
	/// <param name="other">ペアのテレポーター</param>
	void SetPair(Teleporter* other) { pair_ = other; }

	/// <summary>
	/// 有効化状態を設定します。
	/// </summary>
	/// <param name="active">有効化状態</param>
	void SetActive(bool active) { isActive_ = active; }

	/// <summary>
	/// 有効化状態を取得します。
	/// </summary>
	/// <returns>有効化状態</returns>
	bool IsActive() const { return isActive_; }

	/// <summary>
	/// このテレポーターがゴールかどうかを判定します。
	/// </summary>
	/// <returns>ゴールテレポーターかどうか</returns>
	bool IsGoal() const { return pairID_ == "GOAL"; }

	/// <summary>
	/// 位置の取得を行います。
	/// </summary>
	/// <returns>現在位置（Float3）</returns>
	const Float3& GetTranslate() const { return object_->transform_.translate_; }

	/// <summary>
	/// ゴール時のコールバック関数を設定します。
	/// </summary>
	/// <param name="callback">ゴール時のコールバック関数</param>
	void SetOnGoalCallback(std::function<void()> callback) { onGoalCallback_ = callback; }

private:
	// =========================================================
	// Internal Methods
	// =========================================================

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// ----- Object -----
	std::unique_ptr<Object3D> object_;					/* テレポーターオブジェクト */

	// ----- Collision -----
	std::unique_ptr<Collider> collider_;				/* コライダー */
	Float3 colliderSize_ = {3.0f, 0.5f, 3.0f};			/* コライダーサイズ */

	// ----- Parameters -----
	std::string pairID_;								/* 固有ID */
	Teleporter* pair_ = nullptr;						/* ペアのテレポーターのポインタ */
	bool isActive_ = false;								/* 使用可能フラグ */

	// ----- Others -----
	std::function<void()> onGoalCallback_ = nullptr;	/* ゴール時のコールバック関数 */
};
