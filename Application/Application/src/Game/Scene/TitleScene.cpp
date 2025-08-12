#include "TitleScene.h"

// C++
#include <numbers>

// Engine
#include <ImguiWrapper.h>
#include <SceneManager.h>
#include <Engine/Util/TimeManager.h>

void TitleScene::Initialize() {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// カメラのインスタンスを生成
	camera = std::make_unique<Camera>(Float3{0.0f, 30.0f, -50.0f}, Float3{0.5f, 0.0f, 0.0f}, 0.45f);
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

	// タイトル
	uint32_t textureTitle = TextureManager::Load("resources/Images/UI/title.png", dxBase->GetDevice());
	spriteTitle_ = std::make_unique<Sprite>();
	spriteTitle_->Initialize(spriteCommon.get(), textureTitle);
	spriteTitle_->SetPosition({ 640.0f, 100.0f });
	spriteTitle_->SetAnchorPoint({ 0.5f, 0.5f });

	// スタートボタン
	uint32_t textureStart = TextureManager::Load("resources/Images/UI/startButton.png", dxBase->GetDevice());
	spriteStartButton_ = std::make_unique<Sprite>();
	spriteStartButton_->Initialize(spriteCommon.get(), textureStart);
	spriteStartButton_->SetPosition({ 640.0f, 620.0f });
	spriteStartButton_->SetAnchorPoint({ 0.5f, 0.5f });

	///
	///	オブジェクト
	/// 
	
	modelDiorama_ = ModelManager::LoadModelFile("resources/Models", "Object/Diorama/diorama.obj", dxBase->GetDevice());
	modelDiorama_.material.textureHandle = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());

	objectDiorama_ = std::make_unique<Object3D>();
	objectDiorama_->model_ = &modelDiorama_;
	objectDiorama_->materialCB_.data_->color = { 0.25f, 0.25f, 0.25f, 1.0f };
}

void TitleScene::Finalize() {}

void TitleScene::Update() {
	// オブジェクト更新
	objectDiorama_->UpdateMatrix();
	// 回転
	objectDiorama_->transform_.rotate.y += 0.5f * TimeManager::GetInstance()->GetDeltaTime();
	// 上下振幅
	static float timer = 0.0f;
	timer += TimeManager::GetInstance()->GetDeltaTime() * 2.0f;
	float yoffset = sinf(timer) * 1.0f;
	objectDiorama_->transform_.translate.y = yoffset;


	// スプライト更新
	spriteTitle_->Update();
	spriteStartButton_->Update();

#ifdef _DEBUG
	// デバッグカメラ更新
	DebugCameraUpdate(input);
#endif
}

void TitleScene::Draw() {
	DirectXBase* dxBase = DirectXBase::GetInstance();
	SRVManager* srvManager = SRVManager::GetInstance();

	// 描画前処理
	dxBase->PreDraw();
	// 描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = {srvManager->descriptorHeap.heap_.Get()};
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
	
	objectDiorama_->Draw();

	///
	///	↑ ここまで3Dオブジェクトの描画コマンド
	///

	// Spriteの描画準備。全ての描画に共通のグラフィックスコマンドを積む
	spriteCommon->PreDraw();

	///
	/// ↓ ここからスプライトの描画コマンド
	///

	spriteTitle_->Draw();
	spriteStartButton_->Draw();

	///
	/// ↑ ここまでスプライトの描画コマンド
	///

	///
	///	デバッグ表示
	///
#ifdef _DEBUG
	ImGui::Begin("TitleSceneInfo");

	ImGui::Text("fps:%.2f", ImGui::GetIO().Framerate);
	ImGui::Checkbox("useDebugCamera", &useDebugCamera);

	if (ImGui::Button("GAMEPLAY")) {
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}

	ImGui::DragFloat3("camera.translate", &camera->transform.translate.x, 0.01f);
	ImGui::DragFloat3("camera.rotate", &camera->transform.rotate.x, 0.01f);

	ImGui::End();

#endif
	// ImGuiの内部コマンドを生成する
	ImguiWrapper::Render(dxBase->GetCommandList());
	// 描画後処理
	dxBase->PostDraw();
	// フレーム終了処理
	dxBase->EndFrame();
}

#ifdef _DEBUG
void TitleScene::DebugCameraUpdate(Input* input) {
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
