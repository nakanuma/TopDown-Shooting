#include "ResultScene.h"

// C++
#include <numbers>

// Engine
#include <ImguiWrapper.h>
#include <SceneManager.h>
#include <Engine/Util/TimeManager.h>

// Application
#include <src/Game/Transition/FadeTransition.h>
#include <src/Game/System/ResultStats.h>

void ResultScene::Initialize() {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// カメラのインスタンスを生成
	camera = std::make_unique<Camera>(Float3{ 0.0f, 30.0f, -50.0f }, Float3{ 0.5f, 0.0f, 0.0f }, 0.45f);
	Camera::Set(camera.get()); // 現在のカメラをセット

	// デバッグカメラの生成と初期化
	debugCamera = std::make_unique<DebugCamera>();
	debugCamera->Initialize();

	// SpriteCommonの生成と初期化
	spriteCommon = std::make_unique<SpriteCommon>();
	spriteCommon->Initialize(DirectXBase::GetInstance());

	// TextureManagerの初期化
	TextureManager::Initialize(dxBase->GetDevice(), SRVManager::GetInstance());

	// SoundManagerの初期化
	soundManager = std::make_unique<SoundManager>();
	soundManager->Initialize();

	// Inputの初期化
	input = Input::GetInstance();

	// LightManagerの初期化
	lightManager = LightManager::GetInstance();
	lightManager->Initialize();

	///
	///	スプライト生成
	/// 
	
	// タイトルボタン
	uint32_t textureTitleButton = TextureManager::Load("resources/Images/UI/titleButton.png", dxBase->GetDevice());
	spriteTitleButton_ = std::make_unique<Sprite>();
	spriteTitleButton_->Initialize(spriteCommon.get(), textureTitleButton);
	spriteTitleButton_->SetPosition({ 640.0f, 620.0f });
	spriteTitleButton_->SetAnchorPoint({ 0.5f, 0.5f });

	///
	///	オブジェクト
	/// 

	///
	///	フェード
	/// 

	FadeTransition::GetInstance()->Initialize(spriteCommon.get());
	FadeTransition::GetInstance()->StartFadeIn(1.0f);
}

void ResultScene::Finalize() {}

void ResultScene::Update() {
	/*if (input->IsTriggerMouse(0)) {
		FadeTransition::GetInstance()->StartFadeOut(1.0f, []() {
			SceneManager::GetInstance()->ChangeScene("TITLE");
			}, 0.2f);
		ResultStats::GetInstance()->Clear();
	}*/

	// フェード更新
	FadeTransition::GetInstance()->Update();

	// スプライト更新
	spriteTitleButton_->Update();

#ifdef _DEBUG
	// デバッグカメラ更新
	DebugCameraUpdate(input);
#endif
}

void ResultScene::Draw() {
	DirectXBase* dxBase = DirectXBase::GetInstance();
	SRVManager* srvManager = SRVManager::GetInstance();

	// 描画前処理
	dxBase->PreDraw();
	// 描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvManager->descriptorHeap.heap_.Get() };
	dxBase->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
	// ImGuiのフレーム開始処理
	ImguiWrapper::NewFrame();
	// カメラの定数バッファを設定
	Camera::TransferConstantBuffer();
	// ライトの定数バッファを設定
	lightManager->TransferContantBuffer();

	///
	///	↓ ここから3Dオブジェクトの描画コマンド
	///

	///
	///	↑ ここまで3Dオブジェクトの描画コマンド
	///

	// Spriteの描画準備。全ての描画に共通のグラフィックスコマンドを積む
	spriteCommon->PreDraw();

	///
	/// ↓ ここからスプライトの描画コマンド
	///
	
	spriteTitleButton_->Draw();

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
	ImGui::Checkbox("useDebugCamera", &useDebugCamera);

	if (ImGui::Button("TITLE")) {
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}
	if (ImGui::Button("GAMEPLAY")) {
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}

	ImGui::DragFloat3("camera.translate", &camera->transform.translate.x, 0.01f);
	ImGui::DragFloat3("camera.rotate", &camera->transform.rotate.x, 0.01f);

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

#ifdef _DEBUG
void ResultScene::DebugCameraUpdate(Input* input) {
	// 前回のカメラモード状態を保持
	static bool prevUseDebugCamera = false;

	// デバッグカメラが有効になった瞬間に通常カメラのTransformを保存
	if (useDebugCamera && !prevUseDebugCamera) {
		savedCameraTransform = camera->transform;
	}

	// デバッグカメラが有効の場合
	if (useDebugCamera) {
		// デバッグカメラの更新
		debugCamera->Update(input);
		// 通常カメラにデバッグカメラのTransformを適用
		camera->transform = debugCamera->transform_;
	} else if (!useDebugCamera && prevUseDebugCamera) {
		// 通常カメラのTransformを再現
		camera->transform = savedCameraTransform;
	}

	// 現在のカメラモードを保存して次のフレームで使う
	prevUseDebugCamera = useDebugCamera;
}
#endif // _DEBUG
