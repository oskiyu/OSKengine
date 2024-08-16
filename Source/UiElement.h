#pragma once

#include "ApiCall.h"
#include "LinkedList.hpp"
#include "UniquePtr.hpp"
#include "HashMap.hpp"
#include "OwnedPtr.h"
#include "UiAnchor.h"

#include "SdfDrawCall2D.h"
#include "Color.hpp"
#include "Vector4.hpp"

#include <glm/glm.hpp>
#include <string>
#include "DynamicArray.hpp"
#include "DefineAs.h"
#include "SdfDrawCall2D.h"

namespace OSK::GRAPHICS {
	class SdfBindlessRenderer2D;
}

namespace OSK::IO {
	class KeyboardState;
}

namespace OSK::UI {

	class IContainer;


	/// @brief Clase base para todos los elementos de interfaz de usuario.
	/// @todo Establecer unidad de tama�o independiente de la resoluci�n.
	class OSKAPI_CALL IElement {

	protected:

		/// @brief Establece el tama�o de la interfaz.
		/// @param size 
		explicit IElement(const Vector2f& size);

	public:

		virtual ~IElement() = default;

		OSK_DEFINE_AS(IElement);

		/// @brief Renderiza el elemento.
		/// @param renderer Renderizador de 2D.
		virtual void Render(GRAPHICS::SdfBindlessRenderer2D* renderer) const;

		/// @brief Actualiza el estado o cualquier otra caracter�stica del
		/// elemento de acuerdo a la posici�n del cursor (rat�n).
		/// @param cursorPosition Posici�n del cursor.
		/// @param isPressed True si se est� pulsando el cursor.
		/// 
		/// @return True si el click se ha procesado dentro del elemento (y por lo tanto,
		/// se debe detener la comprobaci�n para el resto de elementos).
		virtual bool UpdateByCursor(Vector2f cursorPosition, bool isPressed);

		/// @brief Actualiza el estado o cualquier otra caracter�stica del
		/// elemento de acuerdo al estado del teclado.
		/// @param state Estado en el instante actual.
		virtual void UpdateByKeyboard(const IO::KeyboardState& previous, const IO::KeyboardState& current);

	public:

		/// @brief A�ade una llamada de renderizado al elemento.
		/// @param drawCall Nueva llamada de renderizado.
		void AddDrawCall(const GRAPHICS::SdfDrawCall2D& drawCall);
		void AddDrawCall(GRAPHICS::SdfDrawCall2D&& drawCall);

		/// @brief A�ade una lista de llamadas de renderizado.
		/// @param drawCall Lista de llamadas de renderizado.
		void AddAllDrawCalls(const DynamicArray<GRAPHICS::SdfDrawCall2D>& drawCall);
		void AddAllDrawCalls(DynamicArray<GRAPHICS::SdfDrawCall2D>&& drawCall);

		/// @return Lista con todas las llamadas de renderizado.
		DynamicArray<GRAPHICS::SdfDrawCall2D>& GetAllDrawCalls();
		const DynamicArray<GRAPHICS::SdfDrawCall2D>& GetAllDrawCalls() const;

		/// @brief Establece si el elemento debe ser visible o no.
		/// Si no es visible, tampoco responder�
		/// a eventos de entrada.
		/// @param visibility Si debe ser visible o no.
		void SetVisibility(bool visibility);

		/// @brief Establece el elemento como visible.
		/// Si ya era visible, no ocurrir� nada.
		void SetVisible();

		/// @brief Establece el elemento como invisible.
		/// Si no es visible, tampoco responder�
		/// a eventos de entrada.
		/// Si ya era invisible, no ocurrir� nada.
		void SetInvisible();

		/// @return Si es visible o no.
		bool IsVisible() const;


		/// @brief Bloquea el elemento para que no
		/// responda a eventos de entrada.
		/// Si ya estaba bloqueado, no ocurrir� nada.
		void Lock();

		/// @brief Desbloquea el elemento para que
		/// responda a eventos de entrada.
		/// Si ya estaba desbloqueado, no ocurrir� nada.
		void Unlock();

		/// @return Si est� bloqueado o no.
		bool IsLocked() const;


		/// @brief Establece el padding (m�rgenes interiores).
		/// @param padding Padding, tal que:
		/// X -> padding izquierdo y derecho.
		/// Y -> padding superior e inferior.
		void SetPadding(const Vector2f& padding);

		/// @brief Establece el padding (m�rgenes interiores).
		/// @param padding 
		/// @param padding Padding, tal que:
		/// X -> padding izquierdo.
		/// Y -> padding superior.
		/// Z -> padding derecho.
		/// W -> padding inferior.
		void SetPadding(const Vector4f& padding);


		/// @brief Establece los m�rgenes externos.
		/// @param margin M�rgenes, tal que:
		/// X -> margen izquierdo y derecho.
		/// Y -> margen superior e inferior.
		void SetMargin(const Vector2f& margin);

		/// @brief Establece los m�rgenes externos.
		/// @param margin M�rgenes, tal que:
		/// X -> margen izquierdo.
		/// Y -> margen superior.
		/// Z -> margen derecho.
		/// W -> margen inferior.
		void SetMargin(const Vector4f& margin);


