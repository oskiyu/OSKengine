#pragma once

#include "ApiCall.h"

#include <glm/glm.hpp>

#include "Vector3.hpp"
#include "AtomicHolder.h"
#include "MutexHolder.h"
#include "Quaternion.h"

namespace OSK {

	class OSKAPI_CALL Transform3D {

	public:

		/// @brief Establece la posición local.
		/// @param position Nueva posición respecto al padre.
		void SetPosition(const Vector3f& position);

		/// @brief Establece la escala local.
		/// @param scale Nueva escala respecto al padre.
		void SetScale(const Vector3f& scale);

		/// @brief Establece la rotación local.
		/// @param rotation Nueva rotación respecto al padre.
		void SetRotation(const Quaternion& rotation);


		/// @brief Suma el vector 3D a la posición.
		/// @param positionDelta Cambio de posición.
		void AddPosition(const Vector3f& positionDelta);

		/// @brief Suma el vector 3D a la posición.
		/// @param positionDelta Cambio de posición.
		/// 
		/// @pre No debe haber ningún otro hilo llamando a _ApplyChanges().
		/// @pre No debe haber ningún otro hilo lllamando a AddPosition().
		/// @threadsafety Esta función es thread-safe, siempre y
		/// cuando se cumplan las precondiciones.
		/// 
		/// @note En condiciones normales, sólamente se llama a _ApplyChanges()
		/// en el sistema TransformApplierSystem (que se ejecuta de
		/// manera exclusiva).
		void AddPosition_ThreadSafe(const Vector3f& positionDelta);


		/// @brief Suma un vector 3D a la escala.
		/// @param scaleDelta Cambio de escala.
		void AddScale(const Vector3f& scaleDelta);

		/// @brief Suma un vector 3D a la escala.
		/// @param scaleDelta Cambio de escala.
		/// 
		/// @pre No debe haber ningún otro hilo llamando a _ApplyChanges().
		/// @pre No debe haber ningún otro hilo lllamando a AddPosition().
		/// @threadsafety Esta función es thread-safe, siempre y
		/// cuando se cumplan las precondiciones.
		/// 
		/// @note En condiciones normales, sólamente se llama a _ApplyChanges()
		/// en el sistema TransformApplierSystem (que se ejecuta de
		/// manera exclusiva).
		void AddScale_ThreadSafe(const Vector3f& scaleDelta);


		/// @brief Aplica una rotación al transform.
		/// @param rotationDelta Cambio de rotación.
		void ApplyRotation(const Quaternion& rotationDelta);

		/// @brief Aplica una rotación al transform.
		/// @param rotationDelta Cambio de rotación.
		/// 
		/// @pre No debe haber ningún otro hilo llamando a _ApplyChanges().
		/// @pre No debe haber ningún otro hilo lllamando a AddPosition().
		/// @threadsafety Esta función es thread-safe, siempre y
		/// cuando se cumplan las precondiciones.
		/// 
		/// @note En condiciones normales, sólamente se llama a _ApplyChanges()
		/// en el sistema TransformApplierSystem (que se ejecuta de
		/// manera exclusiva).
		void ApplyRotation_ThreadSafe(const Quaternion& rotationDelta);


		/// @brief Rota el transform respecto a sí mismo.
		/// @param angle Ángulo.
		/// @param axis Eje sobre el que se rota.
		/// @pre El ángulo debe estar en grados.
		void RotateLocalSpace(float angle, const Vector3f& axis);

		/// @brief Rota el transform respecto a sí mismo.
		/// @param angle Ángulo.
		/// @param axis Eje sobre el que se rota.
		/// @pre El ángulo debe estar en grados.
		/// 
		/// @pre No debe haber ningún otro hilo llamando a _ApplyChanges().
		/// @pre No debe haber ningún otro hilo lllamando a AddPosition().
		/// @threadsafety Esta función es thread-safe, siempre y
		/// cuando se cumplan las precondiciones.
		/// 
		/// @note En condiciones normales, sólamente se llama a _ApplyChanges()
		/// en el sistema TransformApplierSystem (que se ejecuta de
		/// manera exclusiva).
		void RotateLocalSpace_ThreadSafe(float angle, const Vector3f& axis);


