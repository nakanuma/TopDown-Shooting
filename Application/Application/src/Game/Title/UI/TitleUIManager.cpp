#include "TitleUIManager.h"

void TitleUIManager::Initialize() {
	// スプライト基盤機能生成
	spriteCommon_ = std::make_unique<Cygnus::SpriteCommon>();
	spriteCommon_->Initialize(Cygnus::DirectXBase::GetInstance());

	///
	/// 各種UIパーツ生成・初期化
	/// 
	
	// タイトルロゴ
	titleLogo_ = std::make_unique<TitleLogo>();
	titleLogo_->Initialize(spriteCommon_.get());

	// スタートボタン
	startButton_ = std::make_unique<StartButton>();
	startButton_->Initialize(spriteCommon_.get());

	// 終了ボタン
	exitButton_ = std::make_unique<ExitButton>();
	exitButton_->Initialize(spriteCommon_.get());

	// マウスカーソル
	cursor_ = std::make_unique<MouseCursor>();
	cursor_->Initialize();
}

void TitleUIManager::Update() {
	// タイトルロゴ更新
	titleLogo_->Update();

	// スタートボタン更新
	startButton_->Update();

	// 終了ボタン更新
	exitButton_->Update();

	// マウスカーソル更新
	cursor_->Update();
}

void TitleUIManager::Draw() {
	// タイトルロゴ描画
	titleLogo_->Draw();

	// スタートボタン描画
	startButton_->Draw();

	// 終了ボタン描画
	exitButton_->Draw();

	// マウスカーソル描画
	cursor_->Draw();
}