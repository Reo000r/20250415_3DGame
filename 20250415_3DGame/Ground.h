#pragma once

class Ground final {
public:
	Ground();
	~Ground();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init();

	/// <summary>
	/// 内部変数の更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画全般
	/// </summary>
	void Draw();

private:

	int _textureHandle;
};