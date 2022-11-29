#include "FixFPS.h"
#include <thread>

void FixFPS::Initialize() {
	// ���ݎ��Ԃ��L�^
	reference_ = std::chrono::steady_clock::now();
}

void FixFPS::Update() {
#pragma region �萔�̒�`
	// 1/60�b�s�b�^���̎���
	const std::chrono::microseconds
		kMinTime(uint64_t(1000000.0f / 60.0f));

	// 1/60�b���킸���ɒZ������
	const std::chrono::microseconds
		kMinCheckTime(uint64_t(1000000.0f / 65.0f));
#pragma endregion

	// ���ݎ��Ԏ擾
	std::chrono::steady_clock::time_point now
		= std::chrono::steady_clock::now();

	// �O��L�^����̌o�ߎ��Ԏ擾
	std::chrono::microseconds elapsed
		= std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	// 1/60�b (���킸���ɒZ������)�����ĂȂ��Ȃ�
	if (elapsed < kMinCheckTime) {
		// 1/60�b�o�߂���܂Ŕ����ȃX���[�v���J��Ԃ�
		while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
			// 1�}�C�N���b�X���[�v
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}

	//���݂̎��Ԃ��L�^
	reference_ = std::chrono::steady_clock::now();
}