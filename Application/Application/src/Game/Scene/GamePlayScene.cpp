#include "GamePlayScene.h" 
#include "ImguiWrapper.h" 
#include "DirectXBase.h"
#include "SRVManager.h"
#include "SpriteCommon.h"
#include "RTVManager.h"

// C++
#include <numbers>

void GamePlayScene::Initialize()
{
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

	// レンダーテクスチャ生成
	renderTexture_ = RTVManager::CreateRenderTargetTexture(Window::GetWidth(), Window::GetHeight());

	///
	///	↓ ゲームシーン用 
	///	

	/* オブジェクト関連 */

	// フィールド生成
	field_ = std::make_unique<Field>();
	field_->Initialize();

	// プレイヤー生成
	player_ = std::make_unique<Player>();
	player_->Initialize();

	// 敵のスポーンマネージャー生成
	enemySpawnManager_ = std::make_unique<EnemySpawnManager>();
	enemySpawnManager_->Initialize();


	/* その他 */

	// 追従カメラ生成
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize(camera->GetCurrent()->transform.translate); // 初期オフセット
	followCamera_->SetTarget(&player_->GetTranslate()); // プレイヤーを追従対象にセット
}

void GamePlayScene::Finalize()
{
}

void GamePlayScene::Update() { 
	// 追従カメラの更新
	followCamera_->Update();
	camera->transform.translate = followCamera_->GetCameraPosition();


	// フィールド更新
	field_->Update();
	// プレイヤー更新
	player_->Update();
	// 敵のスポーンマネージャー更新
	enemySpawnManager_->Update();


	// コリジョンマネージャーの更新（全てのコライダーの衝突判定）
	CollisionManager::GetInstance()->Update();

#ifdef _DEBUG 
	// デバッグカメラ更新
	DebugCameraUpdate(input);
#endif
}

void GamePlayScene::Draw()
{
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

#ifdef _DEBUG
	// レンダーターゲットをレンダーテクスチャにセット
	RTVManager::SetRenderTarget(renderTexture_);
	RTVManager::ClearRTV(renderTexture_);
#endif // DEBUG

	///
	///	↓ ここから3Dオブジェクトの描画コマンド
	/// 

	// フィールド描画
	field_->Draw();
	// プレイヤー描画
	player_->Draw();
	// 敵のスポーンマネージャー描画
	enemySpawnManager_->Draw();

	///
	///	↑ ここまで3Dオブジェクトの描画コマンド
	/// 

	// Spriteの描画準備。全ての描画に共通のグラフィックスコマンドを積む
	spriteCommon->PreDraw();

	///
	/// ↓ ここからスプライトの描画コマンド
	/// 

	// プレイヤーUI描画
	player_->DrawUI();

	///
	/// ↑ ここまでスプライトの描画コマンド
	/// 

#ifdef _DEBUG 
	ImGui::Begin("GameSceneInfo");
	ImGui::Text("fps:%.2f", ImGui::GetIO().Framerate);
	ImGui::DragFloat3("camera.translate", &camera->transform.translate.x, 0.1f);
	ImGui::DragFloat3("camera.rotate", &camera->transform.rotate.x, 0.01f);
	ImGui::Checkbox("useDebugCamera", &useDebugCamera);
	ImGui::End();

	// コリジョンマネージャーのデバッグ表示
	CollisionManager::GetInstance()->Debug();

	// レンダーテクスチャをImGuiWindowに描画
	ImGuiUtil::ImageWindow("Scene", renderTexture_);
	RTVManager::SetRTtoBB();
#endif
	// ImGuiの内部コマンドを生成する
	ImguiWrapper::Render(dxBase->GetCommandList());
	// 描画後処理
	dxBase->PostDraw();
	// フレーム終了処理
	dxBase->EndFrame();
}

#ifdef _DEBUG
void GamePlayScene::DebugCameraUpdate(Input* input)
{
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