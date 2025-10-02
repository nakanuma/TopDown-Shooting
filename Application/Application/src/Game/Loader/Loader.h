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

		Float3 colliderSize; // 障害物で使用
		std::string pairID; // テレポーターで使用
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

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// ファイルが更新されたかを取得
	/// </summary>
	bool HasFileChanged() const { return fileChanged_; }

	/// <summary>
	/// ファイル更新フラグをリセット
	/// </summary>
	void ResetFileChangedFlag() { fileChanged_ = false; }

private:
	// ---------------------------------------------------------
	// データ
	// ---------------------------------------------------------

	/// <summary>
	/// 全データのコンテナ
	/// </summary>
	std::vector<TransformData> datas_;

	// ---------------------------------------------------------
	// 自動リロード用
	// ---------------------------------------------------------

	/// <summary>
	/// 最後にファイルが更新された時間を保持
	/// </summary>
	time_t lastModifiedTime = 0;

	/// <summary>
	/// ファイルが更新されたか
	/// </summary>
	bool fileChanged_ = false;

	/// <summary>
	/// 指定ファイルの更新された時間を取得
	/// </summary>
	time_t GetLastModifiedTime(const std::string& filename);
};
