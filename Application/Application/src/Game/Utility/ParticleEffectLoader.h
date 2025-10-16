#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Engine/Model/ModelManager.h>

// =========================================================
// パーティクルのモデル管理・登録をあらかじめ行うクラス
// =========================================================
class ParticleEffectLoader {
public:
	/// <summary>
	/// インスタンスの取得を行います。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static ParticleEffectLoader* GetInstance();

	/// <summary>
	/// パーティクルのモデル読み込み・登録を行います。
	/// </summary>
	void LoadAndRegisterAll();

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// ----- Models -----
	ModelManager::ModelData modelSmoothCube_; 			/* 滑らかな立方体モデル */
	ModelManager::ModelData modelPlane_;				/* 平面モデル */
};