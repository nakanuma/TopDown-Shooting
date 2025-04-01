#pragma once
#include "Framework.h"
//#include "TitleScene.h"
//#include "GamePlayScene.h"

class MyGame : public Framework
{
public:
	// 初期化
	void Initialize() override;

	// 終了
	void Finalize() override;

	// 毎フレーム更新
	void Update() override;

	// 描画
	void Draw() override;

private:
}; 

