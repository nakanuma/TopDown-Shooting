#include "MissileSmokeParticle.h"

// Engine
#include <Engine/Math/Easing.h>
#include <Engine/Math/MyMath.h>
#include <Engine/Util/RandomGenerator.h>

MissileSmokeParticle::MissileSmokeParticle(ModelManager::ModelData& model) {
	// オブジェクト設定
	object_.model_ = &model;
	object_.gTransformationMatrices.numMaxInstance_ = kMaxParticles;
	object_.gTransformationMatrices.Create();

	// ビルボード適用設定
	isBillboard_ = {false, false, false};
	// ブレンドモード設定
	blendMode_ = BlendMode::Normal;
}

MissileSmokeParticleData MissileSmokeParticle::CreateParticle(const Float3& pos, const Float3& velocity, const float& angle) {
	MissileSmokeParticleData p;
	auto rand = RandomGenerator::GetInstance();

	// 位置（オフセットを加える）
	Float3 offset = rand->RandomValue({-0.4f, -0.4f, -0.4f}, {0.4f, 0.4f, 0.4f});
	p.transform.translate = pos + offset;
	// 回転
	p.transform.rotate = rand->RandomValue({0.0f, 0.0f, 0.0f}, {PIf * 2.0f, PIf * 2.0f, PIf * 2.0f});
	// スケール
	p.transform.scale = {0.2f, 0.2f, 0.2f};
	// 速度ベクトル
	p.velocity = rand->RandomValue({-1.0f, -1.0f, -1.0f}, {1.0f, 1.0f, 1.0f});
	// 色
	p.color = {1.0f, 1.0f, 1.0f, 1.0f};
	// 生存時間
	p.lifeTime = 0.8f;
	// 経過時間
	p.currentTime = 0.0f;
	// 初期スケール
	p.initScale = p.transform.scale;

	return p;
}

void MissileSmokeParticle::UpdateParticle(MissileSmokeParticleData& p, float dt) {
	float t = std::clamp(p.currentTime / p.lifeTime, 0.0f, 1.0f);

	// 移動
	p.transform.translate += (p.velocity * dt);

	// 縮小
	if (t > 0.8f) {                       // 4/5に到達したら
		float localT = (t - 0.8f) / 0.2f; // 0~1に正規化
		float easeT = Easing::EaseInQuad(localT);
		p.transform.scale = p.initScale * (1.0f - easeT);
	}

	// 色
	Float4 color;
	if (t < 1.0f / 3.0f) {
		// 白->橙
		float localT = t / (1.0f / 3.0f); // 0~1に正規化
		color.x = Easing::Lerp(1.0f, 1.0f, localT);
		color.y = Easing::Lerp(1.0f, 0.5f, localT);
		color.z = Easing::Lerp(1.0f, 0.0f, localT);
	} else if (t < 2.0f / 3.0f) {
		// 橙->赤
		float localT = (t - 1.0f / 3.0f) / (1.0f / 3.0f); // 0~1に正規化
		color.x = Easing::Lerp(1.0f, 1.0f, localT);
		color.y = Easing::Lerp(0.5f, 0.0f, localT);
		color.z = Easing::Lerp(0.0f, 0.0f, localT);
	} else {
		// 赤->黒
		float localT = (t - 2.0f / 3.0f) / (1.0f / 3.0f); // 0~1に正規化
		color.x = Easing::Lerp(1.0f, 0.0f, localT);
		color.y = Easing::Lerp(0.0f, 0.0f, localT);
		color.z = Easing::Lerp(0.0f, 0.0f, localT);
	}
	color.w = 1.0f;

	p.color = color;
}
