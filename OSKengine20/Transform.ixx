#define GLM_ENABLE_EXPERIMENTAL
#include <vector>
#include <glm.hpp>
#include <assimp/matrix4x4.h>
#include <ext/matrix_transform.hpp>
#include <gtx/quaternion.hpp>
#include <gtx/matrix_decompose.hpp>
#include <gtx/string_cast.hpp>

export module OSKengine.Types.Transform;

export import OSKengine.Types.Vector3;
export import OSKengine.Types.Vector4;
export import OSKengine.Types.Quaternion;

import OSKengine.Logger;

export namespace OSK {

	/// <summary>
	/// Clase que almacena el 'transform' de un objeto en un mundo 3D.
	/// El transform contiene posición, escala y rotación.
	/// A este transform se pueden enlazar otros transform ahijados,
	/// de tal manera que al cambiar el transform padre,
	/// el resto de transforms también ven sus variables cambiadas.
	/// </summary>
	class Transform {

		friend class PhysicsSystem;
		friend class ContentManager;

	public:

		/// <summary>
		/// Transform por defecto.
		/// </summary>
		Transform() {
			modelMatrix = glm::mat4(1);

			localPosition = Vector3f(0);
			localScale = Vector3f(1);

			globalPosition = Vector3f(0);
			globalScale = Vector3f(1);

			isAttached = false;
			parentTransform = nullptr;
		}

		/// <summary>
		/// Crea el transform.
		/// </summary>
		/// <param name="position">Posición en el mundo.</param>
		/// <param name="scale">Escala en el mundo.</param>
		Transform(const Vector3f& position, const Vector3f& scale) {
			modelMatrix = glm::mat4(1.0f);
			localPosition = position;
			localScale = scale;

			isAttached = false;
			parentTransform = nullptr;

			UpdateModel();
		}

		/// <summary>
		/// Establece la posición.
		/// También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="position">Nueva posición respecto al padre.</param>
		void SetPosition(const Vector3f& position) {
			localPosition = position;

			UpdateModel();
		}

		/// <summary>
		/// Establece la escala.
		/// También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="scale">Nueva escala respecto al padre.</param>
		void SetScale(const Vector3f& scale) {
			localScale = scale;

			if (parentTransform)
				localScale += parentTransform->GetScale();

			UpdateModel();
		}

		/// <summary>
		/// Establece la rotación.
		/// También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="rotation">Nueva rotación respecto al padre.</param>
		void SetRotation(const Quaternion& rotation) {
			this->localRotation = localRotation;

			UpdateModel();
		}

		/// <summary>
		/// Suma el vector 3D a la posición.
		/// También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="positionDelta">Posición a añadir.</param>
		void AddPosition(const Vector3f& positionDelta) {
			SetPosition(localPosition + positionDelta);
		}

		/// <summary>
		/// Suma un vector 3D a la escala.
		/// También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="scaleDelta">Escala a añadir.</param>
		void AddScale(const Vector3f& scaleDelta) {
			SetScale(localScale + scaleDelta);
		}

		/// <summary>
		/// Aplica una rotación al transform.
		/// También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="rotationDelta">Rotación a aplicar.</param>
		void ApplyRotation(const Quaternion& rotationDelta) {
			Quaternion q = localRotation;
			q.GetGlm() *= rotationDelta.GetGlm();

			SetRotation(q);
		}

		/// <summary>
		/// Rota el transform respecto a sí mismo.
		/// También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="angle">Ángulo.</param>
		/// <param name="axis">Eje sobre el que se rota.</param>
		void RotateLocalSpace(float angle, const Vector3f& axis) {
			localRotation.Rotate_LocalSpace(angle, axis);

			UpdateModel();
		}

		/// <summary>
		/// Rota el transform respecto al mundo.
		/// También actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		/// <param name="angle">Ángulo.</param>
		/// <param name="axis">Eje sobre el que se rota.</param>
		void RotateWorldSpace(float angle, const Vector3f& axis) {
			localRotation.Rotate_WorldSpace(angle, axis);

			UpdateModel();
		}

