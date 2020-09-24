#include "ContentManager.h"

#include "VulkanRenderer.h"
#include <gtc\type_ptr.hpp>
#include "stbi_image.h"
#include <ktx.h>

#include "VulkanImage.h"
#include "FileIO.h"

#include "VulkanImageGen.h"

using namespace OSK::VULKAN;

namespace OSK {

	Assimp::Importer ContentManager::GlobalImporter;

	const std::string ContentManager::DEFAULT_TEXTURE_PATH = "assets/engine/default_texture";
	ModelTexture* ContentManager::DefaultTexture = nullptr;

	ContentManager::ContentManager(VulkanRenderer* renderer) {
		this->renderer = renderer;

		Textures = {};
		ModelTextures = {};
		SkyboxTextures = {};
		ModelDatas = {};
		Sprites = {};

		TextureFromPath = {};
		ModelTextureFromPath = {};
		SkyboxTextureFromPath = {};
		ModelDataFromPath = {};
	}

	ContentManager::~ContentManager() {
		Unload();
	}

	void ContentManager::Unload() {
		if (!Textures.empty()) {
			for (auto& i : Textures) {
				i->Albedo.Destroy();
				delete i;
			}
		}

		if (!ModelTextures.empty())
			for (auto& i : ModelTextures)
				delete i;
		
		if (!SkyboxTextures.empty()) {
			for (auto& i : SkyboxTextures) {
				i->texture.Destroy();
				delete i;
			}
		}

		if (!ModelDatas.empty())
			for (auto& i : ModelDatas)
				delete i;

		if (!Sprites.empty()) {
			for (auto& i : Sprites) {
				renderer->DestroyBuffer(i->VertexBuffer);
				renderer->DestroyBuffer(i->IndexBuffer);
			}
		}
		
		Textures.clear();
		ModelTextures.clear();
		SkyboxTextures.clear();
		ModelDatas.clear();
		Sprites.clear();

		TextureFromPath.clear();
		ModelTextureFromPath.clear();
		SkyboxTextureFromPath.clear();
		ModelDataFromPath.clear();
		std::cout << "Unloaded content manager." << std::endl;
	}

	Texture* ContentManager::LoadTexture(const std::string& path) {
		if (TextureFromPath.find(path) != TextureFromPath.end())
			return TextureFromPath[path];

		Texture* loadedTexture = new Texture();

		int width;
		int height;
		int nChannels;

		stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &nChannels, STBI_rgb_alpha);
		VkDeviceSize size = (VkDeviceSize)width * (VkDeviceSize)height * (VkDeviceSize)nChannels;
		loadedTexture->sizeX = width;
		loadedTexture->sizeY = height;

		VulkanBuffer stagingBuffer;
		renderer->CreateBuffer(stagingBuffer, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		void* data;
		vkMapMemory(renderer->LogicalDevice, stagingBuffer.Memory, 0, size, 0, &data);
		memcpy(data, pixels, static_cast<size_t>(size));
		vkUnmapMemory(renderer->LogicalDevice, stagingBuffer.Memory);

		stbi_image_free(pixels);
		VulkanImageGen::CreateImage(&loadedTexture->Albedo, { (uint32_t)width, (uint32_t)height }, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, 1);

		VulkanImageGen::TransitionImageLayout(&loadedTexture->Albedo, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, 1);
		VulkanImageGen::CopyBufferToImage(&stagingBuffer, &loadedTexture->Albedo, width, height);
		VulkanImageGen::TransitionImageLayout(&loadedTexture->Albedo, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 1, 1);

		renderer->DestroyBuffer(stagingBuffer);

		VulkanImageGen::CreateImageView(&loadedTexture->Albedo, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);

		renderer->createDescriptorSets(loadedTexture);

		Textures.push_back(loadedTexture);
		TextureFromPath[path] = Textures.back();

		return TextureFromPath[path];
	}

	ModelData* ContentManager::LoadModelData(const std::string& path) {
		if (ModelDataFromPath.find(path) != ModelDataFromPath.end())
			return ModelDataFromPath.at(path);

		TempModelData data = GetModelTempData(path, 1.0f);

		ModelData* m = CreateModel(data.Vertices, data.Indices);
		ModelDataFromPath[path] = m;

		return m;
	}

