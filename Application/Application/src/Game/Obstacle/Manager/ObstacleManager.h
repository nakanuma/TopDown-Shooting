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
	void Update(const Cygnus::Float3& playerPos);

	/// <summary>
	/// モデルの描画処理を行います。
	/// </summary>
	void Draw(const Cygnus::Float3& playerPos);

	/// <summary>
	/// シャドウマップ用の描画処理を行います。
	/// </summary>
	void DrawShadow(const Cygnus::Float3& playerPos);

	/// <summary>
	/// デバッグ用の描画処理を行います。
	/// </summary>
	void Debug();

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// 障害物とプレイヤーの距離を計算し、有効化状態を取得します。
	/// </summary>
	/// <param name="obstaclePos">障害物の位置</param>
	/// <param name="playerPos">プレイヤーの位置</param>
	/// <returns></returns>
	bool IsActiveDistance(const Cygnus::Float3& obstaclePos, const Cygnus::Float3& playerPos) const;

private:
	// =========================================================
	// Constants
	// =========================================================
	const float kActiveDistance = 75.0f; /* 障害物の有効化距離 */

	// =========================================================
	// Member Variables
	// =========================================================
	std::vector<std::unique_ptr<Obstacle>> obstacles_;           /* 全ての障害物を格納したコンテナ*/
	std::unordered_map<std::string, std::string> tagModelMap_{}; /* タグとモデルデータキーのマップ */
};
