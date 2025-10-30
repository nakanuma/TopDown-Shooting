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
		std::string tag;			/* タグ（string） */
		Float3 translate;			/* 位置 */
		Float3 rotate;				/* 回転（Euler） */
		Float3 scale;				/* スケール */

		Float3 colliderSize;		/* コライダーサイズ */
		std::string pairID;			/* ペアID（string） */
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

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	void Update();

	/// <summary>
	/// ファイルが更新されたかを取得します。
	/// </summary>
	/// <returns></returns>
	bool HasFileChanged() const { return fileChanged_; }

	/// <summary>
	/// ファイル更新フラグをリセットします。
	/// </summary>
	void ResetFileChangedFlag() { fileChanged_ = false; }

private:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// 指定ファイルの更新された時間を取得します。
	/// </summary>
	/// <param name="filename">ファイルパス</param>
	/// <returns>最終更新時間</returns>
	time_t GetLastModifiedTime(const std::string& filename);

private:
	// =========================================================
	// Member Variables
	// =========================================================

	// ----- Container -----
	std::vector<TransformData> datas_;		/* 全データのコンテナ */

	// ----- Auto Reload -----
	time_t lastModifiedTime = 0;			/* 最後にファイルが更新された時間 */
	bool fileChanged_ = false;				/* ファイル更新フラグ */
};
