#pragma once
#include <DxLib.h>

class Application final {
	// シングルトン化
private:
	Application() {}
	Application(const Application&) = delete;
	void operator=(const Application&) = delete;

#ifdef _DEBUG	// コンソールDebug用変数
	FILE* _out;
	FILE* _in;
#endif	// _DEBUG

public:
	/// <summary>
	/// シングルトンオブジェクトを返す
	/// </summary>
	/// <returns>Applicationシングルトンオブジェクト</returns>
	static Application& GetInstance();

	/// <summary>
	/// アプリケーションの初期化
	/// </summary>
	/// <returns>true:初期化成功 / false:初期化失敗</returns>
	bool Init();

	/// <summary>
	/// メインループを起動
	/// </summary>
	void Run();

	/// <summary>
	/// アプリケーションの後処理
	/// </summary>
	void Terminate();
};

