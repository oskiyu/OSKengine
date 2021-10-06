#include "ContentManager.h"

#include "VulkanImageGen.h"
#include "VulkanRenderer.h"

#include <gtc\type_ptr.hpp>
#include "stbi_image.h"

#include "ToString.h"
#include "GPUImage.h"
#include "FileIO.h"

#include <al.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stbi_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stbi_image_write.h"

#include "MaterialSlot.h"

using namespace OSK::VULKAN;
using namespace OSK::Animation;

namespace OSK {

	Assimp::Importer ContentManager::globalImporter;
	int ContentManager::assimpFlags = aiProcess_Triangulate | aiProcess_GenNormals;

	const std::string ContentManager::DEFAULT_TEXTURE_PATH = "assets/engine/default_texture/default_texture.png";
	Texture* ContentManager::DefaultTexture = nullptr;

	ContentManager::ContentManager(RenderAPI* renderer) {
		this->renderer = renderer;

		textures = {};
		modelDatas = {};
		sprites = {};

		textureFromPath = {};
		modelDataFromPath = {};
	}

	ContentManager::~ContentManager() {
		Unload();
	}

	void ContentManager::Unload() {
		for (auto& i : textures)
			i.Delete();

		for (auto& i : modelDatas)
			i.Delete();

		for (auto& i : sounds)
			i.Delete();

		textures.clear();
		modelDatas.clear();
		sprites.clear();
		fonts.clear();
		sounds.clear();

		textureFromPath.clear();
		modelDataFromPath.clear();
		fontsFromPath.clear();
		soundsFromPath.clear();
	}

	Texture* ContentManager::LoadTexture(const std::string& path, TextureFilterType filter) {
		if (textureFromPath.find(path) != textureFromPath.end())
			return textureFromPath[path];

		OSK_ASSERT(FileIO::FileExists(path), "No existe la textura " + path + ".");

		Texture* loadedTexture = new Texture();
		loadedTexture->image = new GpuImage;

		int width;
		int height;
		int nChannels = 0;

		stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &nChannels, nChannels);
		VkDeviceSize size = (VkDeviceSize)width * (VkDeviceSize)height * 4;
		loadedTexture->size.X = (uint32_t)width;
		loadedTexture->size.Y = (uint32_t)height;

