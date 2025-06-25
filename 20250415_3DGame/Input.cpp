#include "Input.h"
#include "StringUtility.h"

#include <DxLib.h>

namespace {
    const char* kKeyConfigSignature = "kcfg";
    const char* kKeyConfigFilename = "keyconfig.dat";
}

Input& Input::GetInstance() {
    // この宣言の時点でメモリが確保されてアプリ終了まで残る
    static Input input;
    return input;
}

void Input::Update() {
    _last = _current;

    // 直前の情報をコピー
    std::copy(std::begin(_currentRawKeybdState), std::end(_currentRawKeybdState), std::begin(_lastRawKeybdState));
    _lastRawPadState = _currentRawPadState;
    // ハードウェア(周辺機器)の状態を取得
    GetHitKeyStateAll(_currentRawKeybdState);//ハードウェアから現在の入力状態を取得
    _currentRawPadState = GetJoypadInputState(DX_INPUT_PAD1);//パッド１の状態を取得

    //入力チェック(生の入力をゲームのイベントに変換していく)
    for (const auto& inputRow : _inputTable) {
        // 行のキー
        const auto& key = inputRow.first;
        // 「押してる状態」の初期化
        _current[key] = false;

        // 入力定義vectorのループ
        for (const auto& hardInput : inputRow.second) {
            if (hardInput.type == PeripheralType::keybd) {
                _current[key] = _currentRawKeybdState[hardInput.id];
            }
            else if (hardInput.type == PeripheralType::pad1) {
                _current[key] = hardInput.id & _currentRawPadState;
            }
            // どれか一つでも「押されている」状態ならもう調べない
            // 機器の情報が欲しいわけではない為、誰かが押されていればもうOK
            if (_current[key]) {
                break;
            }
        }
    }

    // 左右スティック更新
    int xInput, zInput;
    GetJoypadAnalogInputRight(&xInput, &zInput, DX_INPUT_PAD1);
    _rightStickInput = { static_cast<float>(xInput), 0, static_cast<float>(zInput) };
    GetJoypadAnalogInput(&xInput, &zInput, DX_INPUT_PAD1);
    _leftStickInput = { static_cast<float>(xInput), 0, static_cast<float>(zInput) };
}

bool Input::IsPress(const char* key) const {
    // そもそも登録されていないキータイプならreturn
    if (!_current.contains(key)) {
        return false;
    }
    // 押されているかどうかを返す
    return _current.at(key);

    // (旧処理)
    //// 押されていればtrue
    //return (_padInput & button);
}

bool Input::IsTrigger(const char* key) const {
    // そもそも登録されていないキータイプならreturn
    if (!_current.contains(key)) {
        return false;
    }
    // 1f前は押されてない かつ 今押されている ならtrue
    return (_current.at(key) && !_last.at(key));

    // (旧処理)
    //// 押されていればtrue.そうでないならfalse
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
    // 一時テーブルにコピー
    _tempInputTable = _inputTable;
    // 表示順序初期化
    _orderForDisplay = { "ok","next","back","pause","action","dash","jump" };
}

void Input::SetDefault()
{
    //入力と名前の対応表を作る
    _inputTable["next"] = { {PeripheralType::keybd, KEY_INPUT_RETURN},
                            {PeripheralType::pad1, PAD_INPUT_8}     // Startボタン
    };
    _inputTable["back"] = { {PeripheralType::keybd, KEY_INPUT_ESCAPE},
                            {PeripheralType::pad1, PAD_INPUT_B}     // Bボタン
    };

    _inputTable["ok"] = { {PeripheralType::keybd, KEY_INPUT_RETURN},
                        {PeripheralType::pad1, PAD_INPUT_A}         // Aボタン
    };
    _inputTable["pause"] = { {PeripheralType::keybd, KEY_INPUT_P},
                            {PeripheralType::pad1, PAD_INPUT_R}     // STARTボタン
    };

    //ゲーム中のボタンテーブル
    _inputTable["action"] = { {PeripheralType::keybd, KEY_INPUT_Z},
                            {PeripheralType::pad1, PAD_INPUT_X}     // Yボタン
    };
    _inputTable["dash"] = { {PeripheralType::keybd, KEY_INPUT_X},
                        {PeripheralType::pad1, PAD_INPUT_C}         // Xボタン
    };
    _inputTable["jump"] = { {PeripheralType::keybd, KEY_INPUT_C},
                        {PeripheralType::pad1, PAD_INPUT_A}         // (多分)Aボタン
    };
    _inputTable["Rbutton"] = { {PeripheralType::keybd, KEY_INPUT_I},
                        {PeripheralType::pad1, PAD_INPUT_5}         // 右ボタン
    };
    _inputTable["Lbutton"] = { {PeripheralType::keybd, KEY_INPUT_P},
                        {PeripheralType::pad1, PAD_INPUT_6}         // 左ボタン
    };

    //上下左右
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
    _tempInputTable = _inputTable;  // 一時テーブルにコピー
}

