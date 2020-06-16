#include "StreamOutput.h"

StreamOutput::StreamOutput(Rendering & renderer, UINT size, UINT bindFlags)
{
	
	D3D11_BUFFER_DESC cbd = {};
	cbd.BindFlags = bindFlags | D3D11_BIND_STREAM_OUTPUT;
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.CPUAccessFlags = 0u;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = size;
	cbd.StructureByteStride = 0;

	renderer.ThrowIfFailed(renderer.GetDevice()->CreateBuffer(&cbd, nullptr, &pStreamOutput));
	
}
