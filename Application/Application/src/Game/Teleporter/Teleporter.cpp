#include "Teleporter.h"

// Application
#include <src/Game/Player/Player.h>

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void Teleporter::Initialize(const Float3& position, ModelManager::ModelData* model)
{
	///
	///	オブジェクト生成
	/// 
	
	object_ = std::make_unique<Object3D>();
	object_->model_ = model;
	object_->transform_.translate = position;
	object_->materialCB_.data_->color = { 1.0f, 0.0f, 0.0f, 1.0f };

	///
	///	コライダー生成
	/// 
	
	collider_ = std::make_unique<AABBCollider>();
	collider_->SetTag("Teleporter");
	collider_->SetOwner(this);

	// コライダーを登録
	CollisionManager::GetInstance()->Register(collider_.get());
	UpdateCollider(); // 生成時にコライダーの更新を行っておく（初期化時1フレームのみ衝突を回避）

	// 使用可能にしておく
	isActive_ = true;
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void Teleporter::Update()
{
	///
	///	コライダー更新処理
	/// 
	
	UpdateCollider();

	///
	///	オブジェクト更新処理
	/// 
	
	object_->UpdateMatrix();
}

// ---------------------------------------------------------
// 描画処理
// ---------------------------------------------------------
void Teleporter::Draw()
{
	// オブジェクト描画
	object_->Draw();
}

// ---------------------------------------------------------
// 衝突時コールバック
// ---------------------------------------------------------
void Teleporter::OnCollision(Collider* other)
{
	// vs Player
	if (other->GetTag() == "Player") {
		Player* player = static_cast<Player*>(other->GetOwner());

		// プレイヤーをペアのテレポーター位置へ送る（todo : 今は直接移動なので、ここで数秒待ってテレポートする演出を入れる）
		if (pair_ && pair_->isActive_) {
			player->SetTranslate({ 
				pair_->GetTranslate().x,
				player->GetTranslate().y,
				pair_->GetTranslate().z, 
			});

			// 使用したテレポーターは無効化する
			this->isActive_ = false;
			pair_->isActive_ = false;

			// デバッグ用に使用したテレポーターは青くする（後で消す）
			this->object_->materialCB_.data_->color = { 0.0f, 0.0f, 1.0f, 1.0f };
			pair_->object_->materialCB_.data_->color = { 0.0f, 0.0f, 1.0f, 1.0f };
		}
	}
}

// ---------------------------------------------------------
// コライダー更新処理
// ---------------------------------------------------------
void Teleporter::UpdateCollider()
{
	if (AABBCollider* aabb = dynamic_cast<AABBCollider*>(collider_.get())) {
		Float3 center = object_->transform_.translate;
		Float3 size = colliderSize_;

		aabb->min_ = center - size;
		aabb->max_ = center + size;
	}
}
