#include "GamePlayScene.h"
#include "DirectXBase.h"
#include "ImguiWrapper.h"
#include "RTVManager.h"
#include "SRVManager.h"
#include "SpriteCommon.h"
#include <Engine/ParticleEffect/ParticleEffectManager.h>
#include <Engine/Model/SkyBoxManager.h>
#include <Engine/3D/LineDrawer.h>
#include <Engine/DirectX/ShadowMapManager.h>
#include <Engine/3D/LightCamera.h>

// C++
#include <numbers>

// Engine
#include <Engine/Scene/SceneManager.h>

// Application
#include <src/Game/Camera/CameraShake.h>
#include <src/Game/Utility/ParticleEffectLoader.h>
#include <src/Game/Transition/FadeTransition.h>
#include <src/Game/Waypoint/WaypointManager.h>
#include <src/Game/System/ResultStats.h>

void GamePlayScene::Initialize() {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// カメラのインスタンスを生成
	camera = std::make_unique<Camera>(Float3{0.0f, 50.0f, -55.0f}, Float3{std::numbers::pi_v<float> / 4.0f, 0.0f, 0.0f}, 0.45f);
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

	// SkyBoxの初期化
	SkyBoxManager::GetInstance()->Initialize("resources/Images/skybox.dds");

	///
	///	↓ ゲームシーン用
	///

	// 最初にコライダーのクリア
	CollisionManager::GetInstance()->Clear();

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
	teleporterManager_->Initialize(loader_->GetAllDatas());

	/* その他 */

	// 追従カメラ生成
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize(camera->GetCurrent()->transform.translate); // 初期オフセット
	followCamera_->SetTarget(&player_->GetTranslate());                   // プレイヤーを追従対象にセット

	// パーティクル生成
	ParticleEffectLoader::GetInstance()->LoadAndRegisterAll();

	// ポストエフェクト管理
	postEffectManager_ = std::make_unique<PostEffectManager>();
	postEffectManager_->Initialize();
	postEffectManager_->SetEffectType(PostEffectType::Vignette);

	// フェード
	FadeTransition::GetInstance()->Initialize(spriteCommon.get());
	FadeTransition::GetInstance()->StartFadeIn(1.0f, 0.2f);

	// ウェイポイント初期化
	obstacleManager_->Update(player_->GetTranslate()); // レイキャストで障害物のコライダーが必要になるためここで一度更新しておく
	CollisionManager::GetInstance()->Update(); // 障害物のコライダーが未登録状態のためここで一度更新しておく
	WaypointManager::GetInstance()->Initialize(loader_->GetAllDatas());

	// シャドウマップ生成
	shadowMapHandle_ = ShadowMapManager::GetInstance()->CreateShadowMap(Window::GetWidth(), Window::GetHeight());

	// 平行光源の初期値設定
	LightManager::GetInstance()->directionalLightCB_.data_->direction = {0.367f, -0.653f, -0.662f};
}

void GamePlayScene::Finalize() {}

