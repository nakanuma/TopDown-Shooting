#include "PlayerUI.h"

// Engine
#include <Engine/Texture/TextureManager.h>
#include <Engine/3D/Camera.h>

// Application
#include <src/Game/Player/Player.h>
#include <src/Game/Utility/Utility.h>

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void PlayerUI::Initialize() 
{
	///
	///	基盤機能
	///

	DirectXBase* dxBase = DirectXBase::GetInstance();

	// スプライト基盤
	spriteCommon_ = std::make_unique<SpriteCommon>();
	spriteCommon_->Initialize(DirectXBase::GetInstance());

	///
	///	スプライト関連
	/// 

	/* レティクル） */

	// クロスヘア（十字線）スプライト初期化
	uint32_t textureTarget = TextureManager::Load("resources/Images/game/crosshair.png", dxBase->GetDevice());
	spriteCrosshair_ = std::make_unique<Sprite>();
	spriteCrosshair_->Initialize(spriteCommon_.get(), textureTarget);
	spriteCrosshair_->SetAnchorPoint({0.5f, 0.5f});
	spriteCrosshair_->SetSize({64.0f, 64.0f});

	/* HPバー */

	// HPバー（後景）
	uint32_t textureHPBackground = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());
	spriteHPBackground_ = std::make_unique<Sprite>();
	spriteHPBackground_->Initialize(spriteCommon_.get(), textureHPBackground);
	spriteHPBackground_->SetSize(kHPBarSize);
	spriteHPBackground_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});

	// HPバー（前景）
	uint32_t textureHPForeground = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());
	spriteHPForeground_ = std::make_unique<Sprite>();
	spriteHPForeground_->Initialize(spriteCommon_.get(), textureHPForeground);
	spriteHPForeground_->SetSize(kHPBarSize);
	spriteHPForeground_->SetColor({1.0f, 0.2f, 0.2f, 1.0f});
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void PlayerUI::Update() 
{
	///
	///	レティクル
	/// 

	// クロスヘア（十字線）
	spriteCrosshair_->Update();

	///
	///	HPバー
	/// 

	// HPバー（後景）
	spriteHPBackground_->Update();
	// HPバー（前景）
	spriteHPForeground_->Update();
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void PlayerUI::Draw(const Player* player) {
	/*--------------*/
	/* レティクル関連 */
	/*--------------*/

	///
	///	クロスヘア描画
	///

	// カーソルのワールド座標をスクリーン座標に変換してスプライト位置を設定（あとで整理）
	Float3 screenPos = Float3::Transform(Utility::CalclateCursorPosition(), Camera::GetCurrent()->GetViewProjectionMatrix());

	float screenWidth = static_cast<float>(Window::GetWidth());
	float screenHeight = static_cast<float>(Window::GetHeight());

	float screenX = (screenPos.x + 1.0f) * 0.5f * screenWidth;
	float screenY = (1.0f - screenPos.y) * 0.5f * screenHeight;

	spriteCrosshair_->SetPosition({screenX, screenY});

	spriteCrosshair_->Draw();

	/*--------------*/
	/*   HPバー関連   */
	/*--------------*/

	///
	/// HPバー（後景）描画
	///

	spriteHPBackground_->SetPosition({
	    Window::GetWidth() / 2.0f - kHPBarSize.x / 2.0f, // 画面の中央 - サイズ半分で中央揃え
	    (Window::GetHeight() / 8.0f) * 7.0f              // 画面縦サイズの 7/8 の位置へ設定
	});
	spriteHPBackground_->Draw();

	///
	/// HPバー（前景）描画
	///

	float hpRatio = static_cast<float>(player->GetCurrentHP()) / static_cast<float>(player->GetMaxHP()); // HP割合

	Float2 hpBarForegroundSize = {kHPBarSize.x * hpRatio, kHPBarSize.y};
	spriteHPForeground_->SetSize(hpBarForegroundSize); // 現在HPに応じてサイズ変更

	spriteHPForeground_->SetPosition({
	    Window::GetWidth() / 2.0f - kHPBarSize.x / 2.0f, // 画面の中央 - サイズ半分で中央揃え
	    (Window::GetHeight() / 8.0f) * 7.0f              // 画面縦サイズの 7/8 の位置へ設定
	});
	spriteHPForeground_->Draw();
}
