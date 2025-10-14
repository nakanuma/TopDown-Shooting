#include "BossEnemy.h"

// C++
#include <numbers>

// Engine
#include <Engine/ParticleEffect/ParticleEffectManager.h>
#include <Engine/Scene/SceneManager.h>
#include <Engine/Util/RandomGenerator.h>
#include <Engine/Util/TimeManager.h>

// Application
#include <src/Game/Bullet/GroundWarning/GroundWarning.h>
#include <src/Game/Bullet/HomingMissile/HomingMissile.h>
#include <src/Game/Bullet/Manager/BulletManager.h>
#include <src/Game/Player/Player.h>
#include <src/Game/System/ResultStats.h>

void BossEnemy::Initialize(const Float3& position, ModelManager::ModelData* model, Player* player) {
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
	objectEnemy_->materialCB_.data_->color = {0.2f, 0.2f, 0.2f, 1.0f};

	///
	///	コライダー生成
	///

	collider_ = std::make_unique<OBBCollider>();
	collider_->SetTag("BossEnemy");
	collider_->SetOwner(this);
	colliderSize_ = {5.0f, 3.0f, 5.0f};

	// コライダーを登録
	CollisionManager::GetInstance()->Register(collider_.get());
	UpdateCollider(); // 生成時にコライダーの更新を行っておく（初期化時1フレームのみ衝突を回避）

	///
	///	スプライト生成
	///

	// HPバー（後景）
	uint32_t textureHPBackground = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());
	spriteHPBackground_ = std::make_unique<Sprite>();
	spriteHPBackground_->Initialize(spriteCommon_.get(), textureHPBackground);
	spriteHPBackground_->SetSize(kHPBarSizeBoss);
	spriteHPBackground_->SetPosition({kHPBarPosition.x - (kHPBarSizeBoss.x / 2.0f), kHPBarPosition.y}); // 中心になるよう設定
	spriteHPBackground_->SetColor({0.0f, 0.0f, 0.0f, 1.0f});                                            // 黒

	// HPバー（前景）
	uint32_t textureHPForeground = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());
	spriteHPForeground_ = std::make_unique<Sprite>();
	spriteHPForeground_->Initialize(spriteCommon_.get(), textureHPForeground);
	spriteHPForeground_->SetSize(kHPBarSizeBoss);
	spriteHPForeground_->SetPosition({kHPBarPosition.x - (kHPBarSizeBoss.x / 2.0f), kHPBarPosition.y}); // 中心になるよう設定
	spriteHPForeground_->SetColor({1.0f, 0.0f, 0.0f, 1.0f});                                            // 赤

	///
	///	パラメーター設定
	///

	isActive_ = false;

	isDead_ = false;

	// HPの設定
	currentHP_ = 1000;
	maxHP_ = currentHP_;

	targetPlayer_ = player;

	///
	///	ビヘイビアツリー構築
	///

	BuildBehaviorTree();

	btEditor_ = std::make_unique<BehaviorTreeEditor<BossEnemy>>();
	btEditor_->SetBehaviorTree(behaviorTree_.get());
}

void BossEnemy::Update() {
	///
	/// オブジェクト更新処理
	///

	objectEnemy_->UpdateMatrix();
	objectEnemy_->UpdateShadowMatrix();

	if (!isActive_)
		return;

	///
	///	ビヘイビアツリーを評価
	///

	if (behaviorTree_) {
		behaviorTree_->Tick(this, TimeManager::GetInstance()->GetDeltaTime());
	}

	///
	/// コライダー更新処理
	///

	UpdateCollider();

	///
	///	スプライト更新処理
	///

	// HPバー（後景）更新
	spriteHPBackground_->Update();
	// HPバー（前景）更新
	spriteHPForeground_->Update();
}

void BossEnemy::Draw() {
	// オブジェクト描画処理
	objectEnemy_->Draw();
}

void BossEnemy::DrawShadow() { objectEnemy_->DrawShadow(); }

