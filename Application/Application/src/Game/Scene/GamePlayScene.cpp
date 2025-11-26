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
#include <src/Game/System/ResultStats.h>
#include <src/Game/Transition/FadeTransition.h>
#include <src/Game/Transition/SplitBlockTransition.h>
#include <src/Game/Waypoint/WaypointManager.h>
#include <src/Game/Utility/Utility.h>

void GamePlayScene::Initialize() {
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
	///	↓ ゲームシーン用
	///

	// 最初にコライダーのクリア
	CollisionManager::GetInstance()->Clear();

	// パーティクルのクリア
	ParticleEffectManager::GetInstance()->Clear();

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
	teleporterManager_->Initialize(loader_->GetAllDatas()); // ローダーから取得したデータを使用
	teleporterManager_->SetGoalCallback([this]() { TransitionToResult(); }); // ゴール時のコールバック関数を設定

	// 発光オブジェクト生成
	emissiveObject_ = std::make_unique<EmissiveObject>();
	emissiveObject_->Initialize();

	// 弾リストのクリア
	BulletManager::GetInstance()->Clear();

	/* その他 */

	// 追従カメラ生成
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize(camera_->GetCurrent()->transform_.translate_); // 初期オフセット
	followCamera_->SetTarget(&player_->GetTranslate());                   // プレイヤーを追従対象にセット

	// ポストエフェクト管理
	postEffectManager_ = std::make_unique<PostEffectManager>();
	postEffectManager_->Initialize();
	postEffectManager_->SetEffectType(PostEffectType::Vignette);

	// ゲームスタート時の演出制御クラス
	gameStartSequence_ = std::make_unique<GameStartSequence>();
	gameStartSequence_->Initialize(spriteCommon_.get());

	// ゲームオーバー時の演出制御クラス
	gameOverSequence_ = std::make_unique<GameOverSequence>();
	gameOverSequence_->Initialize(spriteCommon_.get());
	gameOverSequence_->SetPlayer(player_.get());

	// ゲームクリア時の演出制御クラス
	gameClearSequence_ = std::make_unique<GameClearSequence>();
	gameClearSequence_->Initialize(spriteCommon_.get());
	gameClearSequence_->SetBoss(enemyManager_->GetBoss());

	// トランジション
	FadeTransition::GetInstance()->Initialize(spriteCommon_.get());
	SplitBlockTransition::GetInstance()->StartOpen(kSplitBlockOpenDuration, kSplitBlockOpenDelay);

	// ウェイポイント初期化
	obstacleManager_->Update(player_->GetTranslate()); // レイキャストで障害物のコライダーが必要になるためここで一度更新しておく
	CollisionManager::GetInstance()->Update();         // 障害物のコライダーが未登録状態のためここで一度更新しておく
	WaypointManager::GetInstance()->Initialize();

	// シャドウマップ生成
	shadowMapHandle_ = ShadowMapManager::GetInstance()->CreateShadowMap(Window::GetWidth(), Window::GetHeight());

	// 平行光源の初期値設定
	LightManager::GetInstance()->directionalLightCB_.data_->direction = kDirectionalLightDirection;
	LightManager::GetInstance()->directionalLightCB_.data_->intensity = kDirectionalLightIntensity;
}

void GamePlayScene::Finalize() {}

