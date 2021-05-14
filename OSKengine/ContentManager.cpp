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

using namespace OSK::VULKAN;
using namespace OSK::Animation;

namespace OSK {

	Assimp::Importer ContentManager::globalImporter;

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
			delete i;

		for (auto& i : modelDatas)
			delete i;

		for (auto& i : sprites) {
			//i->VertexBuffer.Free();
		}

		for (auto& i : sounds)
			delete i;

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

	Texture* ContentManager::LoadTexture(const std::string& path) {
		if (textureFromPath.find(path) != textureFromPath.end())
			return textureFromPath[path];

		Texture* loadedTexture = new Texture();

		int width;
		int height;
		int nChannels;

		stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &nChannels, STBI_rgb_alpha);
		VkDeviceSize size = (VkDeviceSize)width * (VkDeviceSize)height * (VkDeviceSize)nChannels;
		loadedTexture->size.X = (uint32_t)width;
		loadedTexture->size.Y = (uint32_t)height;

		GPUDataBuffer stagingBuffer = renderer->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stagingBuffer.Allocate(size);

		stagingBuffer.Write(pixels, size);

		stbi_image_free(pixels);
		uint32_t mipLevels = GetMaxMipLevels(width, height);
		VULKAN::VulkanImageGen::CreateImage(&loadedTexture->image, { (uint32_t)width, (uint32_t)height }, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 1, (VkImageCreateFlagBits)0, mipLevels);

		VULKAN::VulkanImageGen::TransitionImageLayout(&loadedTexture->image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels, 1);
		VULKAN::VulkanImageGen::CopyBufferToImage(&stagingBuffer, &loadedTexture->image, width, height);
		VULKAN::VulkanImageGen::CreateMipmaps(loadedTexture->image, { loadedTexture->size.X, loadedTexture->size.Y }, mipLevels);
		VULKAN::VulkanImageGen::CreateImageSampler(loadedTexture->image, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, mipLevels);
		VULKAN::VulkanImageGen::TransitionImageLayout(&loadedTexture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels, 1);

		stagingBuffer.Free();

		VULKAN::VulkanImageGen::CreateImageView(&loadedTexture->image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, mipLevels);

		textures.push_back(loadedTexture);
		textureFromPath[path] = textures.back();

