#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <string>
#include <vector>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Engine/Math/MyMath.h>

// =========================================================
// JSONファイルから読み込んだステージデータを管理するクラス
// =========================================================
class Loader {
public:
	/// <summary>
	/// ステージオブジェクト1つ分の変換情報データ
	/// </summary>
	struct TransformData {
		std::string tag;					/* タグ（string） */
		Cygnus::Float3 translate;			/* 位置 */
		Cygnus::Float3 rotate;				/* 回転（Euler） */
		Cygnus::Float3 scale;				/* スケール */

		Cygnus::Float3 colliderSize;		/* コライダーサイズ */
		std::string pairID;					/* ペアID（string） */
	};

public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// JSONファイルの読み込みを行います。
	/// </summary>
	/// <param name="filepath">読み込むファイルのパス（string）</param>
	void LoadFromFile(const std::string& filepath);

	/// <summary>
	/// 全データを取得します。
	/// </summary>
	/// <returns>トランスフォームデータ</returns>
	const std::vector<TransformData>& GetAllDatas() const { return datas_; }

	/// <summary>
	/// タグを指定して単体データを取得します。
	/// </summary>
	/// <param name="tag">タグ（string）</param>
	/// <returns>トランスフォームデータ</returns>
	TransformData GetDataByTag(const std::string& tag) const;

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// Blender座標系の3次元ベクトルをEngine座標系に変換します。
	/// </summary>
	/// <param name="blenderVec"></param>
	/// <returns></returns>
	Cygnus::Float3 ConvertToEngineCoords(const std::vector<float>& blenderVec);

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr size_t kBlenderIndexX = 0;
	static constexpr size_t kBlenderIndexY = 1;
	static constexpr size_t kBlenderIndexZ = 2;

	static constexpr Cygnus::Float3 kDefaultLocation = {0.0f, 0.0f, 0.0f};		/* 位置のデフォルト値 */
	static constexpr Cygnus::Float3 kDefaultRotation = {0.0f, 0.0f, 0.0f};		/* 回転のデフォルト値 */
	static constexpr Cygnus::Float3 kDefaultScale = {1.0f, 1.0f, 1.0f};			/* スケールのデフォルト値 */
	static constexpr Cygnus::Float3 kDefaultColliderSize = {1.0f, 1.0f, 1.0f};	/* コライダーサイズのデフォルト値 */

	// =========================================================
	// Member Variables
	// =========================================================
	std::vector<TransformData> datas_;		/* 全データのコンテナ */
};
