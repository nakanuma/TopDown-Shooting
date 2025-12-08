#pragma once

// ---------------------------------------------------------
// Foward Declaration
// ---------------------------------------------------------
class GamePlayScene;

// =========================================================
// ゲーム状態の基底クラス
// =========================================================
class IGameState
{
public:
	/// <summary>
	/// 仮想デストラクタ
	/// </summary>
	virtual ~IGameState() = default;

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// 更新処理を行います。
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// 描画処理を行います。
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 通常モデルのシャドウマップ描画処理を行います。
	/// </summary>
	virtual void DrawShadow() = 0;

	/// <summary>
	/// スキニングモデルのシャドウマップ描画処理を行います。
	/// </summary>
	virtual void DrawShadowSkinning() = 0;

	/// <summary>
	/// UI描画処理を行います。
	/// </summary>
	virtual void DrawUI() = 0;

	/// <summary>
	/// デバッグ表示を行います。
	/// </summary>
	virtual void Debug() = 0;

	/// <summary>
	/// 次の状態へ遷移可能か確認します。
	/// </summary>
	/// <returns>遷移可能ならtrue</returns>
	virtual bool CanTransition() const = 0;

protected:
	// =========================================================
	// Member Variables
	// =========================================================
	GamePlayScene* scene_ = nullptr;	// GamePlaySceneへの参照を保持
};

