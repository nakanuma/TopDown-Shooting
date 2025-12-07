#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <deque>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Collider/Collider.h>
#include <Collider/CollisionManager.h>
#include <Engine/3D/Object3D.h>

// =========================================================
// 弾の基底クラス
// =========================================================
class Bullet {
public:
	// =========================================================
	// Public Methods
	// =========================================================

	/// <summary>
	/// 弾の初期化処理を行います。
	/// </summary>
	/// <param name="position">初期位置</param>
	/// <param name="direciton">方向</param>
	/// <param name="model">モデルデータ</param>
	virtual void Initialize(const Cygnus::Float3& position, const Cygnus::Float3& direction, Cygnus::ModelManager::ModelData* model) = 0;

	/// <summary>
	/// 毎フレームの更新処理を行います。
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// モデルの描画処理を行います。
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 弾道の描画処理を行います。
	/// </summary>
	void DrawTrail();

	/// <summary>
	/// 弾のライフサイクル終了処理を行います。（死亡フラグを立て、コライダー登録の解除を行う）
	/// </summary>
	void FinishLifeCycle();

	// =========================================================
	// Getter / Setter
	// =========================================================

	/// <summary>
	/// 弾の攻撃力を取得します。
	/// </summary>
	/// <returns>攻撃力（int）</returns>
	int32_t GetDamage() const { return damage_; }

	/// <summary>
	/// 弾の現在位置を取得します。
	/// </summary>
	/// <returns>現在の位置（Float3）</returns>
	const Cygnus::Float3& GetTranslate() const { return objectBullet_->transform_.translate_; }

	/// <summary>
	/// 死亡したかどうかを取得します。
	/// </summary>
	/// <returns>死亡フラグ</returns>
	bool IsDead() const { return isDead_; }

protected:
	// =========================================================
	// Internal Methods
	// =========================================================

	/// <summary>
	/// 血の出る敵に衝突した際のパーティクル発生を行います。
	/// </summary>
	/// <param name="position">位置</param>
	/// <param name="velocity">速度ベクトル</param>
	void EmitBloodHitParticles(const Cygnus::Float3& position, const Cygnus::Float3& velocity);

	/// <summary>
	/// 硬い敵・物に衝突した際のパーティクル発生を行います。
	/// </summary>
	/// <param name="position">位置</param>
	/// <param name="velocity">速度ベクトル</param>
	void EmitHardHitParticles(const Cygnus::Float3& position, const Cygnus::Float3& velocity);

protected:
	// =========================================================
	// Constants
	// =========================================================
	static constexpr int32_t kBloodSplatterCount = 2; /* 血飛沫パーティクルの発生数 */
	static constexpr int32_t kBloodSmokeCount = 5;    /* 血煙パーティクルの発生数 */
	static constexpr int32_t kBloodScatterCount = 2;  /* 後方血飛散パーティクルの発生数 */

	static constexpr int32_t kBackscatterCount = 4; /* 後方飛散パーティクルの発生数 */
	static constexpr int32_t kImpactSmokeCount = 7; /* 衝撃煙パーティクルの発生数 */

	static constexpr size_t kMaxTrailPoints = 10;                          /* 保持する履歴数 */
	static constexpr float kTrailLineWidth = 0.5f;                         /* トレイル線の幅 */
	static constexpr Cygnus::Float4 kTrailHeadColor = {1.0f, 1.0f, 0.33f, 1.0f};   /* トレイル線の先頭色 */
	static constexpr Cygnus::Float4 kTrailTailColor = {1.0f, 0.215f, 0.06f, 0.0f}; /* トレイル線の末尾色 */

	// =========================================================
	// Member Variables
	// =========================================================

	// ----- Object -----
	std::unique_ptr<Cygnus::Object3D> objectBullet_; /* 弾オブジェクト */

	// ----- Collision -----
	std::unique_ptr<Cygnus::Collider> collider_; /* コライダー */

	// ----- Parameters -----
	int32_t damage_ = 0;                   /* 弾の攻撃力 */
	float speed_ = 0.0f;                   /* 移動速度 */
	Cygnus::Float3 velocity_ = {0.0f, 0.0f, 0.0f}; /* 速度ベクトル */
	bool isDead_ = false;                  /* 死亡フラグ */

	// ----- Others -----
	float elapsedTime_ = 0;            /* 経過時間 */
	std::deque<Cygnus::Float3> trailPoints_{}; /* 過去フレームの位置を保持 */
};