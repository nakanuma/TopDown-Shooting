#include "GamePlayScene.h"
#include "DirectXBase.h"
#include "ImguiWrapper.h"
#include "RTVManager.h"
#include "SRVManager.h"
#include "SpriteCommon.h"
#include <Engine/3D/LightCamera.h>
#include <Engine/3D/LineDrawer.h>
#include <Engine/DirectX/ShadowMapManager.h>
#include <Engine/Model/SkyBoxManager.h>
#include <Engine/ParticleEffect/ParticleEffectManager.h>

// C++
#include <numbers>

// Engine
#include <Engine/Scene/SceneManager.h>

// Application
#include <src/Game/Camera/CameraShake.h>
#include <src/Game/Transition/FadeTransition.h>
#include <src/Game/Transition/SplitBlockTransition.h>
#include <src/Game/Utility/Utility.h>
#include <src/Game/Waypoint/WaypointManager.h>

#include <src/Game/GameState/GameStart/GameStartState.h>
#include <src/Game/GameState/GamePlay/GamePlayState.h>
#include <src/Game/GameState/GameOver/GameOverState.h>
#include <src/Game/GameState/GameClear/GameClearState.h>

void GamePlayScene::Initialize() {
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

	///
	///	↓ ゲームシーン用
	///

	// 最初にコライダーのクリア
	Cygnus::CollisionManager::GetInstance()->Clear();

	// パーティクルのクリア
	Cygnus::ParticleEffectManager::GetInstance()->Clear();

	/* ステージデータ */

	// ローダー生成
	loader_ = std::make_unique<Loader>();
	loader_->LoadFromFile("resources/Stages/data.json");

	/* オブジェクト関連 */

	// フィールド生成
	field_ = std::make_unique<Field>();
	field_->Initialize();

	// プレイヤー生成
	player_ = std::make_unique<Player>();
	player_->Initialize(loader_->GetDataByTag("PLAYER"));

	// 敵の管理クラス生成
	enemyManager_ = std::make_unique<EnemyManager>();
	enemyManager_->Initialize(loader_->GetAllDatas(), player_.get()); // ローダーから取得したデータを使用

	// 障害物の管理クラス生成
	obstacleManager_ = std::make_unique<ObstacleManager>();
	obstacleManager_->Initialize(loader_->GetAllDatas()); // ローダーから取得したデータを使用

	// テレポーターの管理クラス生成
	teleporterManager_ = std::make_unique<TeleporterManager>();
	teleporterManager_->Initialize(loader_->GetAllDatas());                  // ローダーから取得したデータを使用
	teleporterManager_->SetGoalCallback([this]() { TransitionToResult(); }); // ゴール時のコールバック関数を設定

	// 弾リストのクリア
	BulletManager::GetInstance()->Clear();

	/* その他 */

	// 追従カメラ生成
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize(camera_->GetCurrent()->transform_.translate_); // 初期オフセット
	followCamera_->SetTarget(&player_->GetTranslate());                      // プレイヤーを追従対象にセット

	// ポストエフェクト管理
	postEffectManager_ = std::make_unique<Cygnus::PostEffectManager>();
	postEffectManager_->Initialize();
	postEffectManager_->SetEffectType(Cygnus::PostEffectType::DamageVignette);

	player_->SetPostEffectManager(postEffectManager_.get()); // プレイヤーにポストエフェクトマネージャーをセット

	// トランジション
	FadeTransition::GetInstance()->Initialize(spriteCommon_.get());
	SplitBlockTransition::GetInstance()->StartOpen(kSplitBlockOpenDuration, kSplitBlockOpenDelay);

	// ウェイポイント初期化
	obstacleManager_->Update(player_->GetTranslate()); // レイキャストで障害物のコライダーが必要になるためここで一度更新しておく
	Cygnus::CollisionManager::GetInstance()->Update();         // 障害物のコライダーが未登録状態のためここで一度更新しておく
	WaypointManager::GetInstance()->Initialize();

	// シャドウマップ生成
	shadowMapHandle_ = Cygnus::ShadowMapManager::GetInstance()->CreateShadowMap(Cygnus::Window::GetWidth(), Cygnus::Window::GetHeight());

	// 平行光源の初期値設定
	Cygnus::LightManager::GetInstance()->directionalLightCB_.data_->direction = kDirectionalLightDirection;
	Cygnus::LightManager::GetInstance()->directionalLightCB_.data_->intensity = kDirectionalLightIntensity;

	// ゲーム状態の初期化
	InitializeGameStates();
}

void GamePlayScene::Finalize() {}

void GamePlayScene::Update() {
	Cygnus::LightManager::GetInstance()->ClearEmissiveLights(); // エミッシブライトをクリア
	Cygnus::LightManager::GetInstance()->ClearAreaLights();     // エリアライトをクリア

	// ゲーム状態ごとの更新処理
	stateManager_->Update();

	/* 全ての状態共通で更新するもの */
	// カメラシェイクの更新
	CameraShake::GetInstance()->Update();
	// SkyBox更新
	Cygnus::SkyBoxManager::GetInstance()->Update();
	// コリジョンマネージャーの更新（全てのコライダーの衝突判定）
	Cygnus::CollisionManager::GetInstance()->Update();
	// パーティクルエフェクトマネージャー更新
	Cygnus::ParticleEffectManager::GetInstance()->Update(Cygnus::TimeManager::GetInstance()->GetDeltaTime());
}

