#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <MyMath.h>
#include <Sprite.h>
#include <SpriteCommon.h>

// ---------------------------------------------------------
// Application Includes
// ---------------------------------------------------------
#include <src/Game/Enemy/BossEnemy/BossEnemy.h>

// =========================================================
// ゲームクリア時の演出を制御するクラス
// =========================================================
class GameClearSequence
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
    /// ゲームクリア演出を開始します。
    /// </summary>
    void Start();

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
    /// ゲームクリア演出の有効化状態を取得します。
    /// </summary>
    /// <returns>演出を行っているか</returns>
    bool IsActive() const { return phase_ != Phase::None; }

    /// <summary>
    /// カメラ制御の有効化状態を取得します。
    /// </summary>
    /// <returns>カメラ制御の有効化状態</returns>
    bool IsControllingCamera() const { return IsActive() && phase_ == Phase::Rotate; }

    /// <summary>
    /// 終了フラグを取得します
    /// </summary>
    /// <returns>終了したかどうか</returns>
    bool IsFinished() const { return phase_ == Phase::Finish; }

    /// <summary>
    /// ボスのセットを行います。
    /// </summary>
    /// <param name="boss"></param>
    void SetBoss(BossEnemy* boss) { boss_ = boss; }

private:
    // =========================================================
    // Internal Methods
    // =========================================================

    /// <summary>
    /// カメラ回転時の更新処理を行います。
    /// </summary>
    void UpdateRotate();

    /// <summary>
    /// 爆発とクリア文字演出時の更新処理を行います。
    /// </summary>
    void UpdateExplodeAndText();

private:
    /// <summary>
    /// 演出のフェーズ
    /// </summary>
    enum class Phase {
        None,                                                       /* 初期状態 */
        Rotate,                                                     /* カメラ回転（ボス中心に） */
        ExplodeAndText,                                             /* 爆発とクリア時文字 */
        Finish,                                                     /* 終了状態 */
    };

    // =========================================================
    // Member Variables
    // =========================================================

    // ----- Parameters -----
    Phase phase_ = Phase::None;                                     /* 現在フェーズ */
    float timer_ = 0.0f;                                            /* 全体のタイマー */

    float particleEmitTimer_;                                       /* パーティクル発生用タイマー */
    const float kExplodeEmitInterval = 0.2f;                        /* 爆発パーティクル発生頻度 */

    Float3 savedCameraPos_;                                         /* 演出開始時のカメラ位置を保存 */
    Float3 savedCameraRot_;                                         /* 演出開始時のカメラ回転を保存 */

    BossEnemy* boss_ = nullptr;                                     /* ボスのポインタ */
    Float3 lastBossPosition_;                                       /* ボスの最終位置を保存 */

    const float kCameraRotateDuration = 5.0f;                       /* カメラ回転にかける時間 */
    const float kCameraDistance = 40.0f;                            /* ボスからのカメラ距離 */

    const float kCameraRotZOffset = -0.13f;                         /* カメラ回転のZ軸オフセット */

    const float kCameraHeightStart = 6.0f;                          /* カメラ回転時の開始時高さ */
    const float kCameraHeightEnd = 2.0f;                            /* カメラ回転時の終了時高さ */

    const float kExplodeDuration = 3.0f;                            /* 爆発フェーズにかける時間 */

    Float2 savedClearTextSize_;                                     /* クリア文字スプライトの初期サイズを保存 */

    // -----Sprite-----
    std::unique_ptr<Sprite> spriteBackGround_;                      /* 黒い背景スプライト */
    std::unique_ptr<Sprite> spriteClearText_;                       /* クリア時文字 */
};

