#include "ParticleEffectLoader.h"

// Engine
#include <Engine/ParticleEffect/ParticleEffectManager.h>

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


ParticleEffectLoader* ParticleEffectLoader::GetInstance() {
	static ParticleEffectLoader instance;
	return &instance;
}

void ParticleEffectLoader::LoadAndRegisterAll() {
	DirectXBase* dxBase = DirectXBase::GetInstance();

	/* パーティクルモデル生成 + パーティクル登録 */
	modelSmoothCube_ = ModelManager::LoadModelFile("resources/Models/", "smoothCube.obj", dxBase->GetDevice());
	modelSmoothCube_.material.textureHandle = TextureManager::Load("resources/Images/white.png", dxBase->GetDevice());

	modelPlane_ = ModelManager::LoadModelFile("resources/Models", "plane.obj", dxBase->GetDevice());
	modelPlane_.material.textureHandle = TextureManager::Load("resources/Images/Effect/circle.png", dxBase->GetDevice());

	// 後ろへ飛散するパーティクル
	auto backScatterParticle = std::make_unique<BackscatterParticle>(modelSmoothCube_);
	ParticleEffectManager::GetInstance()->Register("backscatter", std::move(backScatterParticle));
	// ミサイルの煙パーティクル
	auto missileSmokeParticle = std::make_unique<MissileSmokeParticle>(modelSmoothCube_);
	ParticleEffectManager::GetInstance()->Register("missileSmoke", std::move(missileSmokeParticle));
	// 地面警告表示パーティクル
	auto groundWarningParticle = std::make_unique<RedCircleParticle>(modelPlane_);
	ParticleEffectManager::GetInstance()->Register("redCircle", std::move(groundWarningParticle));
	// 地面警告攻撃の煙パーティクル
	auto explodeSmokeParticle = std::make_unique<ExplodeSmokeParticle>(modelSmoothCube_);
	ParticleEffectManager::GetInstance()->Register("explodeSmoke", std::move(explodeSmokeParticle));
	// 地面警告攻撃の飛散パーティクル
	auto explodeScatterParticle = std::make_unique<ExplodeScatterParticle>(modelSmoothCube_);
	ParticleEffectManager::GetInstance()->Register("explodeScatter", std::move(explodeScatterParticle));
	// 煙パーティクル
	auto smokeParticle = std::make_unique<SmokeParticle>(modelSmoothCube_);
	ParticleEffectManager::GetInstance()->Register("smoke", std::move(smokeParticle));
	// 火花パーティクル
	auto sparkParticle = std::make_unique<SparkParticle>(modelSmoothCube_);
	ParticleEffectManager::GetInstance()->Register("spark", std::move(sparkParticle));
	// 壁崩壊パーティクル
	auto wallCollapseParticle = std::make_unique<WallCollapseParticle>(modelSmoothCube_);
	ParticleEffectManager::GetInstance()->Register("wallCollapse", std::move(wallCollapseParticle));
	// 血が飛び散るパーティクル
	auto bloodSplatterParticle = std::make_unique<BloodSplatterParticle>(modelSmoothCube_);
	ParticleEffectManager::GetInstance()->Register("bloodSplatter", std::move(bloodSplatterParticle));
}
