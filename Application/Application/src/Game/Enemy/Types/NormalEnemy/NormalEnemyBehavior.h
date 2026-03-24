#pragma once

// ---------------------------------------------------------
// C++ Includes
// ---------------------------------------------------------
#include <memory>

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <Engine/BehaviourTree/BehaviorTree.h>

// ---------------------------------------------------------
// Forward Declaration
// ---------------------------------------------------------
class NormalEnemy;

// =========================================================
// 通常の敵の行動を制御するクラス
// =========================================================
class NormalEnemyBehavior {
public:
	/// <summary>
	/// BehaviorTreeの構築を行います。
	/// </summary>
	static std::unique_ptr<Cygnus::BehaviorTree<NormalEnemy>> CreateTree(NormalEnemy* e);

private:
	/// <summary>
	/// プレイヤー検出判定を行います。
	/// </summary>
	static bool CheckDetect(NormalEnemy* e);

	/// <summary>
	/// プレイヤー方向を見続けます。
	/// </summary>
	/// <returns></returns>
	static Cygnus::BehaviorStatus FaceToPlayer(NormalEnemy* e, float dt);

	/// <summary>
	/// 射撃処理を行います。
	/// </summary>
	/// <returns></returns>
	static Cygnus::BehaviorStatus ActionShoot(NormalEnemy* e, float dt);

	/// <summary>
	/// リロード処理を行います。
	/// </summary>
	/// <returns></returns>
	static Cygnus::BehaviorStatus ActionReload(NormalEnemy* e, float dt);

	/// <summary>
	/// 移動するかどうかの判定・準備を行います。
	/// </summary>
	/// <returns></returns>
	static Cygnus::BehaviorStatus ActionDecideMove(NormalEnemy* e, float dt);

	/// <summary>
	/// 移動を行います。
	/// </summary>
	/// <returns></returns>
	static Cygnus::BehaviorStatus ActionMove(NormalEnemy* e, float dt);

	/// <summary>
	/// 索敵行動を行います。
	/// </summary>
	static Cygnus::BehaviorStatus ActionSearchLookAround(NormalEnemy* e, float dt);

	/// <summary>
	/// 経路探索を行ってプレイヤーに近づきます。
	/// </summary>
	static Cygnus::BehaviorStatus ActionApproachPlayer(NormalEnemy* e, float dt);
};
