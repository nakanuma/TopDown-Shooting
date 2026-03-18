#include "EventManager.h"

void EventManager::Initialize(const std::vector<Loader::TransformData>& datas) {
	// イベントトリガーの生成
	for (auto& data : datas) {
		if (data.tag == "EVENTTRIGGER") {
			auto trigger = std::make_unique<EventTrigger>();
			trigger->Initialize(data.translate, data.colliderSize);
			trigger->SetEventID(data.pairID); // イベント識別用IDを設定

			triggers_.emplace_back(std::move(trigger)); // コンテナに格納
		}
	}
}

void EventManager::Update() {
	// 全てのトリガーを更新
	for (auto& trigger : triggers_) {
		trigger->Update();
	}
}

bool EventManager::CheckTrigger(const std::string& eventID) {
	for (auto& trigger : triggers_) {
		if (trigger->GetEventID() == eventID && trigger->IsTriggered()) { // 指定したIDのトリガーが引かれていればtrue
			return true;
		}
	}
	return false;
}
