#pragma once

// Application
#include <src/Game/Loader/Loader.h>
#include <src/Game/Teleporter/Teleporter.h>

/// <summary>
/// テレポーターの管理
/// </summary>
class TeleporterManager
{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(std::vector<Loader::TransformData> datas);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// デバッグ表示
	/// </summary>
	void Debug();

private:
	// ---------------------------------------------------------
	// モデル
	// ---------------------------------------------------------

	ModelManager::ModelData modelTeleporter_;

	// ---------------------------------------------------------
	// テレポーター
	// ---------------------------------------------------------

	std::vector<std::unique_ptr<Teleporter>> teleporters_;
};

