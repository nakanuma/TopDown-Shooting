#include "NormalEnemy.h"

// C++
#include <algorithm>

// Engine
#include <DirectXBase.h>
#include <Collider/CollisionManager.h>
#include <Camera.h>

// Application
#include <src/Game/Utility/Utility.h>

// Externals
#include <ImguiWrapper.h>

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void NormalEnemy::Initialize(const Float3& position, ModelManager::ModelData* model)
{
	///
	///	基盤機能生成
	/// 

	DirectXBase* dxBase = DirectXBase::GetInstance();

	spriteCommon_ = std::make_unique<SpriteCommon>();
	spriteCommon_->Initialize(dxBase);

	///
	/// オブジェクト生成
	///

	objectEnemy_ = std::make_unique<Object3D>();
	objectEnemy_->model_ = model;
	objectEnemy_->transform_.translate = position;

	///
	///	コライダー生成
	/// 

	collider_ = std::make_unique<SphereCollider>();
	collider_->SetTag("NormalEnemy");
	collider_->SetOwner(this);

	// コライダーを登録
	CollisionManager::GetInstance()->Register(collider_.get());

	///
	///	スプライト生成
	/// 

	// HPバー（後景）
	uint32_t textureHPBackground = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());
	spriteHPBackground_ = std::make_unique<Sprite>();
	spriteHPBackground_->Initialize(spriteCommon_.get(), textureHPBackground);
	spriteHPBackground_->SetSize({ 100.0f, 25.0f });
	spriteHPBackground_->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });

	// HPバー（前景）
	uint32_t textureHPForeground = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());
	spriteHPForeground_ = std::make_unique<Sprite>();
	spriteHPForeground_->Initialize(spriteCommon_.get(), textureHPForeground);
	spriteHPForeground_->SetSize({ 100.0f, 25.0f });
	spriteHPForeground_->SetColor({ 0.0f, 1.0f, 0.5f, 1.0f });

	///
	///	パラメーター設定
	/// 

	// HPの設定
	currentHP_ = 5;
	maxHP_ = currentHP_; // 最大HPには設定した現在HPを設定（全Enemyクラス共通）
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void NormalEnemy::Update()
{
	///
	/// コライダー更新処理
	///

	UpdateCollider();

	///
	/// オブジェクト更新処理
	/// 

	objectEnemy_->UpdateMatrix();

	///
	///	スプライト更新処理
	/// 

	// HPバー（後景）更新
	spriteHPBackground_->Update();
	// HPバー（前景）更新
	spriteHPForeground_->Update();
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void NormalEnemy::Draw()
{
	// オブジェクト描画
	objectEnemy_->Draw();
}

// ---------------------------------------------------------
// UI描画処理
// ---------------------------------------------------------
void NormalEnemy::DrawUI()
{
	// オブジェクトのワールド座標->スクリーン座標に変換
	Float3 screenPosition = Utility::WorldToScreen(objectEnemy_->transform_.translate);
	// 上にずらす分のオフセット
	float offset = 60.0f;

	// HP割合
	float hpRatio = static_cast<float>(currentHP_) / static_cast<float>(maxHP_);

	///
	/// HPバー（後景）描画
	/// 

	// スクリーン座標をセット
	spriteHPBackground_->SetPosition({
		screenPosition.x - kHPBarSize.x / 2.0f, // HPバーが中心になるように設定,
		screenPosition.y - offset // オフセット分上にずらす
		});
	spriteHPBackground_->Draw();

	///
	///	HPバー（前景）描画
	/// 
	
	// 現在HPに応じてサイズ変更
	Float2 hpBarForegroundSize = { kHPBarSize.x * hpRatio, kHPBarSize.y };
	spriteHPForeground_->SetSize(hpBarForegroundSize);

	// スクリーン座標をセット
	spriteHPForeground_->SetPosition({
		screenPosition.x - kHPBarSize.x / 2.0f, // HPバーが中心になるように設定
		screenPosition.y - offset // オフセット分上にずらす
		});

	spriteHPForeground_->Draw();

}

// ---------------------------------------------------------
// 衝突時コールバック
// ---------------------------------------------------------
void NormalEnemy::OnCollision(Collider* other)
{
	// 衝突したコライダーがPlayerBulletだった場合の処理
	if (other->GetTag() == "PlayerBullet") {
		// HPを減らす
		currentHP_ -= 1;

		// HPが0になった敵を死亡させる
		if (currentHP_ <= 0) {
			isDead_ = true;
		}
	}
}

// ---------------------------------------------------------
// 破棄される際に呼ばれる関数
// ---------------------------------------------------------
void NormalEnemy::OnDestroy()
{
	// コリジョンマネージャーからコライダーの登録を解除
	CollisionManager::GetInstance()->Unregister(collider_.get());
}

// ---------------------------------------------------------
// コライダー更新処理
// ---------------------------------------------------------
void NormalEnemy::UpdateCollider()
{
	// 中心点
	collider_->center_ = objectEnemy_->transform_.translate;
	// 半径
	collider_->radius_ = radius_;
}
