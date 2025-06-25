#include "Input.h"
#include "StringUtility.h"

#include <DxLib.h>

namespace {
    const char* kKeyConfigSignature = "kcfg";
    const char* kKeyConfigFilename = "keyconfig.dat";
}

Input& Input::GetInstance() {
    // ���̐錾�̎��_�Ń��������m�ۂ���ăA�v���I���܂Ŏc��
    static Input input;
    return input;
}

void Input::Update() {
    _last = _current;

    // ���O�̏����R�s�[
    std::copy(std::begin(_currentRawKeybdState), std::end(_currentRawKeybdState), std::begin(_lastRawKeybdState));
    _lastRawPadState = _currentRawPadState;
    // �n�[�h�E�F�A(���Ӌ@��)�̏�Ԃ��擾
    GetHitKeyStateAll(_currentRawKeybdState);//�n�[�h�E�F�A���猻�݂̓��͏�Ԃ��擾
    _currentRawPadState = GetJoypadInputState(DX_INPUT_PAD1);//�p�b�h�P�̏�Ԃ��擾

    //���̓`�F�b�N(���̓��͂��Q�[���̃C�x���g�ɕϊ����Ă���)
    for (const auto& inputRow : _inputTable) {
        // �s�̃L�[
        const auto& key = inputRow.first;
        // �u�����Ă��ԁv�̏�����
        _current[key] = false;

        // ���͒�`vector�̃��[�v
        for (const auto& hardInput : inputRow.second) {
            if (hardInput.type == PeripheralType::keybd) {
                _current[key] = _currentRawKeybdState[hardInput.id];
            }
            else if (hardInput.type == PeripheralType::pad1) {
                _current[key] = hardInput.id & _currentRawPadState;
            }
            // �ǂꂩ��ł��u������Ă���v��ԂȂ�������ׂȂ�
            // �@��̏�񂪗~�����킯�ł͂Ȃ��ׁA�N����������Ă���΂���OK
            if (_current[key]) {
                break;
            }
        }
    }

    // ���E�X�e�B�b�N�X�V
    int xInput, zInput;
    GetJoypadAnalogInputRight(&xInput, &zInput, DX_INPUT_PAD1);
    _rightStickInput = { static_cast<float>(xInput), 0, static_cast<float>(zInput) };
    GetJoypadAnalogInput(&xInput, &zInput, DX_INPUT_PAD1);
    _leftStickInput = { static_cast<float>(xInput), 0, static_cast<float>(zInput) };
}

bool Input::IsPress(const char* key) const {
    // ���������o�^����Ă��Ȃ��L�[�^�C�v�Ȃ�return
    if (!_current.contains(key)) {
        return false;
    }
    // ������Ă��邩�ǂ�����Ԃ�
    return _current.at(key);

    // (������)
    //// ������Ă����true
    //return (_padInput & button);
}

bool Input::IsTrigger(const char* key) const {
    // ���������o�^����Ă��Ȃ��L�[�^�C�v�Ȃ�return
    if (!_current.contains(key)) {
        return false;
    }
    // 1f�O�͉�����ĂȂ� ���� ��������Ă��� �Ȃ�true
    return (_current.at(key) && !_last.at(key));

    // (������)
    //// ������Ă����true.�����łȂ��Ȃ�false
    //bool isNow = (_padInput & button);
    //bool isLast = (_lastInput & button);
    //return (isNow && !isLast);
}

Vector3 Input::GetPadRightSitck() const {
    return _rightStickInput;
}


Vector3 Input::GetPadLeftSitck() const {
    return _leftStickInput;
}

Input::Input() {
    SetDefault();
    LoadInputTable();
    // �ꎞ�e�[�u���ɃR�s�[
    _tempInputTable = _inputTable;
    // �\������������
    _orderForDisplay = { "ok","next","back","pause","action","dash","jump" };
}

void Input::SetDefault()
{
    //���͂Ɩ��O�̑Ή��\�����
    _inputTable["next"] = { {PeripheralType::keybd, KEY_INPUT_RETURN},
                            {PeripheralType::pad1, PAD_INPUT_8}     // Start�{�^��
    };
    _inputTable["back"] = { {PeripheralType::keybd, KEY_INPUT_ESCAPE},
                            {PeripheralType::pad1, PAD_INPUT_B}     // B�{�^��
    };

    _inputTable["ok"] = { {PeripheralType::keybd, KEY_INPUT_RETURN},
                        {PeripheralType::pad1, PAD_INPUT_A}         // A�{�^��
    };
    _inputTable["pause"] = { {PeripheralType::keybd, KEY_INPUT_P},
                            {PeripheralType::pad1, PAD_INPUT_R}     // START�{�^��
    };

    //�Q�[�����̃{�^���e�[�u��
    _inputTable["action"] = { {PeripheralType::keybd, KEY_INPUT_Z},
                            {PeripheralType::pad1, PAD_INPUT_X}     // Y�{�^��
    };
    _inputTable["dash"] = { {PeripheralType::keybd, KEY_INPUT_X},
                        {PeripheralType::pad1, PAD_INPUT_C}         // X�{�^��
    };
    _inputTable["jump"] = { {PeripheralType::keybd, KEY_INPUT_C},
                        {PeripheralType::pad1, PAD_INPUT_A}         // (����)A�{�^��
    };
    _inputTable["Rbutton"] = { {PeripheralType::keybd, KEY_INPUT_I},
                        {PeripheralType::pad1, PAD_INPUT_5}         // �E�{�^��
    };
    _inputTable["Lbutton"] = { {PeripheralType::keybd, KEY_INPUT_P},
                        {PeripheralType::pad1, PAD_INPUT_6}         // ���{�^��
    };

    //�㉺���E
    _inputTable["up"] = { {PeripheralType::keybd, KEY_INPUT_UP},
                            {PeripheralType::pad1, PAD_INPUT_UP}
    };
    _inputTable["down"] = { {PeripheralType::keybd, KEY_INPUT_DOWN},
                            {PeripheralType::pad1, PAD_INPUT_DOWN}
    };
    _inputTable["left"] = { {PeripheralType::keybd, KEY_INPUT_LEFT},
                            {PeripheralType::pad1, PAD_INPUT_LEFT}
    };
    _inputTable["right"] = { {PeripheralType::keybd, KEY_INPUT_RIGHT},
                            {PeripheralType::pad1, PAD_INPUT_RIGHT}
    };
    _tempInputTable = _inputTable;  // �ꎞ�e�[�u���ɃR�s�[
}

