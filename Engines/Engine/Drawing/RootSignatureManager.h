#pragma once
#include <d3d12.h>
#include <string>
#include <unordered_map>
#include <wrl.h>

/// <summary>
/// ルートシグネチャ管理クラス
/// </summary>
class RootSignatureManager
{
public:
	//インスタンス取得
	static RootSignatureManager* GetInstance();
	//初期化
	void Initialize(ID3D12Device* device);
	//ルートシグネチャ作成
	void CreateRootSignature(Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob, const std::string& name);
	//ルートシグネチャ取得
	ID3D12RootSignature* GetRootSignature(const std::string& name);

private:

	ID3D12Device* device_;
	//ルートシグネチャと名前を紐づけたマップ
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12RootSignature>> rootSignatures_;

private:
	//シングルトン化
	RootSignatureManager() = default;
	~RootSignatureManager() = default;
	RootSignatureManager(const RootSignatureManager&) = delete;
	const RootSignatureManager& operator=(const RootSignatureManager&) = delete;

};