	TempModelData ContentManager::GetModelTempData(const std::string& path, const float_t& scale) const {
		const aiScene* scene;

		scene = GlobalImporter.ReadFile(path.c_str(), AssimpFlags);

		std::vector<Vertex> Vertices;

		//Meshes.
		for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
			//Vertices.
			for (uint32_t v = 0; v < scene->mMeshes[i]->mNumVertices; v++) {
				Vertex vertex{};
				glm::vec3 vec3 = glm::make_vec3(&scene->mMeshes[i]->mVertices[v].x) * scale;
				vertex.Position = vec3;

				vertex.Normals.x = scene->mMeshes[i]->mNormals[v].x;
				vertex.Normals.y = -scene->mMeshes[i]->mNormals[v].y;
				vertex.Normals.z = scene->mMeshes[i]->mNormals[v].z;

				vertex.TextureCoordinates = glm::make_vec2(&scene->mMeshes[i]->mTextureCoords[0][v].x);

				if (scene->mMeshes[i]->HasVertexColors(0))
					vertex.Color = glm::make_vec3(&scene->mMeshes[i]->mColors[0][v].r);
				else
					vertex.Color = Color::WHITE().ToGLM();

				vertex.Position.y *= -1.0f;

				Vertices.push_back(vertex);
			}
		}