		return textureFromPath[path];
	}

	Texture* ContentManager::LoadSkyboxTexture(const std::string& folderPath) {
		if (textureFromPath.find(folderPath) != textureFromPath.end())
			return textureFromPath[folderPath];

		Texture* texture = new Texture();

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
			stbi_uc* _pixels = stbi_load((folderPath + facesOrder[i]).c_str(), &width, &height, &nChannels, STBI_rgb_alpha);
			memcpy(totalImage + size * i, _pixels, size);
			stbi_image_free(_pixels);
		}

		GPUDataBuffer stagingBuffer = renderer->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stagingBuffer.Allocate(size * 7);

		stagingBuffer.Write(totalImage, size * 6);

		const uint32_t levels = 1;
		VULKAN::VulkanImageGen::CreateImage(&texture->image, { (uint32_t)width, (uint32_t)height }, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 6, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT, levels);

		VkCommandBuffer copyCmd = renderer->beginSingleTimeCommandBuffer();
		uint32_t offset = 0;
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

		VULKAN::VulkanImageGen::TransitionImageLayout(&texture->image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, levels, 6);
		vkCmdCopyBufferToImage(copyCmd, stagingBuffer.buffer, texture->image.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, bufferCopyRegions.size(), bufferCopyRegions.data());
		renderer->endSingleTimeCommandBuffer(copyCmd);
		VULKAN::VulkanImageGen::TransitionImageLayout(&texture->image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, levels, 6);

		stagingBuffer.Free();

		VULKAN::VulkanImageGen::CreateImageView(&texture->image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_CUBE, 6, levels);
		VULKAN::VulkanImageGen::CreateImageSampler(texture->image, VK_FILTER_LINEAR, VK_SAMPLER_ADDRESS_MODE_REPEAT, 1);

		delete[] totalImage;

		textures.push_back(texture);
		textureFromPath[folderPath] = texture;

		return textureFromPath[folderPath];
	}

	ModelData* ContentManager::LoadModelData(const std::string& path) {
		if (modelDataFromPath.find(path) != modelDataFromPath.end())
			return modelDataFromPath.at(path);

		TempModelData data = GetModelTempData(path, 1.0f);

		ModelData* m = CreateModel(data.vertices, data.indices);
		modelDataFromPath[path] = m;

		return m;
	}

	TempModelData ContentManager::GetModelTempData(const std::string& path, float scale) const {
		const aiScene* scene;

		scene = globalImporter.ReadFile(path.c_str(), ASSIMP_FLAGS);

		std::vector<Vertex> Vertices;

		//Meshes.
		for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
			//Vertices.
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
		}

		TempModelData data{};
		data.vertices = Vertices;
		data.indices = indices;

		return data;
	}

	ModelData* ContentManager::CreateModel(const std::vector<Vertex>& vertices, const std::vector<vertexIndex_t>& indices) {
		ModelData* model = new ModelData();

		//Vertex buffer.
		{
			size_t size = vertices.size() * sizeof(Vertex);
			GPUDataBuffer stagingBuffer = renderer->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			stagingBuffer.Allocate(size);

			stagingBuffer.Write(vertices.data(), size);

			model->vertexBuffer = renderer->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			model->vertexBuffer.Allocate(size);
			
			renderer->CopyBuffer(stagingBuffer, model->vertexBuffer, size);

			stagingBuffer.Free();
		}
		//Index buffer.
		{
			size_t size = indices.size() * sizeof(vertexIndex_t);
			GPUDataBuffer stagingBuffer = renderer->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			stagingBuffer.Allocate(size);

			stagingBuffer.Write(indices.data(), size);

			model->indexBuffer = renderer->CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			model->indexBuffer.Allocate(size);
			
			renderer->CopyBuffer(stagingBuffer, model->indexBuffer, size);

			stagingBuffer.Free();

			model->indicesCount = indices.size();
		}

		modelDatas.push_back(model);

		return model;
	}

	void ContentManager::LoadSkybox(Skybox& skybox, const std::string& path) {
		skybox.instance = renderer->GetMaterialSystem()->GetMaterial(renderer->defaultSkyboxMaterial_Name)->CreateInstance();
		skybox.instance->SetBuffer("Camera", renderer->uniformBuffers);
		skybox.instance->SetTexture("Texture", LoadSkyboxTexture(path));
		skybox.instance->FlushUpdate();
	}

	void ContentManager::LoadModel(Model& model, const std::string& path) {
		model.data = LoadModelData(path);
		auto direct = path.substr(0, path.find_last_of('/'));

		model.logicalDevice = renderer->logicalDevice;
	}

	void ContentManager::LoadAnimatedModel(AnimatedModel& model, const std::string& path) {
		static uint32_t animationCount = 1;

		TempModelData modelData = GetModelTempData(path);

		auto scene = globalImporter.ReadFile(path.c_str(), ASSIMP_FLAGS);

		model.globalInverseTransform = AnimatedModel::AiToGLM(scene->mRootNode->mTransformation);
		model.globalInverseTransform = glm::inverse(model.globalInverseTransform);
		model.bones.resize(modelData.vertices.size());

		int numberOfAnimations = 0;

		uint32_t vertexBase = 0;
		for (uint32_t m = 0; m < scene->mNumMeshes; m++) {
			const aiMesh* mesh = scene->mMeshes[m];
			for (uint32_t i = 0; i < mesh->mNumBones; i++) {
				uint32_t index = 0;
				std::string name(mesh->mBones[i]->mName.data);

				if (model.boneMapping.find(name) == model.boneMapping.end()) {
					index = numberOfAnimations;
					numberOfAnimations++;
					BoneInfo bone;
					model.boneInfos.push_back(bone);
					model.boneInfos[index].offset = AnimatedModel::AiToGLM(mesh->mBones[i]->mOffsetMatrix);
					model.boneMapping[name] = index;
				}
				else {
					index = model.boneMapping[name];
				}

				for (uint32_t w = 0; w < mesh->mBones[i]->mNumWeights; w++) {
					uint32_t vertexID = vertexBase + mesh->mBones[i]->mWeights[w].mVertexId;
					model.bones[vertexID].Add(index, mesh->mBones[i]->mWeights[w].mWeight);
				}
			}
			vertexBase += mesh->mNumVertices;
		}

		for (uint32_t i = 0; i < modelData.vertices.size(); i++) {
			for (uint32_t b = 0; b < OSK_ANIM_MAX_BONES_PER_VERTEX; b++) {
				modelData.vertices[i].bondeIDs[b] = model.bones[i].IDs[b];
				modelData.vertices[i].boneWeights[b] = model.bones[i].weights[b];
			}
		}

		for (uint32_t i = 0; i < scene->mNumAnimations; i++) {
			auto aiAnim = scene->mAnimations[i];

			Animation::SAnimation animation;
			animation.name = aiAnim->mName.C_Str();
			animation.duration = aiAnim->mDuration;
			animation.boneChannels.reserve(aiAnim->mNumChannels);
			animation.ticksPerSecond = aiAnim->mTicksPerSecond;

			for (uint32_t ch = 0; ch < aiAnim->mNumChannels; ch++) {
				auto channel = aiAnim->mChannels[ch];

				Animation::SNodeAnim node;
				node.name = channel->mNodeName.C_Str();

				node.positionKeys.reserve(channel->mNumPositionKeys);
				node.rotationKeys.reserve(channel->mNumRotationKeys);
				node.scalingKeys.reserve(channel->mNumScalingKeys);
				
				for (uint32_t key = 0; key < channel->mNumPositionKeys; key++) {
					Animation::SVectorKey newKey;
					newKey.time = channel->mPositionKeys[key].mTime;
					Vector3f vec{ channel->mPositionKeys[key].mValue.x, channel->mPositionKeys[key].mValue.y, channel->mPositionKeys[key].mValue.z };
					newKey.value = vec;
					node.positionKeys.push_back(newKey);
				}

				for (uint32_t key = 0; key < channel->mNumRotationKeys; key++) {
					Animation::SQuaternionKey newKey;
					newKey.time = channel->mRotationKeys[key].mTime;
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
					newKey.time = channel->mScalingKeys[key].mTime;
					Vector3f vec{ channel->mScalingKeys[key].mValue.x, channel->mScalingKeys[key].mValue.y, channel->mScalingKeys[key].mValue.z };
					newKey.value = vec;
					node.scalingKeys.push_back(newKey);
				}

				animation.boneChannels.push_back(node);
			}

			model.animations.push_back(animation);
		}

		model.data = CreateModel(modelData.vertices, modelData.indices);
		model.rootNode = GetNodes(scene->mRootNode);
		model.SetAnimation(0);
		model.SetupAnimationIndices();
		
		model.Update(0);

		model.logicalDevice = renderer->logicalDevice;
		model.animationBufferOffset = animationCount;
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

	void ContentManager::LoadSprite(Sprite& sprite, const std::string& path) {
		CreateSprite(sprite);
		sprite.texture = LoadTexture(path);
		sprite.material = renderer->GetMaterialSystem()->GetMaterial(renderer->defaultMaterial2D_Name)->CreateInstance();
		sprite.UpdateMaterialTexture();
		sprite.material->FlushUpdate();
	}

	void ContentManager::CreateSprite(Sprite& sprite) {
		renderer->createSpriteVertexBuffer(&sprite);
		sprites.push_back(&sprite);
	}

	Font* ContentManager::LoadFont(const std::string& source, uint32_t size) {
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
			uint32_t sizeX;
			uint32_t sizeY;

			uint32_t left;
			uint32_t top;

			uint32_t advanceX;
		};

		//Font info.
		Texture* fontTexture = new Texture();
		uint32_t textureSizeX = 0;
		uint32_t textureSizeY = 0;
		uint8_t* data = nullptr;
		
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

		numberOfPixels = textureSizeX * textureSizeY;
		data = new uint8_t[numberOfPixels];
		memset(data, 0, numberOfPixels);

		uint32_t currentX = 0;
		for (uint32_t c = 0; c < 255; c++) {
			for (uint32_t i = 0; i < faces[c].sizeY; i++) {
				memcpy(&data[currentX + textureSizeX * i], &faces[c].data[faces[c].sizeX * i], faces[c].sizeX);
			}

			currentX += faces[c].sizeX;
		}

		VULKAN::GPUImage image = VULKAN::VulkanImageGen::CreateImageFromBitMap(textureSizeX, textureSizeY, data);

		fontTexture->size.X = textureSizeX;
		fontTexture->size.Y = textureSizeY;
		fontTexture->image = image;
		fontTexture->image.sampler = renderer->globalImageSampler;

		textures.push_back(fontTexture);

		fuente->fontMaterial = renderer->GetMaterialSystem()->GetMaterial(renderer->defaultMaterial2D_Name)->CreateInstance();
		fuente->fontMaterial->SetTexture(fontTexture);
		fuente->fontMaterial->FlushUpdate();

		currentX = 0;
		for (uint32_t c = 0; c < 255; c++) {
			fuente->characters[c] = {};

			FontChar& character = fuente->characters.at(c);

			character.size = Vector2(faces[c].sizeX, faces[c].sizeY);
			character.bearing = Vector2(faces[c].left, faces[c].top);
			character.advance = faces[c].advanceX;
			
			renderer->createSpriteVertexBuffer(&fuente->characters[c].sprite);

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

	void ContentManager::LoadHeightmap(Heightmap& map, const std::string& path) {
		int width;
		int height;
		int nChannels;

		stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &nChannels, STBI_grey);
		VkDeviceSize size = (VkDeviceSize)width * (VkDeviceSize)height * (VkDeviceSize)nChannels;
		map.size.X = width;
		map.size.Y = height;

		map.data = new uint8_t[width * height];
		memcpy(map.data, pixels, sizeof(uint8_t) * width * height);

		stbi_image_free(pixels);
	}

	SoundEmitterComponent* ContentManager::LoadSoundEntity(const std::string& path) {
		if(soundsFromPath.find(path) != soundsFromPath.end())
			return soundsFromPath[path];

		SoundEmitterComponent* sound = new SoundEmitterComponent();

		std::ifstream file(path.c_str(), std::ifstream::binary);

		if (!file.is_open())
			throw std::runtime_error("ERROR: el archivo " + path + "no existe.");

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