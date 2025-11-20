#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Engine/Math/MyMath.h>
#include <Engine/Model/ModelManager.h>
#include <Engine/ParticleEffect/BaseParticleEffect.h>

/// <summary>
/// パーティクルデータ
/// </summary>
struct ShellEjectionParticleData {
	Transform transform;				/* 位置 */
	Float3 velocity;					/* 速度ベクトル */
	Float4 color;						/* 色 */
	float lifeTime;						/* 生存時間 */
	float currentTime;					/* 経過時間 */

	Float3 rotationSpeed;				/* 回転速度 */
};

// =========================================================
// 薬莢排出パーティクルクラス
// 射撃時に銃からに発生
// =========================================================
class ShellEjectionParticle : public BaseParticleEffect<ShellEjectionParticleData>
{
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="model">モデルデータ</param>
	ShellEjectionParticle(ModelManager::ModelData& model);

protected:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// パーティクル固有の生成処理を行います。
	/// </summary>
	/// <param name="pos">初期位置</param>
	/// <param name="velocity">速度ベクトル</param>
	/// <param name="angle">初期回転角</param>
	/// <returns>パーティクルデータ</returns>
	ShellEjectionParticleData CreateParticle(const Float3& pos, const Float3& velocity, const float& angle) override;

	/// <summary>
	/// パーティクル固有の毎フレームの更新処理を行います。
	/// </summary>
	/// <param name="p">パーティクルデータ</param>
	/// <param name="dt">デルタタイム</param>
	void UpdateParticle(ShellEjectionParticleData& p, float dt) override;

private:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr Float3 kInitialScale = { 0.1f, 0.1f, 0.25f };			/* 初期スケール */
	static constexpr float kMinRightSpeed = 5.0f;							/* 最小右方向速度 */
	static constexpr float kMaxRightSpeed = 7.5f;							/* 最大右方向速度 */
	static constexpr float kMinUpSpeed = 8.0f;								/* 最小上方向速度 */
	static constexpr float kMaxUpSpeed = 10.0f;								/* 最大上方向速度 */
	static constexpr Float4 kInitialColor = { 0.4f, 0.4f, 0.0f, 1.0f };		/* 初期色 */
	static constexpr float kLifeTime = 1.0f;								/* 生存時間（秒） */
	static constexpr Float3 kMinRotationSpeed = { -5.0f, -5.0f, -5.0f };	/* 最小回転速度 */
	static constexpr Float3 kMaxRotationSpeed = { 5.0f, 5.0f, 5.0f };		/* 最大回転速度 */

	static constexpr float kGravity = -50.0f;	/* 重力加速度 */
};