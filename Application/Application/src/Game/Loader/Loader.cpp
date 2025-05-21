#include "Loader.h"

// C++
#include <fstream>
#include <iostream>

// Externals
#include <externals/nlohmann/json.hpp>

// ---------------------------------------------------------
// JSONファイルの読み込み
// ---------------------------------------------------------
void Loader::LoadFromFile(const std::string& filepath) 
{ 
	std::ifstream file(filepath); 
	if (!file.is_open()) {
		std::cerr << "Failed to open JSON file: " << filepath << std::endl;
		return;
	}

	nlohmann::json j;
	file >> j;

	objects_.clear();

	for (const auto& item : j) {
		Loader::TransformData data;
		data.tag = item.value("tag", "");

		auto loc = item.value("location", std::vector<float>{0, 0, 0});
		auto rot = item.value("rotation", std::vector<float>{0, 0, 0});
		auto scl = item.value("scale", std::vector<float>{1, 1, 1});

		// Blender -> Engine への座標変換
		data.translate = Float3(-loc[0], loc[2], -loc[1]); // X反転、Yを反転、YとZ入れ替え
		data.rotate = Float3(rot[0], rot[1], rot[2]);
		data.scale = Float3(scl[0], scl[2], scl[1]); // YとZ入れ替え

		objects_.push_back(data);
	}
}
