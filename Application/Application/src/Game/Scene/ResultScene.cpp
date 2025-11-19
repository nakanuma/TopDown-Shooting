#include "ResultScene.h"

// C++
#include <numbers>

// Engine
#include <Engine/Util/TimeManager.h>
#include <ImguiWrapper.h>
#include <LightCamera.h>
#include <SceneManager.h>
#include <ShadowMapManager.h>

// Application
#include <src/Game/Bullet/Manager/BulletManager.h>
#include <src/Game/System/ResultStats.h>
#include <src/Game/Transition/FadeTransition.h>

void ResultScene::Initialize() {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// カメラのインスタンスを生成
	camera_ = std::make_unique<Camera>(kInitialCameraPosition, kInitialCameraRotation, kCameraFovY);
	Camera::Set(camera_.get()); // 現在のカメラをセット

	// SpriteCommonの生成と初期化
	spriteCommon_ = std::make_unique<SpriteCommon>();
	spriteCommon_->Initialize(DirectXBase::GetInstance());

	// TextureManagerの初期化
	TextureManager::Initialize(dxBase->GetDevice(), SRVManager::GetInstance());

	// SoundManagerの初期化
	soundManager_ = std::make_unique<SoundManager>();
	soundManager_->Initialize();

	// Inputの初期化
	input_ = Input::GetInstance();

	// LightManagerの初期化
	lightManager_ = LightManager::GetInstance();
	lightManager_->Initialize();

	///
	///	スプライト生成
	///

	// 背景
	uint32_t textureBackGround = TextureManager::Load("white.png");
	spriteBackGround_ = std::make_unique<Sprite>();
	spriteBackGround_->Initialize(spriteCommon_.get(), textureBackGround);
	spriteBackGround_->SetColor(kBackgroundColor);
	spriteBackGround_->SetSize(kBackgroundSize);

	// タイトルボタン
	uint32_t textureTitleButton = TextureManager::Load("UI/titleButton.png");
	spriteTitleButton_ = std::make_unique<Sprite>();
	spriteTitleButton_->Initialize(spriteCommon_.get(), textureTitleButton);
	spriteTitleButton_->SetPosition(kTitleButtonPosition);
	spriteTitleButton_->SetAnchorPoint(kAnchorPoint);

	// 戦績
	uint32_t textureRecord = TextureManager::Load("UI/record.png");
	spriteRecord_ = std::make_unique<Sprite>();
	spriteRecord_->Initialize(spriteCommon_.get(), textureRecord);
	spriteRecord_->SetPosition(kRecordPosition);
	spriteRecord_->SetAnchorPoint(kAnchorPoint);

	// 合計ダメージ
	spriteTotalDamage_ = std::make_unique<NumberSprite>();
	spriteTotalDamage_->Initialize(static_cast<float>(ResultStats::GetInstance()->GetTotalDamage()));
	// キル数
	spriteDefeated_ = std::make_unique<NumberSprite>();
	spriteDefeated_->Initialize(static_cast<float>(ResultStats::GetInstance()->GetDefeated()));
	// 命中率
	spriteHitRate_ = std::make_unique<NumberSprite>();
	spriteHitRate_->Initialize(ResultStats::GetInstance()->GetHitRate(), kDecimalPlaces);
	// クリアタイム
	spriteClearTime_ = std::make_unique<NumberSprite>();
	spriteClearTime_->Initialize(ResultStats::GetInstance()->GetClearTime(), kDecimalPlaces);

	///
	///	フェード
	///

	FadeTransition::GetInstance()->Initialize(spriteCommon_.get());
	FadeTransition::GetInstance()->StartFadeIn(kFadeInDuration);

	///
	///	その他
	///

	// シャドウマップ生成
	shadowMapHandle_ = ShadowMapManager::GetInstance()->CreateShadowMap(Window::GetWidth(), Window::GetHeight());
}

void ResultScene::Finalize() {}

void ResultScene::Update() {
	if (input_->IsTriggerMouse(0) && FadeTransition::GetInstance()->IsFinished()) {
		FadeTransition::GetInstance()->StartFadeOut(kFadeOutDuration, []() { SceneManager::GetInstance()->ChangeScene("TITLE"); }, kFadeOutDelay);
		ResultStats::GetInstance()->Clear();   // 戦績をクリア
		BulletManager::GetInstance()->Clear(); // 弾リストをクリア
	}

	// フェード更新
	FadeTransition::GetInstance()->Update();

	// スプライト更新
	spriteBackGround_->Update();
	spriteTitleButton_->Update();
	spriteRecord_->Update();

	spriteTotalDamage_->Update(kTotalDamatePosition);
	spriteDefeated_->Update(kDefeatedPosition);
	spriteHitRate_->Update(kHitRatePosition);
	spriteClearTime_->Update(kClearTimePosition);
}

void ResultScene::Draw() {
	DirectXBase* dxBase = DirectXBase::GetInstance();
	SRVManager* srvManager = SRVManager::GetInstance();

	// 描画前処理
	dxBase->PreDraw();
	// 描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvManager->descriptorHeap_.heap_.Get() };
	dxBase->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
	// ImGuiのフレーム開始処理
	ImguiWrapper::NewFrame();
	// カメラの定数バッファを設定
	Camera::TransferConstantBuffer();
	// ライトの定数バッファを設定
	lightManager_->TransferContantBuffer();
	// LightCameraの定数バッファを送信
	LightCamera::GetInstance()->TransferConstantBuffer();

	///
	///	↓ ここから3Dオブジェクトの描画コマンド
	///

	///
	///	↑ ここまで3Dオブジェクトの描画コマンド
	///

	// Spriteの描画準備。全ての描画に共通のグラフィックスコマンドを積む
	spriteCommon_->PreDraw();

	///
	/// ↓ ここからスプライトの描画コマンド
	///

	spriteBackGround_->Draw();
	spriteTitleButton_->Draw();
	spriteRecord_->Draw();

	spriteTotalDamage_->Draw();
	spriteDefeated_->Draw();
	spriteHitRate_->Draw();
	spriteClearTime_->Draw();

	// フェード描画
	FadeTransition::GetInstance()->Draw();

	///
	/// ↑ ここまでスプライトの描画コマンド
	///

	///
	///	デバッグ表示
	///
#ifdef _DEBUG
	ImGui::Begin("ResultSceneInfo");

	ImGui::Text("fps:%.2f", ImGui::GetIO().Framerate);

	if (ImGui::Button("TITLE")) {
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}
	if (ImGui::Button("GAMEPLAY")) {
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}

	ImGui::DragFloat3("camera.translate", &camera_->transform_.translate_.x, 0.01f);
	ImGui::DragFloat3("camera.rotate", &camera_->transform_.rotate_.x, 0.01f);

	ImGui::End();

	// デバッグ表示
	ResultStats::GetInstance()->Debug();

#endif
	// ImGuiの内部コマンドを生成する
	ImguiWrapper::Render(dxBase->GetCommandList());
	// 描画後処理
	dxBase->PostDraw();
	// フレーム終了処理
	dxBase->EndFrame();
}
