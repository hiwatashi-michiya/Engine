#include "InputLayout.h"

ML_InputLayout::ML_InputLayout()
{
}

ML_InputLayout::~ML_InputLayout()
{
}

void ML_InputLayout::SetElements(const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputElement)
{

	inputLayout_.pInputElementDescs = inputElement.data();
	inputLayout_.NumElements = UINT(inputElement.size());

}
