#pragma once

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/GameState/Base/IGameState.h>
#include <src/Game/Sequence/GameClearSequence.h>

// =========================================================
// ゲームクリア状態
// =========================================================
class GameClearState : public IGameState {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="scene">ゲームプレイシーン</param>
	explicit GameClearState(GamePlayScene* scene);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameClearState() override = default;

	/// <summary>
	/// 初期化処理を行います。
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 終了処理を行います。
	/// </summary>
	void Finalize() override;

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

	/// <summary>
	/// プレイヤーの死亡状態を取得します。
	/// </summary>
	/// <returns>死亡していたらtrue</returns>
	bool IsPlayerDead() const override;

	/// <summary>
	/// ボスの死亡状態を取得します。
	/// </summary>
	/// <returns>死亡していたらtrue</returns>
	bool IsBossDying() const override;

private:
	std::unique_ptr<GameClearSequence> gameClearSequence_;	// ゲームクリア時演出の制御クラス
};
