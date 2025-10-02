#pragma once

// Engine
#include <Engine/Model/ModelManager.h>

/// <summary>
/// パーティクルのモデル管理・登録を行うクラス
/// </summary>
class ParticleEffectLoader
{
public:
	/// <summary>
	/// インスタンス取得
	/// </summary>
	static ParticleEffectLoader* GetInstance();

	/// <summary>
	/// パーティクルのモデル読み込み・登録
	/// </summary>
	void LoadAndRegisterAll();

private:
	// パーティクル用モデル
	ModelManager::ModelData modelSmoothCube_;
	ModelManager::ModelData modelPlane_;
};

