#include"InputElement.h"

ML_InputElement::ML_InputElement()
{
}

ML_InputElement::~ML_InputElement()
{
}

void ML_InputElement::SetElement(const char* semanticName, uint32_t semanticIndex, DXGI_FORMAT format, uint32_t alignedByteIndex, uint32_t elementIndex)
{

	if (elementIndex >= inputElement_.size()) {
		return;
	}

	inputElement_[elementIndex].SemanticName = semanticName;
	inputElement_[elementIndex].SemanticIndex = semanticIndex;
	inputElement_[elementIndex].Format = format;
	inputElement_[elementIndex].AlignedByteOffset = alignedByteIndex;

}