		/// @brief Rota el transform respecto al mundo.
		/// @param angle Ángulo.
		/// @param axis Eje sobre el que se rota.
		/// @pre El ángulo debe estar en grados.
		void RotateWorldSpace(float angle, const Vector3f& axis);

		/// @brief Rota el transform respecto al mundo.
		/// @param angle Ángulo.
		/// @param axis Eje sobre el que se rota.
		/// @pre El ángulo debe estar en grados.
		/// 
		/// @pre No debe haber ningún otro hilo llamando a _ApplyChanges().
		/// @pre No debe haber ningún otro hilo lllamando a AddPosition().
		/// @threadsafety Esta función es thread-safe, siempre y
		/// cuando se cumplan las precondiciones.
		/// 
		/// @note En condiciones normales, sólamente se llama a _ApplyChanges()
		/// en el sistema TransformApplierSystem (que se ejecuta de
		/// manera exclusiva).
		void RotateWorldSpace_ThreadSafe(float angle, const Vector3f& axis);

		/// @brief Transforma un punto respecto a este transform.
		/// @param point Punto antes de la transformación.
		/// @return Punto transformado.
		Vector3f TransformPoint(const Vector3f& point) const;


		/// @brief Actualiza manualmente el valor de la matriz modelo.
		/// @param matrix Nueva matriz modelo.
		void OverrideMatrix(const glm::mat4& matrix);


		/// @return Posición en el mundo 3D.
		Vector3f GetPosition() const;

		/// @return Escala en el mundo 3D.
		Vector3f GetScale() const;

		/// @return Orientación en el mundo 3D.
		Quaternion GetRotation() const;


		/// @return Posición local.
		Vector3f GetLocalPosition() const;

		/// @return Escala local.
		Vector3f GetLocalScale() const;

		/// @return Orientación local.
		Quaternion GetLocalRotation() const;


		/// @return Matriz modelo, para renderizado.
		glm::mat4 GetAsMatrix() const;


		/// @return Vector unitario 3D que apunta hacia el frente de la entidad.
		Vector3f GetForwardVector() const;

		/// @return Vector unitario 3D que apunta hacia la derecha de la entidad.
		Vector3f GetRightVector() const;

		/// @return Vector unitario 3D que apunta hacia arriba de la entidad.
		Vector3f GetTopVector() const;


		/// @brief Configura qué elementos
		/// se heredan del transform padre,
		/// en caso de que exista.
		struct InheritanceConfig {
			bool inheritsPosition = true;
			bool inheritsRotation = true;
			bool inheritsScale = true;
		};


		/// @brief Aplica los cambios realizados sobre el transform
		/// cuando no tiene transform padre.
		void _ApplyChanges();

		/// @brief Aplica los cambios realizados sobre el transform
		/// cuando sí tiene transform padre.
		/// @param parent Transform padre (no nulo).
		/// @param config Configuración.
		void _ApplyChanges(const Transform3D* parent, const InheritanceConfig& config);

	private:

		/// @brief Actualiza la matriz modelo.
		void UpdateModel(const Transform3D* parent, const InheritanceConfig& config);

		/// @brief Matriz modelo (para renderizado).
		glm::mat4 m_matrix = glm::mat4(1.0f);

		Vector3f m_position = Vector3f::Zero;
		Vector3f m_scale = Vector3f::One;
		glm::mat4 m_totalRotation = glm::mat4(1.0f);

		Vector3f m_localPosition = Vector3f::Zero;
		Vector3f m_localScale = Vector3f::One;
		Quaternion m_localRotation{};



		// --- Changes --- //

		AtomicHolder<bool> m_isPositionDirty = AtomicHolder<bool>(false);
		AtomicHolder<bool> m_isRotationDirty = AtomicHolder<bool>(false);
		AtomicHolder<bool> m_isScaleDirty = AtomicHolder<bool>(false);

		Vector3f m_changeInPosition = Vector3f::Zero;
		Quaternion m_changeInRotation = {};
		Vector3f m_changeInScale = Vector3f::Zero;


		// Multithreading

		MutexHolder m_positionMutex;
		MutexHolder m_rotationMutex;
		MutexHolder m_scaleMutex;

	};

}
