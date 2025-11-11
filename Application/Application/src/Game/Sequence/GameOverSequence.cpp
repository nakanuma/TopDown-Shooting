#include "GameOverSequence.h"

// Engine
#include <ImguiWrapper.h>
#include <TimeManager.h>
#include <Camera.h>
#include <Easing.h>
#include <RandomGenerator.h>
#include <TextureManager.h>
#include <Input/Input.h>
#include <SceneManager.h>
#include <ParticleEffect/ParticleEffectManager.h>

// Application
#include <src/Game/Transition/FadeTransition.h>
#include <src/Game/Utility/Utility.h>
#include <src/Game/Player/Player.h>

void GameOverSequence::Initialize(SpriteCommon* spriteCommon) {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	// スプライト生成
	uint32_t textureYouDied = TextureManager::Load("UI/youDied.png");
	spriteDiedText_ = std::make_unique<Sprite>();
	spriteDiedText_->Initialize(spriteCommon, textureYouDied);
	spriteDiedText_->SetAnchorPoint({0.5f, 0.5f});
	spriteDiedText_->SetPosition(kDiedTextStartPos);

	uint32_t textureBackToTitle = TextureManager::Load("UI/backToTitle.png");
	spriteBackToTitleText_ = std::make_unique<Sprite>();
	spriteBackToTitleText_->Initialize(spriteCommon, textureBackToTitle);
	spriteBackToTitleText_->SetAnchorPoint({0.5f, 0.5f});
	spriteBackToTitleText_->SetPosition(kBackToTitleTextPos);
	backToTitleTextEndSize_ = spriteBackToTitleText_->GetSize(); // 最終サイズを保存しておく
	spriteBackToTitleText_->SetSize(backToTitleTextStartSize_); // スプライトを初期サイズにしておく（0, 0）
}

void GameOverSequence::Start(const Float3& playerPos)
{
	phase_ = Phase::Intro; // ゲームオーバー演出の開始
	timer_ = 0.0f;
	targetPos_ = playerPos;	// プレイヤー死亡位置を注視点に

	approachStartPos_ = Camera::GetCurrent()->transform.translate; // カメラ接近時の開始位置を設定
	approachEndPos_ = Float3::Lerp(approachStartPos_, targetPos_, kApproachDistance); // どれだけの割合近づくかを設定
}

void GameOverSequence::Update() {
	// プレイヤーの死亡を検出してゲームオーバー演出を開始
	if(player_->IsDead() && !IsActive()) {
		Start(player_->GetTranslate());
	}

	// ゲームオーバー演出が行われていない間はスキップ
	if (!IsActive()) return;

	// タイマー更新
	timer_ += TimeManager::GetInstance()->GetDeltaTime();

	// スプライト更新
	spriteDiedText_->Update();
	spriteBackToTitleText_->Update();

	// フェーズ毎の更新処理
	switch (phase_) {
	case Phase::Intro:
		// 少し待機して次のフェーズへ
		if (timer_ > kWaitTime) {
			timer_ = 0.0f;
			phase_ = Phase::Approach;
		}
		break;
	case Phase::Approach: {
		// カメラ接近時の更新処理
		UpdateApproach();
		break;
	}
	case Phase::Rotate:
		// カメラ回転時の更新処理
		UpdateRotate();
		break;
	case Phase::DiedText:
		// "YOU DIED"スプライトの更新処理
		UpdateDiedText();
		break;
	case Phase::BackToTitleText:
		// "クリックでタイトルへ"スプライトの更新処理
		UpdateBackToTitleText();
		break;
	case Phase::Finish:
		// 左クリック入力でタイトルシーンへ移行
		if (Input::GetInstance()->IsTriggerMouse(0) && FadeTransition::GetInstance()->IsFinished() && Utility::IsInsideClientCursor()) {
			FadeTransition::GetInstance()->StartFadeOut(
				0.5f,
				[]() {
					SceneManager::GetInstance()->ChangeScene("TITLE");
				},
				0.25f);
		}
		break;
	}
}

void GameOverSequence::DrawUI() {
	// ゲームオーバー演出が行われていない間はスキップ
	if(!IsActive()) return;

	spriteDiedText_->Draw();
	spriteBackToTitleText_->Draw();
}

