#pragma once
#include <chrono>

class FixFPS {
private: //��{�I�Ȋ֐�
	void Initialize();
	void Update();

private: //�ŗL�̃����o�֐�
	//�L�^����(FPS�Œ�p)
	std::chrono::steady_clock::time_point reference_;

public: //�Q�b�^�[
	void GetInitialize() {
		Initialize();
	}

	void GetUpdate() {
		Update();
	}
};