		SharedPtr<GpuDataBuffer> stagingBuffer = new GpuDataBuffer;
		renderer->AllocateBuffer(stagingBuffer.GetPointer(), size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		std::vector<uint8_t> finalPixels;
		finalPixels.reserve(size);

		for (size_t i = 0; i < width * height * nChannels; i += nChannels) {
			for (uint8_t p = 0; p < nChannels; p++)
				finalPixels.push_back(pixels[i + p]);

			for (uint8_t p = nChannels; p < 4; p++)
				finalPixels.push_back(0);
		}

		stagingBuffer->Write(finalPixels.data(), size);

		stbi_image_free(pixels);

		uint32_t mipLevels = GetMaxMipLevels(width, height);

		VULKAN::VulkanImageGen::CreateImage(loadedTexture->image.GetPointer(), { (uint32_t)width, (uint32_t)height }, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, mipLevels, VK_SAMPLE_COUNT_1_BIT);

		VULKAN::VulkanImageGen::TransitionImageLayout(loadedTexture->image.GetPointer(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels, 1);
		VULKAN::VulkanImageGen::CopyBufferToImage(stagingBuffer.GetPointer(), loadedTexture->image.GetPointer(), width, height);
		VULKAN::VulkanImageGen::CreateMipmaps(loadedTexture->image.Get(), { loadedTexture->size.X, loadedTexture->size.Y }, mipLevels);

		VkFilter vFilter = VK_FILTER_LINEAR;
		if (filter == TextureFilterType::NEAREST)
			vFilter = VK_FILTER_NEAREST;

		VULKAN::VulkanImageGen::CreateImageSampler(loadedTexture->image.Get(), vFilter, VK_SAMPLER_ADDRESS_MODE_REPEAT, mipLevels);
		VULKAN::VulkanImageGen::CreateImageView(loadedTexture->image.GetPointer(), VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, mipLevels);
		VULKAN::VulkanImageGen::TransitionImageLayout(loadedTexture->image.GetPointer(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels, 1);

		textures.push_back(loadedTexture);
		textureFromPath[path] = textures.back().GetPointer();

		return textureFromPath[path];
	}

	void ContentManager::CreateTextureFromBitmap(Texture* texture, Bitmap bitmap) {
		OSK_ASSERT(texture, "'Textura' es null.");

		texture->image = VULKAN::VulkanImageGen::CreateImageFromBitMap(bitmap);
		texture->size = bitmap.size;

		textures.push_back(texture);
	}

	Texture* ContentManager::LoadSkyboxTexture(const std::string& folderPath) {
		if (textureFromPath.find(folderPath) != textureFromPath.end())
			return textureFromPath[folderPath];

		Texture* texture = new Texture();
		texture->image = new GpuImage;
		
		const static std::vector<std::string> facesOrder = {
			"/right.jpg",
			"/left.jpg",
			"/top.jpg",
			"/bottom.jpg",
			"/front.jpg",
			"/back.jpg"
		};

		int width;
		int height;
		int nChannels;

		stbi_uc* totalImage = nullptr;

		stbi_uc* pixels = stbi_load((folderPath + facesOrder[0]).c_str(), &width, &height, &nChannels, STBI_rgb_alpha);
		nChannels = 4;
		VkDeviceSize size = (VkDeviceSize)width * (VkDeviceSize)height * (VkDeviceSize)nChannels;

		totalImage = new stbi_uc[size * 6];

		memcpy(totalImage, pixels, size);
		stbi_image_free(pixels);

		for (uint32_t i = 1; i < 6; i++) {
			OSK_ASSERT(FileIO::FileExists(folderPath + facesOrder[i]), "No existe la textura " + (folderPath + facesOrder[i]) + ".");

			stbi_uc* _pixels = stbi_load((folderPath + facesOrder[i]).c_str(), &width, &height, &nChannels, STBI_rgb_alpha);
			memcpy(totalImage + size * i, _pixels, size);
			stbi_image_free(_pixels);
		}

		SharedPtr<GpuDataBuffer> stagingBuffer = new GpuDataBuffer;
		renderer->AllocateBuffer(stagingBuffer.GetPointer(), size * 7, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		stagingBuffer->Write(totalImage, size * 6);

		const uint32_t levels = 1;
		VULKAN::VulkanImageGen::CreateImage(texture->image.GetPointer(), { (uint32_t)width, (uint32_t)height }, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 6, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, levels, VK_SAMPLE_COUNT_1_BIT);

		VkCommandBuffer copyCmd = renderer->beginSingleTimeCommandBuffer();
		VkDeviceSize offset = stagingBuffer->memorySubblock->totalOffsetFromBlock;
		std::vector<VkBufferImageCopy> bufferCopyRegions;
		for (uint32_t face = 0; face < 6; face++) {
			VkBufferImageCopy bufferCopyRegion = {};
			bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			bufferCopyRegion.imageSubresource.mipLevel = 0;
			bufferCopyRegion.imageSubresource.baseArrayLayer = face;
			bufferCopyRegion.imageSubresource.layerCount = 1;
			bufferCopyRegion.imageExtent.width = (uint32_t)width;
			bufferCopyRegion.imageExtent.height = (uint32_t)height;
			bufferCopyRegion.imageExtent.depth = 1;
			bufferCopyRegion.bufferOffset = offset;
			bufferCopyRegions.push_back(bufferCopyRegion);

			offset += size;
		}

		VULKAN::VulkanImageGen::TransitionImageLayout(texture->image.GetPointer(), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, levels, 6);
		vkCmdCopyBufferToImage(copyCmd, stagingBuffer->memorySubblock->vkBuffer, texture->image->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, (uint32_t)bufferCopyRegions.size(), bufferCopyRegions.data());
		renderer->endSingleTimeCommandBuffer(copyCmd);
		VULKAN::VulkanImageGen::TransitionImageLayout(texture->image.GetPointer(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, levels, 6);

		VULKAN::VulkanImageGen::CreateImageView(texture->image.GetPointer(), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_CUBE, 6, levels);
		VULKAN::VulkanImageGen::CreateImageSampler(texture->image.Get(), VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, 1);

		delete[] totalImage;

		textures.push_back(texture);
		textureFromPath[folderPath] = texture;

		return textureFromPath[folderPath];
	}

	ModelData* ContentManager::LoadModelData(const std::string& path) {
		if (modelDataFromPath.find(path) != modelDataFromPath.end())
			return modelDataFromPath.at(path);

		OSK_ASSERT(FileIO::FileExists(path), "No existe el modelo en " + path + ".");

		TempModelData data = GetModelTempData(path, 1.0f);

		ModelData* m = CreateModel(data.vertices, data.indices);
		modelDataFromPath[path] = m;

		return m;
	}

	void ContentManager::ProcessMeshNode(const aiNode* node, const aiScene* scene, std::vector<Vertex>* vertices, std::vector<vertexIndex_t>* indices, const Vector3f& prevPosition, float scale) const {
		aiVector3D pos; 
		aiQuaternion rot;
		aiVector3D scl;
		node->mTransformation.Decompose(scl, rot, pos);

		auto matrix = AnimatedModel::AiToGLM(node->mTransformation);

		Vector3f position = prevPosition + glm::make_vec3(&pos.x);

		position = prevPosition + glm::vec3(matrix[3]);

		for (uint32_t i = 0; i < node->mNumMeshes; i++) {

			const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			for (uint32_t v = 0; v < mesh->mNumVertices; v++) {
				Vertex vertex{};
				glm::vec3 vec3 = glm::make_vec3(&mesh->mVertices[v].x) * scale;

				vertex.position = vec3 + position.ToGLM();

				vertex.normals.x = mesh->mNormals[v].x;
				vertex.normals.y = -mesh->mNormals[v].y;
				vertex.normals.z = mesh->mNormals[v].z;

				vertex.textureCoordinates.x = mesh->mTextureCoords[0][v].x;
				vertex.textureCoordinates.y = -mesh->mTextureCoords[0][v].y;

				if (mesh->HasVertexColors(0))
					vertex.color = glm::make_vec3(&mesh->mColors[0][v].r);
				else
					vertex.color = Color::WHITE().ToGLM();

				vertex.position.y *= -1.0f;

				vertices->push_back(vertex);
			}
		}

		for (uint32_t i = 0; i < node->mNumMeshes; i++) {
			uint32_t indexBase = static_cast<uint32_t>(indices->size());

			const aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			//Vertices.
			for (uint32_t f = 0; f < mesh->mNumFaces; f++) {
				for (uint32_t ind = 0; ind < 3; ind++) {
					indices->push_back(mesh->mFaces[f].mIndices[ind] + indexBase);
				}
			}
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++)
			ProcessMeshNode(node->mChildren[i], scene, vertices, indices, position, scale);
	}

	TempModelData ContentManager::GetModelTempData(const std::string& path, float scale) const {
		const aiScene* scene;

		OSK_ASSERT(FileIO::FileExists(path), "No existe el modelo en " + path + ".");

		scene = globalImporter.ReadFile(path.c_str(), assimpFlags);

		std::vector<Vertex> vertices;
		std::vector<vertexIndex_t> indices;

		ProcessMeshNode(scene->mRootNode, scene, &vertices, &indices, { 0 }, scale);

		//Meshes.
		/*for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
			//Vertices.
			scene->mRootNode->mTransformation.Decompose();
			for (uint32_t v = 0; v < scene->mMeshes[i]->mNumVertices; v++) {
				Vertex vertex{};
				glm::vec3 vec3 = glm::make_vec3(&scene->mMeshes[i]->mVertices[v].x) * scale;
				vertex.position = vec3;

				vertex.normals.x = scene->mMeshes[i]->mNormals[v].x;
				vertex.normals.y = -scene->mMeshes[i]->mNormals[v].y;
				vertex.normals.z = scene->mMeshes[i]->mNormals[v].z;

				vertex.textureCoordinates = glm::make_vec2(&scene->mMeshes[i]->mTextureCoords[0][v].x);

				if (scene->mMeshes[i]->HasVertexColors(0))
					vertex.color = glm::make_vec3(&scene->mMeshes[i]->mColors[0][v].r);
				else
					vertex.color = Color::WHITE().ToGLM();

				vertex.position.y *= -1.0f;

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
		}*/

		TempModelData data{};
		data.vertices = vertices;
		data.indices = indices;

		return data;
	}

	ModelData* ContentManager::CreateModel(const std::vector<Vertex>& vertices, const std::vector<vertexIndex_t>& indices) {
		ModelData* model = new ModelData();

		//Vertex buffer.
		{
			size_t size = vertices.size() * sizeof(Vertex);
			SharedPtr<GpuDataBuffer> stagingBuffer = new GpuDataBuffer;
			renderer->AllocateBuffer(stagingBuffer.GetPointer(), size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			stagingBuffer->Write(vertices.data(), size);

			model->vertexBuffer = new GpuDataBuffer;
			renderer->AllocateBuffer(model->vertexBuffer.GetPointer(), size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			
			renderer->CopyBuffer(stagingBuffer.Get(), model->vertexBuffer.Get(), size);
		}
		//Index buffer.
		{
			size_t size = indices.size() * sizeof(vertexIndex_t);
			SharedPtr<GpuDataBuffer> stagingBuffer = new GpuDataBuffer;
			renderer->AllocateBuffer(stagingBuffer.GetPointer(), size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			stagingBuffer->Write(indices.data(), size);

			model->indexBuffer = new GpuDataBuffer;
			renderer->AllocateBuffer(model->indexBuffer.GetPointer(), size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			
			renderer->CopyBuffer(stagingBuffer.Get(), model->indexBuffer.Get(), size);

			model->indicesCount = indices.size();
		}

		modelDatas.push_back(model);

		return model;
	}

	void ContentManager::LoadSkybox(Skybox* skybox, const std::string& path) {
		OSK_ASSERT(skybox, "'Skybox' es null.");

		skybox->instance = renderer->GetMaterialSystem()->GetMaterial(MPIPE_SKYBOX)->CreateInstance().GetPointer();
		skybox->instance->GetMaterialSlot(MSLOT_SKYBOX_CAMERA)->SetBuffer("Camera", renderer->GetDefaultCamera()->GetUniformBuffer());
		skybox->instance->GetMaterialSlot(MSLOT_SKYBOX_TEXTURE)->SetTexture("Cubemap", LoadSkyboxTexture(path));
		skybox->instance->FlushUpdate();
	}

	void ContentManager::LoadModel(Model* model, const std::string& path) {
		OSK_ASSERT(FileIO::FileExists(path), "No existe el modelo en " + path + ".");
		OSK_ASSERT(model, "'Model' es null.");

		model->data = LoadModelData(path);
		auto direct = path.substr(0, path.find_last_of('/'));

		model->logicalDevice = renderer->logicalDevice;
	}

	void ContentManager::LoadAnimatedModel(AnimatedModel* model, const std::string& path) {
		static uint32_t animationCount = 1;

		OSK_ASSERT(FileIO::FileExists(path), "No existe el modelo en " + path + ".");
		OSK_ASSERT(model, "'Model' es null.");

		TempModelData modelData = GetModelTempData(path);

		auto scene = globalImporter.ReadFile(path.c_str(), assimpFlags);

		model->globalInverseTransform = AnimatedModel::AiToGLM(scene->mRootNode->mTransformation);
		model->globalInverseTransform = glm::inverse(model->globalInverseTransform);
		model->bones.resize(modelData.vertices.size());

		int numberOfAnimations = 0;
		size_t numberOfBones = 0;

		uint32_t vertexBase = 0;
		for (uint32_t m = 0; m < scene->mNumMeshes; m++) {
			const aiMesh* mesh = scene->mMeshes[m];
			for (uint32_t i = 0; i < mesh->mNumBones; i++) {
				uint32_t index = 0;
				std::string name(mesh->mBones[i]->mName.data);

				if (model->boneMapping.find(name) == model->boneMapping.end()) {
					index = numberOfBones;
					numberOfBones++;
					BoneInfo bone;
					model->boneInfos.push_back(bone);
					model->boneInfos[index].offset = AnimatedModel::AiToGLM(mesh->mBones[i]->mOffsetMatrix);
					model->boneMapping[name] = index;
				}
				else {
					index = model->boneMapping[name];
				}

				for (uint32_t w = 0; w < mesh->mBones[i]->mNumWeights; w++) {
					uint32_t vertexID = vertexBase + mesh->mBones[i]->mWeights[w].mVertexId;
					model->bones[vertexID].Add(index, mesh->mBones[i]->mWeights[w].mWeight);
				}
			}
			vertexBase += mesh->mNumVertices;
		}

		for (uint32_t i = 0; i < modelData.vertices.size(); i++) {
			for (uint32_t b = 0; b < OSK_ANIM_MAX_BONES_PER_VERTEX; b++) {
				modelData.vertices[i].bondeIDs[b] = model->bones[i].IDs[b];
				modelData.vertices[i].boneWeights[b] = model->bones[i].weights[b];
			}
		}

		for (uint32_t i = 0; i < scene->mNumAnimations; i++) {
			auto aiAnim = scene->mAnimations[i];

			Animation::SAnimation animation;
			animation.name = aiAnim->mName.C_Str();
			animation.duration = (deltaTime_t)aiAnim->mDuration;
			animation.boneChannels.reserve(aiAnim->mNumChannels);
			animation.ticksPerSecond = (deltaTime_t)aiAnim->mTicksPerSecond;

			for (uint32_t ch = 0; ch < aiAnim->mNumChannels; ch++) {
				auto channel = aiAnim->mChannels[ch];

				Animation::SNodeAnim node;
				node.name = channel->mNodeName.C_Str();

				node.positionKeys.reserve(channel->mNumPositionKeys);
				node.rotationKeys.reserve(channel->mNumRotationKeys);
				node.scalingKeys.reserve(channel->mNumScalingKeys);
				
				for (uint32_t key = 0; key < channel->mNumPositionKeys; key++) {
					Animation::SVectorKey newKey;
					newKey.time = (deltaTime_t)channel->mPositionKeys[key].mTime;
					Vector3f vec{ channel->mPositionKeys[key].mValue.x, channel->mPositionKeys[key].mValue.y, channel->mPositionKeys[key].mValue.z };
					newKey.value = vec;
					node.positionKeys.push_back(newKey);
				}

				for (uint32_t key = 0; key < channel->mNumRotationKeys; key++) {
					Animation::SQuaternionKey newKey;
					newKey.time = (deltaTime_t)channel->mRotationKeys[key].mTime;
					Quaternion quat;
					quat.Quat.x = channel->mRotationKeys[key].mValue.x;
					quat.Quat.y = channel->mRotationKeys[key].mValue.y;
					quat.Quat.z = channel->mRotationKeys[key].mValue.z;
					quat.Quat.w = channel->mRotationKeys[key].mValue.w;
					newKey.value = quat;
					node.rotationKeys.push_back(newKey);
				}

				for (uint32_t key = 0; key < channel->mNumScalingKeys; key++) {
					Animation::SVectorKey newKey;
					newKey.time = (deltaTime_t)channel->mScalingKeys[key].mTime;
					Vector3f vec{ channel->mScalingKeys[key].mValue.x, channel->mScalingKeys[key].mValue.y, channel->mScalingKeys[key].mValue.z };
					newKey.value = vec;
					node.scalingKeys.push_back(newKey);
				}

				animation.boneChannels.push_back(node);
			}

			model->animations.push_back(animation);
		}

		numberOfAnimations = scene->mNumAnimations;

		model->data = CreateModel(modelData.vertices, modelData.indices);
		model->rootNode = GetNodes(scene->mRootNode);
		model->SetAnimation(0);
		model->SetupAnimationIndices();
		
		model->Update(0);

		model->logicalDevice = renderer->logicalDevice;
		model->animationBufferOffset = animationCount;

		animationCount++;
	}

	SNode ContentManager::GetNodes(const aiNode* node) {
		SNode newNode;
		newNode.name = node->mName.C_Str();
		newNode.matrix = AnimatedModel::AiToGLM(node->mTransformation);
		newNode.children.resize(node->mNumChildren);
		for (uint32_t i = 0; i < node->mNumChildren; i++) {
			newNode.children.push_back(GetNodes(node->mChildren[i]));
		}

		return newNode;
	}

	void ContentManager::LoadSprite(Sprite* sprite, const std::string& path) {
		OSK_ASSERT(FileIO::FileExists(path), "No existe la textura en " + path + ".");
		OSK_ASSERT(sprite, "'Sprite' es null.");

		CreateSprite(sprite);
		sprite->texture = LoadTexture(path);
		sprite->material = renderer->GetMaterialSystem()->GetMaterial(MPIPE_2D)->CreateInstance().GetPointer();
		sprite->UpdateMaterialTexture();
		sprite->material->GetMaterialSlot(MSLOT_TEXTURE_2D)->FlushUpdate();
	}

	void ContentManager::CreateSprite(Sprite* sprite) {
		OSK_ASSERT(sprite, "'Sprite' es null.");

		renderer->createSpriteVertexBuffer(sprite);
		sprites.push_back(sprite);
	}

	Font* ContentManager::LoadFont(const std::string& source, uint32_t size) {
		OSK_ASSERT(FileIO::FileExists(source), "No existe la fuente en " + source + ".");

		if (fontsFromPath.find(source) != fontsFromPath.end())
			return fontsFromPath[source];

		Font* fuente = new Font();

		static FT_Library ftLib = nullptr;
		if (ftLib == nullptr)
			if (FT_Init_FreeType(&ftLib))
				throw std::runtime_error("ERROR: cargar fuente.");

		FT_Face face;

		FT_Error result = FT_New_Face(ftLib, source.c_str(), 0, &face);
		if (result)
			Logger::Log(LogMessageLevels::BAD_ERROR, "cargar fuente " + source, __LINE__);

		FT_Set_Pixel_Sizes(face, 0, size);

		FT_Face lastFace = nullptr;

		uint32_t numberOfPixels = 0;

		struct __face {
			uint8_t* data = nullptr;
			uint32_t sizeX = 0;
			uint32_t sizeY = 0;

			uint32_t left = 0;
			uint32_t top = 0;

			uint32_t advanceX = 0;
		};

		//Font info.
		Texture* fontTexture = new Texture();
		uint32_t textureSizeX = 0;
		uint32_t textureSizeY = 0;
		
		__face faces[255];

		for (uint32_t c = 0; c < 255; c++) {
			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				Logger::Log(LogMessageLevels::BAD_ERROR, "freetype: load glyph: " + std::to_string(c) + " de " + source, __LINE__);
				continue;
			}

			faces[c].sizeX = face->glyph->bitmap.width;
			faces[c].sizeY = face->glyph->bitmap.rows;

			faces[c].left = face->glyph->bitmap_left;
			faces[c].top = face->glyph->bitmap_top;

			faces[c].advanceX = face->glyph->advance.x;

			faces[c].data = new uint8_t[faces[c].sizeX * faces[c].sizeY];

			memcpy(faces[c].data, face->glyph->bitmap.buffer, faces[c].sizeX * faces[c].sizeY);
		}

		for (uint32_t c = 0; c < 255; c++) {
			textureSizeX += faces[c].sizeX;

			if (faces[c].sizeY > textureSizeY)
				textureSizeY = faces[c].sizeY;
		}

		Bitmap bitmap;
		bitmap.format = TextureFormat::INTERNAL_FONT;
		bitmap.size = { textureSizeX, textureSizeY };
		
		numberOfPixels = textureSizeX * textureSizeY;
		
		bitmap.bytes.resize(numberOfPixels);

		memset(bitmap.bytes.data(), 0, numberOfPixels);

		uint32_t currentX = 0;
		for (uint32_t c = 0; c < 255; c++) {
			for (uint32_t i = 0; i < faces[c].sizeY; i++) {
				memcpy(&bitmap.bytes[currentX + textureSizeX * i], &faces[c].data[faces[c].sizeX * i], faces[c].sizeX);
			}

			currentX += faces[c].sizeX;
		}

		SharedPtr<VULKAN::GpuImage> image = VULKAN::VulkanImageGen::CreateImageFromBitMap(bitmap);

		fontTexture->size.X = textureSizeX;
		fontTexture->size.Y = textureSizeY;
		fontTexture->image = image;
		fontTexture->image->sampler = renderer->globalImageSampler;

		textures.push_back(fontTexture);

		fuente->fontMaterial = renderer->GetMaterialSystem()->GetMaterial(MPIPE_2D)->CreateInstance().GetPointer();
		fuente->fontMaterial->GetMaterialSlot(MSLOT_TEXTURE_2D)->SetTexture("Texture",fontTexture);
		fuente->fontMaterial->GetMaterialSlot(MSLOT_TEXTURE_2D)->FlushUpdate();

		currentX = 0;
		for (uint32_t c = 0; c < 255; c++) {
			fuente->characters[c] = {};

			FontChar& character = fuente->characters.at(c);

			character.size = Vector2f((float)faces[c].sizeX, (float)faces[c].sizeY);
			character.bearing = Vector2f((float)faces[c].left, (float)faces[c].top);
			character.advance = faces[c].advanceX;
			
			fuente->characters[c].sprite.texture = fontTexture;
			fuente->characters[c].sprite.SetTexCoords(Vector4ui{ currentX, 0, faces[c].sizeX, faces[c].sizeY }.ToVector4f());

			renderer->createSpriteVertexBuffer(&fuente->characters[c].sprite);

			fuente->characters[c].sprite.material = fuente->fontMaterial;

			sprites.push_back(&fuente->characters[c].sprite);

			currentX += faces[c].sizeX;
		}

		for (uint32_t i = 0; i < 255; i++)
			delete[] faces[i].data;
	
		FT_Done_Face(face);

		fuente->size = size;

		fontsFromPath[source] = fuente;
		fonts.push_back(fuente);

		return fuente;
	}

	void ContentManager::LoadHeightmap(Heightmap* map, const std::string& path) {
		OSK_ASSERT(map, "'Map' es null.");
		OSK_ASSERT(FileIO::FileExists(path), "No existe el heightmap " + path + ".");

		int width;
		int height;
		int nChannels;

		stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &nChannels, STBI_grey);
		VkDeviceSize size = (VkDeviceSize)width * (VkDeviceSize)height * (VkDeviceSize)nChannels;
		map->size.X = width;
		map->size.Y = height;

		map->data = new uint8_t[width * height];
		memcpy(map->data.GetPointer(), pixels, sizeof(uint8_t) * width * height);

		stbi_image_free(pixels);
	}

	SoundEmitterComponent* ContentManager::LoadSoundEntity(const std::string& path) {
		if(soundsFromPath.find(path) != soundsFromPath.end())
			return soundsFromPath[path];

		OSK_ASSERT(FileIO::FileExists(path), "No existe el sonido " + path + ".");

		SoundEmitterComponent* sound = new SoundEmitterComponent();

		std::ifstream file(path.c_str(), std::ifstream::binary);

		char chunkId[5] = "\0";
		uint32_t size = 0;
		file.read(chunkId, 4);
		file.read((char*)&size, 4);

		chunkId[4] = '\0';

		file.read(chunkId, 4);

		chunkId[4] = '\0';

		file.read(chunkId, 4);
		file.read((char*)&size, 4);

		short formatTag = 0;
		short channels = 0;
		int samplesPerSec = 0;
		int averageBytesPerSec = 0;
		short blockAlign = 0;
		short bitsPerSample = 0;

		file.read((char*)&formatTag, 2);
		file.read((char*)&channels, 2);
		file.read((char*)&samplesPerSec, 4);
		file.read((char*)&averageBytesPerSec, 4);
		file.read((char*)&blockAlign, 2);
		file.read((char*)&bitsPerSample, 2);

		if (size > 16) {
			file.seekg((int)file.tellg() + (size - 16));
		}

		file.read(chunkId, 4);
		file.read((char*)&size, 4);

		chunkId[4] = '\0';

		unsigned char* data = new unsigned char[size];

		file.read((char*)data, size);

		//Crear el audio.
		alGenBuffers(1, &sound->bufferID);
		alBufferData(sound->bufferID, (channels == 2) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, data, size, samplesPerSec);

		alGenSources(1, &sound->sourceID);
		alSourcei(sound->sourceID, AL_BUFFER, sound->bufferID);

		delete[] data;
		data = NULL;

		file.close();

		sounds.push_back(sound);
		soundsFromPath[path] = sound;

		return sound;
	}

}
