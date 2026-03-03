#pragma once

#include "ApiCall.h"
#include <string_view>

#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Color.hpp"

#include "InvalidArgumentException.h"

#include <any>
#include "HashMap.hpp"
#include "Uuid.h"
#include "UniquePtr.hpp"


namespace OSK::GRAPHICS {

	using VertexAttribUuid = BaseUuid<class VertexAttributes>;

}

OSK_DEFINE_UUID_HASH(OSK::GRAPHICS::VertexAttribUuid);


/// @brief Define el identificador único para el atributo.
/// @param uniqueName Nombre único.
#define OSK_VERTEX_ATTRIB(uniqueName) \
static constexpr std::string_view GetAttribName() { return uniqueName; } \
static VertexAttribUuid GetUuid() { \
	const static VertexAttribUuid uuid = StaticUuidProvider::New<VertexAttribUuid>(); \
	return uuid; \
}


namespace OSK::GRAPHICS {

	struct IVertexAttribute {};

	/// @brief Concepto para comprobar que un tipo de atributo
	/// de vértices ha usado el macro OSK_VERTEX_ATTRIB.
	/// @tparam TAttrib tipo de atributo.
	template<typename TAttrib>
	concept IsVertexAttribute = requires (TAttrib) {
		{ TAttrib::GetAttribName() };
	};


	/// @brief Atributo que contiene la posición 3D del vértice.
	struct VertexPositionAttribute3D : public IVertexAttribute {
		OSK_VERTEX_ATTRIB("OSK::VertexPositionAttribute3D");

		explicit(false) VertexPositionAttribute3D(Vector3f position) : position(position.x, position.y, position.z, 1) {}

		Vector4f position;

	};

	/// @brief Resto de atributos de meshes PBR 3D:
	///	- Vector normal.
	/// - Color.
	/// - Coordenadas de texturas (UV).
	/// - Vector tangente.
	struct VertexAttributes3D : public IVertexAttribute {
		OSK_VERTEX_ATTRIB("OSK::VertexAttributes3D");

		explicit(false) VertexAttributes3D(
			Vector3f normal,
			Color color,
			Vector2f texCoords,
			Vector3f tangent
		) :
			normal(normal.x, normal.y, normal.z, 1),
			color(color),
			texCoords(texCoords),
			tangent(tangent.x, tangent.y, tangent.z, 1.0){ }
		


		Vector4f normal;
		Color color;
		Vector2f texCoords;
		Vector4f tangent;
	};

	/// @brief Atributos para modelos animados:
	/// - Índices de los huesos.
	/// - Pesos de los huesos.
	struct VertexAnimationAttributes3D : public IVertexAttribute {
		OSK_VERTEX_ATTRIB("OSK::VertexAnimationAttributes3D");

		explicit(false) VertexAnimationAttributes3D(
			Vector4f boneIndices,
			Vector4f boneWeights
		) :
			boneIndices(boneIndices), boneWeights(boneWeights) { }

		Vector4f boneIndices;
		Vector4f boneWeights;
	};


	struct VerticesAttributesMap {

		explicit(false) VerticesAttributesMap() {}

		explicit(false) VerticesAttributesMap(
			USize32 structSize,
			std::any list
		) noexcept :
			structSize(structSize), list(MakeUnique<std::any>(std::move(list)))
		{

		}

		USize32 vertexCount = 0;
		USize32 structSize = 0;

		UniquePtr<std::any> list;
		const void* listStart = nullptr;

		OSK_DISABLE_COPY(VerticesAttributesMap);
	};


	/// @brief Mapa con todos los atributos de todos los vértices
	/// de un modelo 3D.
	struct VerticesAttributesMaps {

		std::unordered_map<VertexAttribUuid, VerticesAttributesMap> attributes;


		/// @param uuid UUID del atributo.
		/// @return True si contiene la lista de atributos.
		bool HasAttribute(VertexAttribUuid uuid) const {
			return attributes.contains(uuid);
		}

		/// @brief Añade un atributo de vértice.
		/// @tparam TAttrib Tipo de atributo.
		/// @param attribute Atributo a añadir.
		template <typename TAttrib> requires IsVertexAttribute<TAttrib>
		void AddVertexAttribute(const TAttrib& attribute) {
			const auto uuid = TAttrib::GetUuid();

			if (!HasAttribute(uuid)) {
				attributes.try_emplace(
					uuid,
					sizeof(TAttrib),		  // .structSize
					DynamicArray<TAttrib>{}); // .list

				VerticesAttributesMap& attrib = attributes[uuid];
				attrib.listStart = std::any_cast<const DynamicArray<TAttrib>&>(attrib.list.GetValue()).GetData();
			}

			VerticesAttributesMap& attributeList = attributes[uuid];
			auto& castedList = std::any_cast<DynamicArray<TAttrib>&>(attributeList.list.GetValue());
			castedList.Insert(attribute);
			attributeList.vertexCount++;
			attributeList.listStart = castedList.GetData();
		}

		/// @brief Obtiene una lista de atributos en concreto.
		/// @tparam TAttrib Tipo de atributo.
		/// @return Lista con todos los atributos.
		/// 
		/// @pre Debe haberse añadido al menos un atributo del tipo dado.
		/// @throws InvalidArgumentException si no se cumple la precondición.
		template <typename TAttrib> requires IsVertexAttribute<TAttrib>
		const DynamicArray<TAttrib>& GetVerticesAttributesList() const {
			OSK_ASSERT(
				HasAttribute(TAttrib::GetUuid()), 
				InvalidArgumentException(std::format("No existe el atributo {}", TAttrib::GetAttribName())));

			return std::any_cast<const DynamicArray<TAttrib>&>(attributes.find(TAttrib::GetUuid())->second.list.GetValue());
		}

		VerticesAttributesMap& GetVerticesAttributes(VertexAttribUuid uuid) {
			return attributes.find(uuid)->second;
		}

		const VerticesAttributesMap& GetVerticesAttributes(VertexAttribUuid uuid) const {
			return attributes.find(uuid)->second;
		}

		template <typename TAttrib> requires IsVertexAttribute<TAttrib>
		const DynamicArray<TAttrib>& TGetVerticesAttributes() const {
			OSK_ASSERT(
				HasAttribute(TAttrib::GetUuid()),
				InvalidArgumentException(std::format("No existe el atributo {}", TAttrib::GetAttribName())));

			return std::any_cast<DynamicArray<TAttrib>>(GetVerticesAttributes(TAttrib::GetUuid()).list.GetValue());
		}
	};

}
