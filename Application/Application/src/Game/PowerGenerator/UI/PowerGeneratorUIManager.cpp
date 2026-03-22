#include "PowerGeneratorUIManager.h"

void PowerGeneratorUIManager::Initialize() {
	// スプライト基盤機能生成
	spriteCommon_ = std::make_unique<Cygnus::SpriteCommon>();
	spriteCommon_->Initialize(Cygnus::DirectXBase::GetInstance());

	///
	/// 各種UIパーツ生成・初期化
	/// 

	// HPバー
	hpBar_ = std::make_unique<PowerGeneratorHPBar>();
	hpBar_->Initialize(spriteCommon_.get());
}

void PowerGeneratorUIManager::Update(const PowerGeneratorUIState& state) {
	// HPバー更新
	hpBar_->Update(state.worldPos, state.hpRatio);
}

void PowerGeneratorUIManager::Draw() {
	// HPバー描画
	hpBar_->Draw();
}