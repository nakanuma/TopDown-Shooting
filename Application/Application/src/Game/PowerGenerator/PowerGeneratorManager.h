#pragma once

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Loader/Loader.h>
#include <src/Game/PowerGenerator/PowerGenerator.h>

// =========================================================
// 発電機の管理クラス
// =========================================================
class PowerGeneratorManager
{
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	/// <param name="data"></param>
	void Initialize(const std::vector<Loader::TransformData>& datas);

	/// <summary>
	/// 更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理を行います。
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
	/// デバッグ用の描画処理を行います。
	/// </summary>
	void Debug();

	/// <summary>
	/// データのクリアを行います。
	/// </summary>
	void Clear();

	// =========================================================
	// Accessor
	// =========================================================

private:
	// =========================================================
	// Member Variables
	// =========================================================
	std::vector<std::unique_ptr<PowerGenerator>> generators_;	/* 発電機を格納した配列 */
};

