#include "GameResourceLoader.h"

// Engine
#include <ModelManager.h>
#include <Engine/ParticleEffect/ParticleEffectManager.h>
#include <SkyBoxManager.h>

// Application
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

GameResourceLoader* GameResourceLoader::GetInstance() {
	static GameResourceLoader instance;
	return &instance;
}

void GameResourceLoader::Initialize() {
	// モデルデータの読み込み
	LoadAllModelData();
	// パーティクルの生成と登録
	RegisterAllParticleEffect();

	// SkyBoxの初期化
	SkyBoxManager::GetInstance()->Initialize("skybox.dds");
}

void GameResourceLoader::LoadAllModelData() {
	// ---------------------------------------------------------
	// プリミティブ
	// ---------------------------------------------------------

	// 板
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"Plane",
		"Primitive/Plane/plane.obj",
		"white.png"
	);

	// スフィア
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"Sphere", 
		"Primitive/Sphere/sphere.obj", 
		"white.png"
	);

	// キューブ
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"Cube",
		"Primitive/Cube/cube.obj",
		"white.png"
	);

	// ---------------------------------------------------------
	// 弾モデル
	// ---------------------------------------------------------

	// 弾（プレイヤー・敵共通で使用）
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"Bullet", 
		"Primitive/Cube/cube.obj", 
		"white.png"
	);
	// ミサイル（ボスで使用）
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"Missile", 
		"Bullet/Missile/missile.obj", 
		"white.png"
	);

	// ---------------------------------------------------------
	// 敵モデル
	// ---------------------------------------------------------

	// 通常敵
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"NormalEnemy", 
		"Character/Enemy/NormalEnemy/normalEnemy.obj", 
		"Character/Enemy/NormalEnemy/normalEnemy.png"
	);
	// 固定砲台敵
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"ImmobileEnemy", 
		"Character/Enemy/ImmobileEnemy/immobileEnemy.obj", 
		"Character/Enemy/ImmobileEnemy/immobileEnemy.png"
	);
	// ボス敵
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"BossEnemy", 
		"Character/Enemy/BossEnemy/bossEnemy.obj", 
		"Character/Enemy/BossEnemy/bossEnemy.png"
	);

	// ---------------------------------------------------------
	// 障害物モデル
	// ---------------------------------------------------------

	// コンテナ
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"Container",
		"Obstacle/Container/container.obj",
		"Obstacle/container.png"
	);
	// フェンス
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"Fence",
		"Obstacle/Fence/fence.obj",
		"Obstacle/fence.png"
	);
	// 壁
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"Wall",
		"Obstacle/Wall/wall.obj",
		"Obstacle/wall.png"
	);
	// レンガのパレット積み
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"BrickPallet",
		"Obstacle/BrickPallet/brickPallet.obj",
		"Obstacle/brickPallet.png"
	);
	// コンクリートバリア
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"ConcreteBarrier",
		"Obstacle/ConcreteBarrier/concreteBarrier.obj",
		"Obstacle/concreteBarrier.png"
	);
	// ドラム缶
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"DrumCan",
		"Obstacle/DrumCan/drumCan.obj",
		"Obstacle/drumCan.png"
	);
	// 貯水タンク
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"WaterTank",
		"Obstacle/WaterTank/waterTank.obj",
		"Obstacle/waterTank.png"
	);
	// 鉄骨の束
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"SteelBundle",
		"Obstacle/SteelBundle/steelBundle.obj",
		"Obstacle/steelBundle.png"
	);
	// 縦型タンク
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"VerticalTank",
		"Obstacle/VerticalTank/verticalTank.obj",
		"Obstacle/verticalTank.png"
	);
	// ISOタンク
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"ISOTank",
		"Obstacle/ISOTank/ISOTank.obj",
		"Obstacle/ISOTank.png"
	);
	// IBCコンテナ
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"IBCContainer",
		"Obstacle/IBCContainer/IBCContainer.obj",
		"Obstacle/IBCContainer.png"
	);
	// 壊れそうな壁
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"CrumblingWall",
		"Obstacle/CrumblingWall/crumblingWall.obj",
		"Obstacle/crumblingWall.png"
	);

	// ---------------------------------------------------------
	// パーティクル用モデル
	// ---------------------------------------------------------

	// 地面警告表示用の板
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"RedCircle",
		"Primitive/Plane/plane.obj",
		"Effect/circle.png"
	);

	// 死亡時のクロスパーティクル用の板
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"DeathCross",
		"Primitive/Plane/plane.obj",
		"Effect/glow.png"
	);

	// 銃のマズルフラッシュ用の板
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"MuzzleFlash",
		"Primitive/Plane/planeAlign.obj",
		"Effect/muzzle.png"
	);

	// ---------------------------------------------------------
	// その他モデル
	// ---------------------------------------------------------

	// ダイナマイト
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"Dynamite",
		"Object/Dynamite/dynamite.obj",
		"Object/dynamite.png"
	);

	// テレポーター
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"Teleporter",
		"Object/Teleporter/teleporter.obj",
		"white.png"
	);

	// 銃
	ModelManager::GetInstance()->LoadAndRegisterModel(
		"Gun",
		"Object/Gun/gun.obj",
		"white.png"
	);
}

