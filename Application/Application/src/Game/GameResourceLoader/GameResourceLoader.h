#pragma once

// =========================================================
// テクスチャやモデルなどのリソースを予め読み込んでおくクラス
// =========================================================
class GameResourceLoader
{
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// インスタンスの取得を行います。
	/// </summary>
	/// <returns>シングルトンインスタンス</returns>
	static GameResourceLoader* GetInstance();

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	void Initialize();

private:
	// =========================================================
	// Internal Methods
	// =========================================================
	/// <summary>
	/// 全てのモデルデータの読み込みを行います。
	/// </summary>
	void LoadAllModelData();

	/// <summary>
	/// 全てのパーティクルの生成と登録を行います。
	/// </summary>
	void RegisterAllParticleEffect();
};