void GamePlayScene::Update() {
	LightManager::GetInstance()->ClearEmissiveLights(); // エミッシブライトをクリア
	LightManager::GetInstance()->ClearAreaLights(); // エリアライトをクリア

	// ----------------------------------------------------------------------

	// ゲームスタート時演出の更新
	if (!gameStartSequence_->IsFinished()) {
		gameStartSequence_->Update();
		// ゲームクリア時演出の更新
	} else if (gameClearSequence_->IsControllingCamera()) {
		gameClearSequence_->Update();
		// 通常ゲーム時のカメラ制御更新
	} else {
		// プレイヤーが生きている間のみ
		if (!player_->IsDead()) {
			// 追従カメラの更新
			followCamera_->Update();
			// 追従カメラ + カメラシェイクを現在カメラに適用
			camera_->transform_.translate_ = followCamera_->GetCameraPosition() + CameraShake::GetInstance()->GetOffset();
		}
	}

	// ボスが死亡した瞬間にゲームクリア演出を開始
	auto boss = enemyManager_->GetBoss();
	if (boss != nullptr) {
		if (boss->IsDying() && !gameClearSequence_->IsActive()) {
			gameClearSequence_->Start();
		}
	}

	// ゲームクリア演出が終了したらゴールテレポーターを有効化する
	if (gameClearSequence_->IsFinished()) {
		teleporterManager_->EnableGoalTeleporter();
	}

	// ----------------------------------------------------------------------

	// カメラシェイクの更新
	CameraShake::GetInstance()->Update();
	// フィールド更新
	field_->Update();
	// プレイヤー更新（スタート演出終了で操作可能に）
	player_->Update(gameStartSequence_->IsFinished());
	// 敵の更新
	enemyManager_->Update();
	// 障害物の更新
	obstacleManager_->Update(player_->GetTranslate());
	// テレポーターの更新
	teleporterManager_->Update();
	// 弾の更新
	BulletManager::GetInstance()->Update();
	// 発光オブジェクト更新
	emissiveObject_->Update();
	// トランジション更新
	SplitBlockTransition::GetInstance()->Update();
	FadeTransition::GetInstance()->Update();
	// ウェイポイントの更新
	WaypointManager::GetInstance()->Update();
	// ゲームオーバー時演出の更新
	gameOverSequence_->Update();
	// ゲームクリア時演出の更新（カメラ制御していない間も更新を続ける）
	if (!gameClearSequence_->IsControllingCamera()) {
		gameClearSequence_->Update();
	}

	// SkyBox更新
	SkyBoxManager::GetInstance()->Update();
	// コリジョンマネージャーの更新（全てのコライダーの衝突判定）
	CollisionManager::GetInstance()->Update();
	// パーティクルエフェクトマネージャー更新
	ParticleEffectManager::GetInstance()->Update(TimeManager::GetInstance()->GetDeltaTime());

	// クリアタイム（経過時間）の記録
	ResultStats::GetInstance()->AddTime();
}

