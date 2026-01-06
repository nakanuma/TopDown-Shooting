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
#include <CommandManager.h>

// Application
#include <src/Game/Transition/FadeTransition.h>
#include <src/Game/Transition/SplitBlockTransition.h>
#include <src/Game/Utility/Utility.h>

void TitleScene::Initialize() {
	Cygnus::DirectXBase* dxBase = Cygnus::DirectXBase::GetInstance();

	// カメラのインスタンスを生成
	camera_ = std::make_unique<Cygnus::Camera>(kInitialCameraPosition, kInitialCameraRotation, kCameraFovY);
	Cygnus::Camera::Set(camera_.get()); // 現在のカメラをセット

	// SpriteCommonの生成と初期化
	spriteCommon_ = std::make_unique<Cygnus::SpriteCommon>();
	spriteCommon_->Initialize(Cygnus::DirectXBase::GetInstance());

	// TextureManagerの初期化
	Cygnus::TextureManager::Initialize(dxBase->GetDevice(), Cygnus::SRVManager::GetInstance());

	// SoundManagerの初期化
	soundManager_ = std::make_unique<Cygnus::SoundManager>();
	soundManager_->Initialize();

	// Inputの初期化
	input_ = Cygnus::Input::GetInstance();

	// LightManagerの初期化
	lightManager_ = Cygnus::LightManager::GetInstance();
	lightManager_->Initialize();

	// ポストエフェクト管理
	postEffectManager_ = std::make_unique<Cygnus::PostEffectManager>();
	postEffectManager_->Initialize();
	postEffectManager_->SetEffectType(Cygnus::PSOType::Vignette);

	// ローダー生成
	loader_ = std::make_unique<Loader>();
	loader_->LoadFromFile("resources/Stages/title.json");

	///
	///	スプライト生成
	///

	// タイトル
	uint32_t textureTitle = Cygnus::TextureManager::Load("UI/title.png");
	spriteTitle_ = std::make_unique<Cygnus::Sprite>();
	spriteTitle_->Initialize(spriteCommon_.get(), textureTitle);
	spriteTitle_->SetPosition(kTitlePosition);
	spriteTitle_->SetAnchorPoint(kAnchorPoint);

	uint32_t textureStart = Cygnus::TextureManager::Load("UI/startButton.png");
	spriteStartButton_ = std::make_unique<Cygnus::Sprite>();
	spriteStartButton_->Initialize(spriteCommon_.get(), textureStart);
	spriteStartButton_->SetPosition(kStartButtonPosition);
	spriteStartButton_->SetAnchorPoint(kAnchorPoint);

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
	FadeTransition::GetInstance()->StartFadeIn(kFadeInDuration, kFadeInDelay);

	///
	///	その他
	///

	// シャドウマップ生成
	shadowMapHandle_ = Cygnus::ShadowMapManager::GetInstance()->CreateShadowMap(Cygnus::Window::GetWidth(), Cygnus::Window::GetHeight());

	// 平行光源の初期値設定
	Cygnus::LightManager::GetInstance()->directionalLightCB_.data_->direction = kDirectionalLightDirection;
	Cygnus::LightManager::GetInstance()->directionalLightCB_.data_->intensity = kDirectionalLightIntensity;
}

void TitleScene::Finalize() {}

void TitleScene::Update() {
	// 中心を向きながらカメラ回転
	UpdateOrbitCamera(kCameraTargetPosition, kOrbitCameraRadius, kOrbitCameraHeight, kOrbitCameraSpeed);

	// 左クリック入力でゲームシーンへ移行
	if (input_->IsTriggerMouse(0) && SplitBlockTransition::GetInstance()->IsFinished() && Utility::IsInsideClientCursor()) {
		SplitBlockTransition::GetInstance()->StartClose(
			kSplitBlockCloseDuration,
			[]() {
				Cygnus::SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
				Cygnus::ParticleEffectManager::GetInstance()->Clear();
			},
			kSplitBlockCloseDelay);
	}

	///
	///	オブジェクト更新
	///

	// 床の更新
	field_->Update();
	// 障害物の更新
	obstacleManager_->Update({ 0.0f, 0.0f, 0.0f });

	// パーティクルエフェクトマネージャー更新
	Cygnus::ParticleEffectManager::GetInstance()->Update(Cygnus::TimeManager::GetInstance()->GetDeltaTime());

	///
	///	スプライト更新
	///

	spriteTitle_->Update();

	// タイトルの上下移動
	static float floatTimer = 0.0f;
	floatTimer += Cygnus::TimeManager::GetInstance()->GetDeltaTime();
	float floatAmount = sinf(floatTimer * kTitleFloatSpeed) * kTitleFloatAmplitude;
	spriteTitle_->SetPosition({ kTitleBasePosition.x, kTitleBasePosition.y + floatAmount });

	spriteStartButton_->Update();

	// スタートボタンを点滅
	static float blinkTimer = 0.0f;
	blinkTimer += Cygnus::TimeManager::GetInstance()->GetDeltaTime();
	float alpha = (sinf(blinkTimer * kStartButtonBlinkSpeed) + kStartButtonBlinkOffset) / kStartButtonBlinkScale;
	spriteStartButton_->SetColor({ kStartButtonBaseColor.x, kStartButtonBaseColor.y, kStartButtonBaseColor.z, alpha });

	// トランジション更新
	SplitBlockTransition::GetInstance()->Update();
	FadeTransition::GetInstance()->Update();

	// SkyBox更新
	Cygnus::SkyBoxManager::GetInstance()->Update();

	///
	///	一旦決め打ちでパーティクル発生
	///

	// タンク煙
	static int32_t smokeCounter = 0;
	smokeCounter++;
	if (smokeCounter % kSmokeEmitInterval == 0) {
		Cygnus::ParticleEffectManager::GetInstance()->Emit("smoke", kSmokePosition1, kSmokeEmitCount);
		Cygnus::ParticleEffectManager::GetInstance()->Emit("smoke", kSmokePosition2, kSmokeEmitCount);
		Cygnus::ParticleEffectManager::GetInstance()->Emit("smoke", kSmokePosition3, kSmokeEmitCount);
	}

	// タンク火花
	static int sparkCounter = 0;
	static int nextSparkInterval = 0;
	sparkCounter++;
	if (sparkCounter >= nextSparkInterval) {
		Cygnus::ParticleEffectManager::GetInstance()->Emit("spark", kSparkPosition1, kSparkEmitCount, kSparkDirection1);
		Cygnus::ParticleEffectManager::GetInstance()->Emit("spark", kSparkPosition2, kSparkEmitCount, kSparkDirection2);
		Cygnus::ParticleEffectManager::GetInstance()->Emit("spark", kSparkPosition3, kSparkEmitCount, kSparkDirection3);

		// 次の間隔をランダムに設定
		nextSparkInterval = Cygnus::RandomGenerator::GetInstance()->RandomValue(kMinSparkInterval, kMaxSparkInterval);
		sparkCounter = 0;
	}
}