void GamePlayScene::Update() {
	ResultStats::GetInstance()->AddTime(); // クリアタイム（経過時間）の記録

	/*ShowCursor(FALSE);*/

	// 追従カメラの更新
	followCamera_->Update();
	// カメラシェイクの更新
	CameraShake::GetInstance()->Update();
	// 追従カメラ + カメラシェイクを現在カメラに適用
	camera->transform.translate = followCamera_->GetCameraPosition() + CameraShake::GetInstance()->GetOffset();

	// フィールド更新
	field_->Update();
	// プレイヤー更新
	player_->Update();
	// 敵の更新
	enemyManager_->Update();
	// 障害物の更新
	obstacleManager_->Update(player_->GetTranslate());
	// テレポーターの更新
	teleporterManager_->Update();
	// 弾の更新
	BulletManager::GetInstance()->Update();
	// フェード更新
	FadeTransition::GetInstance()->Update();
	// ウェイポイントの更新
	WaypointManager::GetInstance()->Update();

	// SkyBox更新
	SkyBoxManager::GetInstance()->Update();
	// コリジョンマネージャーの更新（全てのコライダーの衝突判定）
	CollisionManager::GetInstance()->Update();
	// パーティクルエフェクトマネージャー更新
	ParticleEffectManager::GetInstance()->Update(TimeManager::GetInstance()->GetDeltaTime());

#ifdef _DEBUG
	// デバッグカメラ更新
	DebugCameraUpdate(input);

	loader_->Update();
	// ステージデータファイルに変更があれば再生成
	if (loader_->HasFileChanged()) {
		// ステージデータ再読み込み
		loader_->LoadFromFile("resources/Stages/data.json");

		// 各ステージデータ要素の再生成
		enemyManager_->Reload(loader_->GetAllDatas());
		obstacleManager_->Reload(loader_->GetAllDatas());

		// リセットしたことを知らせる
		loader_->ResetFileChangedFlag();
	}

#endif
}

