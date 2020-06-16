#pragma once



class Instancer
{
	friend class Importer;
	friend class Exporter;
	static constexpr auto CBufferSlot = 1u;

public:

	void Init(Rendering& renderer)
	{
		assert(indicesToBBoxes.size() > 0 && transformMatrices.size() > 0);
		tempMatsBuffer.resize(transformMatrices.size(), DirectX::XMMatrixIdentity());
		vsMatCBuffer.Create(renderer, CBufferSlot, transformMatrices.size() * sizeof(DirectX::XMMATRIX));
	}

	void OnUpdate(Rendering& renderer, SceneTree& octree)
	{
		numOfInstancesToDraw = 0;
		for (int i = 0; i < indicesToBBoxes.size(); i++)
		{
			auto& bbox = octree.allModelsAABBoxes[indicesToBBoxes[i]];

			if (bbox.bToDraw)
			{
				tempMatsBuffer[numOfInstancesToDraw] = transformMatrices[i];
				numOfInstancesToDraw++;
				bbox.bToDraw = false;
			}
		}

		if (numOfInstancesToDraw != 0) {
			vsMatCBuffer.Update(renderer, tempMatsBuffer.data(), numOfInstancesToDraw * sizeof(DirectX::XMMATRIX));
		}
		
	}

	void Bind(Rendering& renderer)
	{
		vsMatCBuffer.Bind(renderer);
	}
	
	UINT numOfInstancesToDraw;
		
	std::vector<UINT> indicesToBBoxes;
	std::vector<DirectX::XMMATRIX> transformMatrices;

protected:
	VertexConstBuffer<DirectX::XMMATRIX> vsMatCBuffer;

	std::vector<DirectX::XMMATRIX> tempMatsBuffer;

};