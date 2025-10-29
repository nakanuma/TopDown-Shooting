#include "Loader.h"

// C++
#include <fstream>
#include <iostream>

// Externals
#include <externals/nlohmann/json.hpp>

void Loader::LoadFromFile(const std::string& filepath) {
	std::ifstream file(filepath);
	// ファイルを開けなかったらエラーを出力して終了
	if (!file.is_open()) {
		std::cerr << "Failed to open JSON file: " << filepath << std::endl;
		return;
	}

	// ファイルの内容をjsonで読み込み
	nlohmann::json j;
	file >> j;

	// 既存のデータはクリアしておく
	datas_.clear();

	for (const auto& item : j) {
		Loader::TransformData data;
		// タグ読み込み
		data.tag = item.value("tag", "");

		// 各種パラメータ読み込み
		auto loc = item.value("location", std::vector<float>{0, 0, 0});
		auto rot = item.value("rotation", std::vector<float>{0, 0, 0});
		auto scl = item.value("scale", std::vector<float>{1, 1, 1});
		auto col = item.value("colliderSize", std::vector<float>{1, 1, 1});

		// Blender -> Engine への座標変換
		data.translate = Float3(loc[0], loc[2], loc[1]); // YとZ入れ替え
		data.rotate = Float3(DegToRad(rot[0]), DegToRad(rot[1]), DegToRad(rot[2]));
		data.scale = Float3(scl[0], scl[2], scl[1]); // YとZ入れ替え
		data.colliderSize = Float3(col[0], col[2], col[1]); // YとZ入れ替え

		// ペアID読み込み
		data.pairID = item.value("pair_id", "");

		datas_.push_back(data);
	}
}

Loader::TransformData Loader::GetDataByTag(const std::string& tag) const {
	// 全てのデータの中から指定されたタグを持ったデータを返す
	for (const auto& data : datas_) {
		if (data.tag == tag) {
			return data;
		}
	}

	assert(false);
	return TransformData{};
}

time_t Loader::GetLastModifiedTime(const std::string& filename) {
	struct stat result; // ファイルの情報を格納する構造体
	// ファイルの状態を取得
	if (stat(filename.c_str(), &result) == 0) {
		return result.st_mtime; // ファイルの最終更新時刻を返す
	}
	return 0;
}

void Loader::Update() {
	// ファイルの変更を検知
	time_t newModifiedTime = GetLastModifiedTime("resources/Stages/data.json");
	if (newModifiedTime > lastModifiedTime) {
		lastModifiedTime = newModifiedTime; // 最終更新時刻を更新しておく
		fileChanged_ = true;                // ファイルに変更があったことを知らせる
	}
}
