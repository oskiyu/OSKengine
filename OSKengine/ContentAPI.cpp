#include "ContentAPI.h"

#ifndef OSK_LOAD_XD
#define STB_IMAGE_IMPLEMENTATION
#include "stbi_image.h"
#endif

#include <al.h>
#include <assimp/matrix4x4.h>

#include <fstream>
#include <sstream>
#include <iostream>

namespace OSK {

	ContentAPI::ContentAPI(const std::string& path) {
		Directory = path;
	}


	ContentAPI::~ContentAPI() {

	}


	void ContentAPI::LoadModel(Model& model, const std::string& path) {

		if (model.IsLoaded) {
			model.Meshes.clear();
		}

		if (model.IsLoaded) {
			model.Meshes.clear();
		}

		auto direct = path.substr(0, path.find_last_of('/'));
		LoadTexture(model.Diffuse, direct + "/td");
		LoadTexture(model.Specular, direct + "/ts");

		static Assimp::Importer importer;

		auto scene = importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs);
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
			Logger::Log(LogMessageLevels::BAD_ERROR, "cargar modelo " + path, __LINE__);
			return;
		}

		std::vector<aiNode*> nodes;
		addNode(scene->mRootNode, nodes);

		std::vector<aiNodeAnim*> nodesAnim;
		if (scene->mNumAnimations > 0) {
			for (int32_t i = 0; i < scene->mAnimations[0]->mNumChannels; i++) {
				nodesAnim.push_back(scene->mAnimations[0]->mChannels[i]);
			}
		}

		for (uint32_t i = 0; i < scene->mNumMeshes; i++) {
			for (uint32_t b = 0; b < scene->mMeshes[i]->mNumBones; b++) {
				model.Skeletal = true;

				auto aibone = scene->mMeshes[i]->mBones[b];
				std::string boneName = aibone->mName.data;

				auto from = aibone->mOffsetMatrix;
				glm::mat4 to;

				to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
				to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
				to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
				to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;

				Transform transform = Transform(to);

				Bone bone = Bone(model.Skeleton.Bones.size(), boneName);
				bone.Transform = transform;

				for (uint32_t j = 0; j < nodes.size(); j++) {
					if (nodes[j]->mName.data == boneName) {
						bone.AINode = nodes[j];
					}
				}

				for (uint32_t j = 0; j < nodesAnim.size(); j++) {
					if (nodesAnim[j]->mNodeName.data == boneName) {
						bone.AINodeAnim = nodesAnim[j];
					}
				}

				model.Skeleton.Bones.push_back(bone);
			}
		}

		if (model.Skeletal) {
			for (uint32_t i = 0; i < model.Skeleton.Bones.size(); i++) {
				if (model.Skeleton.Bones[i].AINode == nullptr) {
					Logger::Log(LogMessageLevels::BAD_ERROR, "nulltpr: AINode: " + std::to_string(i), __LINE__);
					continue;
				}
				if (model.Skeleton.Bones[i].AINodeAnim == nullptr) {
					Logger::Log(LogMessageLevels::BAD_ERROR, "AINodeAnim: " + std::to_string(i), __LINE__);
					continue;
				}

				auto parentName = model.Skeleton.Bones[i].AINode->mParent->mName.data;
				for (uint32_t n = 0; n < model.Skeleton.Bones.size(); n++) {
					if (model.Skeleton.Bones[n].Name == parentName) {
						model.Skeleton.Bones[i].Transform.AttachTo(&model.Skeleton.Bones[n].Transform);
						break;
					}
				}
			}
		}

		processMeshes(scene->mRootNode, scene, model);

		if (model.Skeletal) {
			model.Skeleton.Update(0.0f);
		}


		model.IsLoaded = true;

		Logger::Log(LogMessageLevels::INFO, "Modelo cargado: " + path);
	}


	void ContentAPI::LoadTexture(Texture& texture, const std::string& path) {
		unsigned int ID;
		glGenTextures(1, &ID);

		int sizeX;
		int sizeY;
		int nrComp;
		unsigned char* data;
#ifndef OSK_LOAD_XD
		data = stbi_load((path + ".png").c_str(), &sizeX, &sizeY, &nrComp, 0);
#endif	
#ifdef OSK_LOAD_XD
		std::ifstream ifile(std::string(path + ".xd").c_str(), std::ios::binary);
		ifile.read(reinterpret_cast<char*>(&nrComp), sizeof(nrComp));
		ifile.read(reinterpret_cast<char*>(&sizeX), sizeof(sizeX));
		ifile.read(reinterpret_cast<char*>(&sizeY), sizeof(sizeY));
		data = new unsigned char[sizeX * sizeY * nrComp];
		ifile.read(reinterpret_cast<char*>(&data[0]), nrComp * sizeX * sizeY);
		ifile.close();
#endif
		if (data) {
#ifdef OSK_SAVE_XD
			if (ExportAsXd) {
				std::ofstream ofile(std::string(path + ".xd").c_str(), std::ios::binary);
				ofile.write(reinterpret_cast<const char*>(&nrComp), sizeof(nrComp));
				ofile.write(reinterpret_cast<const char*>(&sizeX), sizeof(sizeX));
				ofile.write(reinterpret_cast<const char*>(&sizeY), sizeof(sizeY));
				ofile.write(reinterpret_cast<char*>(&data[0]), nrComp * sizeX * sizeY);
				ofile.close();
			}
#endif
			GLenum format;
			if (nrComp == 1)
				format = GL_RED;
			else if (nrComp == 3)
				format = GL_RGB;
			else if (nrComp == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, ID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, sizeX, sizeY, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#ifndef OSK_LOAD_XD
		stbi_image_free(data);
#endif
#ifdef OSK_USE_INFO_LOGS
			Logger::Log(LogMessageLevels::INFO, "Textura cargada: " + path);
#endif
			texture.ID = ID;
			texture.Size = Vector2(sizeX, sizeY);
			texture.IsLoaded = true;
		}
		else {
			Logger::Log(LogMessageLevels::INFO, "texture: " + path, __LINE__);
#ifndef OSK_LOAD_XD
			stbi_image_free(data);
#endif
		}
	}


	void ContentAPI::LoadSkybox(Skybox& skybox, const std::array<std::string, 6> faces) {
		glGenTextures(1, &skybox.ID);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.ID);

		const int xd = 8;
		int nrComp;
		int sizeX;
		int sizeY;
		for (uint32_t i = 0; i < faces.size(); i++) {
			unsigned char* data;
#ifndef OSK_LOAD_XD
			data = stbi_load((faces[i] + ".jpg").c_str(), &sizeX, &sizeY, &nrComp, 0);
#endif
#ifdef OSK_LOAD_XD
			std::ifstream ifile((faces[i] + ".xd").c_str(), std::ios::binary);
			ifile.read(reinterpret_cast<char*>(&nrComp), sizeof(nrComp));
			ifile.read(reinterpret_cast<char*>(&sizeX), sizeof(sizeX));
			ifile.read(reinterpret_cast<char*>(&sizeY), sizeof(sizeY));
			data = new unsigned char[sizeX * sizeY * nrComp];
			ifile.read(reinterpret_cast<char*>(&data[0]), sizeX * sizeY * nrComp);
			ifile.close();
#endif
			if (data) {
#ifdef OSK_SAVE_XD
				if (ExportAsXd) {
					std::ofstream ofile((faces[i] + ".xd").c_str(), std::ios::binary);
					ofile.write(reinterpret_cast<char*>(&nrComp), sizeof(nrComp));
					ofile.write(reinterpret_cast<char*>(&sizeX), sizeof(sizeX));
					ofile.write(reinterpret_cast<char*>(&sizeY), sizeof(sizeY));
					ofile.write(reinterpret_cast<char*>(&data[0]), nrComp * sizeX * sizeY);
					ofile.close();
				}
#endif
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, sizeX, sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				skybox.IsLoaded = true;
			}
			else {
				Logger::Log(LogMessageLevels::BAD_ERROR, "skybox: " + faces[i], __LINE__);
			}
#ifndef OSK_LOAD_XD
		stbi_image_free(data);
#endif
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}


	void ContentAPI::LoadShaderFromFile(Shader& shader, const std::string& vertexPath, const std::string& fragmentPath) {
		std::string vertexCode;
		std::string fragmentCode;

		std::ifstream vShaderFile;
		std::ifstream fShaderFile;

		std::stringstream vShaderStream;
		std::stringstream fShaderStream;
		try {
			vShaderFile.open(vertexPath + ".vs");
			fShaderFile.open(fragmentPath + ".fs");

			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();

			vShaderFile.close();
			fShaderFile.close();

			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}

		catch (std::ifstream::failure e) {
			Logger::Log(LogMessageLevels::BAD_ERROR, "Shader(): ", __LINE__);
			return;
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		unsigned int vertex;
		unsigned int fragment;

		int success;
		char infoLog[512];

		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);

		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			Logger::Log(LogMessageLevels::BAD_ERROR, "vertex shader compile: " + std::string(vertexPath) + "/" + infoLog, __LINE__);
		}

		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);

		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			Logger::Log(LogMessageLevels::BAD_ERROR, "shader compile: " + std::string(fragmentPath) + "/" + infoLog, __LINE__);
		}

		shader.ProgramID = glCreateProgram();
		glAttachShader(shader.ProgramID, vertex);
		glAttachShader(shader.ProgramID, fragment);
		glLinkProgram(shader.ProgramID);

		glGetProgramiv(shader.ProgramID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader.ProgramID, 512, NULL, infoLog);
			Logger::Log(LogMessageLevels::BAD_ERROR, "shader program linking: " + std::string(infoLog), __LINE__);
		}

		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}


	void ContentAPI::LoadShaderFromSourceCode(OSK::Shader& shader, const std::string& vertexCode, const std::string& fragmentCode) {
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		unsigned int vertex;
		unsigned int fragment;

		int success;
		char infoLog[512];

		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);

		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			Logger::Log(LogMessageLevels::BAD_ERROR, "vertex shader compile: " + std::string(infoLog), __LINE__);
		}

		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);

		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			Logger::Log(LogMessageLevels::BAD_ERROR, "fragment shader compile: " + std::string(infoLog), __LINE__);
		}

		shader.ProgramID = glCreateProgram();
		glAttachShader(shader.ProgramID, vertex);
		glAttachShader(shader.ProgramID, fragment);
		glLinkProgram(shader.ProgramID);

		glGetProgramiv(shader.ProgramID, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(shader.ProgramID, 512, NULL, infoLog);
			Logger::Log(LogMessageLevels::BAD_ERROR, "shader program linking: " + std::string(infoLog), __LINE__);
		}

		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}


	void ContentAPI::LoadSound(SoundEntity& audio, const std::string& source) {
		
		std::ifstream file(source.c_str(), std::ifstream::binary);

		if (!file.is_open()) {
			Logger::Log(LogMessageLevels::BAD_ERROR, "abrir " + source, __LINE__);
			return;
		}

		char chunkId[5] = "\0";
		uint32_t size = 0;
		file.read(chunkId, 4);
		file.read((char*)& size, 4);

		chunkId[4] = '\0';

		file.read(chunkId, 4);

		chunkId[4] = '\0';

		file.read(chunkId, 4);
		file.read((char*)& size, 4);

		short formatTag = 0;
		short channels = 0;
		int samplesPerSec = 0;
		int averageBytesPerSec = 0;
		short blockAlign = 0;
		short bitsPerSample = 0;

		file.read((char*)& formatTag, 2);
		file.read((char*)& channels, 2);
		file.read((char*)& samplesPerSec, 4);
		file.read((char*)& averageBytesPerSec, 4);
		file.read((char*)& blockAlign, 2);
		file.read((char*)& bitsPerSample, 2);

		if (size > 16) {
			file.seekg((int)file.tellg() + (size - 16));
		}

		file.read(chunkId, 4);
		file.read((char*)& size, 4);

		chunkId[4] = '\0';

		unsigned char* data = new unsigned char[size];

		file.read((char*)data, size);

		//Crear el audio.
		alGenBuffers(1, &audio.BufferID);
		alBufferData(audio.BufferID, (channels == 2) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, data, size, samplesPerSec);

		alGenSources(1, &audio.SourceID);
		alSourcei(audio.SourceID, AL_BUFFER, audio.BufferID);

		delete[] data;
		data = NULL;
		
		file.close();

#ifdef OSK_USE_INFO_LOGS
		Logger::Log(LogMessageLevels::INFO, "audio cargado: " + source);
#endif
	}


	void ContentAPI::LoadFont(Font& fuente, const std::string& source, const int32_t size) {
		static FT_Library ftLibrary = nullptr;
		if (ftLibrary == nullptr) {
			if (FT_Init_FreeType(&ftLibrary)) {
				Logger::Log(LogMessageLevels::BAD_ERROR, "iniciar ft", __LINE__);
			}
		}

		FT_Face face;

		FT_Error result = FT_New_Face(ftLibrary, source.c_str(), 0, &face);
		if (result) {
			Logger::Log(LogMessageLevels::BAD_ERROR, "cargar fuente " + source, __LINE__);
			return;
		}

		FT_Set_Pixel_Sizes(face, 0, size);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		for (GLubyte c = 0; c < 255; c++) {

			if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
				Logger::Log(LogMessageLevels::BAD_ERROR, "freetype: load glyph: " + std::to_string(c) + " de " + source, __LINE__);
				continue;
			}

			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			FontChar ch = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			fuente.Characters.insert(std::pair<GLuint, FontChar>(c, ch));
		}
		glBindTexture(GL_TEXTURE_2D, 0);

		FT_Done_Face(face);

		fuente.Size = size;
		fuente.IsLoaded = true;
	}


	void ContentAPI::processNode(aiNode* node, const aiScene* scene, Model& model) {
		/*for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			for (uint32_t j = 0; j < mesh->mNumBones; j++) {
				model.Skeletal = true;

				aiBone* aibone = mesh->mBones[j];
				std::string boneName = aibone->mName.data;
				
				auto from = aibone->mOffsetMatrix;
				glm::mat4 to;
				
				to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
				to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
				to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
				to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
				
				OSK::Transform transform = OSK::Transform(to);

				OSK::Bone bone = OSK::Bone(model.Skeleton.Bones.size(), boneName);
				bone.Transform = transform;

				for (uint32_t n = 0; n < scene->) {
					for (uint32_t a = 0; a < scene.n)
					bone.AINode = findAiNode(boneName);
					bone.AINodeAnim = findAiNodeAnim(boneName);
				}

				model.Skeleton.Bones.push_back(bone);
			}

			processMesh(mesh, scene, model);
		}
		// then do the same for each of its children
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			processNode(node->mChildren[i], scene, model);
		}*/
	}


	void ContentAPI::processMesh(aiMesh* mesh, const aiScene* scene, Model& model) {
		std::vector<Vertex> vertices;
		std::vector<vertexIndex_t> indices;

		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			Vertex vertex;
			Vector3 vector;

			if (mesh->mVertices != nullptr) {
				vector.X = mesh->mVertices[i].x;
				vector.Y = mesh->mVertices[i].y;
				vector.Z = mesh->mVertices[i].z;
			}
			else {
				Logger::Log(LogMessageLevels::WARNING, "vértices", __LINE__);
				vector = Vector3(0.0f);
			}
			vertex.Position = vector;

			if (mesh->mNormals != nullptr) {
				vector.X = mesh->mNormals[i].x;
				vector.Y = mesh->mNormals[i].y;
				vector.Z = mesh->mNormals[i].z;
			}
			else {
				Logger::Log(LogMessageLevels::WARNING, "normales", __LINE__);
				vector = Vector3(0.0f);
			}
			vertex.Normal = vector;

			if (mesh->mTextureCoords[0]) {
				Vector2 vec;
				vec.X = mesh->mTextureCoords[0][i].x * 1;
				vec.Y = mesh->mTextureCoords[0][i].y * 1;
				vertex.TexCoords = vec;
			}
			else {
				vertex.TexCoords = Vector2(0.0f, 0.0f);
			}

			vertices.push_back(vertex);
		}

		if (model.Skeletal) {
			for (uint32_t i = 0; i < mesh->mNumBones; i++) {
				auto aibone = mesh->mBones[i];

				for (uint32_t j = 0; j < aibone->mNumWeights; j++) {
					aiVertexWeight vertexWeight = aibone->mWeights[j];

					int32_t startVertexID = vertexWeight.mVertexId;

					for (uint32_t k = 0; k < OSK::__VERTEX::MAX_BONES_AMOUNT; k++) {
						if (vertices[startVertexID].BoneWeights[k] == 0.0f) {
							for (uint32_t l = 0; l < model.Skeleton.Bones.size(); l++) {
								if (model.Skeleton.Bones[l].Name == aibone->mName.data) {
									vertices[startVertexID].BoneID[k] = model.Skeleton.Bones[l].ID;
								}
							}

							vertices[startVertexID].BoneWeights[k] = vertexWeight.mWeight;

							break;
						}

						if (k >= OSK::__VERTEX::MAX_BONES_AMOUNT - 1) {
							break;
						}
					}
				}
			}
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			aiFace face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}

		Mesh oskMesh = { vertices, indices };
		model.Meshes.push_back(oskMesh);
	}


	void ContentAPI::addNode(aiNode* node, std::vector<aiNode*>& nodes) {
		nodes.push_back(node);

		for (uint32_t i = 0; i < node->mNumChildren; i++) {
			addNode(node->mChildren[i], nodes);
		}
	}


	void ContentAPI::processMeshes(aiNode* node, const aiScene* scene, Model& model) {
		for (uint32_t i = 0; i < node->mNumMeshes; i++) {
			processMesh(scene->mMeshes[node->mMeshes[i]], scene, model);
		}

		for (uint32_t i = 0; i < node->mNumChildren; i++) {
			processMeshes(node->mChildren[i], scene, model);
		}
	}
	
}