int Input::GetKeyboradState() const
{
    // 全チェックし、どれかひとつでも入力があったらそれを返す
    for (int i = 0; i < _countof(_currentRawKeybdState); ++i) {
        if (_currentRawKeybdState[i] && !_lastRawKeybdState[i]) {
            return i;
        }
    }
    return -1;
}

int Input::GetPadState(int padno) const
{
    // 指定番号のパッド情報をを取得する
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
    auto err = fopen_s(&fp, kKeyConfigFilename, "wb");//バイナリで「書き込み」
    //識別子書き込み(４バイト)
    std::string signature = kKeyConfigSignature;//ファイルを識別するための識別子
    fwrite(signature.data(), signature.size(), 1, fp);//識別子の書き込み

    //バージョンの書き込み
    const float version = 1.0f;
    fwrite(&version, sizeof(version), 1, fp);//4バイト

    //データサイズの書き込み
    auto size = _inputTable.size();
    fwrite(&size, sizeof(size), 1, fp);//4バイト

    //データ本体の書き込み
    for (const auto& record : _inputTable) {
        //キーの書き込み(イベント名)
        byte nameSize = static_cast<byte>(record.first.length());//文字列数
        fwrite(&nameSize, sizeof(nameSize), 1, fp);//文字列のサイズ
        fwrite(record.first.data(), record.first.size(), 1, fp);//文字列本体
        //実データ値の書き込み(「入力種別＋入力ID」の配列)
        //まずは、実データ配列のサイズを取得します
        byte inputDataSize = static_cast<byte>(record.second.size());
        fwrite(&inputDataSize, sizeof(inputDataSize), 1, fp);
        fwrite(record.second.data(), //書き込む実データの先頭アドレス
            sizeof(record.second[0]), //書き込むデータ１つあたりのサイズ
            record.second.size(), //書き込むデータ数
            fp);
    }

    fclose(fp);
}

void Input::LoadInputTable()
{
    std::string filename = kKeyConfigFilename;
    auto wfilename = StringUtility::GetWStringFromString(filename);
    auto handle = FileRead_open(wfilename.c_str());
    if (handle == 0) {//何頭の原因で読み込み失敗したら読み込まない
        return;
    }
    struct Header {
        char signature[4];//シグネチャ
        float version;//バージョン
        size_t dataNum;//データ数
    };
    Header header = {};//ヘッダーの読み込み
    FileRead_read(&header, sizeof(header), handle);
    //データ数の分だけ読み込んでいく
    for (int i = 0; i < header.dataNum; ++i) {
        //データはまず、イベント名を読み込む
        //名前のサイズ
        byte nameSize = 0;
        FileRead_read(&nameSize, sizeof(nameSize), handle);
        //実際に名前文字列を取得
        std::string strEventName;
        strEventName.resize(nameSize);//名前のサイズ分確保
        FileRead_read(strEventName.data(), nameSize * sizeof(char), handle);

        //名前が終わった後は実データなので、まず実データ個数を取得
        byte inputDataNum = 0;
        FileRead_read(&inputDataNum, sizeof(inputDataNum), handle);
        std::vector<InputState> inputStates;
        inputStates.resize(inputDataNum);
        FileRead_read(inputStates.data(), sizeof(inputStates[0]) * inputDataNum, handle);
        _inputTable[strEventName] = inputStates;
    }

    FileRead_close(handle);
}
