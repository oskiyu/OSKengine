#include "Terrain.h"

namespace OSK {

	Terrain::Terrain(ContentManager* Content) {
		this->content = Content;
	}

	Terrain::~Terrain() {

	}

	Vector2f Terrain::GetMapSize() const {
		return mapSizeInWorlds;
	}

	void Terrain::CreateMesh(const std::string& path, const Vector2f& quadSize, float maxHeight) {
		content->LoadHeightmap(&map, path);
		mapSizeInQuads = map.size;
		this->quadSize = quadSize;
		mapSizeInWorlds = quadSize * mapSizeInQuads.ToVector2f();

		heightMultiply = -maxHeight / 255.0f;

		std::vector<Vertex> vertices = {};
		std::vector<vertexIndex_t> indices = {};

		vertices.reserve(mapSizeInQuads.X * mapSizeInQuads.Y);
		indices.reserve(mapSizeInQuads.X * mapSizeInQuads.Y);

		uint32_t i = 0;
		for (uint32_t x = 0; x < mapSizeInQuads.X; x++) {
			for (uint32_t y = 0; y < mapSizeInQuads.Y; y++) {
				Vertex v{};
				float_t altura = map.data[i] * heightMultiply;
				i++;

				v.position = { x * quadSize.X, altura, y * quadSize.Y };
				v.textureCoordinates = { x % 2, y % 2 };

				Vector3f normal = { 0.0f, -1.0f, 0.0f };

				float left = altura;
				float right = altura;
				float top = altura;
				float bot = altura;

				const int32_t nX = x;
				const int32_t nY = y;
				//const Vector3f thisheight = 
				Vector3f R = { (nX + 1) * quadSize.X, GetVertexHeight({ nX + 1, nY }), (nY)*quadSize.Y };
				Vector3f L = { (nX - 1) * quadSize.X, GetVertexHeight({ nX - 1, nY }), (nY)*quadSize.Y };
				Vector3f T = { (nX) * quadSize.X, GetVertexHeight({ nX, nY + 1 }), (nY + 1)*quadSize.Y };
				Vector3f B = { (nX) * quadSize.X, GetVertexHeight({ nX + 1, nY - 1 }), (nY - 1)*quadSize.Y };
				
				normal = (L - R).Cross(T - B).GetNormalized();

				v.normals = -normal.ToGLM();

				vertices.push_back(v);
			}
		}

		for (uint32_t x = 0; x < mapSizeInQuads.X - 1; x++) {
			for (uint32_t y = 0; y < mapSizeInQuads.Y - 1; y++) {
				vertexIndex_t index0 = y + x * mapSizeInQuads.Y;
				vertexIndex_t index1 = y + 1 + x * mapSizeInQuads.Y;
				vertexIndex_t index2 = y + (x + 1) * mapSizeInQuads.Y;

				indices.push_back(index0);
				indices.push_back(index1);
				indices.push_back(index2);

				index0 = y + 1 + x * mapSizeInQuads.Y;
				index1 = y + (x + 1) * mapSizeInQuads.Y + 1;
				index2 = y + (x + 1) * mapSizeInQuads.Y;

				indices.push_back(index0);
				indices.push_back(index1);
				indices.push_back(index2);
			}
		}

		colisionBox.position = { 0.0f };
		colisionBox.position.Y = baseHeight;
		colisionBox.size = { mapSizeInWorlds.X, maxHeight, mapSizeInWorlds.Y };

		terrainModel = content->CreateModel(vertices, indices);
	}

	float_t Terrain::GetHeight(const Vector2f& point) const {
		Vector2f newPoint = { point.X / quadSize.X, point.Y / quadSize.Y };
		uint32_t index = (int)newPoint.X * mapSizeInQuads.Y + (int)newPoint.Y;
		if (index > mapSizeInQuads.X * mapSizeInQuads.Y)
			return baseHeight;

		if (point.X < 0 || point.Y < 0)
			return baseHeight;

		uint32_t baseIndex = index;
		uint32_t rightIndex = baseIndex + mapSizeInQuads.Y;
		uint32_t botIndex = baseIndex + 1;
		uint32_t botRightIndex = rightIndex + 1;

		float* heights = new float[4];

		const float diffX = newPoint.X - (int)newPoint.X;
		const float diffY = newPoint.Y - (int)newPoint.Y;
		const Vector2f playerPos = { diffX, diffY };

		const bool isTopLeftTriangle = diffX + diffY < 1.0f;

		float final = baseHeight;

#define __RIGHT 0
#define __BOT 1
#define __BOTRIGHT 2
#define __ORIGINAL 3

		heights[__RIGHT] = map.data[rightIndex];
		heights[__BOT] = map.data[botIndex];
		heights[__BOTRIGHT] = map.data[botRightIndex];
		heights[__ORIGINAL] = map.data[baseIndex];

		if (isTopLeftTriangle) {
			Vector3f p0 = { 0, 0, heights[__ORIGINAL] * heightMultiply };
			Vector3f p1 = { 0, 1, heights[__BOT] * heightMultiply };
			Vector3f p2 = { 1, 0, heights[__RIGHT] * heightMultiply };

			final = barryCentric(p0, p1, p2, playerPos);
		}
		else {
			Vector3f p0 = { 1, 1, heights[__BOTRIGHT] * heightMultiply };
			Vector3f p1 = { 0, 1, heights[__BOT] * heightMultiply };
			Vector3f p2 = { 1, 0, heights[__RIGHT] * heightMultiply };

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
		uint32_t index = (int)point.X * mapSizeInQuads.Y + (int)point.Y;
		if (index > mapSizeInQuads.X * mapSizeInQuads.Y || point.X < 0 || point.Y < 0)
			return baseHeight;

		return map.data[index] * heightMultiply;
	}

	CollisionBox Terrain::GetCollisionBox() const {
		return colisionBox;
	}

	float Terrain::barryCentric(const Vector3f& p1, const Vector3f& p2, const Vector3f& p3, const Vector2f& pos) const {
		float det = (p2.Y - p3.Y) * (p1.X - p3.X) + (p3.X - p2.X) * (p1.Y - p3.Y);
		float l1 = ((p2.Y - p3.Y) * (pos.X - p3.X) + (p3.X - p2.X) * (pos.Y - p3.Y)) / det;
		float l2 = ((p3.Y - p1.Y) * (pos.X - p3.X) + (p1.X - p3.X) * (pos.Y - p3.Y)) / det;
		float l3 = 1.0f - l1 - l2;
		return l1 * p1.Z + l2 * p2.Z + l3 * p3.Z;
	}

}