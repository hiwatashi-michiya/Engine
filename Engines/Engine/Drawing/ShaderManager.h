#pragma once
#include <d3d12.h>
#include <dxcapi.h>
#include <format>
#include <unordered_map>
#include <wrl.h>

/// <summary>
/// シェーダー管理クラス
/// </summary>
class ShaderManager
{
public:
	//シェーダーの種類
	enum ShaderType {
		kVS, //vertex
		kPS, //pixel


		kMaxShaderType, //最大数
	
	};
	//インスタンス取得
	static ShaderManager* GetInstance();
	//初期化
	void Initialize();
	//シェーダーコンパイル
	IDxcBlob* CompileShader(const std::wstring& filePath, ShaderType type, const std::string& name);

private:

	IDxcBlob* CompileShader(
		//CompilerするShaderファイルへのパス
		const std::wstring& filePath,
		//Compilerに使用するProfile
		const wchar_t* profile,
		//初期化で生成したものを3つ
		Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils,
		Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler,
		Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler);

	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<IDxcBlob>> blobs_;

private:
	//シングルトン化
	ShaderManager() = default;
	~ShaderManager() = default;
	ShaderManager(const ShaderManager&) = delete;
	const ShaderManager& operator=(const ShaderManager&) = delete;

};