		std::vector<vertexIndex_t> indices{};
		//Meshes.
		for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
			uint32_t indexBase = static_cast<uint32_t>(indices.size());
			//Vertices.
			for (uint32_t f = 0; f < scene->mMeshes[i]->mNumFaces; f++) {
				for (uint32_t ind = 0; ind < 3; ind++) {
					indices.push_back(scene->mMeshes[i]->mFaces[f].mIndices[ind] + indexBase);
				}
			}
		}

		TempModelData data{};
		data.Vertices = Vertices;
		data.Indices = indices;

		return data;
	}

	ModelData* ContentManager::CreateModel(const std::vector<Vertex>& vertices, const std::vector<vertexIndex_t>& indices) {
		ModelData* model = new ModelData();

		//Vertex buffer.
		{
			size_t size = vertices.size() * sizeof(Vertex);
			VulkanBuffer stagingBuffer;
			renderer->CreateBuffer(stagingBuffer, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			void* data;
			vkMapMemory(renderer->LogicalDevice, stagingBuffer.Memory, 0, size, 0, &data);
			memcpy(data, vertices.data(), size);
			vkUnmapMemory(renderer->LogicalDevice, stagingBuffer.Memory);

			renderer->CreateBuffer(model->VertexBuffer, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			renderer->CopyBuffer(stagingBuffer, model->VertexBuffer, size);

			renderer->DestroyBuffer(stagingBuffer);
		}
		//Index buffer.
		{
			size_t size = indices.size() * sizeof(vertexIndex_t);
			VulkanBuffer stagingBuffer;
			renderer->CreateBuffer(stagingBuffer, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			void* data;
			vkMapMemory(renderer->LogicalDevice, stagingBuffer.Memory, 0, size, 0, &data);
			memcpy(data, indices.data(), size);
			vkUnmapMemory(renderer->LogicalDevice, stagingBuffer.Memory);

			renderer->CreateBuffer(model->IndexBuffer, size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			renderer->CopyBuffer(stagingBuffer, model->IndexBuffer, size);

			renderer->DestroyBuffer(stagingBuffer);

			model->IndicesCount = indices.size();
		}

		ModelDatas.push_back(model);

		return ModelDatas[ModelDatas.size() - 1];
	}

	ModelTexture* ContentManager::LoadModelTexture(const std::string& rootPath) {
		if (ModelTextureFromPath.find(rootPath) != ModelTextureFromPath.end())
			return ModelTextureFromPath[rootPath];

		ModelTexture* loadedTexture = new ModelTexture(renderer->LogicalDevice);

		//Albedo.
		{
			int width;
			int height;
			int nChannels;
			uint32_t mipLevels;

			std::string path = rootPath + "/td.png";
			stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &nChannels, STBI_rgb_alpha);
			VkDeviceSize size = (VkDeviceSize)width * (VkDeviceSize)height * (VkDeviceSize)nChannels;
			loadedTexture->sizeX = width;
			loadedTexture->sizeY = height;

			mipLevels = getMaxMipLevels(width, height);

			VulkanBuffer stagingBuffer;
			renderer->CreateBuffer(stagingBuffer, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			void* data;
			vkMapMemory(renderer->LogicalDevice, stagingBuffer.Memory, 0, size, 0, &data);
			memcpy(data, pixels, static_cast<size_t>(size));
			vkUnmapMemory(renderer->LogicalDevice, stagingBuffer.Memory);

			stbi_image_free(pixels);
			VulkanImageGen::CreateImage(&loadedTexture->Albedo, { (uint32_t)width, (uint32_t)height }, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, mipLevels);

			VulkanImageGen::TransitionImageLayout(&loadedTexture->Albedo, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels, 1);
			VulkanImageGen::CopyBufferToImage(&stagingBuffer, &loadedTexture->Albedo, width, height);
			VulkanImageGen::CreateMipmaps(loadedTexture->Albedo, { loadedTexture->sizeX, loadedTexture->sizeY }, mipLevels);
			VulkanImageGen::CreateImageSampler(loadedTexture->Albedo, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, mipLevels);

			renderer->DestroyBuffer(stagingBuffer);

			VulkanImageGen::CreateImageView(&loadedTexture->Albedo, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, mipLevels);
		}
		//Specular
		{
			int width;
			int height;
			int nChannels;
			uint32_t mipLevels;

			std::string path = rootPath + "/ts.png";
			stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &nChannels, STBI_rgb_alpha);
			VkDeviceSize size = (VkDeviceSize)width * (VkDeviceSize)height * (VkDeviceSize)nChannels;
			loadedTexture->sizeX = width;
			loadedTexture->sizeY = height;

			mipLevels = getMaxMipLevels(width, height);

			VulkanBuffer stagingBuffer;
			renderer->CreateBuffer(stagingBuffer, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			void* data;
			vkMapMemory(renderer->LogicalDevice, stagingBuffer.Memory, 0, size, 0, &data);
			memcpy(data, pixels, static_cast<size_t>(size));
			vkUnmapMemory(renderer->LogicalDevice, stagingBuffer.Memory);

			stbi_image_free(pixels);
			VulkanImageGen::CreateImage(&loadedTexture->Specular, { (uint32_t)width, (uint32_t)height }, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, mipLevels);

			VulkanImageGen::TransitionImageLayout(&loadedTexture->Specular, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels, 1);
			VulkanImageGen::CopyBufferToImage(&stagingBuffer, &loadedTexture->Specular, width, height);
			VulkanImageGen::CreateMipmaps(loadedTexture->Specular, { loadedTexture->sizeX, loadedTexture->sizeY }, mipLevels);
			VulkanImageGen::CreateImageSampler(loadedTexture->Specular, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, mipLevels);

			renderer->DestroyBuffer(stagingBuffer);

			VulkanImageGen::CreateImageView(&loadedTexture->Specular, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, mipLevels);
		}
		renderer->CreateNewPhongDescriptorSet(loadedTexture, loadedTexture->Albedo.Sampler, loadedTexture->Specular.Sampler);

		ModelTextures.push_back(loadedTexture);
		ModelTextureFromPath[rootPath] = ModelTextures.back();

		return ModelTextureFromPath[rootPath];
	}

	SkyboxTexture* ContentManager::LoadSkyboxTexture(const std::string& path) {
		if (SkyboxTextureFromPath.find(path) != SkyboxTextureFromPath.end())
			return SkyboxTextureFromPath[path];

		SkyboxTexture* texture = new SkyboxTexture();

		ktxResult result;
		ktxTexture* ktxtexture;

		result = ktxTexture_CreateFromNamedFile(path.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktxtexture);
		if (result != KTX_SUCCESS) {
			Logger::Log(LogMessageLevels::BAD_ERROR, "cargar imagen: " + path + "; code: " + std::to_string(result));
			return nullptr;
		}

		uint32_t width = ktxtexture->baseWidth;
		uint32_t height = ktxtexture->baseHeight;
		uint32_t levels = ktxtexture->numLevels;

		ktx_uint8_t* ktxtexturedata = ktxTexture_GetData(ktxtexture);
		ktx_size_t size = ktxTexture_GetDataSize(ktxtexture);

		VulkanBuffer stagingBuffer;
		renderer->CreateBuffer(stagingBuffer, size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		void* data;
		vkMapMemory(renderer->LogicalDevice, stagingBuffer.Memory, 0, size, 0, &data);
		memcpy(data, ktxtexturedata, static_cast<size_t>(size));
		vkUnmapMemory(renderer->LogicalDevice, stagingBuffer.Memory);

		VulkanImageGen::CreateImage(&texture->texture, { width, height }, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 6, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, levels);

		VkCommandBuffer copyCmd = renderer->beginSingleTimeCommandBuffer();
		std::vector<VkBufferImageCopy> bufferCopyRegions;
		uint32_t offset = 0;
		for (uint32_t face = 0; face < 6; face++) {
			for (uint32_t level = 0; level < levels; level++) {
				ktx_size_t offset;
				KTX_error_code ret = ktxTexture_GetImageOffset(ktxtexture, level, 0, face, &offset);
				assert(ret == KTX_SUCCESS);

				VkBufferImageCopy bufferCopyRegion = {};
				bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				bufferCopyRegion.imageSubresource.mipLevel = level;
				bufferCopyRegion.imageSubresource.baseArrayLayer = face;
				bufferCopyRegion.imageSubresource.layerCount = 1;
				bufferCopyRegion.imageExtent.width = ktxtexture->baseWidth >> level;
				bufferCopyRegion.imageExtent.height = ktxtexture->baseHeight >> level;
				bufferCopyRegion.imageExtent.depth = 1;
				bufferCopyRegion.bufferOffset = offset;
				bufferCopyRegions.push_back(bufferCopyRegion);
			}
		}

		VulkanImageGen::TransitionImageLayout(&texture->texture, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, levels, 6);
		vkCmdCopyBufferToImage(copyCmd, stagingBuffer.Buffer, texture->texture.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, bufferCopyRegions.size(), bufferCopyRegions.data());
		renderer->endSingleTimeCommandBuffer(copyCmd);
		VulkanImageGen::TransitionImageLayout(&texture->texture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, levels, 6);


		renderer->DestroyBuffer(stagingBuffer);

		VulkanImageGen::CreateImageView(&texture->texture, VK_FORMAT_R8G8B8A8_UNORM, 1U, VK_IMAGE_VIEW_TYPE_CUBE, 6, levels);

		renderer->CreateNewSkyboxDescriptorSet(texture);

		ktxTexture_Destroy(ktxtexture);

		SkyboxTextures.push_back(texture);
		SkyboxTextureFromPath[path] = texture;
		
		return SkyboxTextureFromPath[path];
	}

	void ContentManager::LoadSkybox(Skybox& skybox, const std::string& path) {
		skybox.texture = LoadSkyboxTexture(path);
	}

	void ContentManager::LoadModel(Model& model, const std::string& path) {
		model.Data = LoadModelData(path);
		auto direct = path.substr(0, path.find_last_of('/'));
		model.texture = LoadModelTexture(direct);
	}

	void ContentManager::LoadAnimatedModel(AnimatedModel& model, const std::string& path) {
		TempModelData modelData = GetModelTempData(path);

		model.scene = GlobalImporter.ReadFile(path.c_str(), AssimpFlags);
		model.SetAnimation(0);

		model.GlobalInverseTransform = model.scene->mRootNode->mTransformation;
		model.GlobalInverseTransform.Inverse();
		model.Bones.resize(modelData.Vertices.size());

		uint32_t vertexBase = 0;
		for (uint32_t m = 0; m < model.scene->mNumMeshes; m++) {
			const aiMesh* mesh = model.scene->mMeshes[m];
			for (uint32_t i = 0; i < mesh->mNumBones; i++) {
				uint32_t index = 0;
				std::string name(mesh->mBones[i]->mName.data);

				if (model.BoneMapping.find(name) == model.BoneMapping.end()) {
					index = model.NumBones;
					model.NumBones++;
					BoneInfo bone;
					model.BoneInfos.push_back(bone);
					model.BoneInfos[index].Offset = mesh->mBones[i]->mOffsetMatrix;
					model.BoneMapping[name] = index;
				}
				else {
					index = model.BoneMapping[name];
				}

				for (uint32_t w = 0; w < mesh->mBones[i]->mNumWeights; w++) {
					//CHANGED
					uint32_t vertexID = vertexBase + mesh->mBones[i]->mWeights[w].mVertexId;
					model.Bones[vertexID].Add(index, mesh->mBones[i]->mWeights[w].mWeight);
					//std::cout << "ADDED BONE DATA FOR VERTEX: " << vertexID << " W: " << mesh->mBones[i]->mWeights[w].mWeight << std::endl;
				}
			}
			vertexBase += mesh->mNumVertices;
		}
		model.BoneTransforms.resize(model.NumBones);

		for (uint32_t i = 0; i < modelData.Vertices.size(); i++) {
			for (uint32_t b = 0; b < OSK_ANIM_MAX_BONES_PER_VERTEX; b++) {
				modelData.Vertices[i].BondeIDs[b] = model.Bones[i].IDs[b];
				modelData.Vertices[i].BoneWeights[b] = model.Bones[i].Weights[b];
			}
		}

		model.Data = CreateModel(modelData.Vertices, modelData.Indices);
		model.SetAnimation(0);

		model.Update(0);
	}

	void ContentManager::LoadSprite(Sprite& sprite, const std::string& path) {
		sprite.texture = LoadTexture(path);

		renderer->createSpriteVertexBuffer(&sprite);
		renderer->createSpriteIndexBuffer(&sprite);

		Sprites.push_back(&sprite);
	}

	void ContentManager::LoadFont(Font& fuente, const std::string& source, uint32_t size) {
		static FT_Library ftLib = nullptr;
		if (ftLib == nullptr)
			if (FT_Init_FreeType(&ftLib))
				throw std::runtime_error("ERROR: cargar fuente.");

		FT_Face face;

		FT_Error result = FT_New_Face(ftLib, source.c_str(), 0, &face);;
		if (result)
			Logger::Log(LogMessageLevels::BAD_ERROR, "cargar fuente " + source, __LINE__);

		FT_Set_Pixel_Sizes(face, 0, size);

		FT_Face lastFace = nullptr;
		uint8_t* data = nullptr;
		size_t numberOfPixels = 0;
		for (uint8_t c = 0; c < 255; c++) {
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				Logger::Log(LogMessageLevels::BAD_ERROR, "freetype: load glyph: " + std::to_string(c) + " de " + source, __LINE__);
				continue;
			}

			if (face->glyph->bitmap.width <= 0 || face->glyph->bitmap.rows <= 0) {
				FontChar character{};
				character.sprite.texture = nullptr;
				character.Size = Vector2(lastFace->glyph->bitmap.width, lastFace->glyph->bitmap.rows);
				character.Bearing = Vector2(lastFace->glyph->bitmap_left, lastFace->glyph->bitmap_top);
				character.Advance = lastFace->glyph->advance.x;

				fuente.Characters[c] = character;

				continue;
			}
			numberOfPixels += face->glyph->bitmap.width * face->glyph->bitmap.rows;
			Texture* texture = new Texture();
			VULKAN::VulkanImage image = VulkanImageGen::CreateImageFromBitMap(face->glyph->bitmap.width, face->glyph->bitmap.rows, face->glyph->bitmap.buffer);
			texture->Albedo = image;
			renderer->createDescriptorSets(texture);

			Textures.push_back(texture);

			FontChar character{};
			character.sprite.texture = Textures.back();

			renderer->createSpriteVertexBuffer(&character.sprite);
			renderer->createSpriteIndexBuffer(&character.sprite);

			character.Size = Vector2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
			character.Bearing = Vector2(face->glyph->bitmap_left, face->glyph->bitmap_top);
			character.Advance = face->glyph->advance.x;

			fuente.Characters[c] = character;

			lastFace = face;

			Sprites.push_back(&fuente.Characters[c].sprite);
		}

		FT_Done_Face(face);

		fuente.Size = size;
	}

	void ContentManager::LoadHeightmap(Heightmap& map, const std::string& path) {
		int width;
		int height;
		int nChannels;

		stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &nChannels, STBI_grey);
		VkDeviceSize size = (VkDeviceSize)width * (VkDeviceSize)height * (VkDeviceSize)nChannels;
		map.Size.X = width;
		map.Size.Y = height;

		map.Data = new uint8_t[width * height];
		memcpy(map.Data, pixels, sizeof(uint8_t) * width * height);

		stbi_image_free(pixels);
	}

}