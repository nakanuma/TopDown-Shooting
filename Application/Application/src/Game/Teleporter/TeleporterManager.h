#pragma once

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Loader/Loader.h>
#include <src/Game/Teleporter/Teleporter.h>

// =========================================================
// テレポーターの管理クラス
// =========================================================
class TeleporterManager {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 全てのテレポーターの初期化処理を行います。
	/// </summary>
	/// <param name="datas">初期位置や回転などのTransformデータ</param>
	void Initialize(const std::vector<Loader::TransformData>& datas);

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
	/// デバッグ用の描画処理を行います。
	/// </summary>
	void Debug();

	/// <summary>
	/// ゴールテレポーターを有効化します。
	/// </summary>
	void EnableGoalTeleporter();

	/// <summary>
	/// データのクリアを行います。
	/// </summary>
	void Clear();

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// ゴール時のコールバック関数を設定します。
	/// </summary>
	/// <param name="callback">ゴール時のコールバック関数</param>
	void SetGoalCallback(std::function<void()> callback) { goalCallback_ = callback; }

	/// <summary>
	/// 次ステージ移行時のコールバック関数を設定します。
	/// </summary>
	/// <param name="callback"></param>
	void SetNextFloorCallback(std::function<void()> callback) { nextFloorCallback_ = callback; }

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// ----- Container -----
	std::vector<std::unique_ptr<Teleporter>> teleporters_;	/* テレポーター全てを格納したコンテナ */

	// ----- Others -----
	std::function<void()> goalCallback_ = nullptr;			/* ゴール時のコールバック関数 */
	std::function<void()> nextFloorCallback_ = nullptr;		/* 次ステージ移行時のコールバック関数 */
};
