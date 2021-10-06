#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <glm.hpp>
#include "Quaternion.h"

#include <vector>

#include <assimp/matrix4x4.h>

namespace OSK {

	/// <summary>
	/// Clase que almacena el 'transform' de un objeto en un mundo 3D.
	/// El transform contiene posición, escala y rotación.
	/// A este transform se pueden enlazar otros transform ahijados,
	/// de tal manera que al cambiar el transform padre,
	/// el resto de transforms también ven sus variables cambiadas.
	/// </summary>
	class OSKAPI_CALL Transform {

		friend class PhysicsSystem;
		friend class ContentManager;

	public:

		/// <summary>
		/// Transform por defecto.
		/// </summary>
		Transform();

		/// <summary>
		/// Crea el transform.
		/// </summary>
		/// <param name="position">Posición en el mundo.</param>
		/// <param name="scale">Escala en el mundo.</param>
		Transform(const Vector3f& position, const Vector3f& scale);
		
		/// <summary>
		/// Establece la posición.
		/// También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="position">Nueva posición respecto al padre.</param>
		void SetPosition(const Vector3f& position);

		/// <summary>
		/// Establece la escala.
		/// También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="scale">Nueva escala respecto al padre.</param>
		void SetScale(const Vector3f& scale);

		/// <summary>
		/// Establece la rotación.
		/// También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="rotation">Nueva rotación respecto al padre.</param>
		void SetRotation(const Quaternion& rotation);

		/// <summary>
		/// Suma el vector 3D a la posición.
		/// También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="positionDelta">Posición a añadir.</param>
		void AddPosition(const Vector3f& positionDelta);

		/// <summary>
		/// Suma un vector 3D a la escala.
		/// También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="scaleDelta">Escala a añadir.</param>
		void AddScale(const Vector3f& scaleDelta);

		/// <summary>
		/// Aplica una rotación al transform.
		/// También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="rotationDelta">Rotación a aplicar.</param>
		void ApplyRotation(const Quaternion& rotationDelta);

		/// <summary>
		/// Rota el transform respecto a sí mismo.
		/// También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="angle">Ángulo.</param>
		/// <param name="axis">Eje sobre el que se rota.</param>
		void RotateLocalSpace(float angle, const Vector3f& axis);

		/// <summary>
		/// Rota el transform respecto al mundo.
		/// También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="angle">Ángulo.</param>
		/// <param name="axis">Eje sobre el que se rota.</param>
		void RotateWorldSpace(float angle, const Vector3f& axis);

		/// <summary>
		/// Actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		void UpdateModel();

		/// <summary>
		/// Enlaza este transform a su nuevo transform padre.
		/// </summary>
		/// <param name="baseTransform">Tranform padre.</param>
		void AttachTo(Transform* baseTransform);

		/// <summary>
		/// Libera este transform de su padre.
		/// </summary>
		void UnAttach();

		/// <summary>
		/// Vector posición en el mundo 3D.
		/// </summary>
		Vector3f GetPosition() const;

		/// <summary>
		/// Vector escala en el mundo 3D.
		/// </summary>
		Vector3f GetScale() const;

		/// <summary>
		/// Posición respecto al padre.
		/// </summary>
		Vector3f GetLocalPosition() const;

		/// <summary>
		/// Escala respecto al padre.
		/// </summary>
		Vector3f GetLocalScale() const;

		/// <summary>
		/// Orientación respecto al padre.
		/// </summary>
		Quaternion GetLocalRotation() const;

		/// <summary>
		/// Orientación.
		/// </summary>
		Quaternion GetRotation() const;

		/// <summary>
		/// Matriz modelo.
		/// </summary>
		glm::mat4 AsMatrix() const;

		/// <summary>
		/// Transform padre.
		/// </summary>
		Transform* GetParent() const;

		/// <summary>
		/// Transformaciones ahijadas.
		/// </summary>
		std::vector<Transform*> GetChildTransforms() const;

		/// <summary>
		/// Devuelve un vector unitario 3D que apunta hacia el frente de la entidad.
		/// </summary>
		Vector3f GetForwardVector() const;

		/// <summary>
		/// Devuelve un vector unitario 3D que apunta hacia la derecha de la entidad.
		/// </summary>
		Vector3f GetRightVector() const;

		/// <summary>
		/// Devuelve un vector unitario 3D que apunta hacia arriba de la entidad.
		/// </summary>
		Vector3f GetTopVector() const;

		void SetMatrix(const glm::mat4& matrix) {
			modelMatrix = matrix;
		}

	private:

		/// <summary>
		/// Convierte la matriz de assimp en matriz de glm.
		/// </summary>
		/// <param name="src">Matriz assimp.</param>
		/// <returns>Matriz glm.</returns>
		static glm::mat4 toGlmMat(const aiMatrix4x4* src);

		/// <summary>
		/// Vector posición en el mundo 3D.
		/// </summary>
		Vector3f globalPosition;

		/// <summary>
		/// Vector escala en el mundo 3D.
		/// </summary>
		Vector3f globalScale;

		/// <summary>
		/// Posición respecto al padre.
		/// </summary>
		Vector3f localPosition;

		/// <summary>
		/// Escala respecto al padre.
		/// </summary>
		Vector3f localScale;

		/// <summary>
		/// Orientación.
		/// </summary>
		Quaternion localRotation;

		/// <summary>
		/// GlobalRotation
		/// </summary>
		glm::mat4 globalRotation = glm::mat4(1.0f);

		/// <summary>
		/// Matriz modelo.
		/// </summary>
		glm::mat4 modelMatrix;

		/// <summary>
		/// Transform padre.
		/// </summary>
		Transform* parentTransform = nullptr;

		/// <summary>
		/// Transformaciones ahijadas.
		/// </summary>
		std::vector<Transform*> childTransforms;

		/// <summary>
		/// True si tiene padre.
		/// </summary>
		bool isAttached = false;

		/// <summary>
		/// True si tiene ahijados.
		/// </summary>
		bool isParent = false;

	};

}