#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <MyMath.h>
#include <Sprite.h>
#include <SpriteCommon.h>

// ---------------------------------------------------------
// Foward Declaration
// ---------------------------------------------------------
class Player;

// =========================================================
// ゲームオーバー時の演出を制御するクラス
// =========================================================
class GameOverSequence
{
public:
    // =========================================================
    // Public Methods
    // =========================================================

    /// <summary>
    /// 初期化処理を行います。
    /// </summary>
    void Initialize(SpriteCommon* spriteCommon);

    /// <summary>
    /// ゲームオーバー時演出を開始します。
    /// </summary>
    /// <param name="playerPos"></param>
    void Start(const Float3& playerPos);

    /// <summary>
    /// 毎フレーム更新処理を行います。
    /// </summary>
    void Update();

    /// <summary>
    /// UIの描画処理を行います。
    /// </summary>
    void DrawUI();

    /// <summary>
    /// デバッグ用の描画処理を行います。
    /// </summary>
    void Debug();

    // =========================================================
    // Getter / Setter
    // =========================================================

    /// <summary>
    /// ゲームオーバー演出の有効化状態を取得します。
    /// </summary>
    /// <returns></returns>
    bool IsActive() const { return phase_ != Phase::None; }

    /// <summary>
    /// 終了フラグを取得します。
    /// </summary>
    /// <returns></returns>
    bool IsFinished() const { return phase_ == Phase::Finish; }

    /// <summary>
    /// プレイヤーのポインタを設定します。
    /// </summary>
    /// <param name="player">プレイヤーのポインタ</param>
    void SetPlayer(Player* player) { player_ = player; }

private:
    // =========================================================
    // Internal Methods
    // =========================================================

    /// <summary>
    /// カメラ接近時の更新処理を行います。
    /// </summary>
    void UpdateApproach();

    /// <summary>
    /// カメラ回転時の更新処理を行います。
    /// </summary>
    void UpdateRotate();

    /// <summary>
    /// "YOU DIED"スプライトの更新処理を行います。
    /// </summary>
    void UpdateDiedText();

    /// <summary>
    /// "クリックでタイトルへ"スプライトの更新処理を行います。
    /// </summary>
    void UpdateBackToTitleText();

private:
    /// <summary>
    /// 演出のフェーズを表す構造体
    /// </summary>
    enum class Phase {
        None,                                                       /* 初期状態 */
        Intro,                                                      /* 開始時 */
        Approach,                                                   /* カメラ接近 */
        Rotate,                                                     /* カメラ回転 */
        DiedText,                                                   /* "YOU DIED"文字スプライトの表示 */
        BackToTitleText,                                            /* "クリックでタイトルへ"文字スプライトの表示 */
        Finish,                                                     /* 終了 */
    };

    // ----- Parameters -----
    Phase phase_ = Phase::None;                                     /* 現在フェーズ */
    float timer_ = 0.0f;                                            /* タイマー */

    Float3 targetPos_;                                              /* カメラの注視点となる位置（プレイヤー死亡位置） */

    const float kWaitTime = 0.25f;                                  /* 開始時の待機時間（秒） */

    const float kApproachDuration = 1.0f;                           /* カメラ接近にかかる時間 */
    const float kApproachDistance = 0.3f;                           /* 注視点までどれだけの距離近づくか（0.0f～1.0f） */
    Float3 approachStartPos_;                                       /* カメラ接近時の開始位置 */
    Float3 approachEndPos_;                                         /* カメラ接近時の終了位置 */

    const float kRotateDuration = 3.0f;                             /* カメラ回転にかかる時間 */

    const float kDiedTextDuration = 1.0f;                           /* "YOU DIED"文字スプライトのアニメーションにかかる時間 */
    const Float2 kDiedTextStartPos = {640.0f, -160.0f};             /* "YOU DIED"文字スプライトの初期位置 */
    const Float2 kDiedTextEndPos = {640.0f, 160.0f};                /* "YOU DIED"文字スプライトの最終位置 */

    const float kBackToTitleTextDuration = 0.5f;                    /* "クリックでタイトルへ"文字スプライトのアニメーションにかかる時間 */
    Float2 backToTitleTextStartSize_ = {0.0f, 0.0f};                /* "クリックでタイトルへ"文字スプライトの初期サイズ */
    Float2 backToTitleTextEndSize_;                                 /* "クリックでタイトルへ"文字スプライトの最終サイズ */
    const Float2 kBackToTitleTextPos = { 640.0f, 540.0f };          /* "クリックでタイトルへ"文字スプライトの位置 */

    // ----- Sprite -----
    std::unique_ptr<Sprite> spriteDiedText_;                        /* "YOU DIED"文字スプライト */
    std::unique_ptr<Sprite> spriteBackToTitleText_;                 /* "クリックでタイトルへ"文字スプライト */

    // ----- Others -----
    Player* player_ = nullptr;                                      /* プレイヤーのポインタ */
};