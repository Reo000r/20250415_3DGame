#pragma once
#include "Vector3.h"

#include <map>
#include <string>
#include <vector>

#define USE_STICK	// 操作でスティックを使う

/// <summary>
/// 入力系をコントロールするクラス
/// シングルトン化する
/// </summary>
class Input final {
public:
	/// <summary>
	/// シングルトンオブジェクトを返す
	/// </summary>
	/// <returns>シングルトンオブジェクト</returns>
	static Input& GetInstance();

	/// <summary>
	/// 入力情報の更新
	/// </summary>
	void Update();

	/// <summary>
	/// 押されているかどうかの取得
	/// </summary>
	/// <param name="button">判定を行うボタン</param>
	/// <returns>押されていればtrue、でなければfalse</returns>
	bool IsPress(const char* key) const;

	/// <summary>
	/// 押された瞬間かどうかの取得
	/// </summary>
	/// <param name="button">判定を行うボタン</param>
	/// <returns>押されていればtrue、でなければfalse</returns>
	bool IsTrigger(const char* key) const;

	/// <summary>
	/// 右スティックの入力情報をVector3型で返す
	/// x、zに値を入れる
	/// </summary>
	/// <returns>右スティックの入力情報</returns>
	Vector3 GetPadRightSitck() const;
	/// <summary>
	/// 右スティックの入力情報をVector3型で返す
	/// x、zに値を入れる
	/// 最後にあった入力を返す
	/// </summary>
	/// <returns>右スティックの入力情報</returns>
	Vector3 GetPadRightSitckLast() const;

	/// <summary>
	/// 左スティックの入力情報をVector3型で返す
	/// x、zに値を入れる
	/// </summary>
	/// <returns>左スティックの入力情報</returns>
	Vector3 GetPadLeftSitck() const;
	/// <summary>
	/// 左スティックの入力情報をVector3型で返す
	/// x、zに値を入れる
	/// 最後にあった入力を返す
	/// </summary>
	/// <returns>左スティックの入力情報</returns>
	Vector3 GetPadLeftSitckLast() const;

private:
	Input();
	Input(const Input&) = delete;
	void operator=(const Input&) = delete;

	// 周辺機器種別
	enum class PeripheralType {
		keybd,	// キーボード
		pad1	// パッド１
	};
	///入力情報定義用
	struct InputState {
		PeripheralType type;	// 周辺機器種別
		int id;					// 実際の入力を表す定数
	};

	using InputTable_t = std::map<std::string, std::vector<InputState>>;
	InputTable_t _inputTable;		// 名前と実際の入力の対応表
	InputTable_t _tempInputTable;	// 名前と実際の入力の対応表(一時的なコピー)

	/// <summary>
	/// 入力を初期値に設定する
	/// </summary>
	void SetDefault();

	///// <summary>
	///// キーコンフィグ用(行った変更を確定する)
	///// </summary>
	//void CommitEdittedInputTable();

	///// <summary>
	///// キーコンフィグ用(行った変更をなかったことにする)
	///// </summary>
	//void RollbackEdittedInputTable();

	// 押されたかどうかを記録していくもの
	using InputRecord_t = std::map<std::string, bool>;
	InputRecord_t _current;		// 現在押してるかどうか
	InputRecord_t _last;		// 直前に押されてたかどうか

	std::vector<std::string> _orderForDisplay;	// 表示用(mapの順序はコントロールできないため)

	// 入力保存
	int _currentRawPadState;			// 今の生パッドステート
	int _lastRawPadState;				// 直前の生パッドステート
	char _currentRawKeybdState[256];	// 今のキーボード状態
	char _lastRawKeybdState[256];		// 直前のキーボード状態

	// 左右スティック入力情報
	Vector3 _rightStickInput;
	Vector3 _leftStickInput;
	// 左右スティックの最後の入力情報
	Vector3 _rightStickLastInput;
	Vector3 _leftStickLastInput;

	/// <summary>
	/// 現在のキーボード状態を調べて最も
	/// 若いキーコードを返す
	/// ひとつも押されてなければ-1を返す
	/// </summary>
	/// <returns></returns>
	int GetKeyboradState()const;

	/// <summary>
	/// 現在のPAD状態を調べて最も
	/// 若いキー入力を返す
	/// ひとつも押されてなければ-1を返す
	/// </summary>
	int GetPadState(int padno)const;

	/// <summary>
	/// 現在の入力テーブルをセーブする
	/// </summary>
	void SaveInputTable();

	/// <summary>
	/// 入力テーブルをロードして反映する
	/// </summary>
	void LoadInputTable();
};