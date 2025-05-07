#include "FollowCamera.h"

// ---------------------------------------------------------
// 初期化処理
// ---------------------------------------------------------
void FollowCamera::Initialize(const Float3& offset)
{
	offset_ = offset;
}

// ---------------------------------------------------------
// 更新処理
// ---------------------------------------------------------
void FollowCamera::Update()
{
	Float3 targetPos = *targetTranslate_ + offset_;
	currentPos_ = Float3::Lerp(currentPos_, targetPos, 0.1f);
}

// ---------------------------------------------------------
// 追従目標をセット
// ---------------------------------------------------------
void FollowCamera::SetTarget(const Float3* translate)
{
	targetTranslate_ = translate;
	if (targetTranslate_) {
		currentPos_ = *targetTranslate_ + offset_;
	}
}
