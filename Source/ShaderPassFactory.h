#pragma once

#include "ApiCall.h"
#include "OwnedPtr.h"

#include <string>

#include <functional>

#include "IShaderPass.h"
#include "MutexHolder.h"
#include "HashMap.hpp"

namespace OSK::GRAPHICS {

	/// @brief Clase que es capaz de generar pases
	/// de renderizado a partir de sus nombres.
	/// @threadsafe
	class OSKAPI_CALL ShaderPassFactory {

	public:

		using FactoryMethod = OwnedPtr<IShaderPass>(*)();

	public:

		/// @brief Tegistra un pase para poder ser creado
		/// posteriormente.
		/// @tparam TShaderPass Tipo de pase.
		/// @threadsafe
		template <typename TShaderPass>
		void RegisterShaderPass() {
			RegisterShaderPass(
				static_cast<std::string>(TShaderPass::GetRenderPassName()),
				TShaderPass::CreateInstance);
		}

		/// @brief Tegistra un pase para poder ser creado
		/// posteriormente.
		/// @param name Nombre del pase.
		/// @param method Método de generación del pase.
		/// @threadsafe
		void RegisterShaderPass(const std::string& name, FactoryMethod method);

		/// @param name Nombre del pase.
		/// @return True si el pase ha sido registrado.
		/// @threadsafe
		bool ContainsShaderPass(std::string_view name) const;

		/// @param name Nombre del pase.
		/// @return Pase creado.
		/// @threadsafe
		/// 
		/// @pre El pase debe haber sido previamente registrado 
		/// (comprobable mediante ContainsShaderPass).
		/// @throws InvalidArgumentException si se incumple la
		/// precondición.
		OwnedPtr<IShaderPass> CreatePass(std::string_view name) const;

	private:

		std::unordered_map<std::string, FactoryMethod, StringHasher, std::equal_to<>> m_methods{};
		mutable MutexHolder m_mutex{};

	};

}
