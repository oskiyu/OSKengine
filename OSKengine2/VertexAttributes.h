#pragma once

#include "OSKmacros.h"
#include <string_view>

#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Color.hpp"

#include "InvalidArgumentException.h"

#include <any>


/// @brief Define el identificador único para el atributo.
/// @param uniqueName Nombre único.
#define OSK_VERTEX_ATTRIB(uniqueName) \
static constexpr std::string_view GetAttribName() { return uniqueName; }


namespace OSK::GRAPHICS {

	/// @brief Concepto para comprobar que un tipo de atributo
	/// de vértices ha usado el macro OSK_VERTEX_ATTRIB.
	/// @tparam TAttrib tipo de atributo.
	template<typename TAttrib>
	concept IsVertexAttribute = requires (TAttrib) {
		{ TAttrib::GetAttribName() };
	};


	/// @brief Atributo que contiene la posición 3D del vértice.
	struct VertexPositionAttribute3D {
		OSK_VERTEX_ATTRIB("OSK::VertexPositionAttribute3D");

		Vector3f position;
	};

	/// @brief Resto de atributos de meshes PBR 3D:
	///	- Vector normal.
	/// - Color.
	/// - Coordenadas de texturas (UV).
	/// - Vector tangente.
	struct VertexAttributes3D {
		OSK_VERTEX_ATTRIB("OSK::VertexAttributes3D");

		Vector3f normal;
		Color color;
		Vector2f texCoords;
		Vector3f tangent;
	};

	/// @brief Atributos para modelos animados:
	/// - Índices de los huesos.
	/// - Pesos de los huesos.
	struct VertexAnimationAttributes3D {
		OSK_VERTEX_ATTRIB("OSK::VertexAnimationAttributes3D");

		Vector4f boneIndices;
		Vector4f boneWeights;
	};


	/// @brief Mapa de un atributo en concreto.
	/// Debe ser casteado a DynamicArray<TAttrib>
	using VerticesAttributesMap = std::any;


	/// @brief Mapa con todos los atributos de todos los vértices
	/// de un modelo 3D.
	struct VerticesAttributesMaps {

		std::unordered_map<std::string, VerticesAttributesMap, StringHasher, std::equal_to<>> attributes;


		/// @param attributeName Nombre único del atributo.
		/// @return True si contiene la lista de atributos.
		bool HasAttribute(std::string_view attributeName) const {
			return attributes.contains(attributeName);
		}

		/// @brief Añade un atributo de vértice.
		/// @tparam TAttrib Tipo de atributo.
		/// @param attribute Atributo a añadir.
		template <typename TAttrib> requires IsVertexAttribute<TAttrib>
		void AddVertexAttribute(const TAttrib& attribute) {
			if (!HasAttribute(TAttrib::GetAttribName())) {
				attributes[static_cast<std::string>(TAttrib::GetAttribName())] = DynamicArray<TAttrib>{};
			}

			auto& attributeList = attributes.find(TAttrib::GetAttribName())->second;
			std::any_cast<DynamicArray<TAttrib>>(attributeList).Insert(attribute);
		}

		/// @brief Obtiene una lista de atributos en concreto.
		/// @tparam TAttrib Tipo de atributo.
		/// @return Lista con todos los atributos.
		/// 
		/// @pre Debe haberse añadido al menos un atributo del tipo dado.
		/// @throws InvalidArgumentException si no se cumple la precondición.
		template <typename TAttrib> requires IsVertexAttribute<TAttrib>
		const DynamicArray<TAttrib>& GetVerticesAttributes() const {
			OSK_ASSERT(HasAttribute(TAttrib::GetAttribName()), InvalidArgumentException(std::format("No existe el atributo {}", TAttrib::GetAttribName())));

			return std::any_cast<DynamicArray<TAttrib>>(attributes.find(TAttrib::GetAttribName())->second);
		}
	};

}
