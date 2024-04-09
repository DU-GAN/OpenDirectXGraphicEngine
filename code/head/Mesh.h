#pragma once

#include <string>
#include <vector>

#include "UString.h"
#include "Vertex_G.h"
#include "Vertex_GB.h"
#include "Exception.h"
#include "GeometryDesc.h"

namespace Rendering
{
	template<typename VertexType>
	class Mesh
	{
	public:
		/*
			Please pass in the vertex and index numbers to pre allocate
			the memory size of the vector and avoid large amounts of data destruction and copying
			The data type of the index will be determined based on the number of indices.
		*/
		Mesh(int vertexNum = 0, int indexNum = 0)
		{
			Reserve(vertexNum, indexNum);
		}

		/*
			Adjusting the pre allocated memory size of vertices and index vectors.
			The data type of the index will be determined based on the number of indices.
		*/
		void Reserve(int vertexNum = -1, int indexNum = -1)
		{
			if (vertexNum > MAX_VECTOR_RESERVE_SIZE || indexNum > MAX_VECTOR_RESERVE_SIZE)
				Exception::ThrowException(LEVEL_Error, "reserve out of max number");

			if (vertexNum >= 0)
			{
				if (vertexNum <= UINT16_MAX)
				{
					indexIs2Byte = true;
				}
				else
				{
					indexIs2Byte = false;
				}
				vertexs.reserve(vertexNum);
			}
			if (indexNum >= 0)
			{
				if (indexIs2Byte)
				{
					indices16.reserve(indexNum);
				}
				else
				{
					indices32.reserve(indexNum);
				}
			}
		}

		// Release the space of vertices and indexed vectors
		void FreeSpace()
		{
			FreeVector(vertexs);
			if (indexIs2Byte)
			{
				FreeVector(indices16);
			}
			else
			{
				FreeVector(indices32);
			}
		}

		void SetName(UString _name)
		{
			name = _name;
		}
		UString GetName()const
		{
			return name;
		}
		int GetVertexNumber()const
		{
			return vertexs.size();
		}
		int GetIndexNumber()const
		{
			if (indexIs2Byte)
				return indices16.size();
			else
				return indices32.size();
		}
		bool IndexIs2Byte()const
		{
			return indexIs2Byte;
		}

		std::vector<VertexType>& GetVertexs()
		{
			return vertexs;
		}

		// Add an index that will be intelligently added to the corresponding array based on the index data type
		void AddIndex(int index)
		{
			if (index < 0)
				Exception::ThrowException(LEVEL_Prompt, "index < 0");

			if (indexIs2Byte)
				indices16.push_back(index);
			else
				indices32.push_back(index);
		}

		void SetPBRMaterilaIndex(int _materialIndex)
		{
			if (_materialIndex < 0)
				Exception::ThrowException(LEVEL_Prompt, "materialIndex < 0");

			materialIndex = _materialIndex;
		}
		int GetPBRMaterialIndex()const
		{
			return materialIndex;
		}

		// Obtain vertex and index information descriptions for the grid
		GeometryDesc GetGeometryDesc()const
		{
			GeometryDesc geoDesc;

			// Set the description of vertex data
			geoDesc.SetVertexData((const void*)(vertexs.data()), sizeof(VertexType), vertexs.size() * sizeof(VertexType));

			// Set the description of index data
			if (indexIs2Byte)
			{
				geoDesc.SetIndexData((const void*)(indices16.data()),
					sizeof(std::uint16_t), indices16.size() * sizeof(std::uint16_t));
				geoDesc.SetIndexCount(indices16.size());
			}
			else
			{
				geoDesc.SetIndexData((const void*)(indices32.data()),
					sizeof(std::uint32_t), indices32.size() * sizeof(std::uint32_t));
				geoDesc.SetIndexCount(indices32.size());
			}
			return geoDesc;
		}

		// Obtain vertex and index information descriptions for multiple meshes
		static std::vector<GeometryDesc> GetGeometryDescs(const std::vector<Mesh<VertexType>>& meshs)
		{
			if (meshs.empty())
				Exception::ThrowException(LEVEL_Error, "meshs is empty");

			std::vector<GeometryDesc> geoDescs(meshs.size());
			for (int i = 0; i < meshs.size(); ++i)
			{
				geoDescs[i] = meshs[i].GetGeometryDesc();
			}
			return geoDescs;
		}

		DirectX::XMFLOAT4X4& GetTransform()
		{
			return transform;
		}

	public:
		UString name;
		std::vector<VertexType> vertexs;

		/*
			The index can be 2 bytes or 4 bytes,
			and the Mesh constructor and Reserve will dynamically determine based on parameters, with a default value of 4 bytes
		*/
		bool indexIs2Byte = false;
		std::vector<std::uint16_t> indices16;
		std::vector<std::uint32_t> indices32;

		int materialIndex = -1;

		DirectX::XMFLOAT4X4 transform = Identity4X4();
	};
}