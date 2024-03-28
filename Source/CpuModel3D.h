#pragma once

#include "OSKmacros.h"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Color.hpp"
#include "DynamicArray.hpp"

#include "Animator.h"

#include "UniquePtr.hpp"
#include "IGpuImage.h"

#include <array>
#include <optional>


namespace OSK {

	/// @brief Vértice con los atributos que pueden ser cargados
	/// de un archivo.
	struct CpuVertex3D {
		std::optional<Vector3f> position;
		std::optional<Vector2f> uv = Vector2f::Zero;
		std::optional<Vector3f> tangent = Vector3f::Zero;
		std::optional<Vector3f> normal = Vector3f::Zero;
		std::optional<Color> color = Color::White;
		std::optional<std::array<float, 4>> boneWeights;
		std::optional<std::array<float, 4>> boneIds;
	};


	/// @brief Malla de vértices que comparten un material en común.
	/// Es posible que en realidad no tenga un material.
	/// 
	/// Puede contener una combinación de triángulos, líneas y/o puntos
	/// sueltos.
	class OSKAPI_CALL CpuMesh3D {

	public:

		/// @brief Índices de tres vértices que conforman un triángulo.
		using TriangleIndices = std::array<UIndex64, 3>;

		/// @brief Índices de dos vértices que conforman una línea.
		using LineIndices = std::array<UIndex64, 2>;

		/// @brief Índice de un punto aislado.
		using PointIndex = UIndex64;

	public:

		/// @brief Añade un único vértice.
		/// @param vertex Nuevo vértice a añadir.
		void AddVertex(CpuVertex3D vertex);

		/// @brief Añade una lista de vértices.
		/// @param vertices Lista de vértices.
		void AddVertices(const DynamicArray<CpuVertex3D>& vertices);


		/// @brief Registra una lista de triángulos, cada uno representado por los
		/// índices correspondientes.
		/// @param indices Lista con los índices de tres en tres.
		/// 
		/// @pre Cada índice debe ser menor al número de vértices registrados.
		void AddTriangleIndices(const DynamicArray<TriangleIndices>& indices);

		/// @brief Registra una lista de líneas, cada una representada por los
		/// índices correspondientes.
		/// @param indices Lista con los índices de dos en dos.
		/// 
		/// @pre Cada índice debe ser menor al número de vértices registrados.
		void AddLineIndices(const DynamicArray<LineIndices>& indices);

		/// @brief Registra una lista de puntos aislados, representado por
		/// los índices que ocupan en la lista de vértices.
		/// @param indices Lista con los índices.
		/// 
		/// @pre Cada índice debe ser menor al número de vértices registrados.
		void AddPointIndices(const DynamicArray<PointIndex>& indices);


		/// @brief Registra un triángulo, representado por sus tres índices.
		/// @param indices Índices del triángulo.
		/// 
		/// @pre Cada índice debe ser menor al número de vértices registrados.
		void AddTriangleIndex(TriangleIndices indices);

		/// @brief Registra una línea, representada por sus dos índices.
		/// @param indices Índices de la línea.
		/// 
		/// @pre Cada índice debe ser menor al número de vértices registrados.
		void AddLineIndex(LineIndices indices);

		/// @brief Registra un punto aislado, representado por su índice.
		/// @param indices Índice del punto.
		/// 
		/// @pre El índice debe ser menor al número de vértices registrados.
		void AddPointIndex(PointIndex indices);


		/// @returns True si contiene al menos un triángulo.
		bool HasTriangles() const;

		/// @returns True si contiene al menos una línea.
		bool HasLines() const;

		/// @returns True si contiene al menos un punto aislado.
		bool HasPoints() const;


		/// @returns Número de triángulos registrados. 
		USize64 GetTriangleCount() const;

		/// @returns Número de líneas registradas.
		USize64 GetLineCount() const;

		/// @returns Número de puntos aislados registrados.
		USize64 GetPointCount() const;


		/// @returns Lista con los índices de todos los triángulos registrados.
		const DynamicArray<TriangleIndices>& GetTriangles() const;

		/// @returns Lista con los índices de todas las líneas registrados.
		const DynamicArray<LineIndices>& GetLines() const;

		/// @returns Lista con los índices de todos los puntos aislados registrados.
		const DynamicArray<PointIndex>& GetPoints() const;


		/// @returns Lista con todos los vértices.
		const DynamicArray<CpuVertex3D>& GetVertices() const;


		/// @brief Establece el índice del material.
		/// @param index Índice del material, respecto a la lista de materiales del modelo
		/// que posee esta malla.
		/// 
		/// @pre @p index Debe ser menor que el número de materiales registrados en 
		/// el modelo que posee esta malla.
		void SetMaterialIndex(UIndex64 index);

		/// @returns True si tiene asociado un material.
		bool HasMaterial() const;

		/// @returns Índice del material, respecto a la lista de materiales del modelo
		/// que posee esta malla.
		/// 
		/// @pre @c HasMaterial() debe devolver true.
		UIndex64 GetMaterialIndex() const;

	private:

		DynamicArray<CpuVertex3D> m_vertices;

		DynamicArray<TriangleIndices> m_triangles;
		DynamicArray<LineIndices> m_lines;
		DynamicArray<PointIndex> m_points;

		std::optional<UIndex64> m_materialIndex;

	};


	/// @brief Modelo almacenado en memoria RAM.
	class OSKAPI_CALL CpuModel3D {

	public:

		/// @returns Lista con todas las mallas del modelo.
		const DynamicArray<CpuMesh3D>& GetMeshes() const;


		/// @brief Añade una lista de mallas al modelo.
		/// @param meshes Lista de mallas a añadir.
		void AddMeshes(const DynamicArray<CpuMesh3D>& meshes);

		/// @brief Añade una lista de animaciones al modelo.
		/// @param animations lista de animaciones a añadir.
		void AddAnimations(const DynamicArray<GRAPHICS::Animation>& animations);

		/// @brief Añade una lista de skins de animaciones al modelo.
		/// @param animations lista de skins de animaciones a añadir.
		void AddAnimationSkins(const DynamicArray<GRAPHICS::AnimationSkin>& skins);


		/// @returns Lista con todas las animaciones del modelo.
		const DynamicArray<GRAPHICS::Animation>& GetAnimations() const;

		/// @returns Lista con todas las skins de animaciones del modelo.
		const DynamicArray<GRAPHICS::AnimationSkin>& GetAnimationSkins() const;

	private:

		DynamicArray<CpuMesh3D> m_meshes;

		DynamicArray<GRAPHICS::Animation> m_animations;
		DynamicArray<GRAPHICS::AnimationSkin> m_animationSkins;

	};

}
