#include "GameResourceLoader.h"

// Engine
#include <ModelManager.h>
#include <Engine/ParticleEffect/ParticleEffectManager.h>
#include <SkyBoxManager.h>
#include <SoundManager.h>

// Application
#include <src/Game/Transition/SplitBlockTransition.h>

#include <src/Game/Particles/Backscatter/BackscatterParticle.h>
#include <src/Game/Particles/ExplodeScatter/ExplodeScatterParticle.h>
#include <src/Game/Particles/ExplodeSmoke/ExplodeSmokeParticle.h>
#include <src/Game/Particles/GroundWarning/RedCircleParticle.h>
#include <src/Game/Particles/HomingMissile/MissileSmokeParticle.h>
#include <src/Game/Particles/Smoke/SmokeParticle.h>
#include <src/Game/Particles/Spark/SparkParticle.h>
#include <src/Game/Particles/WallCollapse/WallCollapseParticle.h>
#include <src/Game/Particles/BloodSplatter/BloodSplatterParticle.h>
#include <src/Game/Particles/ImpactSmoke/ImpactSmokeParticle.h>
#include <src/Game/Particles/BloodSmoke/BloodSmokeParticle.h>
#include <src/Game/Particles/DeathCrossParticle/DeathCrossParticle.h>
#include <src/Game/Particles/ShellEjection/ShellEjectionParticle.h>
#include <src/Game/Particles/MuzzleFlash/MuzzleFlashParticle.h>
#include <src/Game/Particles/BossFragments/BossFragmentsParticle.h>
#include <src/Game/Particles/BloodScatter/BloodScatterParticle.h>
#include <src/Game/Particles/TeleporterRing/TeleporterRingParticle.h>
#include <src/Game/Particles/GunOverheatSmoke/GunOverheatSmokeParticle.h>

GameResourceLoader* GameResourceLoader::GetInstance() {
	static GameResourceLoader instance;
	return &instance;
}

void GameResourceLoader::Initialize() {
	// モデルデータの読み込み
	LoadAllModelData();
	// パーティクルの生成と登録
	RegisterAllParticleEffect();
	// サウンドデータの読み込み
	LoadAllSoundData();

	// SkyBoxの初期化
	Cygnus::SkyBoxManager::GetInstance()->Initialize("skybox.dds");

	// カーソル非表示 + フルスクリーン化
	ShowCursor(FALSE);
}

void GameResourceLoader::LoadAllModelData() {
	// ---------------------------------------------------------
	// プリミティブ
	// ---------------------------------------------------------

	// 板
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"Plane",
		"Primitive/Plane/plane.obj",
		"white.png"
	);

	// スフィア
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"Sphere", 
		"Primitive/Sphere/sphere.obj", 
		"white.png"
	);

	// キューブ
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"Cube",
		"Primitive/Cube/cube.obj",
		"white.png"
	);

	// ---------------------------------------------------------
	// 弾モデル
	// ---------------------------------------------------------

	// 弾（プレイヤー・敵共通で使用）
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"Bullet", 
		"Primitive/Cube/cube.obj", 
		"white.png"
	);
	// ミサイル（ボスで使用）
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"Missile", 
		"Bullet/Missile/missile.obj", 
		"white.png"
	);

	// ---------------------------------------------------------
	// 敵モデル
	// ---------------------------------------------------------

	// 通常敵
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"NormalEnemy", 
		"Character/Enemy/NormalEnemy/normalEnemy.obj", 
		"Character/Enemy/NormalEnemy/normalEnemy.png"
	);
	// 固定砲台敵
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"ImmobileEnemy", 
		"Character/Enemy/ImmobileEnemy/immobileEnemy.obj", 
		"Character/Enemy/ImmobileEnemy/immobileEnemy.png"
	);
	// ボス敵
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"BossEnemy", 
		"Character/Enemy/BossEnemy/bossEnemy.obj", 
		"Character/Enemy/BossEnemy/bossEnemy.png"
	);

	// ---------------------------------------------------------
	// 障害物モデル
	// ---------------------------------------------------------

	// コンテナ
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"Container",
		"Obstacle/Container/container.obj",
		"Obstacle/container.png"
	);
	// フェンス
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"Fence",
		"Obstacle/Fence/fence.obj",
		"Obstacle/fence.png"
	);
	// 壁
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"Wall",
		"Obstacle/Wall/wall.obj",
		"Obstacle/wall.png"
	);
	// レンガのパレット積み
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"BrickPallet",
		"Obstacle/BrickPallet/brickPallet.obj",
		"Obstacle/brickPallet.png"
	);
	// コンクリートバリア
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"ConcreteBarrier",
		"Obstacle/ConcreteBarrier/concreteBarrier.obj",
		"Obstacle/concreteBarrier.png"
	);
	// ドラム缶
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"DrumCan",
		"Obstacle/DrumCan/drumCan.obj",
		"Obstacle/drumCan.png"
	);
	// 貯水タンク
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"WaterTank",
		"Obstacle/WaterTank/waterTank.obj",
		"Obstacle/waterTank.png"
	);
	// 鉄骨の束
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"SteelBundle",
		"Obstacle/SteelBundle/steelBundle.obj",
		"Obstacle/steelBundle.png"
	);
	// 縦型タンク
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"VerticalTank",
		"Obstacle/VerticalTank/verticalTank.obj",
		"Obstacle/verticalTank.png"
	);
	// ISOタンク
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"ISOTank",
		"Obstacle/ISOTank/ISOTank.obj",
		"Obstacle/ISOTank.png"
	);
	// IBCコンテナ
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"IBCContainer",
		"Obstacle/IBCContainer/IBCContainer.obj",
		"Obstacle/IBCContainer.png"
	);
	// 壊れそうな壁
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"CrumblingWall",
		"Obstacle/CrumblingWall/crumblingWall.obj",
		"Obstacle/crumblingWall.png"
	);

	// ---------------------------------------------------------
	// パーティクル用モデル
	// ---------------------------------------------------------

	// 地面警告表示用の板
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"RedCircle",
		"Primitive/Plane/plane.obj",
		"Effect/circle.png"
	);

	// 死亡時のクロスパーティクル用の板
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"DeathCross",
		"Primitive/Plane/plane.obj",
		"Effect/glow.png"
	);

	// 銃のマズルフラッシュ用の板
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"MuzzleFlash",
		"Primitive/Plane/planeAlign.obj",	/* 中心位置をずらした板モデル */
		"Effect/muzzle.png"
	);

	// テレポーター用のリング
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"TeleporterRing",
		"Primitive/Torus/torus.obj",
		"white.png"
	);

	// ---------------------------------------------------------
	// その他モデル
	// ---------------------------------------------------------

	// ダイナマイト
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"Dynamite",
		"Object/Dynamite/dynamite.obj",
		"Object/dynamite.png"
	);

	// テレポーター
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"Teleporter",
		"Object/Teleporter/teleporter.obj",
		"Object/teleporter.png"
	);

	// 発電機
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"PowerGenerator",
		"Object/PowerGenerator/powerGenerator.obj",
		"Object/powerGenerator.png"
	);

	// 銃
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"Gun",
		"Object/Gun/gun.obj",
		"white.png"
	);

	// ピストル
	Cygnus::ModelManager::GetInstance()->LoadAndRegisterModel(
		"Pistol", 
		"Object/Gun/pistol.obj", 
		"white.png"
	);
}