void GamePlayScene::Draw() {
	Cygnus::DirectXBase* dxBase = Cygnus::DirectXBase::GetInstance();
	Cygnus::SRVManager* srvManager = Cygnus::SRVManager::GetInstance();

	// 描画前処理
	dxBase->PreDraw();
	// 描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvManager->descriptorHeap_.heap_.Get() };
	dxBase->GetCommandList()->SetDescriptorHeaps(1, descriptorHeaps);
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

	// ---------------------------------------------------------
	// シャドウマップ描画前処理
	// ---------------------------------------------------------

	// ライトカメラの更新
	Cygnus::LightCamera::GetInstance()->SetDirectionalLight(Cygnus::LightManager::GetInstance()->directionalLightCB_.data_->direction);

	// プレイヤー中心のBBを生成してライトカメラの行列更新（あとで整理）
	Cygnus::LightCamera::BoundingBox playerCenterBB;
	playerCenterBB.SetCenterExtents(player_->GetTranslate(), kShadowBoundingBoxExtents);
	Cygnus::LightCamera::GetInstance()->UpdateViewProjection(playerCenterBB);

	// シャドウマップ描画開始
	Cygnus::ShadowMapManager::GetInstance()->BeginShadowPass(shadowMapHandle_);

	/// =========================================================
	/// ↓ ここから通常モデルのシャドウマップ描画
	/// =========================================================

	// ゲーム状態ごとの通常モデルシャドウマップ描画処理
	stateManager_->DrawShadow();

	/// =========================================================
	/// ↑ ここまで通常モデルのシャドウマップ描画
	/// =========================================================

	// スキニングモデル用PSOをセット
	dxBase->GetCommandList()->SetPipelineState(Cygnus::ShadowMapManager::GetInstance()->GetShadowSkinnedPSO());

	/// =========================================================
	/// ↓ ここからスキニングモデルのシャドウマップ描画
	/// =========================================================

	// ゲーム状態ごとのスキニングモデルシャドウマップ描画処理
	stateManager_->DrawShadowSkinning();

	/// =========================================================
	/// ↑ ここまでスキニングモデルのシャドウマップ描画
	/// =========================================================

	Cygnus::ShadowMapManager::GetInstance()->EndShadowPass(shadowMapHandle_);

	/// =========================================================
	/// ↓ ここから3Dオブジェクト描画
	/// =========================================================

#pragma region メインシーンの3Dオブジェクトのレンダリングを開始
	postEffectManager_->BeginMainScene();
	// -----------------------------------------------

	// ゲーム状態ごとの描画処理
	stateManager_->Draw();

	// -----------------------------------------------
	postEffectManager_->EndMainScene();
#pragma endregion

#pragma region バックバッファへの直接描画
	postEffectManager_->RestoreBackBuffer(true);
	// -----------------------------------------------

	Cygnus::ParticleEffectManager::GetInstance()->Draw();
	Cygnus::LineDrawer::GetInstance()->Draw();

	// -----------------------------------------------
	postEffectManager_->RestoreDepthBufferState();
#pragma endregion

	/// =========================================================
	/// ↑ ここまで3Dオブジェクト描画
	/// =========================================================

	// Spriteの描画準備。全ての描画に共通のグラフィックスコマンドを積む
	spriteCommon_->PreDraw();

	/// =========================================================
	/// ↓ ここからスプライト描画
	/// =========================================================

	// ゲーム状態ごとのUI描画処理
	stateManager_->DrawUI();

	/// =========================================================
	/// ↑ ここまでスプライト描画
	/// =========================================================

#ifdef _DEBUG
	// デバッグ表示
	Debug();
#endif
	// ImGuiの内部コマンドを生成する
	Cygnus::ImguiWrapper::Render(dxBase->GetCommandList());
	// 描画後処理
	dxBase->PostDraw();
	// フレーム終了処理
	dxBase->EndFrame();
}

void GamePlayScene::Debug() {
#ifdef USE_IMGUI
	ImGui::Begin("GameSceneInfo");

	ImGui::Text("fps:%.2f", ImGui::GetIO().Framerate);
	ImGui::DragFloat3("camera.translate", &camera_->transform_.translate_.x, 0.1f);
	ImGui::DragFloat3("camera.rotate", &camera_->transform_.rotate_.x, 0.01f);

	if (ImGui::Button("TITLE")) {
		Cygnus::SceneManager::GetInstance()->ChangeScene("TITLE");
	}
	if (ImGui::Button("RESULT")) {
		Cygnus::SceneManager::GetInstance()->ChangeScene("RESULT");
	}

	ImGui::End();

	stateManager_->Debug();
#endif
}

void GamePlayScene::TransitionToResult() {
	// 既に遷移中ならスキップ
	if (isTransitioning_)
		return;

	// 1度のみ呼び出されるようフラグを立てる
	isTransitioning_ = true;

	// フェードアウトしてリザルトシーンへ
	FadeTransition::GetInstance()->StartFadeOut(kFadeOutDuration, []() { Cygnus::SceneManager::GetInstance()->ChangeScene("RESULT"); }, kFadeOutDelay);
}

void GamePlayScene::InitializeGameStates()
{
	// ステートマネージャー生成
	stateManager_ = std::make_unique<GameStateManager>();

	// 各状態を登録
	stateManager_->RegisterState("GameStart", std::make_unique<GameStartState>(this));
	stateManager_->RegisterState("GamePlay", std::make_unique<GamePlayState>(this));
	stateManager_->RegisterState("GameOver", std::make_unique<GameOverState>(this));
	stateManager_->RegisterState("GameClear", std::make_unique<GameClearState>(this));

	// 初期状態をゲームスタートに設定
	stateManager_->ChangeState("GameStart");
}
