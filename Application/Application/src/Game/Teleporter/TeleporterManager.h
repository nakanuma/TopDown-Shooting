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
	void Initialize(std::vector<Loader::TransformData> datas);

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// モデルの描画処理を行います。
	/// </summary>
	void Draw();

	/// <summary>
	/// デバッグ用の描画処理を行います。
	/// </summary>
	void Debug();

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// ----- Container -----
	std::vector<std::unique_ptr<Teleporter>> teleporters_;	/* テレポーター全てを格納したコンテナ */
};
