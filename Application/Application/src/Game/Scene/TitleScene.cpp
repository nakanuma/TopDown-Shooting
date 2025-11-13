#include "TitleScene.h"

// C++
#include <numbers>

// Engine
#include <Engine/3D/LightCamera.h>
#include <Engine/Util/TimeManager.h>
#include <ImguiWrapper.h>
#include <ParticleEffect/ParticleEffectManager.h>
#include <RandomGenerator.h>
#include <SceneManager.h>
#include <ShadowMapManager.h>
#include <SkyBoxManager.h>

// Application
#include <src/Game/Transition/FadeTransition.h>
#include <src/Game/Transition/SplitBlockTransition.h>
#include <src/Game/Utility/Utility.h>

void TitleScene::Initialize() {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// カメラのインスタンスを生成
	camera_ = std::make_unique<Camera>(Float3{ 0.0f, 30.0f, -50.0f }, Float3{ 0.5f, 0.0f, 0.0f }, 0.45f);
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

	// ローダー生成
	loader_ = std::make_unique<Loader>();
	loader_->LoadFromFile("resources/Stages/title.json");

	///
	///	スプライト生成
	///

	// タイトル
	uint32_t textureTitle = TextureManager::Load("UI/title.png");
	spriteTitle_ = std::make_unique<Sprite>();
	spriteTitle_->Initialize(spriteCommon_.get(), textureTitle);
	spriteTitle_->SetPosition({ 640.0f, 140.0f });
	spriteTitle_->SetAnchorPoint({ 0.5f, 0.5f });

	uint32_t textureStart = TextureManager::Load("UI/startButton.png");
	spriteStartButton_ = std::make_unique<Sprite>();
	spriteStartButton_->Initialize(spriteCommon_.get(), textureStart);
	spriteStartButton_->SetPosition({ 640.0f, 580.0f });
	spriteStartButton_->SetAnchorPoint({ 0.5f, 0.5f });

	///
	///	オブジェクト
	///

	// 床生成
	field_ = std::make_unique<Field>();
	field_->Initialize();

	// 障害物の管理クラス生成
	obstacleManager_ = std::make_unique<ObstacleManager>();
	obstacleManager_->Initialize(loader_->GetAllDatas());

	///
	///	スプライト
	///

	spriteTitle_->Update();
	spriteStartButton_->Update();

	///
	///	トランジション
	///

	SplitBlockTransition::GetInstance()->Initialize(spriteCommon_.get());
	FadeTransition::GetInstance()->Initialize(spriteCommon_.get());
	FadeTransition::GetInstance()->StartFadeIn(0.5f, 0.25f);

	///
	///	その他
	///

	// シャドウマップ生成
	shadowMapHandle_ = ShadowMapManager::GetInstance()->CreateShadowMap(Window::GetWidth(), Window::GetHeight());

	// 平行光源の初期値設定
	LightManager::GetInstance()->directionalLightCB_.data_->direction = { 0.367f, -0.653f, -0.662f };
	LightManager::GetInstance()->directionalLightCB_.data_->intensity = 0.4f;
}

void TitleScene::Finalize() {}

void TitleScene::Update() {
	// 中心を向きながらカメラ回転
	UpdateOrbitCamera({ 0.0f, 0.0f, 0.0f }, 50.0f, 30.0f, 0.25f);

	// 左クリック入力でゲームシーンへ移行
	if (input_->IsTriggerMouse(0) && SplitBlockTransition::GetInstance()->IsFinished() && Utility::IsInsideClientCursor()) {
		SplitBlockTransition::GetInstance()->StartClose(
			1.0f,
			[]() {
				SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
				ParticleEffectManager::GetInstance()->Clear();
			},
			0.5f);
	}

	///
	///	オブジェクト更新
	///

	// 床の更新
	field_->Update();
	// 障害物の更新
	obstacleManager_->Update({ 0.0f, 0.0f, 0.0f });

	// パーティクルエフェクトマネージャー更新
	ParticleEffectManager::GetInstance()->Update(TimeManager::GetInstance()->GetDeltaTime());

	///
	///	スプライト更新
	///

	spriteTitle_->Update();

	// タイトルの上下移動
	static float floatTimer = 0.0f;
	floatTimer += TimeManager::GetInstance()->GetDeltaTime();
	float floatAmount = sinf(floatTimer * 1.2f) * 4.0f;
	Float3 basePos = { 640.0f, 140.0f };
	spriteTitle_->SetPosition({ basePos.x, basePos.y + floatAmount });

	spriteStartButton_->Update();

	// スタートボタンを点滅
	static float blinkTimer = 0.0f;
	blinkTimer += TimeManager::GetInstance()->GetDeltaTime();
	float alpha = (sinf(blinkTimer * 4.0f) + 1.0f) / 2.0f;
	spriteStartButton_->SetColor({ 1.0f, 1.0f, 1.0f, alpha });

	// トランジション更新
	SplitBlockTransition::GetInstance()->Update();
	FadeTransition::GetInstance()->Update();

	// SkyBox更新
	SkyBoxManager::GetInstance()->Update();

	///
	///	一旦決め打ちでパーティクル発生
	///

	// タンク煙
	static int smokeCounter = 0;
	smokeCounter++;
	if (smokeCounter % 5 == 0) {
		ParticleEffectManager::GetInstance()->Emit("smoke", { 12.0f, 4.0f, -12.0f }, 1);
		ParticleEffectManager::GetInstance()->Emit("smoke", { -14.0f, 4.0f, 12.0f }, 1);
		ParticleEffectManager::GetInstance()->Emit("smoke", { 34.0f, 4.0f, 8.0f }, 1);
	}

	// タンク火花
	static int sparkCounter = 0;
	static int nextSparkInterval = 0;
	sparkCounter++;
	if (sparkCounter >= nextSparkInterval) {
		ParticleEffectManager::GetInstance()->Emit("spark", { 25.0f, 4.0f, 16.0f }, 5, { 1.0f, 0.0f, 0.0f });
		ParticleEffectManager::GetInstance()->Emit("spark", { -31.0f, 3.0f, 11.0f }, 5, { -1.0f, 0.0f, 0.0f });
		ParticleEffectManager::GetInstance()->Emit("spark", { -31.0f, 4.0f, -26.0f }, 5, { -1.0f, 0.0f, 0.0f });

		// 次の間隔をランダムに設定
		nextSparkInterval = RandomGenerator::GetInstance()->RandomValue(20, 50);
		sparkCounter = 0;
	}
}

