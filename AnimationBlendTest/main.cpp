#include <DxLib.h>
#include <cassert>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    if (DxLib_Init() == -1) {
        return -1;
    }

#ifdef _DEBUG	// �R���\�[��Debug�p
	FILE* _out;
	FILE* _in;
	AllocConsole();							// �R���\�[��
	_out = 0;
	freopen_s(&_out, "CON", "w", stdout);	// stdout
	_in = 0;
	freopen_s(&_in, "CON", "r", stdin);		// stdin
#endif	// _DEBUG

	ChangeWindowMode(true);
    SetGraphMode(640, 480, 32);
	SetWindowText(L"Test Window");

	if (DxLib_Init()) {
		assert(false && "DxLib_Init_Error");
		return false;
	}

	SetDrawScreen(DX_SCREEN_BACK);

	// 3D�֘A�ݒ�
	SetUseZBuffer3D(true);		// Z�o�b�t�@�L����
	SetWriteZBuffer3D(true);	// Z�o�b�t�@�ւ̏������݂��s��
	SetUseBackCulling(true);	// �o�b�N�J�����O��L���ɂ���

    // �J�����ݒ�
    SetCameraPositionAndTarget_UpVecY(VGet(0, 200, -500), VGet(0, 80, 0));



    // ���f���̓ǂݍ���
    int modelHandle = MV1LoadModel(L"model.mv1"); // ���Ȃ��̃��f���t�@�C�����ɒu�������Ă�������

    // �A�j���[�V���������f���ɃA�^�b�`
    // ��2�����Ń��f���t�@�C�����̃A�j���[�V�����ԍ����w��
    int walkAttachIndex = MV1AttachAnim(modelHandle, MV1GetAnimIndex(modelHandle, L"CharacterArmature|Idle"), -1, FALSE); // �ҋ@�A�j���[�V����
    int runAttachIndex = MV1AttachAnim(modelHandle, MV1GetAnimIndex(modelHandle, L"CharacterArmature|Walk"), -1, FALSE); // �����A�j���[�V����

    // �e�A�j���[�V�����̑����Ԃ��擾
    float totalWalkTime = MV1GetAttachAnimTotalTime(modelHandle, walkAttachIndex);
    float totalRunTime = MV1GetAttachAnimTotalTime(modelHandle, runAttachIndex);

    // �A�j���[�V�����̍Đ�����
    float walkTime = 0.0f;
    float runTime = 0.0f;

    // �u�����h�� (0.0: ����100% -> 1.0: ����100%)
    float blendRate = 0.0f;
    bool isRunning = false; // ����ɐ؂�ւ���t���O






    // ���C�����[�v
    while (ProcessMessage() == 0) {
		// ����̃��[�v���n�܂������Ԃ��o���Ă���
		LONGLONG time = GetNowHiPerformanceCount();

		ClearDrawScreen();


		// ����

        // �X�y�[�X�L�[�������ꂽ��u����v�t���O�𗧂Ă�
        if (CheckHitKey(KEY_INPUT_SPACE) != 0) {
            isRunning = true;
        }
        else {
            isRunning = false;
        }

        // �u�����h����ڕW�l(isRunning ? 1.0f : 0.0f)�ɋ߂Â���
        if (isRunning && blendRate < 1.0f) {
            blendRate += 0.05f; // ���X�ɑ���̊����𑝂₷
            if (blendRate > 1.0f) blendRate = 1.0f;
        }
        else if (!isRunning && blendRate > 0.0f) {
            blendRate -= 0.05f; // ���X�ɕ����̊����𑝂₷
            if (blendRate < 0.0f) blendRate = 0.0f;
        }

        // �e�A�j���[�V�����̃u�����h����ݒ�
        // 2�̃u�����h���̍��v�����1.0�ɂȂ�悤�ɂ���
        MV1SetAttachAnimBlendRate(modelHandle, walkAttachIndex, 1.0f - blendRate);
        MV1SetAttachAnimBlendRate(modelHandle, runAttachIndex, blendRate);

        // �A�j���[�V�������Ԃ̍X�V (���[�v�Đ�)
        walkTime += 1.0f;
        if (walkTime >= totalWalkTime) {
            walkTime = 0.0f;
        }
        runTime += 1.0f;
        if (runTime >= totalRunTime) {
            runTime = 0.0f;
        }

        // �A�j���[�V�����̍Đ����Ԃ����f���ɐݒ�
        MV1SetAttachAnimTime(modelHandle, walkAttachIndex, walkTime);
        MV1SetAttachAnimTime(modelHandle, runAttachIndex, runTime);

        // ���f���̕`��
        MV1DrawModel(modelHandle);

        // �f�o�b�O���̕\��
        DrawFormatString(10, 10, GetColor(255, 255, 255), L"SPACE�L�[�ŕ���/����؂�ւ�");
        DrawFormatString(10, 30, GetColor(255, 255, 255), L"BlendRate: %.2f", blendRate);
        DrawFormatString(10, 50, GetColor(255, 255, 255), L"Walk: %.2f%%, Run: %.2f%%", (1.0f - blendRate) * 100, blendRate * 100);



		ScreenFlip();

		if (CheckHitKey(KEY_INPUT_ESCAPE)) {
			break;
		}

		// FPS60�ɌŒ肷��
		while (GetNowHiPerformanceCount() - time < 16667) {
		}
	}

    DxLib_End();
    return 0;
}