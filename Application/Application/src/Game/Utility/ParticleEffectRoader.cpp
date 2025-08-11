#include "ParticleEffectRoader.h"

// Engine
#include <Engine/ParticleEffect/ParticleEffectManager.h>

// Application
#include <src/Game/Particles/BackscatterParticle.h>
#include <src/Game/Particles/HomingMissile/MissileSmokeParticle.h>
#include <src/Game/Particles/GroundWarning/RedCircleParticle.h>
#include <src/Game/Particles/ExplodeSmokeParticle.h>
#include <src/Game/Particles/ExplodeScatterParticle.h>

// ---------------------------------------------------------
// インスタンス取得
// ---------------------------------------------------------
ParticleEffectRoader* ParticleEffectRoader::GetInstance() 
{
	static ParticleEffectRoader instance;
	return &instance;
}

// ---------------------------------------------------------
// パーティクルのモデル読み込み・登録
// ---------------------------------------------------------
void ParticleEffectRoader::LoadAndRegisterAll()
{
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
}
