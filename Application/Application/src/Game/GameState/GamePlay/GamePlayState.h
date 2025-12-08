#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <src/Game/GameState/Base/IGameState.h>

// =========================================================
// ゲームプレイ状態
// =========================================================
class GamePlayState : public IGameState
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="scene">ゲームプレイシーン</param>
	explicit GamePlayState(GamePlayScene* scene);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GamePlayState() override = default;

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新処理を行います。
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画処理を行います。
	/// </summary>
	void Draw() override;

	/// <summary>
	/// 通常モデルのシャドウマップ描画処理を行います。
	/// </summary>
	void DrawShadow() override;

	/// <summary>
	/// スキニングモデルのシャドウマップ描画処理を行います。
	/// </summary>
	void DrawShadowSkinning() override;

	/// <summary>
	/// UI描画処理を行います。
	/// </summary>
	void DrawUI() override;

	/// <summary>
	/// デバッグ表示を行います。
	/// </summary>
	void Debug() override;

	/// <summary>
	/// 次の状態へ遷移可能か確認します。
	/// </summary>
	/// <returns>遷移可能ならtrue</returns>
	bool CanTransition() const override;

private:
	// =========================================================
	// Member Variables
	// =========================================================
	bool shouldTransitionToGameOver_ = false;		// ゲームオーバー遷移フラグ
	bool shouldTransitionToGameClear_ = false;		// ゲームクリア遷移フラグ
};