void TitleScene::Draw() {
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

	// スカイボックス描画
	SkyBoxManager::GetInstance()->Draw();

	///
	///	シャドウマップ描画
	///

	// ライトカメラの更新
	LightCamera::GetInstance()->SetDirectionalLight(LightManager::GetInstance()->directionalLightCB_.data_->direction);

	// BBを生成してライトカメラの行列更新（あとで整理）
	LightCamera::BoundingBox sceneBB;
	sceneBB.SetCenterExtents({ 0.0f, 0.0f, 0.0f }, { 30.0f, 10.0f, 30.0f });
	LightCamera::GetInstance()->UpdateViewProjection(sceneBB);

	// シャドウマップ描画開始
	ShadowMapManager::GetInstance()->BeginShadowPass(shadowMapHandle_);

	// 通常オブジェクト描画
	//----------------------------------//

	obstacleManager_->DrawShadow({ 0.0f, 0.0f, 0.0f });

	//----------------------------------//

	// アニメーションモデル用PSOをセット
	dxBase->GetCommandList()->SetPipelineState(ShadowMapManager::GetInstance()->GetShadowSkinnedPSO());

	// アニメーションモデル描画
	//----------------------------------//

	//----------------------------------//

	// シャドウマップ描画終了
	ShadowMapManager::GetInstance()->EndShadowPass(shadowMapHandle_);

	///
	///	↓ ここから3Dオブジェクトの描画コマンド
	///

	field_->Draw();
	obstacleManager_->Draw({ 0.0f, 0.0f, 0.0f });

	ParticleEffectManager::GetInstance()->Draw();

	///
	///	↑ ここまで3Dオブジェクトの描画コマンド
	///

	// Spriteの描画準備。全ての描画に共通のグラフィックスコマンドを積む
	spriteCommon_->PreDraw();

	///
	/// ↓ ここからスプライトの描画コマンド
	///

	spriteTitle_->Draw();

	if (FadeTransition::GetInstance()->IsFinished()) {
		spriteStartButton_->Draw();
	}

	// トランジション描画
	SplitBlockTransition::GetInstance()->Draw();
	FadeTransition::GetInstance()->Draw();

	///
	/// ↑ ここまでスプライトの描画コマンド
	///

	///
	///	デバッグ表示
	///
#ifdef _DEBUG
	ImGui::Begin("TitleSceneInfo");

	ImGui::Text("fps:%.2f", ImGui::GetIO().Framerate);

	if (ImGui::Button("GAMEPLAY")) {
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}
	if (ImGui::Button("RESULT")) {
		SceneManager::GetInstance()->ChangeScene("RESULT");
	}

	ImGui::DragFloat3("camera.translate", &camera_->transform_.translate_.x, 0.01f);
	ImGui::DragFloat3("camera.rotate", &camera_->transform_.rotate_.x, 0.01f);

	ImGui::Separator();

	ImGui::Separator();

	ImGui::End();

#endif
	// ImGuiの内部コマンドを生成する
	ImguiWrapper::Render(dxBase->GetCommandList());
	// 描画後処理
	dxBase->PostDraw();
	// フレーム終了処理
	dxBase->EndFrame();
}

void TitleScene::UpdateOrbitCamera(const Float3& target, float radius, float height, float speed) {
	static float angle = 0.0f;
	angle += TimeManager::GetInstance()->GetDeltaTime() * speed;

	// 新しいカメラ位置を円運動で計算
	Float3 cameraPos = { std::cosf(angle) * radius, height, std::sinf(angle) * radius };
	camera_->transform_.translate_ = cameraPos;

	// ターゲットを向くように回転を計算
	Float3 forward = Float3::Normalize(target - cameraPos);

	camera_->transform_.rotate_.y = std::atan2f(forward.x, forward.z);
	camera_->transform_.rotate_.x = std::asinf(-forward.y);
}