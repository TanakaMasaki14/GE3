#pragma once
#include <chrono>

class FixFPS {
private: //基本的な関数
	void Initialize();
	void Update();

private: //固有のメンバ関数
	//記録時間(FPS固定用)
	std::chrono::steady_clock::time_point reference_;

public: //ゲッター
	void GetInitialize() {
		Initialize();
	}

	void GetUpdate() {
		Update();
	}
};