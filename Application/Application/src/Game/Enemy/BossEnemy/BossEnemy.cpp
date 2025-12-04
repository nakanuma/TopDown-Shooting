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
#include <src/Game/Camera/CameraShake.h>

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
	objectEnemy_->transform_.translate_ = position;
	objectEnemy_->transform_.rotate_ = {0.0f, std::numbers::pi_v<float>, 0.0f}; // 手前を向いた状態でスポーン（一時的に）
	objectEnemy_->materialCB_.data_->color = kBossColor;
	objectEnemy_->materialCB_.data_->emissiveColor = kHitBlinkColor;

	///
	///	コライダー生成
	///

	auto obb = std::make_unique<OBBCollider>();
	obb->SetTag("BossEnemy");
	obb->SetFollowTarget(&objectEnemy_->transform_.translate_);
	obb->SetFollowRotation(&objectEnemy_->transform_.rotate_);
	obb->SetSize(kColliderSize);
	obb->SetOwner(this);

	collider_ = std::move(obb);
	CollisionManager::GetInstance()->Register(collider_.get());

	collider_->Update(); // 生成時にコライダーの更新を行っておく（初期化時1フレームのみ衝突を回避）

	///
	///	スプライト生成
	///

	// HPバー（後景）
	uint32_t textureHPBackground = TextureManager::Load("white.png");
	spriteHPBackground_ = std::make_unique<Sprite>();
	spriteHPBackground_->Initialize(spriteCommon_.get(), textureHPBackground);
	spriteHPBackground_->SetSize(kHPBarSizeBoss);
	spriteHPBackground_->SetPosition({kHPBarPosition.x - (kHPBarSizeBoss.x / 2.0f), kHPBarPosition.y}); // 中心になるよう設定
	spriteHPBackground_->SetColor(kHPBarBackgroundColor);

	// HPバー（前景）
	uint32_t textureHPForeground = TextureManager::Load("white.png");
	spriteHPForeground_ = std::make_unique<Sprite>();
	spriteHPForeground_->Initialize(spriteCommon_.get(), textureHPForeground);
	spriteHPForeground_->SetSize(kHPBarSizeBoss);
	spriteHPForeground_->SetPosition({kHPBarPosition.x - (kHPBarSizeBoss.x / 2.0f), kHPBarPosition.y}); // 中心になるよう設定
	spriteHPForeground_->SetColor(kHPBarForegroundColor);

	///
	///	パラメーター設定
	///

	isActive_ = false;

	isDead_ = false;

	// HPの設定
	currentHP_ = kInitialHP;
	maxHP_ = currentHP_;

	// ターゲットの設定
	targetPlayer_ = player;

	///
	///	ビヘイビアツリー構築
	///

	BuildBehaviorTree();

#ifdef USE_IMGUI
	btEditor_ = std::make_unique<BehaviorTreeEditor<BossEnemy>>();
	btEditor_->SetBehaviorTree(behaviorTree_.get());
#endif
}

void BossEnemy::Update() {
	///
	/// オブジェクト更新処理
	///

	objectEnemy_->UpdateMatrix();
	objectEnemy_->UpdateShadowMatrix();

	// 被弾時の発光演出
	HandleHitBlink();

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

	collider_->Update();

	///
	///	スプライト更新処理
	///

	// HPバー（後景）更新
	spriteHPBackground_->Update();
	// HPバー（前景）更新
	spriteHPForeground_->Update();

	///
	///	死亡演出更新
	///

	if (isDying_) {
		// タイマー加算
		dyingTimer_ += TimeManager::GetInstance()->GetDeltaTime();

		// 死亡演出時間の終了で死亡（インスタンス削除）
		if (dyingTimer_ >= kDyingDuration) {
			// パーティクル発生
			for (size_t i = 0; i < kDyingEmitCount; i++) {
				Float3 offset = RandomGenerator::GetInstance()->RandomValue(kExplodeSmokeOffsetMin, kExplodeSmokeOffsetMax);
				ParticleEffectManager::GetInstance()->Emit("explodeSmoke", GetTranslate() + offset, kExplodeSmokeCount); // 煙パーティクル発生
			}
			ParticleEffectManager::GetInstance()->Emit("bossFragments", GetTranslate(), kBossFragmentsCount); // 破片パーティクル発生

			isDead_ = true;
		}
	}
}

void BossEnemy::Draw() {
	// オブジェクト描画処理
	objectEnemy_->Draw();
}

void BossEnemy::DrawShadow() { objectEnemy_->DrawShadow(); }

