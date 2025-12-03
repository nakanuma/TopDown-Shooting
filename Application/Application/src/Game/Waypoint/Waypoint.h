#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <string>
#include <vector>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <MyMath.h>
#include <Object3D.h>

// =========================================================
// ウェイポイント1つ分のクラス
// 主に敵の経路探索に使用
// =========================================================
class Waypoint {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="name">固有の名前</param>
	/// <param name="pos">位置</param>
	/// <param name="model">モデルデータ</param>
	Waypoint(const std::string& name, const Float3& pos, ModelManager::ModelData* model);

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// モデルの描画処理を行います。
	/// </summary>
	void Draw();

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// 名前を取得します。
	/// </summary>
	/// <returns>固有の名前（string）</returns>
	const std::string& GetName() const { return name_; }

	/// <summary>
	/// 位置を取得します。
	/// </summary>
	/// <returns>位置（Float3）</returns>
	const Float3& GetPosition() const { return position_; }

	/// <summary>
	/// 隣接ノードを追加します。
	/// </summary>
	/// <param name="neighbor">隣接ノードのポインタ</param>
	void AddNeighbor(Waypoint* neighbor) { neighbors_.push_back(neighbor); }

	/// <summary>
	/// 隣接ノードのリストをクリアします。
	/// </summary>
	void ClearNeighbors() { neighbors_.clear(); }

	/// <summary>
	/// 隣接ノードを取得します。
	/// </summary>
	/// <returns>隣接ノードのリスト</returns>
	const std::vector<Waypoint*>& GetNeighbors() const { return neighbors_; }
private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Float3 kInitialScale = {0.25f, 0.25f, 0.25f}; /* 初期スケール */

	static constexpr Float4 kColorLightBlue = {0.0f, 1.0f, 1.0f, 1.0f}; /* 水色 */
	static constexpr Float4 kColorYellow = {1.0f, 1.0f, 0.0f, 1.0f};    /* 黄色 */

	// =========================================================
	// Member Variables
	// =========================================================

	// ----- Parameters -----
	std::string name_;                 /* ウェイポイント名 */
	Float3 position_;                  /* 位置 */
	std::vector<Waypoint*> neighbors_; /* 隣接ノード */

	// ----- Object -----
	std::unique_ptr<Object3D> objectSphere_; /* ウェイポイント表示用の球体オブジェクト */
};