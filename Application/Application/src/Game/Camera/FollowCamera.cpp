#include "FollowCamera.h"

// Application
#include <src/Game/Utility/Utility.h>

void FollowCamera::Initialize(const Float3& offset) { offset_ = offset; }

void FollowCamera::Update() {
	if (!targetTranslate_)
		return;

	// 追従位置（プレイヤー + オフセット）
	Float3 targetPos = *targetTranslate_ + offset_;

	// カーソルがクライアント領域内にある場合のみ補正を行う
	if(Utility::IsInsideClientCursor()){
		// カーソルのワールド座標を取得
		Float3 cursorPos = Utility::CalculateCursorPosition();

		// カーソルの方向へ補正
		cursorOffset_ = cursorPos - *targetTranslate_;

		// カメラ補正の強さ（小さくしてカメラが少しだけ動くように）
		float influence = 0.2f;
		cursorOffset_ *= influence;
	}

	// 最終的なターゲット位置にカーソル補正を加える
	targetPos += cursorOffset_;

	// なめらかに追従
	currentPos_ = Float3::Lerp(currentPos_, targetPos, 0.75f);
}

void FollowCamera::SetTarget(const Float3* translate) {
	targetTranslate_ = translate;
	// カメラ位置がターゲット位置に基づいて更新されるように
	if (targetTranslate_) {
		currentPos_ = *targetTranslate_ + offset_;
	}
}