void BossEnemy::DrawUI() {
	if (isDying_)
		return; // 死亡演出中はスキップ

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
#ifdef USE_IMGUI
	ImGui::Begin("BossEnemy");

	ImGui::DragFloat3("translate", &objectEnemy_->transform_.translate_.x);
	ImGui::DragFloat3("rotate", &objectEnemy_->transform_.rotate_.x);
	ImGui::DragFloat3("scale", &objectEnemy_->transform_.scale_.x);

	ImGui::Separator();

	ImGui::Checkbox("isDying", &isDying_);
	ImGui::Text("%.2f", dyingTimer_);

	ImGui::End();
#endif
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

		// 被弾時の発光演出を開始
		if (!isDying_) {
			isHitBlink_ = true;
			hitBlinkPhase_ = HitBlinkPhase::BlinkIn;
			hitBlinkTimer_ = 0.0f;
		}

		// PlayerBulletのdamageを取得
		Bullet* bullet = dynamic_cast<Bullet*>(other->GetOwner());
		int32_t damage = bullet->GetDamage();

		// HPを減らす
		currentHP_ -= damage;
		ResultStats::GetInstance()->AddHit();          // 弾が命中したことを記録
		ResultStats::GetInstance()->AddDamage(damage); // 与えたダメージを記録

		// HPが0になったら死亡させる
		if (currentHP_ <= 0 && !isDying_) {
			isDying_ = true;                           // 死亡演出の開始
			ResultStats::GetInstance()->AddDefeated(); // 撃破したことを記録
		}
	}
}

void BossEnemy::FacePlayer() {
	if (isDying_)
		return; // 死亡演出中はスキップ

	// プレイヤーへの方向ベクトル
	Float3 toPlayer = targetPlayer_->GetTranslate() - objectEnemy_->transform_.translate_;
	// 方向ベクトルからY軸回転角度を計算
	float targetAngle = std::atan2(toPlayer.x, toPlayer.z);
	// Y軸に回転を適用
	objectEnemy_->transform_.rotate_.y = targetAngle;
}

void BossEnemy::MoveTowardPlayer() {
	if (isDying_)
		return; // 死亡演出中はスキップ
	if (targetPlayer_->IsDead())
		return; // プレイヤーが死亡していたらスキップ

	// プレイヤーへの方向ベクトル
	Float3 toPlayer = targetPlayer_->GetTranslate() - objectEnemy_->transform_.translate_;
	toPlayer.y = 0.0f;
	toPlayer = Float3::Normalize(toPlayer);

	// 移動処理
	objectEnemy_->transform_.translate_ += toPlayer * kMoveSpeed * TimeManager::GetInstance()->GetDeltaTime();
}

void BossEnemy::FireHomingMissile() {
	if (isDying_)
		return; // 死亡演出中はスキップ

	// 発射方向
	Float3 direction = targetPlayer_->GetTranslate() - objectEnemy_->transform_.translate_;
	direction = Float3::Normalize(direction);

	// 弾の生成と追加
	auto newBullet = std::make_unique<HomingMissile>();
	newBullet->Initialize(objectEnemy_->transform_.translate_, direction, &ModelManager::GetInstance()->GetModel("Missile"));
	newBullet->SetPlayer(targetPlayer_);
	BulletManager::GetInstance()->AddBullet(std::move(newBullet));
}

void BossEnemy::GroundWarningAttack() {
	if (isDying_)
		return; // 死亡演出中はスキップ

	Float3 playerPos = targetPlayer_->GetTranslate();

	// 弾の生成と追加
	auto newBullet = std::make_unique<GroundWarning>();
	newBullet->Initialize({playerPos.x, 0.0f, playerPos.z}, {0.0f, 0.0f, 0.0f}, &ModelManager::GetInstance()->GetModel("Sphere"));
	BulletManager::GetInstance()->AddBullet(std::move(newBullet));

	// 赤い円エフェクト発生
	ParticleEffectManager::GetInstance()->Emit("redCircle", playerPos + kRedCircleOffset, kRedCircleCount);
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

	// 移動パラレルノード構築
	auto moveParallel = std::make_unique<ParallelNode<BossEnemy>>("moveParallel");
	moveParallel->AddChild(std::move(facePlayer));
	moveParallel->AddChild(std::move(moveTowardPlayer));

	///
	///	攻撃系（並列のもう一方）
	///

	auto wait = std::make_unique<WaitNode<BossEnemy>>(kWaitAttackTimeMin, kWaitAttackTimeMax, "wait"); // 次の攻撃まで待機

	auto randAttack = std::make_unique<ActionNode<BossEnemy>>(
	    [](BossEnemy* enemy, float dt) -> BehaviorStatus {
		    if (RandomGenerator::GetInstance()->RandomValueBool()) {
			    enemy->FireHomingMissile();
		    } else {
			    enemy->GroundWarningAttack();
		    }
		    return BehaviorStatus::Success;
	    },
	    "randAttack");

	// 攻撃シーケンスノード構築
	auto attackSequence = std::make_unique<SequenceNode<BossEnemy>>("attackSequence");
	attackSequence->AddChild(std::move(wait));
	attackSequence->AddChild(std::move(randAttack));

	///
	///	ルートノード構築
	///

	auto root = std::make_unique<ParallelNode<BossEnemy>>("root");
	root->AddChild(std::move(moveParallel));
	root->AddChild(std::move(attackSequence));

	behaviorTree_ = std::make_unique<BehaviorTree<BossEnemy>>(std::move(root));
}