void TitleScene::Draw() {
	Cygnus::DirectXBase* dxBase = Cygnus::DirectXBase::GetInstance();
	Cygnus::SRVManager* srvManager = Cygnus::SRVManager::GetInstance();
	auto* cmd = Cygnus::CommandManager::GetInstance()->GetCommandList();

	// 描画前処理
	dxBase->PreDraw();
	// 描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvManager->descriptorHeap_.heap_.Get() };
	cmd->SetDescriptorHeaps(1, descriptorHeaps);
	// ImGuiのフレーム開始処理
	Cygnus::ImguiWrapper::NewFrame();
	// カメラの定数バッファを設定
	Cygnus::Camera::TransferConstantBuffer();
	// ライトの定数バッファを設定
	lightManager_->TransferContantBuffer();
	// ポストエフェクト用の定数バッファを設定
	postEffectManager_->TransfarConstantBuffer();
	// LightCameraの定数バッファを送信
	Cygnus::LightCamera::GetInstance()->TransferConstantBuffer();

	// スカイボックス描画
	Cygnus::SkyBoxManager::GetInstance()->Draw();

	///
	///	シャドウマップ描画
	///

	// ライトカメラの更新
	Cygnus::LightCamera::GetInstance()->SetDirectionalLight(Cygnus::LightManager::GetInstance()->directionalLightCB_.data_->direction);

	// BBを生成してライトカメラの行列更新（あとで整理）
	Cygnus::LightCamera::BoundingBox sceneBB;
	sceneBB.SetCenterExtents({ 0.0f, 0.0f, 0.0f }, kShadowBoundingBoxExtents);
	Cygnus::LightCamera::GetInstance()->UpdateViewProjection(sceneBB);

	// シャドウマップ描画開始
	Cygnus::ShadowMapManager::GetInstance()->BeginShadowPass(shadowMapHandle_);

	// 通常オブジェクト描画
	//----------------------------------//

	obstacleManager_->DrawShadow({ 0.0f, 0.0f, 0.0f });

	//----------------------------------//

	// アニメーションモデル用PSOをセット
	cmd->SetPipelineState(Cygnus::ShadowMapManager::GetInstance()->GetShadowSkinnedPSO());

	// アニメーションモデル描画
	//----------------------------------//

	//----------------------------------//

	// シャドウマップ描画終了
	Cygnus::ShadowMapManager::GetInstance()->EndShadowPass(shadowMapHandle_);

	///
	///	↓ ここから3Dオブジェクトの描画コマンド
	///

	field_->Draw();
	obstacleManager_->Draw({ 0.0f, 0.0f, 0.0f });
	Cygnus::ParticleEffectManager::GetInstance()->Draw();

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
		Cygnus::SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}
	if (ImGui::Button("RESULT")) {
		Cygnus::SceneManager::GetInstance()->ChangeScene("RESULT");
	}

	ImGui::DragFloat3("camera.translate", &camera_->transform_.translate_.x, 0.01f);
	ImGui::DragFloat3("camera.rotate", &camera_->transform_.rotate_.x, 0.01f);

	ImGui::Separator();

	ImGui::Separator();

	ImGui::End();

#endif
	// ImGuiの内部コマンドを生成する
	Cygnus::ImguiWrapper::Render(cmd);
	// 描画後処理
	dxBase->PostDraw();
	// フレーム終了処理
	dxBase->EndFrame();
}

void TitleScene::UpdateOrbitCamera(const Cygnus::Float3& target, float radius, float height, float speed) {
	static float angle = 0.0f;
	angle += Cygnus::TimeManager::GetInstance()->GetDeltaTime() * speed;

	// 新しいカメラ位置を円運動で計算
	Cygnus::Float3 cameraPos = { std::cosf(angle) * radius, height, std::sinf(angle) * radius };
	camera_->transform_.translate_ = cameraPos;

	// ターゲットを向くように回転を計算
	Cygnus::Float3 forward = Cygnus::Float3::Normalize(target - cameraPos);

	camera_->transform_.rotate_.y = std::atan2f(forward.x, forward.z);
	camera_->transform_.rotate_.x = std::asinf(-forward.y);
}