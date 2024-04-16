#pragma once

#include "Model.h"

using namespace DirectX;

namespace Rendering
{
	class GeometricConstructor
	{
	public:
		static void GenerateSphereMesh(Model<Vertex_G>& model, int STACK_COUNT = 16, int SLICE_COUNT = 32)
		{
			Mesh<Vertex_G>& resMesh = AddElementVector(model.GetMeshs());

			resMesh.Reserve((STACK_COUNT + 1) * (SLICE_COUNT + 1), 2 * SLICE_COUNT * STACK_COUNT + 2 * SLICE_COUNT);
			resMesh.GetVertexs().resize((STACK_COUNT + 1) * (SLICE_COUNT + 1));

			int vertexIndex = 0;
			for (int i = 0; i <= STACK_COUNT; ++i) {
				float v = static_cast<float>(i) / STACK_COUNT;
				float phi = v * MATH_PI;

				for (int j = 0; j <= SLICE_COUNT; ++j) {
					float u = static_cast<float>(j) / SLICE_COUNT;
					float theta = 2.0f * MATH_PI * u;

					resMesh.GetVertexs()[vertexIndex].position.x = sinf(phi) * cosf(theta);
					resMesh.GetVertexs()[vertexIndex].position.y = sinf(phi) * sinf(theta);
					resMesh.GetVertexs()[vertexIndex].position.z = cosf(phi);

					++vertexIndex;
				}
			}

			for (int i = 0; i < STACK_COUNT; ++i) {
				for (int j = 0; j < SLICE_COUNT; ++j) {
					resMesh.AddIndex(i * (SLICE_COUNT + 1) + j);
					resMesh.AddIndex((i + 1) * (SLICE_COUNT + 1) + j);
					resMesh.AddIndex((i + 1) * (SLICE_COUNT + 1) + j + 1);

					resMesh.AddIndex(i * (SLICE_COUNT + 1) + j);
					resMesh.AddIndex((i + 1) * (SLICE_COUNT + 1) + j + 1);
					resMesh.AddIndex(i * (SLICE_COUNT + 1) + j + 1);
				}
			}

			resMesh.AddIndex(STACK_COUNT * (SLICE_COUNT + 1));
			for (int j = 1; j < SLICE_COUNT; ++j) {
				resMesh.AddIndex(STACK_COUNT * (SLICE_COUNT + 1));
				resMesh.AddIndex(STACK_COUNT * (SLICE_COUNT + 1) + j + 1);
				resMesh.AddIndex(STACK_COUNT * (SLICE_COUNT + 1) + j);
			}
			resMesh.AddIndex(STACK_COUNT * (SLICE_COUNT + 1));
			resMesh.AddIndex(STACK_COUNT * (SLICE_COUNT + 1) + 1);
			resMesh.AddIndex(0);

			resMesh.AddIndex(0);
			for (int j = SLICE_COUNT - 1; j > 0; --j) {
				resMesh.AddIndex(0);
				resMesh.AddIndex(j);
				resMesh.AddIndex(j + 1);
			}
			resMesh.AddIndex(0);
			resMesh.AddIndex(1);
			resMesh.AddIndex(SLICE_COUNT + 1);
		}

		static void LoadCubeGeometric(
			std::vector<DirectX::XMFLOAT3>& pos,
			std::vector<DirectX::XMFLOAT2>& tex,
			std::vector<std::uint32_t>& index)
		{
			int vertexNum = 24;
			pos.resize(vertexNum);
			tex.resize(vertexNum);

			int indexNum = 36;
			index.resize(indexNum);

			DirectX::XMFLOAT3 posData[] = {
					{ -0.5f, -0.5f,  0.5f }, { 0.5f, -0.5f,  0.5f }, { 0.5f,  0.5f,  0.5f }, { -0.5f,  0.5f,  0.5f },
					{ -0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f }, { 0.5f,  0.5f, -0.5f }, { -0.5f,  0.5f, -0.5f },
					{ -0.5f,  0.5f,  0.5f }, { -0.5f,  0.5f, -0.5f }, { -0.5f, -0.5f, -0.5f }, { -0.5f, -0.5f,  0.5f },
					{ 0.5f,  0.5f,  0.5f }, { 0.5f,  0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f,  0.5f },
					{ -0.5f,  0.5f,  0.5f }, { 0.5f,  0.5f,  0.5f }, { 0.5f,  0.5f, -0.5f }, { -0.5f,  0.5f, -0.5f },
					{ -0.5f, -0.5f,  0.5f }, { 0.5f, -0.5f,  0.5f }, { 0.5f, -0.5f, -0.5f }, { -0.5f, -0.5f, -0.5f }
			};

			DirectX::XMFLOAT2 texData[] = {
				{ 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f },
				{ 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f },
				{ 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f },
				{ 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f }, { 0.0f, 0.0f },
				{ 1.0f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f },
				{ 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f }
			};

			std::uint32_t indexData[] = {
				0, 1, 2,
				2, 3, 0,

				4, 5, 6,
				6, 7, 4,

				8, 9, 10,
				10, 11, 8,

				12, 13, 14,
				14, 15, 12,

				16, 17, 18,
				18, 19, 16,

				20, 21, 22,
				22, 23, 20
			};

			for (int i = 0; i < vertexNum; ++i)
			{
				pos[i] = posData[i];
				tex[i] = texData[i];
			}

			for (int i = 0; i < indexNum; ++i)
			{
				index[i] = indexData[i];
			}
		}

		inline static void LoadCubeGeometric(Model<Vertex_G>& cube)
		{
			Mesh<Vertex_G>& resMesh = AddElementVector(cube.GetMeshs());

			std::vector<DirectX::XMFLOAT3> pos;
			std::vector<DirectX::XMFLOAT2> tex;
			std::vector<std::uint32_t> index;

			LoadCubeGeometric(pos, tex, index);
			
			resMesh.Reserve(pos.size(), index.size());
			for (int i = 0; i < pos.size(); ++i)
			{
				resMesh.GetVertexs().push_back(Vertex_G());
				resMesh.GetVertexs()[i].position = pos[i];
				resMesh.GetVertexs()[i].texCoords = tex[i];
			}

			for (int i = 0; i < index.size(); ++i)
			{
				resMesh.AddIndex(index[i]);
			}
		}
	};
}