#pragma once

#include "assimp/matrix4x4.h"
#include <DirectXMath.h>

namespace MathConverter
{

	inline DirectX::XMMATRIX AssimpToDirectX(const aiMatrix4x4& mat)
	{
		return DirectX::XMMatrixTranspose(DirectX::XMMatrixSet(mat.a1, mat.a2, mat.a3, mat.a4, mat.b1, mat.b2, mat.b3, mat.b4, mat.c1, mat.c2, mat.c3, mat.c4, mat.d1, mat.d2, mat.d3, mat.d4));
	}

}