int Input::GetKeyboradState() const
{
    // �S�`�F�b�N���A�ǂꂩ�ЂƂł����͂��������炻���Ԃ�
    for (int i = 0; i < _countof(_currentRawKeybdState); ++i) {
        if (_currentRawKeybdState[i] && !_lastRawKeybdState[i]) {
            return i;
        }
    }
    return -1;
}

int Input::GetPadState(int padno) const
{
    // �w��ԍ��̃p�b�h�������擾����
    uint32_t bit = 0b00000000000000000000000000000001;
    for (int i = 0; i < 32; ++i) {
        auto value = (bit << i);
        if ((value & _currentRawPadState) && !(value & _lastRawPadState)) {
            return value;
        }
    }
    return -1;
}

void Input::SaveInputTable()
{
    FILE* fp = nullptr;
    auto err = fopen_s(&fp, kKeyConfigFilename, "wb");//�o�C�i���Łu�������݁v
    //���ʎq��������(�S�o�C�g)
    std::string signature = kKeyConfigSignature;//�t�@�C�������ʂ��邽�߂̎��ʎq
    fwrite(signature.data(), signature.size(), 1, fp);//���ʎq�̏�������

    //�o�[�W�����̏�������
    const float version = 1.0f;
    fwrite(&version, sizeof(version), 1, fp);//4�o�C�g

    //�f�[�^�T�C�Y�̏�������
    auto size = _inputTable.size();
    fwrite(&size, sizeof(size), 1, fp);//4�o�C�g

    //�f�[�^�{�̂̏�������
    for (const auto& record : _inputTable) {
        //�L�[�̏�������(�C�x���g��)
        byte nameSize = static_cast<byte>(record.first.length());//������
        fwrite(&nameSize, sizeof(nameSize), 1, fp);//������̃T�C�Y
        fwrite(record.first.data(), record.first.size(), 1, fp);//������{��
        //���f�[�^�l�̏�������(�u���͎�ʁ{����ID�v�̔z��)
        //�܂��́A���f�[�^�z��̃T�C�Y���擾���܂�
        byte inputDataSize = static_cast<byte>(record.second.size());
        fwrite(&inputDataSize, sizeof(inputDataSize), 1, fp);
        fwrite(record.second.data(), //�������ގ��f�[�^�̐擪�A�h���X
            sizeof(record.second[0]), //�������ރf�[�^�P������̃T�C�Y
            record.second.size(), //�������ރf�[�^��
            fp);
    }

    fclose(fp);
}

void Input::LoadInputTable()
{
    std::string filename = kKeyConfigFilename;
    auto wfilename = StringUtility::GetWStringFromString(filename);
    auto handle = FileRead_open(wfilename.c_str());
    if (handle == 0) {//�����̌����œǂݍ��ݎ��s������ǂݍ��܂Ȃ�
        return;
    }
    struct Header {
        char signature[4];//�V�O�l�`��
        float version;//�o�[�W����
        size_t dataNum;//�f�[�^��
    };
    Header header = {};//�w�b�_�[�̓ǂݍ���
    FileRead_read(&header, sizeof(header), handle);
    //�f�[�^���̕������ǂݍ���ł���
    for (int i = 0; i < header.dataNum; ++i) {
        //�f�[�^�͂܂��A�C�x���g����ǂݍ���
        //���O�̃T�C�Y
        byte nameSize = 0;
        FileRead_read(&nameSize, sizeof(nameSize), handle);
        //���ۂɖ��O��������擾
        std::string strEventName;
        strEventName.resize(nameSize);//���O�̃T�C�Y���m��
        FileRead_read(strEventName.data(), nameSize * sizeof(char), handle);

        //���O���I�������͎��f�[�^�Ȃ̂ŁA�܂����f�[�^�����擾
        byte inputDataNum = 0;
        FileRead_read(&inputDataNum, sizeof(inputDataNum), handle);
        std::vector<InputState> inputStates;
        inputStates.resize(inputDataNum);
        FileRead_read(inputStates.data(), sizeof(inputStates[0]) * inputDataNum, handle);
        _inputTable[strEventName] = inputStates;
    }

    FileRead_close(handle);
}