		/// <summary>
		/// Actualiza la matriz modelo, y la de sus hijos.
		/// </summary>
		void UpdateModel() {
			modelMatrix = glm::mat4(1.0f);

			if (parentTransform) {
				globalRotation = parentTransform->globalRotation * localRotation.ToMat4();

				modelMatrix = glm::translate(modelMatrix, parentTransform->GetPosition().ToGLM());
				modelMatrix *= parentTransform->globalRotation;
			}
			else {
				globalRotation = localRotation.ToMat4();
			}

			//Posición local.
			modelMatrix = glm::translate(modelMatrix, localPosition.ToGLM());

			//Escala local.
			modelMatrix = glm::scale(modelMatrix, localScale.ToGLM());

			//Rotación local.
			modelMatrix = modelMatrix * localRotation.ToMat4();

			//Obtener posición final.
			globalPosition = Vector3f(modelMatrix * Vector4(0, 0, 0, 1).ToGLM());

			if (isParent) {
				for (uint32_t i = 0; i < childTransforms.size(); i++) {

					if (childTransforms[i]->parentTransform == this)
						childTransforms[i]->UpdateModel();
					else
						childTransforms.erase(childTransforms.begin() + i);

				}
			}
		}

		/// <summary>
		/// Enlaza este transform a su nuevo transform padre.
		/// </summary>
		/// <param name="baseTransform">Tranform padre.</param>
		void AttachTo(Transform* baseTransform) {
			for (auto* t : childTransforms) {
				if (t == baseTransform) {
					GetLogger()->Log(LogLevel::WARNING, "este transform no puede atarse a su hijo.");

					return;
				}
			}

			parentTransform = baseTransform;

			baseTransform->childTransforms.push_back(this);
			baseTransform->isParent = true;

			isAttached = true;
		}

		/// <summary>
		/// Libera este transform de su padre.
		/// </summary>
		void UnAttach() {
			parentTransform = nullptr;

			isAttached = false;
		}

		/// <summary>
		/// Vector posición en el mundo 3D.
		/// </summary>
		Vector3f GetPosition() const {
			return globalPosition;
		}

		/// <summary>
		/// Vector escala en el mundo 3D.
		/// </summary>
		Vector3f GetScale() const {
			return globalScale;
		}

		/// <summary>
		/// Posición respecto al padre.
		/// </summary>
		Vector3f GetLocalPosition() const {
			return localPosition;
		}

		/// <summary>
		/// Escala respecto al padre.
		/// </summary>
		Vector3f GetLocalScale() const {
			return localScale;
		}

		/// <summary>
		/// Orientación respecto al padre.
		/// </summary>
		Quaternion GetLocalRotation() const {
			return localRotation;
		}

		/// <summary>
		/// Orientación.
		/// </summary>
		Quaternion GetRotation() const {
			Quaternion q;
			q.GetGlm() = glm::toQuat(globalRotation);

			return q;
		}

		/// <summary>
		/// Matriz modelo.
		/// </summary>
		glm::mat4 AsMatrix() const {
			return modelMatrix;
		}

		/// <summary>
		/// Transform padre.
		/// </summary>
		Transform* GetParent() const {
			return parentTransform;
		}

		/// <summary>
		/// Transformaciones ahijadas.
		/// </summary>
		std::vector<Transform*> GetChildTransforms() const {
			return childTransforms;
		}

		/// <summary>
		/// Devuelve un vector unitario 3D que apunta hacia el frente de la entidad.
		/// </summary>
		Vector3f GetForwardVector() const {
			return Vector3f(GetRotation().GetGlm() * OSK::Vector3f(0, 0, 1).ToGLM()).GetNormalized();
		}

		/// <summary>
		/// Devuelve un vector unitario 3D que apunta hacia la derecha de la entidad.
		/// </summary>
		Vector3f GetRightVector() const {
			return Vector3f(GetRotation().GetGlm() * OSK::Vector3f(-1, 0, 0).ToGLM()).GetNormalized();
		}

		/// <summary>
		/// Devuelve un vector unitario 3D que apunta hacia arriba de la entidad.
		/// </summary>
		Vector3f GetTopVector() const {
			return Vector3f(GetRotation().GetGlm() * OSK::Vector3f(0, 1, 0).ToGLM()).GetNormalized();
		}

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