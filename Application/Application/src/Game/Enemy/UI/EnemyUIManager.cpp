#include "EnemyUIManager.h"

void EnemyUIManager::Initialize() {
	// スプライト基盤機能生成
	spriteCommon_ = std::make_unique<Cygnus::SpriteCommon>();
	spriteCommon_->Initialize(Cygnus::DirectXBase::GetInstance());

	///
	/// 各種UIパーツ生成・初期化
	/// 
	
	// HPバー
	hpBar_ = std::make_unique<EnemyHPBar>();
	hpBar_->Initialize(spriteCommon_.get());

	// リロードバー
	reloadBar_ = std::make_unique<EnemyReloadBar>();
	reloadBar_->Initialize(spriteCommon_.get());

	// 発見アイコン
	detectionIcon_ = std::make_unique<EnemyDetectionIcon>();
	detectionIcon_->Initialize(spriteCommon_.get());

	// リロードアイコン
	reloadIcon_ = std::make_unique<EnemyReloadIcon>();
	reloadIcon_->Initialize(spriteCommon_.get());
}

void EnemyUIManager::Update(const EnemyUIState& state) {
	// HPバー更新
	hpBar_->Update(state.worldPos, state.hpRatio);

	// リロードバー更新
	reloadBar_->Update(state.worldPos, state.reloadRatio, state.isReloading);

	// 発見アイコン更新
	detectionIcon_->Update(state.worldPos);

	// リロードアイコン更新
	reloadIcon_->Update(state.worldPos, state.isReloading);
}

void EnemyUIManager::Draw() {
	// HPバー描画
	hpBar_->Draw();

	// リロードバー描画
	reloadBar_->Draw();

	// 発見アイコン描画
	detectionIcon_->Draw();

	// リロードアイコン描画
	reloadIcon_->Draw();
}