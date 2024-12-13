#pragma once
#include <d3d12.h>
#include <vector>

/// <summary>
/// インプットエレメントを扱いやすくしたクラス
/// </summary>
class ML_InputElement
{
public:
	ML_InputElement();
	~ML_InputElement();
	//サイズセット
	void SetSize(uint32_t size) { inputElement_.resize(size); }
	//エレメントセット
	void SetElement(const char* semanticName, uint32_t semanticIndex,
		DXGI_FORMAT format, uint32_t alignedByteIndex, uint32_t elementIndex);
	//インプットエレメントの本体取得
	const std::vector<D3D12_INPUT_ELEMENT_DESC>& Get() const { return inputElement_; }

private:

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputElement_;

};