void GamePlayScene::Draw() {
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
	// ポストエフェクト用の定数バッファを設定
	postEffectManager_->TransfarConstantBuffer();

	///
	///	↓ ここから3Dオブジェクトの描画コマンド
	///
#ifdef _DEBUG
	/*ImGuiUtil::ImageWindow("rendertexture", postEffectManager_->GetRenderTextureHandle());
	ImGuiUtil::ImageWindow("outlineRT", postEffectManager_->outlineRT_);
	ImGuiUtil::ImageWindow("outlineGH", postEffectManager_->outlineGH_);*/
#endif
	// 通常の描画
	/*postEffectManager_->BeginRenderToTexture();*/
	SkyBoxManager::GetInstance()->Draw();


	///
	///	シャドウマップ描画
	/// 
	
	// シャドウマップ用PSOをセット
	dxBase->GetCommandList()->SetPipelineState(ShadowMapManager::GetInstance()->GetShadowPSO());

	// Viewport / Scissor の設定
	D3D12_VIEWPORT vp{};
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = static_cast<float>(Window::GetWidth());
	vp.Height = static_cast<float>(Window::GetHeight());
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	dxBase->GetCommandList()->RSSetViewports(1, &vp);

	D3D12_RECT sc{};
	sc.left = 0;
	sc.top = 0;
	sc.right = static_cast<LONG>(Window::GetWidth());
	sc.bottom = static_cast<LONG>(Window::GetHeight());
	dxBase->GetCommandList()->RSSetScissorRects(1, &sc);

	// シャドウマップ書き込み前に書き込み状態に遷移
	ShadowMapManager::GetInstance()->TransitionShadowResource(dxBase->GetCommandList(), shadowMapHandle_, D3D12_RESOURCE_STATE_DEPTH_WRITE);

	// ライトカメラの更新
	LightCamera::GetInstance()->SetDirectionalLight(LightManager::GetInstance()->directionalLightCB_.data_->direction); // directionを設定
	
	// プレイヤーに追従するBB
	LightCamera::BoundingBox playerCenterBB;
	playerCenterBB.SetCenterExtents(player_->GetTranslate(), {30.0f, 10.0f, 30.0f});

	LightCamera::GetInstance()->UpdateViewProjection(playerCenterBB); // 行列の更新
	// シャドウマップDSVをセット
	ShadowMapManager::GetInstance()->SetShadowDSV(shadowMapHandle_);
	// シャドウマップをクリア
	ShadowMapManager::GetInstance()->ClearShadowMap(shadowMapHandle_);

	//// シャドウマップ描画対象オブジェクト描画

	obstacleManager_->DrawShadow(player_->GetTranslate());


	//----------------------------------//

	// シャドウマップ用PSOをセット
	dxBase->GetCommandList()->SetPipelineState(ShadowMapManager::GetInstance()->GetShadowSkinnedPSO());

	//// シャドウマップ描画対象スキニングオブジェクト描画

	player_->DrawShadow();

	//----------------------------------//
	
	// 描画後、SRVとして使えるように遷移
	ShadowMapManager::GetInstance()->TransitionShadowResource(dxBase->GetCommandList(), shadowMapHandle_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	// ShadowMapをバインド
	TextureManager::SetDescriptorTable(12, dxBase->GetCommandList(), shadowMapHandle_);
	// LightCameraの定数バッファを送信（PixelShader内で使用）
	LightCamera::GetInstance()->TransferConstantBuffer();

	///
	///	通常描画
	/// 

	// バックバッファ用PSOに切り替え
	dxBase->GetCommandList()->SetPipelineState(dxBase->GetPipelineState());
	// バックバッファDSVに切り替え
	UINT backBufferIndex = dxBase->GetSwapChain()->GetCurrentBackBufferIndex();
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = dxBase->GetRTVHandle(backBufferIndex);
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dxBase->GetDSVHeap()->GetCPUHandle(0);
	dxBase->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

	// オブジェクト通常描画
	field_->Draw();
	player_->Draw();
	enemyManager_->Draw();
	obstacleManager_->Draw(player_->GetTranslate());
	teleporterManager_->Draw();
	BulletManager::GetInstance()->Draw();
	/*WaypointManager::GetInstance()->Draw();*/

	ParticleEffectManager::GetInstance()->Draw();

	//// アウトライン適用パス
	//postEffectManager_->BeginRenderToOutlineTexture();
	//player_->Draw();
	//enemyManager_->Draw();

	//// アウトライン生成 + 描画
	//postEffectManager_->ApplyOutline();
	//postEffectManager_->DrawOutline();

	// Lineの描画コマンドを発行
	LineDrawer::GetInstance()->Render();

	///
	///	↑ ここまで3Dオブジェクトの描画コマンド
	///

	// Spriteの描画準備。全ての描画に共通のグラフィックスコマンドを積む
	spriteCommon->PreDraw();

	///
	/// ↓ ここからスプライトの描画コマンド
	///

	// 敵UI描画
	enemyManager_->DrawUI();
	// プレイヤーUI描画
	player_->DrawUI();
	// フェード描画
	FadeTransition::GetInstance()->Draw();

	///
	/// ↑ ここまでスプライトの描画コマンド
	///

	///
	///	デバッグ表示
	/// 
#ifdef _DEBUG
	ImGui::Begin("GameSceneInfo");

	if (ImGui::Button("Emit")) {
		ParticleEffectManager::GetInstance()->Emit("warningScatter", { 0.0f, 2.0f, -20.0f }, 20);
	}

	if (ImGui::Button("bulletClear")) {
		BulletManager::GetInstance()->Clear();
	}

	ImGui::Text("fps:%.2f", ImGui::GetIO().Framerate);
	ImGui::DragFloat3("camera.translate", &camera->transform.translate.x, 0.1f);
	ImGui::DragFloat3("camera.rotate", &camera->transform.rotate.x, 0.01f);
	ImGui::Checkbox("useDebugCamera", &useDebugCamera);

	ImGui::DragFloat3("DirectionalLight : Direction", &lightManager->directionalLightCB_.data_->direction.x, 0.01f);
	lightManager->directionalLightCB_.data_->direction = Float3::Normalize(lightManager->directionalLightCB_.data_->direction);

	if (ImGui::Button("TITLE")) {
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}
	if (ImGui::Button("RESULT")) {
		SceneManager::GetInstance()->ChangeScene("RESULT");
	}

	ImGui::End();

	/*Debug*/
	CollisionManager::GetInstance()->Debug();
	player_->Debug();
	obstacleManager_->Debug();
	teleporterManager_->Debug();
	enemyManager_->Debug();
	WaypointManager::GetInstance()->Debug();
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
void GamePlayScene::DebugCameraUpdate(Input* input) {
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