void BossEnemy::DrawUI() {
	// HP割合
	float hpRatio = static_cast<float>(currentHP_) / static_cast<float>(maxHP_);

	///
	/// HPバー（後景）描画
	///

	spriteHPBackground_->Draw();

	///
	/// HPバー（前景）描画
	///

	// 現在HPに応じてサイズ変更
	Float2 hpBarForegroundSize = {kHPBarSizeBoss.x * hpRatio, kHPBarSizeBoss.y};
	spriteHPForeground_->SetSize(hpBarForegroundSize);

	spriteHPForeground_->Draw();
}

void BossEnemy::Debug() {
	ImGui::Begin("BossEnemy");

	if (ImGui::Button("Active")) {
		isActive_ = true;
	}
	if (ImGui::Button("Dying")) {
		currentHP_ = 10;
	}

	ImGui::Separator();

	if (ImGui::Button("FireHomingMissile")) {
		FireHomingMissile();
	}
	if (ImGui::Button("GroundWarningAttack")) {
		GroundWarningAttack();
	}

	ImGui::DragFloat3("translate", &objectEnemy_->transform_.translate.x, 0.01f);
	ImGui::DragFloat3("rotate", &objectEnemy_->transform_.rotate.x, 0.01f);
	ImGui::DragFloat3("scale", &objectEnemy_->transform_.scale.x, 0.01f);

	ImGui::End();

	ImGui::Begin("BehaviorTree_BossEnemy");
	// BTエディター描画
	btEditor_->Draw();

	if (ImGui::Button("SAVE")) {
		btEditor_->Save("bossEnemy.json");
	}
	if (ImGui::Button("LOAD")) {
		btEditor_->Load("bossEnemy.json");
	}

	ImGui::End();
}

void BossEnemy::OnCollision(Collider* other) {
	///
	/// vs PlayerBullet
	///
	if (other->GetTag() == "PlayerBullet") {
		// プレイヤーに攻撃されたら有効化（todo : 登場演出を入れる予定なので仮。あとで削除）
		if (!isActive_) {
			isActive_ = true;
		}

		// PlayerBulletのdamageを取得
		Bullet* bullet = dynamic_cast<Bullet*>(other->GetOwner());
		int32_t damage = bullet->GetDamage();

		// HPを減らす
		currentHP_ -= damage;
		ResultStats::GetInstance()->AddHit();          // 弾が命中したことを記録
		ResultStats::GetInstance()->AddDamage(damage); // 与えたダメージを記録

		// HPが0になったら死亡させる
		if (currentHP_ <= 0) {
			isDead_ = true;
			ResultStats::GetInstance()->AddDefeated(); // 撃破したことを記録

			// 死亡したらリザルトへ以降（todo : 死亡演出から遷移予定なので仮。あとで削除）
			SceneManager::GetInstance()->ChangeScene("RESULT");
		}
	}
}

void BossEnemy::UpdateCollider() {
	if (OBBCollider* obb = dynamic_cast<OBBCollider*>(collider_.get())) {
		Float3 center = objectEnemy_->transform_.translate;
		Float3 size = colliderSize_;

		obb->center_ = center;
		obb->size_ = colliderSize_;

		// 回転軸の更新
		Matrix rotMat = Matrix::Rotation(objectEnemy_->transform_.rotate);
		obb->xAxis_ = Float3::Normalize(Float3(rotMat.r[0][0], rotMat.r[1][0], rotMat.r[2][0]));
		obb->yAxis_ = Float3::Normalize(Float3(rotMat.r[0][1], rotMat.r[1][1], rotMat.r[2][1]));
		obb->zAxis_ = Float3::Normalize(Float3(rotMat.r[0][2], rotMat.r[1][2], rotMat.r[2][2]));
	}
}

