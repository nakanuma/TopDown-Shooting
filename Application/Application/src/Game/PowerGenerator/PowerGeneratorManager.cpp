#include "PowerGeneratorManager.h"

// Engine
#include <ImguiWrapper.h>

void PowerGeneratorManager::Initialize(const std::vector<Loader::TransformData>& datas) {
	// 最初にクリアを行っておく
	Clear();

	// 発電機の生成
	for(const auto& data : datas) {
		if(data.tag == "POWERGENERATOR") {
			auto generator = std::make_unique<PowerGenerator>();
			generator->Initialize(data.translate, &Cygnus::ModelManager::GetInstance()->GetModel("PowerGenerator"));

			generators_.emplace_back(std::move(generator));
		}
	}
}

void PowerGeneratorManager::Update() {
	for(auto& generator : generators_) {
		generator->Update();
	}

	// 削除処理
	for (auto& generator : generators_) {
		if (generator->IsDead()) {
			generator->OnDestroy();
		}
	}
	generators_.erase(std::remove_if(generators_.begin(), generators_.end(), 
		[](const std::unique_ptr<PowerGenerator>& generator) { return generator->IsDead(); }), generators_.end());
}

void PowerGeneratorManager::Draw() {
	for (auto& generator : generators_) {
		generator->Draw();
	}
}

void PowerGeneratorManager::DrawUI() {
	for (auto& generator : generators_) {
		generator->DrawUI();
	}
}

void PowerGeneratorManager::DrawShadow() {
	for (auto& generator : generators_) {
		generator->DrawShadow();
	}
}

void PowerGeneratorManager::Debug() {
#ifdef USE_IMGUI
	ImGui::Begin("PowerGeneratorManager");

	ImGui::Text("Total Generators: %zu", generators_.size());

	for (size_t i = 0; i < generators_.size(); ++i) {
		PowerGenerator* generator = generators_[i].get();
		if (!generator)
			continue;

		std::string label = "Generator[" + std::to_string(i) + "]";
		if (ImGui::TreeNode(label.c_str())) {
			// 位置の表示
			const Cygnus::Float3& translate = generator->GetTranslate();
			ImGui::Text("Translate : (%.2f, %.2f, %.2f)", translate.x, translate.y, translate.z);

			ImGui::TreePop();
		}
	}

	ImGui::End();
#endif
}

void PowerGeneratorManager::Clear() {
	for (auto& generator : generators_) {
		generator->OnDestroy();
	}
	generators_.clear();
}

bool PowerGeneratorManager::IsAllDestroyed() const
{
	// 配列が空ならtrue
	if(generators_.empty()) {
		return true;
	} else {
		return false;
	}
}
