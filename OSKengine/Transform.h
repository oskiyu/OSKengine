#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/matrix_decompose.hpp>

#include <string>
#include <array>
#include <vector>

#include <assimp/matrix4x4.h>

#define OSK_MAKE_GLM_MAT4(T) glm::mat<4, 4, T, glm::highp>
#define OSK_MAKE_GLM_VEC3(T) glm::vec<3, T, glm::packed_highp>

namespace OSK {

	//Clase que almacena el 'transform' de un objeto en un mundo 3D.
	//Posición, escala y rotación.
	template <typename P> class Transform_t {

	public:

		//Crea la clase.
		Transform_t<P>() {
			ModelMatrix = OSK_MAKE_GLM_MAT4(P)((P)1);

			Position = Vector3_t<P>(0);
			Scale = Vector3_t<P>(1);
			Rotation = Vector3_t<P>(0);

			GlobalPosition = Vector3_t<P>(0);
			GlobalScale = Vector3_t<P>(1);
			GlobalRotation = Vector3_t<P>(0);

			PositionOffset = Vector3_t<P>(0);
			ScaleOffset = Vector3_t<P>(0);
			RotationOffset = Vector3_t<P>(0);

			isSkeletal = false;
			isAttached = false;
			ParentTransform = nullptr;
		}


		//Crea la clase con la posición, escala y rotación establecidos.
		Transform_t<P>(const Vector3_t<P>& position, const Vector3_t<P>& scale, const Vector3_t<P>& rotation) {
			ModelMatrix = OSK_MAKE_GLM_MAT4(P)((P)1);
			Position = position;
			Scale = scale;
			Rotation = rotation;

			PositionOffset = Vector3_t<P>(0);
			RotationOffset = Vector3_t<P>(0);

			isAttached = false;
			ParentTransform = nullptr;

			isSkeletal = false;

			UpdateModel();
		}
				

		//Crea la clase desde una matriz.
		Transform_t<P>(const OSK_MAKE_GLM_MAT4(P)& model) {
			glm::vec3 position;
			glm::quat rotation;
			glm::vec3 scale;
			glm::vec3 skew;
			glm::vec4 prespective;

			glm::decompose(model, scale, rotation, position, skew, prespective);

			rotation = glm::conjugate(rotation);

			ModelMatrix = glm::mat4((P)1);

			Scale = Vector3_t<P>(1);

			GlobalPosition = Vector3_t<P>(0);
			GlobalScale = Vector3_t<P>(1);
			GlobalRotation = Vector3_t<P>(0);

			PositionOffset = Vector3_t<P>(0);
			ScaleOffset = Vector3_t<P>(0);
			RotationOffset = Vector3_t<P>(0);

			Position = Vector3_t<P>(position);
			Rotation = Vector3_t<P>(glm::eulerAngles(rotation) * (3.14159f / 2.0f));

			isAttached = false;
			ParentTransform = nullptr;

			isSkeletal = true;

			//UpdateModel();

			ModelMatrix = model;
		}


		//Destruye la clase.
		~Transform_t<P>() {

		}


		//Establece la posición.
		//También actualiza la matriz modelo.
		void SetPosition(const Vector3_t<P>& position) {
			Position = position;

			UpdateModel();
		}


		//Establece la escala.
		//También actualiza la matriz modelo.
		void SetScale(const Vector3_t<P>& scale) {
			Scale = scale;

			UpdateModel();
		}


		//Establece la rotación.
		//También actualiza la matriz modelo.
		void SetRotation(const Vector3_t<P>& rotation) {
			Rotation = rotation;

			if (Rotation.X > 360.0f)
				Rotation.X -= 360.0f;
			else if (Rotation.X < -360.0f)
				Rotation.X += 360.0f;

			if (Rotation.Y > 360.0f)
				Rotation.Y -= 360.0f;
			else if (Rotation.Y < -360.0f)
				Rotation.Y += 360.0f;

			if (Rotation.Z > 360.0f)
				Rotation.Z -= 360.0f;
			if (Rotation.Z < -360.0f)
				Rotation.Z += 360.0f;

			UpdateModel();
		}


		//Suma un vector 3D a la posición.
		void AddPosition(const Vector3_t<P>& positionDelta) {
			SetPosition(Position + positionDelta);
		}


		//Suma un vector 3D a la escala.
		void AddScale(const Vector3_t<P>& scaleDelta) {
			SetScale(Scale + scaleDelta);
		}


		//Suma un vector 3D a la rotación.
		void AddRotation(const Vector3_t<P>& rotationDelta) {
			SetRotation(Rotation + rotationDelta);
		}


		//Actualiza la matriz modelo.
		void UpdateModel() {
			GlobalRotation = Rotation + RotationOffset;
			GlobalPosition = Position + PositionOffset;
			GlobalScale = Scale;
			if (UseParentScale)
				GlobalScale = GlobalScale + ScaleOffset;

			ModelMatrix = OSK_MAKE_GLM_MAT4(P)((P)1);

			ModelMatrix = glm::translate(ModelMatrix, PositionOffset.ToGLM_T());

			ModelMatrix = glm::rotate(ModelMatrix, (P)glm::radians((P)RotationOffset.X), OSK_MAKE_GLM_VEC3(P)((P)1, (P)0, (P)0));
			ModelMatrix = glm::rotate(ModelMatrix, (P)glm::radians((P)RotationOffset.Y), OSK_MAKE_GLM_VEC3(P)((P)0, (P)1, (P)0));
			ModelMatrix = glm::rotate(ModelMatrix, (P)glm::radians((P)RotationOffset.Z), OSK_MAKE_GLM_VEC3(P)((P)0, (P)0, (P)1));

			GlobalPosition = Vector3_t<P>(ModelMatrix * Vector4(Position.X, Position.Y, Position.Z, 1).ToGLM());

			ModelMatrix = glm::translate(ModelMatrix, GlobalPosition.ToGLM_T());

			ModelMatrix = glm::scale(ModelMatrix, GlobalScale.ToGLM_T());

			ModelMatrix = glm::rotate(ModelMatrix, (P)glm::radians(Rotation.X), OSK_MAKE_GLM_VEC3(P)((P)1, (P)0, (P)0));
			ModelMatrix = glm::rotate(ModelMatrix, (P)glm::radians(Rotation.Y), OSK_MAKE_GLM_VEC3(P)((P)0, (P)1, (P)0));
			ModelMatrix = glm::rotate(ModelMatrix, (P)glm::radians(Rotation.Z), OSK_MAKE_GLM_VEC3(P)((P)0, (P)0, (P)1));

			if (isParent) {
				for (uint32_t i = 0; i < ChildTransforms.size(); i++) {
					if (ChildTransforms[i]->ParentTransform == this) {
						ChildTransforms[i]->RotationOffset = GlobalRotation;
						ChildTransforms[i]->PositionOffset = GlobalPosition;
						ChildTransforms[i]->ScaleOffset = GlobalScale;
						ChildTransforms[i]->UpdateModel();
					}
					else {
						ChildTransforms.erase(ChildTransforms.begin() + i);
					}
				}
			}
		}


		//Establece el 'transform' baseTransform como base.
		void AttachTo(Transform_t<P>* baseTransform) {
			for (auto* t : ChildTransforms) {
				if (t == baseTransform) {
					Logger::Log(LogMessageLevels::WARNING, "este transform no puede atarse a su hijo.");

					return;
				}
			}

			ParentTransform = baseTransform;

			baseTransform->ChildTransforms.push_back(this);
			baseTransform->isParent = true;

			RotationOffset = ParentTransform->Rotation;
			PositionOffset = ParentTransform->Position;
			ScaleOffset = ParentTransform->Scale;

			isAttached = true;
		}


		//Elimina el 'transform' base.
		void UnAttach() {
			ParentTransform = nullptr;

			RotationOffset = Vector3_t<P>(0.0f);

			isAttached = false;
		}


		//Vector posición en el mundo 3D.
		Vector3_t<P> GlobalPosition;


		//Vector escala en el mundo 3D.
		Vector3_t<P> GlobalScale;


		//Vector rotación en el mundo 3D.
		Vector3_t<P> GlobalRotation;


		//Vector posición.
		Vector3_t<P> Position;


		//Vector escala.
		Vector3_t<P> Scale;


		//Vector rotación.
		Vector3_t<P> Rotation;


		//Establece si ha de usarse la matriz modelo.
		//Solo true en casos de modelos 3D.
		bool UseModelMatrix = false;


		//Matriz "model".
		OSK_INFO_READ_ONLY
			OSK_MAKE_GLM_MAT4(P) ModelMatrix;
	

		//Transformación padre.
		Transform_t<P>* ParentTransform;


		//Transformaciones ahijadas.
		std::vector<Transform_t<P>*> ChildTransforms;


		//Offset de la posición (del transform padre).
		Vector3_t<P> PositionOffset;


		//Offset de la rotación (del transform padre).
		Vector3_t<P> RotationOffset;


		//Offset de la escala (del transform padre).
		Vector3_t<P> ScaleOffset;


		//Establece si ha de heredar la escala padre.
		bool UseParentScale = false;

	private:

		void toGlmMat(const aiMatrix4x4* src, glm::mat4& dst) {
			dst[0][0] = src->a1; dst[1][0] = src->a2;
			dst[2][0] = src->a3; dst[3][0] = src->a4;
			dst[0][1] = src->b1; dst[1][1] = src->b2;
			dst[2][1] = src->b3; dst[3][1] = src->b4;
			dst[0][2] = src->c1; dst[1][2] = src->c2;
			dst[2][2] = src->c3; dst[3][2] = src->c4;
			dst[0][3] = src->d1; dst[1][3] = src->d2;
			dst[2][3] = src->d3; dst[3][3] = src->d4;
		}

		bool isAttached = false;

		bool isParent = false;

		bool isSkeletal = false;

	};

	typedef Transform_t<float_t> Transform;
}