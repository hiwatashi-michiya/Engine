#pragma once
#include <d3d12.h>
#include <vector>
#include "InputElement.h"

class ML_InputLayout
{
public:
	ML_InputLayout();
	~ML_InputLayout();

	void SetElements(const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputElement);

	const D3D12_INPUT_LAYOUT_DESC& Get() const { return inputLayout_; }

private:

	D3D12_INPUT_LAYOUT_DESC inputLayout_{};

};