void BossEnemy::FacePlayer() {
	// プレイヤーへの方向ベクトル
	Float3 toPlayer = targetPlayer_->GetTranslate() - objectEnemy_->transform_.translate;
	// 方向ベクトルからY軸回転角度を計算
	float targetAngle = std::atan2(toPlayer.x, toPlayer.z);
	// Y軸に回転を適用
	objectEnemy_->transform_.rotate.y = targetAngle;
}

void BossEnemy::MoveTowardPlayer() {
	// プレイヤーへの方向ベクトル
	Float3 toPlayer = targetPlayer_->GetTranslate() - objectEnemy_->transform_.translate;
	toPlayer.y = 0.0f;
	toPlayer = Float3::Normalize(toPlayer);

	objectEnemy_->transform_.translate += toPlayer * moveSpeed_ * TimeManager::GetInstance()->GetDeltaTime();
}

void BossEnemy::FireHomingMissile() {
	// 発射方向
	Float3 direction = targetPlayer_->GetTranslate() - objectEnemy_->transform_.translate;
	direction = Float3::Normalize(direction);

	// 弾の生成
	auto newBullet = std::make_unique<HomingMissile>();
	newBullet->Initialize(objectEnemy_->transform_.translate, direction, modelMissile_);
	newBullet->SetPlayer(targetPlayer_);
	BulletManager::GetInstance()->AddBullet(std::move(newBullet));
}

void BossEnemy::GroundWarningAttack() {
	Float3 playerPos = targetPlayer_->GetTranslate();

	// 弾の生成
	auto newBullet = std::make_unique<GroundWarning>();
	newBullet->Initialize({playerPos.x, 0.0f, playerPos.z}, {0.0f, 0.0f, 0.0f}, modelGroundWarning_);
	BulletManager::GetInstance()->AddBullet(std::move(newBullet));

	// 赤い円エフェクト発生
	ParticleEffectManager::GetInstance()->Emit("redCircle", {playerPos.x, 0.1f, playerPos.z}, 1);
}

void BossEnemy::BuildBehaviorTree() {
	///
	///	移動系（並列の一方）
	///

	auto facePlayer = std::make_unique<ActionNode<BossEnemy>>(
	    [](BossEnemy* enemy, float dt) -> BehaviorStatus {
		    enemy->FacePlayer();
		    return BehaviorStatus::Running;
	    },
	    "facePlayer");

	auto moveTowardPlayer = std::make_unique<ActionNode<BossEnemy>>(
	    [](BossEnemy* enemy, float dt) -> BehaviorStatus {
		    enemy->MoveTowardPlayer();
		    return BehaviorStatus::Running;
	    },
	    "moveTowardPlayer");

	auto moveParallel = std::make_unique<ParallelNode<BossEnemy>>("moveParallel");
	moveParallel->AddChild(std::move(facePlayer));
	moveParallel->AddChild(std::move(moveTowardPlayer));

	///
	///	攻撃系（並列のもう一方）
	///

	auto wait = std::make_unique<WaitNode<BossEnemy>>(0.1f, 2.5f, "0.1f ~ 2.5f"); // 次の攻撃まで待機

	auto randAttack = std::make_unique<ActionNode<BossEnemy>>(
	    [](BossEnemy* enemy, float dt) -> BehaviorStatus {
		    if (rand() % 2 == 0) {
			    enemy->FireHomingMissile();
		    } else {
			    enemy->GroundWarningAttack();
		    }
		    return BehaviorStatus::Success;
	    },
	    "randAttack");

	auto attackSequence = std::make_unique<SequenceNode<BossEnemy>>("attackSequence");
	attackSequence->AddChild(std::move(wait));
	attackSequence->AddChild(std::move(randAttack));

	///
	///	ルートノード
	///

	auto root = std::make_unique<ParallelNode<BossEnemy>>("root");
	root->AddChild(std::move(moveParallel));
	root->AddChild(std::move(attackSequence));

	behaviorTree_ = std::make_unique<BehaviorTree<BossEnemy>>(std::move(root));
}