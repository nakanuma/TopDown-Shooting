#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Engine/Model/ModelManager.h>

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Loader/Loader.h>
#include <src/Game/Obstacle/Base/Obstacle.h>

// =========================================================
// 障害物の管理クラス
// =========================================================
class ObstacleManager {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 障害物の初期化処理を行います。
	/// </summary>
	/// <param name="data">初期位置や回転などのTransformデータ</param>
	void Initialize(const std::vector<Loader::TransformData>& datas);

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	void Update(const Float3& playerPos);

	/// <summary>
	/// モデルの描画処理を行います。
	/// </summary>
	void Draw(const Float3& playerPos);

	/// <summary>
	/// シャドウマップ用の描画処理を行います。
	/// </summary>
	void DrawShadow(const Float3& playerPos);

	/// <summary>
	/// デバッグ用の描画処理を行います。
	/// </summary>
	void Debug();

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// ----- Container -----
	std::vector<std::unique_ptr<Obstacle>> obstacles_;							/* 全ての障害物を格納したコンテナ*/

	std::unordered_map<std::string, std::string> tagModelMap_{};				/* タグとモデルデータキーのマップ */

	// ----- Parameters -----
	const float kActiveDistance = 75.0f;										/* 障害物の有効化距離 */
};
