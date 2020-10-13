#include "Terrain.h"

namespace OSK {

	Terrain::Terrain(ContentManager* Content) {
		this->Content = Content;
	}

	Terrain::~Terrain() {

	}

	void Terrain::CreateMesh(const std::string& path, const Vector2f& quadSize, const float_t& maxHeight) {
		Content->LoadHeightmap(Map, path);
		MapSizeInQuads = Map.Size;
		QuadSize = quadSize;
		MapSizeInWorlds = QuadSize * MapSizeInQuads.ToVector2f();

		HeightMultiply = -maxHeight / 255.0f;

		std::vector<Vertex> vertices = {};
		std::vector<vertexIndex_t> indices = {};

		vertices.reserve(MapSizeInQuads.X * MapSizeInQuads.Y);
		indices.reserve(MapSizeInQuads.X * MapSizeInQuads.Y);

		uint32_t i = 0;
		for (uint32_t x = 0; x < MapSizeInQuads.X; x++) {
			for (uint32_t y = 0; y < MapSizeInQuads.Y; y++) {
				Vertex v{};
				float_t altura = Map.Data[i] * HeightMultiply;
				i++;

				v.Position = { x * QuadSize.X, altura, y * QuadSize.Y };
				v.TextureCoordinates = { x % 2, y % 2 };

				Vector3f normal = { 0.0f, -1.0f, 0.0f };

				float left = altura;
				float right = altura;
				float top = altura;
				float bot = altura;

				const int32_t nX = x;
				const int32_t nY = y;
				//const Vector3f thisheight = 
				Vector3f R = { (nX + 1) * QuadSize.X, GetVertexHeight({ nX + 1, nY }), (nY)*QuadSize.Y };
				Vector3f L = { (nX - 1) * QuadSize.X, GetVertexHeight({ nX - 1, nY }), (nY)*QuadSize.Y };
				Vector3f T = { (nX) * QuadSize.X, GetVertexHeight({ nX, nY + 1 }), (nY + 1)*QuadSize.Y };
				Vector3f B = { (nX) * QuadSize.X, GetVertexHeight({ nX + 1, nY - 1 }), (nY - 1)*QuadSize.Y };
				
				normal = (L - R).Cross(T - B).GetNormalized();

				v.Normals = -normal.ToGLM();

				vertices.push_back(v);
			}
		}

		for (uint32_t x = 0; x < MapSizeInQuads.X - 1; x++) {
			for (uint32_t y = 0; y < MapSizeInQuads.Y - 1; y++) {
				vertexIndex_t index0 = y + x * MapSizeInQuads.Y;
				vertexIndex_t index1 = y + 1 + x * MapSizeInQuads.Y;
				vertexIndex_t index2 = y + (x + 1) * MapSizeInQuads.Y;

				indices.push_back(index0);
				indices.push_back(index1);
				indices.push_back(index2);

				index0 = y + 1 + x * MapSizeInQuads.Y;
				index1 = y + (x + 1) * MapSizeInQuads.Y + 1;
				index2 = y + (x + 1) * MapSizeInQuads.Y;

				indices.push_back(index0);
				indices.push_back(index1);
				indices.push_back(index2);
			}
		}

		Box.Position = { 0.0f };
		Box.Position.Y = BaseHeight;
		Box.Size = { MapSizeInWorlds.X, maxHeight, MapSizeInWorlds.Y };

		terrainModel = Content->CreateModel(vertices, indices);
	}

	float_t Terrain::GetHeight(const Vector2f& point) const {
		Vector2f newPoint = { point.X / QuadSize.X, point.Y / QuadSize.Y };
		uint32_t index = (int)newPoint.X * MapSizeInQuads.Y + (int)newPoint.Y;
		if (index > MapSizeInQuads.X * MapSizeInQuads.Y)
			return BaseHeight;

		if (point.X < 0 || point.Y < 0)
			return BaseHeight;

		uint32_t baseIndex = index;
		uint32_t rightIndex = baseIndex + MapSizeInQuads.Y;
		uint32_t botIndex = baseIndex + 1;
		uint32_t botRightIndex = rightIndex + 1;

		float* heights = new float[4];

		const float diffX = newPoint.X - (int)newPoint.X;
		const float diffY = newPoint.Y - (int)newPoint.Y;
		const Vector2f playerPos = { diffX, diffY };

		const bool isTopLeftTriangle = diffX + diffY < 1.0f;

		float final = BaseHeight;

#define __RIGHT 0
#define __BOT 1
#define __BOTRIGHT 2
#define __ORIGINAL 3

		heights[__RIGHT] = Map.Data[rightIndex];
		heights[__BOT] = Map.Data[botIndex];
		heights[__BOTRIGHT] = Map.Data[botRightIndex];
		heights[__ORIGINAL] = Map.Data[baseIndex];

		if (isTopLeftTriangle) {
			Vector3f p0 = { 0, 0, heights[__ORIGINAL] * HeightMultiply };
			Vector3f p1 = { 0, 1, heights[__BOT] * HeightMultiply };
			Vector3f p2 = { 1, 0, heights[__RIGHT] * HeightMultiply };

			final = barryCentric(p0, p1, p2, playerPos);
		}
		else {
			Vector3f p0 = { 1, 1, heights[__BOTRIGHT] * HeightMultiply };
			Vector3f p1 = { 0, 1, heights[__BOT] * HeightMultiply };
			Vector3f p2 = { 1, 0, heights[__RIGHT] * HeightMultiply };

			final = barryCentric(p0, p1, p2, playerPos);
		}

		delete[] heights;

#undef __RIGHT
#undef __BOT
#undef __BOTRIGHT
#undef __ORIGINAL

		return final;
	}

	float_t Terrain::GetVertexHeight(const Vector2i& point) const {
		uint32_t index = (int)point.X * MapSizeInQuads.Y + (int)point.Y;
		if (index > MapSizeInQuads.X * MapSizeInQuads.Y || point.X < 0 || point.Y < 0)
			return BaseHeight;

		return Map.Data[index] * HeightMultiply;
	}

	CollisionBox Terrain::GetCollisionBox() const {
		return Box;
	}

	const float Terrain::barryCentric(const Vector3f& p1, const Vector3f& p2, const Vector3f& p3, const Vector2f& pos) const {
		float det = (p2.Y - p3.Y) * (p1.X - p3.X) + (p3.X - p2.X) * (p1.Y - p3.Y);
		float l1 = ((p2.Y - p3.Y) * (pos.X - p3.X) + (p3.X - p2.X) * (pos.Y - p3.Y)) / det;
		float l2 = ((p3.Y - p1.Y) * (pos.X - p3.X) + (p1.X - p3.X) * (pos.Y - p3.Y)) / det;
		float l3 = 1.0f - l1 - l2;
		return l1 * p1.Z + l2 * p2.Z + l3 * p3.Z;
	}

}