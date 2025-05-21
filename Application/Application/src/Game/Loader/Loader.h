#pragma once

// C++
#include <string>
#include <vector>

// Engine
#include <Engine/Math/MyMath.h>

/// <summary>
/// JSONファイルから読み込んだデータを保持
/// </summary>
class Loader {
public:
	struct TransformData {
		std::string tag;
		Float3 translate;
		Float3 rotate;
		Float3 scale;
	};

public:
	/// <summary>
	/// JSONファイルの読み込み
	/// </summary>
	void LoadFromFile(const std::string& filepath);

	/// <summary>
	/// 全データ取得
	/// </summary>
	const std::vector<TransformData>& GetAllDatas() const { return datas_; }

	/// <summary>
	/// 単体データ取得
	/// </summary>
	TransformData GetDataByTag(const std::string& tag) const;

private:
	// ---------------------------------------------------------
	// データ
	// ---------------------------------------------------------

	/// <summary>
	/// 全データのコンテナ
	/// </summary>
	std::vector<TransformData> datas_;
};
