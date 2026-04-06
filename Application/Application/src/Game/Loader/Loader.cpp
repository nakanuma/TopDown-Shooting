#include "Loader.h"

// C++
#include <fstream>
#include <iostream>

// Externals
#include <externals/nlohmann/json.hpp>

// Application
#include <src/Game/Stage/StageManager.h>

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
		std::string tag = item.value("tag", "");

		/* ステージ設定読み込み */
		if (tag == "STAGE_CONFIG") {
			std::string typeStr = item.value("stage_type", "KILL_ALL");

			// 読み込んだ文字に応じてステージ目標を設定
			if (typeStr == "KILL_ALL") {
				stageConfig_.stageType = static_cast<int>(StageType::killAllEnemies);
			} else if (typeStr == "DESTROY_GENERATORS") {
				stageConfig_.stageType = static_cast<int>(StageType::DestroyAllGeneratos);
			} else if (typeStr == "BOSS_BATTLE") {
				stageConfig_.stageType = static_cast<int>(StageType::BossBattle);
			}

			continue;
		}

		/* 通常オブジェクト読み込み */
		Loader::TransformData data;
		data.tag = tag;

		// 各種パラメータ読み込み
		auto loc = item.value("location", std::vector<float>{kDefaultLocation.x, kDefaultLocation.y, kDefaultLocation.z});
		auto rot = item.value("rotation", std::vector<float>{kDefaultRotation.x, kDefaultRotation.y, kDefaultRotation.z});
		auto scl = item.value("scale", std::vector<float>{kDefaultScale.x, kDefaultScale.y, kDefaultScale.z});
		auto col = item.value("colliderSize", std::vector<float>{kDefaultColliderSize.x, kDefaultColliderSize.y, kDefaultColliderSize.z});

		//Blender -> Engine への座標変換
		data.translate = ConvertToEngineCoords(loc);
		data.rotate = Cygnus::Float3(Cygnus::DegToRad(rot[kBlenderIndexX]), Cygnus::DegToRad(rot[kBlenderIndexY]), Cygnus::DegToRad(rot[kBlenderIndexZ]));
		data.scale = ConvertToEngineCoords(scl);

		// コライダー生成フラグ読み込み
		data.isCollider = item.value("is_collider", 1) != 0;

		// コライダーサイズ読み込み
		data.colliderSize = ConvertToEngineCoords(col);

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

Cygnus::Float3 Loader::ConvertToEngineCoords(const std::vector<float>& blenderVec) {
	// YとZを入れ替え
	return Cygnus::Float3(blenderVec[kBlenderIndexX], blenderVec[kBlenderIndexZ], blenderVec[kBlenderIndexY]);
}
