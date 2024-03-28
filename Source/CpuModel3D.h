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

	/// @brief V�rtice con los atributos que pueden ser cargados
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


	/// @brief Malla de v�rtices que comparten un material en com�n.
	/// Es posible que en realidad no tenga un material.
	/// 
	/// Puede contener una combinaci�n de tri�ngulos, l�neas y/o puntos
	/// sueltos.
	class OSKAPI_CALL CpuMesh3D {

	public:

		/// @brief �ndices de tres v�rtices que conforman un tri�ngulo.
		using TriangleIndices = std::array<UIndex64, 3>;

		/// @brief �ndices de dos v�rtices que conforman una l�nea.
		using LineIndices = std::array<UIndex64, 2>;

		/// @brief �ndice de un punto aislado.
		using PointIndex = UIndex64;

	public:

		/// @brief A�ade un �nico v�rtice.
		/// @param vertex Nuevo v�rtice a a�adir.
		void AddVertex(CpuVertex3D vertex);

		/// @brief A�ade una lista de v�rtices.
		/// @param vertices Lista de v�rtices.
		void AddVertices(const DynamicArray<CpuVertex3D>& vertices);


		/// @brief Registra una lista de tri�ngulos, cada uno representado por los
		/// �ndices correspondientes.
		/// @param indices Lista con los �ndices de tres en tres.
		/// 
		/// @pre Cada �ndice debe ser menor al n�mero de v�rtices registrados.
		void AddTriangleIndices(const DynamicArray<TriangleIndices>& indices);

		/// @brief Registra una lista de l�neas, cada una representada por los
		/// �ndices correspondientes.
		/// @param indices Lista con los �ndices de dos en dos.
		/// 
		/// @pre Cada �ndice debe ser menor al n�mero de v�rtices registrados.
		void AddLineIndices(const DynamicArray<LineIndices>& indices);

		/// @brief Registra una lista de puntos aislados, representado por
		/// los �ndices que ocupan en la lista de v�rtices.
		/// @param indices Lista con los �ndices.
		/// 
		/// @pre Cada �ndice debe ser menor al n�mero de v�rtices registrados.
		void AddPointIndices(const DynamicArray<PointIndex>& indices);


		/// @brief Registra un tri�ngulo, representado por sus tres �ndices.
		/// @param indices �ndices del tri�ngulo.
		/// 
		/// @pre Cada �ndice debe ser menor al n�mero de v�rtices registrados.
		void AddTriangleIndex(TriangleIndices indices);

		/// @brief Registra una l�nea, representada por sus dos �ndices.
		/// @param indices �ndices de la l�nea.
		/// 
		/// @pre Cada �ndice debe ser menor al n�mero de v�rtices registrados.
		void AddLineIndex(LineIndices indices);

		/// @brief Registra un punto aislado, representado por su �ndice.
		/// @param indices �ndice del punto.
		/// 
		/// @pre El �ndice debe ser menor al n�mero de v�rtices registrados.
		void AddPointIndex(PointIndex indices);


		/// @returns True si contiene al menos un tri�ngulo.
		bool HasTriangles() const;

		/// @returns True si contiene al menos una l�nea.
		bool HasLines() const;

		/// @returns True si contiene al menos un punto aislado.
		bool HasPoints() const;


		/// @returns N�mero de tri�ngulos registrados. 
		USize64 GetTriangleCount() const;

		/// @returns N�mero de l�neas registradas.
		USize64 GetLineCount() const;

		/// @returns N�mero de puntos aislados registrados.
		USize64 GetPointCount() const;


		/// @returns Lista con los �ndices de todos los tri�ngulos registrados.
		const DynamicArray<TriangleIndices>& GetTriangles() const;

		/// @returns Lista con los �ndices de todas las l�neas registrados.
		const DynamicArray<LineIndices>& GetLines() const;

		/// @returns Lista con los �ndices de todos los puntos aislados registrados.
		const DynamicArray<PointIndex>& GetPoints() const;


		/// @returns Lista con todos los v�rtices.
		const DynamicArray<CpuVertex3D>& GetVertices() const;


		/// @brief Establece el �ndice del material.
		/// @param index �ndice del material, respecto a la lista de materiales del modelo
		/// que posee esta malla.
		/// 
		/// @pre @p index Debe ser menor que el n�mero de materiales registrados en 
		/// el modelo que posee esta malla.
		void SetMaterialIndex(UIndex64 index);

		/// @returns True si tiene asociado un material.
		bool HasMaterial() const;

		/// @returns �ndice del material, respecto a la lista de materiales del modelo
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


		/// @brief A�ade una lista de mallas al modelo.
		/// @param meshes Lista de mallas a a�adir.
		void AddMeshes(const DynamicArray<CpuMesh3D>& meshes);

		/// @brief A�ade una lista de animaciones al modelo.
		/// @param animations lista de animaciones a a�adir.
		void AddAnimations(const DynamicArray<GRAPHICS::Animation>& animations);

		/// @brief A�ade una lista de skins de animaciones al modelo.
		/// @param animations lista de skins de animaciones a a�adir.
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
