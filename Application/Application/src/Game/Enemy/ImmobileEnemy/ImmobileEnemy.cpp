#include "ImmobileEnemy.h"

// C++
#include <numbers>

// Engine
#include <Engine/Util/RandomGenerator.h>
#include <Engine/Util/TimeManager.h>

// Application
#include <src/Game/Utility/Utility.h>
#include <src/Game/Player/Player.h>
#include <src/Game/Bullet/EnemyBullet/EnemyBullet.h>
#include <src/Game/Bullet/Manager/BulletManager.h>

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void ImmobileEnemy::Initialize(const Float3& position, ModelManager::ModelData* model) {
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
	objectEnemy_->transform_.scale = {1.0f, 1.0f, 1.0f};
	objectEnemy_->transform_.rotate = {0.0f, std::numbers::pi_v<float>, 0.0f}; // 手前を向いた状態でスポーン（一時的に）
	objectEnemy_->materialCB_.data_->color = {0.5f, 0.37f, 0.7f, 1.0f};

	///
	///	コライダー生成
	///

	collider_ = std::make_unique<AABBCollider>();
	collider_->SetTag("ImmobileEnemy");
	collider_->SetOwner(this);
	colliderSize_ = {1.0f, 2.0f, 1.0f};

	// コライダーを登録
	CollisionManager::GetInstance()->Register(collider_.get());

	///
	///	スプライト生成
	///

	// HPバー（後景）
	uint32_t textureHPBackground = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());
	spriteHPBackground_ = std::make_unique<Sprite>();
	spriteHPBackground_->Initialize(spriteCommon_.get(), textureHPBackground);
	spriteHPBackground_->SetSize(kHPBarSize);
	spriteHPBackground_->SetColor({0.0f, 0.0f, 0.0f, 1.0f}); // 黒

	// HPバー（前景）
	uint32_t textureHPForeground = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());
	spriteHPForeground_ = std::make_unique<Sprite>();
	spriteHPForeground_->Initialize(spriteCommon_.get(), textureHPForeground);
	spriteHPForeground_->SetSize(kHPBarSize);
	spriteHPForeground_->SetColor({0.0f, 1.0f, 0.5f, 1.0f}); // 緑

	///
	///	パラメーター設定
	///

	isDead_ = false;

	// HPの設定
	currentHP_ = 8;
	maxHP_ = currentHP_; // 最大HPには設定した現在HPを設定
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void ImmobileEnemy::Update(Player* player) { 
	///
	///	コライダー更新処理
	/// 
	
	UpdateCollider(); 

	///
	///	オブジェクト更新処理
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
void ImmobileEnemy::Draw() {
	// オブジェクト描画
	objectEnemy_->Draw();
}

// ---------------------------------------------------------
// UI描画処理
// ---------------------------------------------------------
void ImmobileEnemy::DrawUI() {
	// オブジェクトのワールド座標->スクリーン座標に変換
	Float3 screenPosition = Utility::WorldToScreen(objectEnemy_->transform_.translate);
	// 上にずらす分のオフセット
	const float kOffsetHPBar = 90.0f;

	// HP割合
	float hpRatio = static_cast<float>(currentHP_) / static_cast<float>(maxHP_);

	///
	/// HPバー（後景）描画
	///

	// スクリーン座標をセット
	spriteHPBackground_->SetPosition({
	    screenPosition.x - kHPBarSize.x / 2.0f, // HPバーが中心になるように設定,
	    screenPosition.y - kOffsetHPBar         // オフセット分上にずらす
	});
	spriteHPBackground_->Draw();

	///
	///	HPバー（前景）描画
	///

	// 現在HPに応じてサイズ変更
	Float2 hpBarForegroundSize = {kHPBarSize.x * hpRatio, kHPBarSize.y};
	spriteHPForeground_->SetSize(hpBarForegroundSize);

	// スクリーン座標をセット
	spriteHPForeground_->SetPosition({
	    screenPosition.x - kHPBarSize.x / 2.0f, // HPバーが中心になるように設定
	    screenPosition.y - kOffsetHPBar         // オフセット分上にずらす
	});
	spriteHPForeground_->Draw();
}

// ---------------------------------------------------------
// 衝突時コールバック
// ---------------------------------------------------------
void ImmobileEnemy::OnCollision(Collider* other) {
	///
	///	vs PlayerBullet
	/// 
	
	if (other->GetTag() == "PlayerBullet") {
		// PlayerBulletのdamageを取得
		Bullet* bullet = dynamic_cast<Bullet*>(other->GetOwner());
		int32_t damage = bullet->GetDamage();

		// HPを減らす
		currentHP_ -= damage;

		// HPが0になった敵を死亡させる
		if (currentHP_ <= 0) {
			isDead_ = true;
		}
	}
}

// ---------------------------------------------------------
// コライダー更新処理
// ---------------------------------------------------------
void ImmobileEnemy::UpdateCollider() {
	if (AABBCollider* aabb = dynamic_cast<AABBCollider*>(collider_.get())) {
		Float3 center = objectEnemy_->transform_.translate;
		Float3 size = colliderSize_;

		// min
		aabb->min_ = center - size;
		aabb->max_ = center + size;
	}
}