		/// @brief Establece si este elemento debe establecer su tama�o
		/// relativo al padre, cambiando de tama�o cuando el padre 
		/// tambi�n cambie de tama�o.
		/// @param keepRelativeSize True si debe cambiar de tama�o junto al padre.
		/// False si conserva su tama�o original.
		void SetKeepRelativeSize(bool keepRelativeSize);

		/// @return True si debe cambiar de tama�o junto al padre.
		/// False si conserva su tama�o original cuando el padre cambia
		/// de tama�o.
		bool KeepsRelativeSize() const;


		/// @brief Establece la posici�n de este elemento.
		/// @param relativePosition Posici�n (en p�xeles).
		virtual void _SetPosition(const Vector2f& newPosition);

		/// @return Posici�n, en p�xeles.
		Vector2f GetPosition() const;


		/// @brief Cambia el tama�o del elemento, sin modifciar contenedores
		/// ni elementos hijos.
		/// @param newSize Nuevo tama�o.
		/// 
		/// @warning No modifica ni la posici�n ni el tama�o de elementos padre,
		/// hijos o hermanos.
		virtual void SetSize(Vector2f newSize);

		/// @return Tama�o del elemento.
		Vector2f GetSize() const;

		/// @return Tama�o del contenido del elemento (tama�o - padding).
		Vector2f GetContentSize() const;

		/// @return Posici�n relativa del contenido (posici�n - padding).
		Vector2f GetContentTopLeftPosition() const;


		/// @return Rect�ngulo que ocupa el elemento, tal que:
		/// X, Y -> posici�n.
		/// Z -> ancho.
		/// W -> alto.
		Vector4f GetRectangle() const;

		/// @brief Establece el anchor del elemento.
		/// No cambiar� su posici�n respecto al contenedor padre.
		/// @param anchor Nuevo anchor.
		void SetAnchor(Anchor anchor);

		/// @return Anchor actual del elemento.
		inline Anchor GetAnchor() const { return m_anchor; }

		/// @return Padding, tal que:
		/// X -> padding izquierdo.
		/// Y -> padding superior.
		/// Z -> padding derecho.
		/// W -> padding inferior.
		Vector4f GetPadding() const;

		/// @return M�rgenes, tal que:
		/// X -> margen izquierdo.
		/// Y -> margen superior.
		/// Z -> margen derecho.
		/// W -> margen inferior.
		Vector4f GetMarging() const;

		/// @return Padding, tal que:
		/// X -> padding izquierdo + padding derecho.
		/// Y -> padding superior + padding inferior.
		Vector2f GetPadding2D() const;

		/// @return M�rgenes, tal que:
		/// X -> margen izquierdo + margen derecho.
		/// Y -> margen superior + margen inferior.
		Vector2f GetMarging2D() const;


		void _SetParent(IContainer* parent);

	protected:

		/// @brief Funci�n que se ejecuta al cambiarse la
		/// posici�n del elemento.
		/// 
		/// Por defecto, reposiciona las llamadas de renderizado
		/// mediante `RepositionDrawCalls(...)`.
		/// @param previousPosition Posici�n anterior.
		/// 
		/// @note La nueva posici�n se puede obtener mediante
		/// `GetRelativePosition()`.
		virtual void OnPositionChanged(const Vector2f& previousPosition);

		/// @brief Funci�n que se ejecuta al cambiarse el
		/// tama�o del elemento.
		/// 
		/// Por defecto, y si debe mantener su tama�o relativo
		/// respecto al padre (`KeepsRelativeSize() == true`), 
		/// reposiciona las llamadas de renderizado mediante `ResizeDrawCalls(...)`.
		/// @param previousSize Tama�o anterior.
		/// @note El nuevo tama�o se puede obtener mediante
		/// `GetSize()`.
		virtual void OnSizeChanged(const Vector2f& previousSize);


		/// @brief Reposiciona todas las llamadas de renderizado
		/// por un delta indicado.
		/// @param positionDiff Delta de posici�n que se aplicar�
		/// a todas las llamadas.
		void RepositionDrawCalls(const Vector2f& positionDiff);

		/// @brief Reescala todas las llamadas de renderizado
		/// por un delta indicado.
		/// @param sizeDiff Delta de tama�o que se aplicar�
		/// a todas las llamadas.
		void ResizeDrawCalls(const Vector2f& sizeDiff);


		/// @return Elemento padre.
		/// Puede ser nullptr.
		IContainer* GetParent();

		/// @return True si tiene un elemento padre.
		bool HasParent() const;

	private:

		DynamicArray<GRAPHICS::SdfDrawCall2D> m_drawCalls{};

		/// @brief Tama�o, en p�xeles.
		Vector2f m_size = Vector2f::Zero;
		Anchor m_anchor = Anchor::DEFAULT;

		/// @brief Posici�n , en p�xeles.
		Vector2f m_position = Vector2f::Zero;

		Vector4f m_padding = 0;
		Vector4f m_margin = 2.0f;

		bool m_isVisible = true;
		bool m_isLocked = false;

		/// @brief True si se debe conservar el tama�o relativo de este elemento
		/// cuando el elemento padre sea cambiado de tama�o.
		bool m_keepRelativeSize = false;

		/// @brief (Opcional) padre de este elemento.
		IContainer* m_parent = nullptr;

	};

}