void GameOverSequence::Debug() {
#ifdef USE_IMGUI
	ImGui::Begin("GameOverSequence");

	ImGui::Text("Timer : %.2f", timer_);

	// フェーズ名の表示
	const char* phaseStr = "";
	switch (phase_)
	{
	case GameOverSequence::Phase::None:	phaseStr = "None"; break;
	case GameOverSequence::Phase::Intro: phaseStr = "Intro"; break;
	case GameOverSequence::Phase::Approach: phaseStr = "Approach"; break;
	case GameOverSequence::Phase::Rotate: phaseStr = "Rotate"; break;
	case GameOverSequence::Phase::DiedText: phaseStr = "DiedText"; break;
	case GameOverSequence::Phase::BackToTitleText: phaseStr = "BackToTitle"; break;
	case GameOverSequence::Phase::Finish: phaseStr = "Finish"; break;
	}
	ImGui::Text("Current Phase : %s", phaseStr);

	ImGui::End();
#endif
}

void GameOverSequence::UpdateApproach()
{
	// 進行度の計算
	float t = std::clamp(timer_ / kApproachDuration, 0.0f, 1.0f);
	float easeT = Easing::EaseOutQuart(t);

	// 開始位置から終了位置までeaseTの割合で補間してカメラ移動
	Float3 newPos = Float3::Lerp(approachStartPos_, approachEndPos_, easeT);
	Camera::GetCurrent()->transform.translate = newPos;

	// 接近が完了したら次のフェーズへ
	if (t >= 1.0f) {
		timer_ = 0.0f;
		phase_ = Phase::Rotate;
	}
}

void GameOverSequence::UpdateRotate()
{
	// 進行度の計算
	float t = std::clamp(timer_ / kRotateDuration, 0.0f, 1.0f);
	float easeT = Easing::EaseOutQuad(t);

	// 回転角（90度回転）
	float angle = DegToRad(90.0f) * easeT;
	// ターゲットを中心にカメラの回転
	Float3 offset = approachEndPos_ - targetPos_;

	// Y軸回転の計算を行う
	Float3 rotateOffset = {
		offset.x * std::cosf(angle) - offset.z * std::sinf(angle),
		offset.y,
		offset.z * std::sinf(angle) + offset.z * std::cosf(angle)
	};

	// 新しいカメラ位置を計算
	Float3 cameraPos = targetPos_ + rotateOffset;
	Camera::GetCurrent()->transform.translate = cameraPos;

	// ターゲットを向くように回転を計算
	Float3 forward = Float3::Normalize(targetPos_ - cameraPos);

	Camera::GetCurrent()->transform.rotate.y = std::atan2f(forward.x, forward.z);
	Camera::GetCurrent()->transform.rotate.x = std::asinf(-forward.y);

	// 回転が完了したら次のフェーズへ
	if (t >= 1.0f) { 
		timer_ = 0.0f; 
		phase_ = Phase::DiedText;
	}
}

void GameOverSequence::UpdateDiedText()
{
	// 進行度の計算
	float t = std::clamp(timer_ / kDiedTextDuration, 0.0f, 1.0f);
	float easeT = Easing::EaseOutQuart(t);

	// 新しい位置へ移動
	Float2 pos = Float2::Lerp(kDiedTextStartPos, kDiedTextEndPos, easeT);
	spriteDiedText_->SetPosition(pos);

	// 移動が終了したら次のフェーズへ
	if(t >= 1.0f){
		timer_ = 0.0f;
		phase_ = Phase::BackToTitleText;
	}
}

void GameOverSequence::UpdateBackToTitleText()
{
	// 進行度の計算
	float t = std::clamp(timer_ / kBackToTitleTextDuration, 0.0f, 1.0f);
	float easeT = Easing::EaseOutBounce(t);

	// 新しいサイズに更新
	Float2 size = Float2::Lerp(backToTitleTextStartSize_, backToTitleTextEndSize_, easeT);
	spriteBackToTitleText_->SetSize(size);

	// 更新が終了したら次のフェーズへ
	if (t >= 1.0f) {
		timer_ = 0.0f;
		phase_ = Phase::Finish;
	}
}