void GameResourceLoader::RegisterAllParticleEffect()
{
	// 後ろへ飛散するパーティクル
	RegisterParticle<BackscatterParticle>("backscatter", "Cube");
	// ミサイルの煙パーティクル
	RegisterParticle<MissileSmokeParticle>("missileSmoke", "Cube");
	// 地面警告表示パーティクル
	RegisterParticle<RedCircleParticle>("redCircle", "RedCircle");
	// 地面警告攻撃の煙パーティクル
	RegisterParticle<ExplodeSmokeParticle>("explodeSmoke", "Cube");
	// 地面警告攻撃の飛散パーティクル
	RegisterParticle<ExplodeScatterParticle>("explodeScatter", "Cube");
	// 煙パーティクル
	RegisterParticle<SmokeParticle>("smoke", "Cube");
	// 火花パーティクル
	RegisterParticle<SparkParticle>("spark", "Cube");
	// 壁崩壊パーティクル
	RegisterParticle<WallCollapseParticle>("wallCollapse", "Cube");
	// 血が飛び散るパーティクル
	RegisterParticle<BloodSplatterParticle>("bloodSplatter", "Cube");
	// 弾衝突時の煙パーティクル
	RegisterParticle<ImpactSmokeParticle>("impactSmoke", "Cube");
	// 弾衝突時の血煙パーティクル
	RegisterParticle<BloodSmokeParticle>("bloodSmoke", "Cube");
	// 死亡時のクロスパーティクル
	RegisterParticle<DeathCrossParticle>("deathCross", "DeathCross");
	// 薬莢排出パーティクル
	RegisterParticle<ShellEjectionParticle>("shellEjection", "Cube");
	// マズルフラッシュパーティクル
	RegisterParticle<MuzzleFlashParticle>("muzzleFlash", "MuzzleFlash");
	// ボスの破片パーティクル
	RegisterParticle<BossFragmentsParticle>("bossFragments", "Cube");
	// 血が飛散するパーティクル
	RegisterParticle<BloodScatterParticle>("bloodScatter", "Cube");
	// テレポーターリングパーティクル
	RegisterParticle<TeleporterRingParticle>("teleporterRing", "TeleporterRing");
	// 銃のオーバーヒート時煙パーティクル
	RegisterParticle<GunOverheatSmokeParticle>("gunOverheatSmoke", "Cube");
}

void GameResourceLoader::LoadAllSoundData() { 
	auto soundManager = Cygnus::SoundManager::GetInstance();

	// プレイヤー射撃音
	soundManager->Load("resources/Sounds/player_shoot.wav", "player_shoot");
	// 敵死亡音
	soundManager->Load("resources/Sounds/enemy_dead.wav", "enemy_dead");
	// 柔らかい敵ヒット音
	soundManager->Load("resources/Sounds/hit_soft_enemy.wav", "hit_soft_enemy");
	// 硬い敵ヒット音
	soundManager->Load("resources/Sounds/hit_hard_enemy.wav", "hit_hard_enemy");
	// 障害物ヒット音
	soundManager->Load("resources/Sounds/hit_obstacle.wav", "hit_obstacle");
	// タイトルのボタン押下音
	soundManager->Load("resources/Sounds/title_button.wav", "title_button");
}
