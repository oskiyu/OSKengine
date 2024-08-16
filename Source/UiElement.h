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
	/// @todo Establecer unidad de tamaño independiente de la resolución.
	class OSKAPI_CALL IElement {

	protected:

		/// @brief Establece el tamaño de la interfaz.
		/// @param size 
		explicit IElement(const Vector2f& size);

	public:

		virtual ~IElement() = default;

		OSK_DEFINE_AS(IElement);

		/// @brief Renderiza el elemento.
		/// @param renderer Renderizador de 2D.
		virtual void Render(GRAPHICS::SdfBindlessRenderer2D* renderer) const;

		/// @brief Actualiza el estado o cualquier otra característica del
		/// elemento de acuerdo a la posición del cursor (ratón).
		/// @param cursorPosition Posición del cursor.
		/// @param isPressed True si se está pulsando el cursor.
		/// 
		/// @return True si el click se ha procesado dentro del elemento (y por lo tanto,
		/// se debe detener la comprobación para el resto de elementos).
		virtual bool UpdateByCursor(Vector2f cursorPosition, bool isPressed);

		/// @brief Actualiza el estado o cualquier otra característica del
		/// elemento de acuerdo al estado del teclado.
		/// @param state Estado en el instante actual.
		virtual void UpdateByKeyboard(const IO::KeyboardState& previous, const IO::KeyboardState& current);

	public:

		/// @brief Añade una llamada de renderizado al elemento.
		/// @param drawCall Nueva llamada de renderizado.
		void AddDrawCall(const GRAPHICS::SdfDrawCall2D& drawCall);
		void AddDrawCall(GRAPHICS::SdfDrawCall2D&& drawCall);

		/// @brief Añade una lista de llamadas de renderizado.
		/// @param drawCall Lista de llamadas de renderizado.
		void AddAllDrawCalls(const DynamicArray<GRAPHICS::SdfDrawCall2D>& drawCall);
		void AddAllDrawCalls(DynamicArray<GRAPHICS::SdfDrawCall2D>&& drawCall);

		/// @return Lista con todas las llamadas de renderizado.
		DynamicArray<GRAPHICS::SdfDrawCall2D>& GetAllDrawCalls();
		const DynamicArray<GRAPHICS::SdfDrawCall2D>& GetAllDrawCalls() const;

		/// @brief Establece si el elemento debe ser visible o no.
		/// Si no es visible, tampoco responderá
		/// a eventos de entrada.
		/// @param visibility Si debe ser visible o no.
		void SetVisibility(bool visibility);

		/// @brief Establece el elemento como visible.
		/// Si ya era visible, no ocurrirá nada.
		void SetVisible();

		/// @brief Establece el elemento como invisible.
		/// Si no es visible, tampoco responderá
		/// a eventos de entrada.
		/// Si ya era invisible, no ocurrirá nada.
		void SetInvisible();

		/// @return Si es visible o no.
		bool IsVisible() const;


		/// @brief Bloquea el elemento para que no
		/// responda a eventos de entrada.
		/// Si ya estaba bloqueado, no ocurrirá nada.
		void Lock();

		/// @brief Desbloquea el elemento para que
		/// responda a eventos de entrada.
		/// Si ya estaba desbloqueado, no ocurrirá nada.
		void Unlock();

		/// @return Si está bloqueado o no.
		bool IsLocked() const;


		/// @brief Establece el padding (márgenes interiores).
		/// @param padding Padding, tal que:
		/// X -> padding izquierdo y derecho.
		/// Y -> padding superior e inferior.
		void SetPadding(const Vector2f& padding);

		/// @brief Establece el padding (márgenes interiores).
		/// @param padding 
		/// @param padding Padding, tal que:
		/// X -> padding izquierdo.
		/// Y -> padding superior.
		/// Z -> padding derecho.
		/// W -> padding inferior.
		void SetPadding(const Vector4f& padding);


		/// @brief Establece los márgenes externos.
		/// @param margin Márgenes, tal que:
		/// X -> margen izquierdo y derecho.
		/// Y -> margen superior e inferior.
		void SetMargin(const Vector2f& margin);

		/// @brief Establece los márgenes externos.
		/// @param margin Márgenes, tal que:
		/// X -> margen izquierdo.
		/// Y -> margen superior.
		/// Z -> margen derecho.
		/// W -> margen inferior.
		void SetMargin(const Vector4f& margin);


		/// @brief Establece si este elemento debe establecer su tamaño
		/// relativo al padre, cambiando de tamaño cuando el padre 
		/// también cambie de tamaño.
		/// @param keepRelativeSize True si debe cambiar de tamaño junto al padre.
		/// False si conserva su tamaño original.
		void SetKeepRelativeSize(bool keepRelativeSize);

		/// @return True si debe cambiar de tamaño junto al padre.
		/// False si conserva su tamaño original cuando el padre cambia
		/// de tamaño.
		bool KeepsRelativeSize() const;


		/// @brief Establece la posición de este elemento.
		/// @param relativePosition Posición (en píxeles).
		virtual void _SetPosition(const Vector2f& newPosition);

		/// @return Posición, en píxeles.
		Vector2f GetPosition() const;


		/// @brief Cambia el tamaño del elemento, sin modifciar contenedores
		/// ni elementos hijos.
		/// @param newSize Nuevo tamaño.
		/// 
		/// @warning No modifica ni la posición ni el tamaño de elementos padre,
		/// hijos o hermanos.
		virtual void SetSize(Vector2f newSize);

		/// @return Tamaño del elemento.
		Vector2f GetSize() const;

		/// @return Tamaño del contenido del elemento (tamaño - padding).
		Vector2f GetContentSize() const;

		/// @return Posición relativa del contenido (posición - padding).
		Vector2f GetContentTopLeftPosition() const;


		/// @return Rectángulo que ocupa el elemento, tal que:
		/// X, Y -> posición.
		/// Z -> ancho.
		/// W -> alto.
		Vector4f GetRectangle() const;

		/// @brief Establece el anchor del elemento.
		/// No cambiará su posición respecto al contenedor padre.
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

		/// @return Márgenes, tal que:
		/// X -> margen izquierdo.
		/// Y -> margen superior.
		/// Z -> margen derecho.
		/// W -> margen inferior.
		Vector4f GetMarging() const;

		/// @return Padding, tal que:
		/// X -> padding izquierdo + padding derecho.
		/// Y -> padding superior + padding inferior.
		Vector2f GetPadding2D() const;

		/// @return Márgenes, tal que:
		/// X -> margen izquierdo + margen derecho.
		/// Y -> margen superior + margen inferior.
		Vector2f GetMarging2D() const;


		void _SetParent(IContainer* parent);

	protected:

		/// @brief Función que se ejecuta al cambiarse la
		/// posición del elemento.
		/// 
		/// Por defecto, reposiciona las llamadas de renderizado
		/// mediante `RepositionDrawCalls(...)`.
		/// @param previousPosition Posición anterior.
		/// 
		/// @note La nueva posición se puede obtener mediante
		/// `GetRelativePosition()`.
		virtual void OnPositionChanged(const Vector2f& previousPosition);

		/// @brief Función que se ejecuta al cambiarse el
		/// tamaño del elemento.
		/// 
		/// Por defecto, y si debe mantener su tamaño relativo
		/// respecto al padre (`KeepsRelativeSize() == true`), 
		/// reposiciona las llamadas de renderizado mediante `ResizeDrawCalls(...)`.
		/// @param previousSize Tamaño anterior.
		/// @note El nuevo tamaño se puede obtener mediante
		/// `GetSize()`.
		virtual void OnSizeChanged(const Vector2f& previousSize);


		/// @brief Reposiciona todas las llamadas de renderizado
		/// por un delta indicado.
		/// @param positionDiff Delta de posición que se aplicará
		/// a todas las llamadas.
		void RepositionDrawCalls(const Vector2f& positionDiff);

		/// @brief Reescala todas las llamadas de renderizado
		/// por un delta indicado.
		/// @param sizeDiff Delta de tamaño que se aplicará
		/// a todas las llamadas.
		void ResizeDrawCalls(const Vector2f& sizeDiff);


		/// @return Elemento padre.
		/// Puede ser nullptr.
		IContainer* GetParent();

		/// @return True si tiene un elemento padre.
		bool HasParent() const;

	private:

		DynamicArray<GRAPHICS::SdfDrawCall2D> m_drawCalls{};

		/// @brief Tamaño, en píxeles.
		Vector2f m_size = Vector2f::Zero;
		Anchor m_anchor = Anchor::DEFAULT;

		/// @brief Posición , en píxeles.
		Vector2f m_position = Vector2f::Zero;

		Vector4f m_padding = 0;
		Vector4f m_margin = 2.0f;

		bool m_isVisible = true;
		bool m_isLocked = false;

		/// @brief True si se debe conservar el tamaño relativo de este elemento
		/// cuando el elemento padre sea cambiado de tamaño.
		bool m_keepRelativeSize = false;

		/// @brief (Opcional) padre de este elemento.
		IContainer* m_parent = nullptr;

	};

}
