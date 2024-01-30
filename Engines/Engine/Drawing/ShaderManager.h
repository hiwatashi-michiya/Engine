#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <dxcapi.h>
#include <format>

class ShaderManager
{
public:
	
	enum ShaderType {
		kVS, //vertex
		kPS, //pixel


		kMaxShaderType, //最大数
	
	};

	static ShaderManager* GetInstance();

	void Initialize();

	Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, ShaderType type);

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

private:

	ShaderManager() = default;
	~ShaderManager() = default;
	ShaderManager(const ShaderManager&) = delete;
	const ShaderManager& operator=(const ShaderManager&) = delete;

};