void GameResourceLoader::RegisterAllParticleEffect()
{
	// 後ろへ飛散するパーティクル
	auto backScatterParticle = std::make_unique<BackscatterParticle>(ModelManager::GetInstance()->GetModel("Cube"));
	ParticleEffectManager::GetInstance()->Register("backscatter", std::move(backScatterParticle));

	// ミサイルの煙パーティクル
	auto missileSmokeParticle = std::make_unique<MissileSmokeParticle>(ModelManager::GetInstance()->GetModel("Cube"));
	ParticleEffectManager::GetInstance()->Register("missileSmoke", std::move(missileSmokeParticle));

	// 地面警告表示パーティクル
	auto groundWarningParticle = std::make_unique<RedCircleParticle>(ModelManager::GetInstance()->GetModel("RedCircle"));
	ParticleEffectManager::GetInstance()->Register("redCircle", std::move(groundWarningParticle));

	// 地面警告攻撃の煙パーティクル
	auto explodeSmokeParticle = std::make_unique<ExplodeSmokeParticle>(ModelManager::GetInstance()->GetModel("Cube"));
	ParticleEffectManager::GetInstance()->Register("explodeSmoke", std::move(explodeSmokeParticle));

	// 地面警告攻撃の飛散パーティクル
	auto explodeScatterParticle = std::make_unique<ExplodeScatterParticle>(ModelManager::GetInstance()->GetModel("Cube"));
	ParticleEffectManager::GetInstance()->Register("explodeScatter", std::move(explodeScatterParticle));

	// 煙パーティクル
	auto smokeParticle = std::make_unique<SmokeParticle>(ModelManager::GetInstance()->GetModel("Cube"));
	ParticleEffectManager::GetInstance()->Register("smoke", std::move(smokeParticle));

	// 火花パーティクル
	auto sparkParticle = std::make_unique<SparkParticle>(ModelManager::GetInstance()->GetModel("Cube"));
	ParticleEffectManager::GetInstance()->Register("spark", std::move(sparkParticle));

	// 壁崩壊パーティクル
	auto wallCollapseParticle = std::make_unique<WallCollapseParticle>(ModelManager::GetInstance()->GetModel("Cube"));
	ParticleEffectManager::GetInstance()->Register("wallCollapse", std::move(wallCollapseParticle));

	// 血が飛び散るパーティクル
	auto bloodSplatterParticle = std::make_unique<BloodSplatterParticle>(ModelManager::GetInstance()->GetModel("Cube"));
	ParticleEffectManager::GetInstance()->Register("bloodSplatter", std::move(bloodSplatterParticle));

	// 弾衝突時の煙パーティクル
	auto impactSmokeParticle = std::make_unique<ImpactSmokeParticle>(ModelManager::GetInstance()->GetModel("Cube"));
	ParticleEffectManager::GetInstance()->Register("impactSmoke", std::move(impactSmokeParticle));

	// 弾衝突時の血煙パーティクル
	auto bloodSmokeParticle = std::make_unique<BloodSmokeParticle>(ModelManager::GetInstance()->GetModel("Cube"));
	ParticleEffectManager::GetInstance()->Register("bloodSmoke", std::move(bloodSmokeParticle));

	// 死亡時のクロスパーティクル
	auto deathCrossParticle = std::make_unique<DeathCrossParticle>(ModelManager::GetInstance()->GetModel("DeathCross"));
	ParticleEffectManager::GetInstance()->Register("deathCross", std::move(deathCrossParticle));

	// 薬莢排出パーティクル
	auto shellEjectionParticle = std::make_unique<ShellEjectionParticle>(ModelManager::GetInstance()->GetModel("Cube"));
	ParticleEffectManager::GetInstance()->Register("shellEjection", std::move(shellEjectionParticle));

	// マズルフラッシュパーティクル
	auto muzzleFlashParticle = std::make_unique<MuzzleFlashParticle>(ModelManager::GetInstance()->GetModel("MuzzleFlash"));
	ParticleEffectManager::GetInstance()->Register("muzzleFlash", std::move(muzzleFlashParticle));
}
