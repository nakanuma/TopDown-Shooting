#include "SceneFactory.h"

#include <src/Game/Scene/GamePlayScene.h>
#include <src/Game/Scene/ResultScene.h>
#include <src/Game/Scene/TestScene.h>
#include <src/Game/Scene/TitleScene.h>

BaseScene* SceneFactory::CreateScene(const std::string& sceneName) {
	// 次のシーンを生成
	BaseScene* newScene = nullptr;

	if (sceneName == "TEST") {
		newScene = new TestScene();
		newScene->Initialize();
	} else if (sceneName == "TITLE") {
		newScene = new TitleScene();
		newScene->Initialize();
	} else if (sceneName == "GAMEPLAY") {
		newScene = new GamePlayScene();
		newScene->Initialize();
	} else if (sceneName == "RESULT") {
		newScene = new ResultScene();
		newScene->Initialize();
	}

	return newScene;
}