void GamePlayScene::Draw() {
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
	// ポストエフェクト用の定数バッファを設定
	postEffectManager_->TransfarConstantBuffer();
	// LightCameraの定数バッファを送信
	LightCamera::GetInstance()->TransferConstantBuffer();

	// スカイボックス描画
	SkyBoxManager::GetInstance()->Draw();

	// ---------------------------------------------------------
	// シャドウマップ描画前処理
	// ---------------------------------------------------------

	// ライトカメラの更新
	LightCamera::GetInstance()->SetDirectionalLight(LightManager::GetInstance()->directionalLightCB_.data_->direction);

	// プレイヤー中心のBBを生成してライトカメラの行列更新（あとで整理）
	LightCamera::BoundingBox playerCenterBB;
	playerCenterBB.SetCenterExtents(player_->GetTranslate(), kShadowBoundingBoxExtents);
	LightCamera::GetInstance()->UpdateViewProjection(playerCenterBB);

	// シャドウマップ描画開始
	ShadowMapManager::GetInstance()->BeginShadowPass(shadowMapHandle_);

	/// =========================================================
	/// ↓ ここから通常モデルのシャドウマップ描画
	/// =========================================================

	// ゲーム開始演出時オブジェクト
	if (!gameStartSequence_->IsFinished()) {
		gameStartSequence_->DrawShadow();
	}

	player_->DrawGunShadow();
	obstacleManager_->DrawShadow(player_->GetTranslate());
	enemyManager_->DrawShadow();
	teleporterManager_->DrawShadow();

	/// =========================================================
	/// ↑ ここまで通常モデルのシャドウマップ描画
	/// =========================================================

	// スキニングモデル用PSOをセット
	dxBase->GetCommandList()->SetPipelineState(ShadowMapManager::GetInstance()->GetShadowSkinnedPSO());
	
	/// =========================================================
	/// ↓ ここからスキニングモデルのシャドウマップ描画
	/// =========================================================

	player_->DrawShadow();

	/// =========================================================
	/// ↑ ここまでスキニングモデルのシャドウマップ描画
	/// =========================================================
	
	ShadowMapManager::GetInstance()->EndShadowPass(shadowMapHandle_);

	/// =========================================================
	/// ↓ ここから3Dオブジェクト描画
	/// =========================================================

	// ゲーム開始演出時オブジェクト
	if (!gameStartSequence_->IsFinished()) {
		gameStartSequence_->Draw();
	}

	// オブジェクト通常描画処理
	field_->Draw();
	player_->Draw();
	enemyManager_->Draw();
	obstacleManager_->Draw(player_->GetTranslate());
	teleporterManager_->Draw();
	emissiveObject_->Draw();
	BulletManager::GetInstance()->Draw();
	ParticleEffectManager::GetInstance()->Draw();
	LineDrawer::GetInstance()->Draw();

	/// =========================================================
	/// ↑ ここまで3Dオブジェクト描画
	/// =========================================================

	// Spriteの描画準備。全ての描画に共通のグラフィックスコマンドを積む
	spriteCommon_->PreDraw();

	/// =========================================================
	/// ↓ ここからスプライト描画
	/// =========================================================

	// 敵UI描画
	enemyManager_->DrawUI();

	// スタート演出中は専用UI表示
	if (!gameStartSequence_->IsFinished()) {
		gameStartSequence_->DrawUI();
		// スタート演出が終了したらゲーム用UI表示
	} else {
		// プレイヤーUI描画（クリア演出のカメラ制御時は非表示になるように）
		if (!gameClearSequence_->IsControllingCamera()) {
			player_->DrawUI();
		}
	}

	// ゲームオーバー時のUI描画
	gameOverSequence_->DrawUI();
	// ゲームクリア時のUI描画
	gameClearSequence_->DrawUI();
	// トランジション描画
	SplitBlockTransition::GetInstance()->Draw();
	FadeTransition::GetInstance()->Draw();

	/// =========================================================
	/// ↑ ここまでスプライト描画
	/// =========================================================

#ifdef _DEBUG
	// デバッグ表示
	Debug();

	player_->Debug();

	gameClearSequence_->Debug();

	emissiveObject_->Debug();

	lightManager_->DrawDebug();

#endif
	// ImGuiの内部コマンドを生成する
	ImguiWrapper::Render(dxBase->GetCommandList());
	// 描画後処理
	dxBase->PostDraw();
	// フレーム終了処理
	dxBase->EndFrame();
}

void GamePlayScene::Debug() {
#ifdef USE_IMGUI
	ImGui::Begin("GameSceneInfo");

	if (ImGui::Button("Emit")) {
		ParticleEffectManager::GetInstance()->Emit("teleporterRing", {156.0f, 2.0f, 20.0f}, 1);
	}

	ImGui::Text("fps:%.2f", ImGui::GetIO().Framerate);
	ImGui::DragFloat3("camera.translate", &camera_->transform_.translate_.x, 0.1f);
	ImGui::DragFloat3("camera.rotate", &camera_->transform_.rotate_.x, 0.01f);

	ImGui::DragFloat3("DirectionalLight : Direction", &lightManager_->directionalLightCB_.data_->direction.x, 0.01f);
	lightManager_->directionalLightCB_.data_->direction = Float3::Normalize(lightManager_->directionalLightCB_.data_->direction);
	ImGui::DragFloat("DirectionalLight : intensity", &lightManager_->directionalLightCB_.data_->intensity, 0.01f);

	if (ImGui::Button("TITLE")) {
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}
	if (ImGui::Button("RESULT")) {
		SceneManager::GetInstance()->ChangeScene("RESULT");
	}

	ImGui::End();
#endif
}

void GamePlayScene::TransitionToResult()
{
	// 既に遷移中ならスキップ
	if (isTransitioning_) return;

	// 1度のみ呼び出されるようフラグを立てる
	isTransitioning_ = true;

	// フェードアウトしてリザルトシーンへ
	FadeTransition::GetInstance()->StartFadeOut(
		kFadeOutDuration,
		[]() {
			SceneManager::GetInstance()->ChangeScene("RESULT");
		},
		kFadeOutDelay);
}