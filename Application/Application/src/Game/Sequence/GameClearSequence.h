#pragma once

// ---------------------------------------------------------
// Engine Includes
// ---------------------------------------------------------
#include <MyMath.h>
#include <Sprite.h>
#include <SpriteCommon.h>

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
    void Initialize();

    /// <summary>
    /// ゲームクリア演出を開始します。
    /// </summary>
    void Start();

    /// <summary>
    /// 毎フレーム更新処理を行います。
    /// </summary>
    void Update();

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

private:
    // =========================================================
    // Internal Methods
    // =========================================================

    /// <summary>
    /// カメラ回転時の更新処理を行います。
    /// </summary>
    void UpdateRotate();

private:
    /// <summary>
    /// 演出のフェーズ
    /// </summary>
    enum class Phase {
        None,                                                       /* 初期状態 */
        Rotate,                                                     /* カメラ回転（ボス中心に） */
        Explode,                                                    /* 爆発演出 */
        ClearText,                                                  /* クリア文字 */
        Finish,                                                     /* 終了状態 */
    };

    // =========================================================
    // Member Variables
    // =========================================================

    // ----- Parameters -----
    Phase phase_ = Phase::None;                                     /* 現在フェーズ */
    float timer_ = 0.0f;                                            /* タイマー */

    Float3 savedCameraPos_;                                         /* 演出開始時のカメラ位置を保存 */
    Float3 savedCameraRot_;                                         /* 演出開始時のカメラ回転を保存 */
};

