#include "FollowCamera.h"

// Application
#include <src/Game/Utility/Utility.h>

void FollowCamera::Initialize(const Cygnus::Float3& offset) { offset_ = offset; }

void FollowCamera::Update() {
	if (!targetTranslate_)
		return;

	// 追従位置（プレイヤー + オフセット）
	Cygnus::Float3 targetPos = *targetTranslate_ + offset_;

	// カーソルがクライアント領域内にある場合のみ補正を行う
	if (Utility::IsInsideClientCursor()) {
		// カーソルのワールド座標を取得
		Cygnus::Float3 cursorPos = Utility::CalculateCursorPosition();

		// カーソルの方向へ補正
		cursorOffset_ = cursorPos - *targetTranslate_;

		// カメラ補正の強さ（小さくしてカメラが少しだけ動くように）
		cursorOffset_ *= kCursorInfluence;
	}

	// 最終的なターゲット位置にカーソル補正を加える
	targetPos += cursorOffset_;

	// なめらかに追従
	currentPos_ = Cygnus::Float3::Lerp(currentPos_, targetPos, kFollowSpeed);
}

void FollowCamera::SetTarget(const Cygnus::Float3* translate) {
	targetTranslate_ = translate;
	// カメラ位置がターゲット位置に基づいて更新されるように
	if (targetTranslate_) {
		currentPos_ = *targetTranslate_ + offset_;
	}
}
