#pragma once
#include <random>
#include "System/Rendering.h"

namespace RandomGenerator
{

	float RandomFrom0To1();

	float Random(float from, float to);

	DirectX::XMFLOAT3 Random(DirectX::XMFLOAT3 from, DirectX::XMFLOAT3 to);

	DirectX::XMVECTOR RandomUnitVector();

	void CreateRandomNumbersTexture1D(Rendering& renderer, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& dest);

	void CreateRandomNumbersTexture2D(Rendering& renderer, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& dest);

	void CreateNoiseTexture(Rendering& renderer, UINT width, UINT height, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& dest);

	void GaussGenerator(uint32_t arraySize, float* array, float fDeviation, float fMultiplier);

}