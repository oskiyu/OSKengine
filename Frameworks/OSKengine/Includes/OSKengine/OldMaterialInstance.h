#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "DescriptorSet.h"
#include "Texture.h"

#include "SharedPtr.hpp"

namespace OSK {

	class OldMaterial;
	class OldMaterialPool;

	//Instancia de un material.

	/// <summary>
	/// Instancia de un material.
	/// Puede definir los buffers y las texturas para uno o varios modelos / sprites.
	/// </summary>
	class OSKAPI_CALL OldMaterialInstance {

		friend class OldMaterial;
		friend class OldMaterialPool;

	public:

		/// <summary>
		/// Elimina la instancia.
		/// <summary/>
		~OldMaterialInstance();

		/// <summary>
		/// Elimina la instancia.
		/// El descriptor set de la instancia queda liberado,
		/// y puede ser usado por otra instancia.
		/// </summary>
		void Free();

		/// <summary>
		/// Establece la textura del pr�ximo binding.
		/// </summary>
		/// <param name="texture">Textura</param>
		void SetTexture(Texture* texture);

		/// <summary>
		/// Establece la textura del binding dado.
		/// </summary>
		/// <param name="texture">Textura.</param>
		/// <param name="binding">Binding.</param>
		void SetTexture(Texture* texture, uint32_t binding);

		/// <summary>
		/// Establece la textura del binding dado.
		/// </summary>
		/// <param name="name">Binding.</param>
		/// <param name="texture">Textura.</param>
		void SetTexture(const std::string& name, Texture* texture);


		/// <summary>
		/// Establece los buffers del pr�ximo binding.
		/// </summary>
		/// <param name="buffers">Buffers.</param>
		void SetBuffer(std::vector<SharedPtr<GpuDataBuffer>>& buffers);

		/// <summary>
		/// Establece los buffers del binding dado.
		/// </summary>
		/// <param name="buffers">Buffers.</param>
		/// <param name="binding">Binding.</param>
		void SetBuffer(std::vector<SharedPtr<GpuDataBuffer>>& buffers, uint32_t binding);

		/// <summary>
		/// Establece los buffers del binding dado.
		/// </summary>
		/// <param name="name">Binding.</param>
		/// <param name="buffers">Buffers.</param>
		void SetBuffer(const std::string& name, std::vector<SharedPtr<GpuDataBuffer>>& buffers);

		/// <summary>
		/// Establece los buffers del pr�ximo binding.
		/// </summary>
		/// <param name="buffers">Buffers.</param>
		void SetDynamicBuffer(std::vector<SharedPtr<GpuDataBuffer>>& buffers);

		/// <summary>
		/// Establece los buffers del binding dado.
		/// </summary>
		/// <param name="buffers">Buffers.</param>
		/// <param name="binding">Binding.</param>
		void SetDynamicBuffer(std::vector<SharedPtr<GpuDataBuffer>>& buffers, uint32_t binding);

		/// <summary>
		/// Establece los buffers del binding dado.
		/// </summary>
		/// <param name="name">Binding.</param>
		/// <param name="buffers">Buffers.</param>
		void SetDynamicBuffer(const std::string& name, std::vector<SharedPtr<GpuDataBuffer>>& buffers);

		/// <summary>
		/// Envia el valor de los bindings a la GPU.
		/// Puede usarse para cambiar ciertos bindings en tiempo de ejecuci�n.
		/// </summary>
		void FlushUpdate();

		/// <summary>
		/// Obtiene el descriptor set de la instancia.
		/// </summary>
		/// <returns>Descriptor set.</returns>
		DescriptorSet* GetDescriptorSet() const;

		/// <summary>
		/// Devuelve true si los bindings han sido establecidos.
		/// </summary>
		/// <returns>Estado de la instancia.</returns>
		bool HasBeenSet() const;

	private:

		/// <summary>
		/// Material al que pertenece la instancia.
		/// </summary>
		OldMaterial* ownerMaterial = nullptr;

		/// <summary>
		/// Material pool al que pertenece la instancia.
		/// </summary>
		OldMaterialPool* ownerPool = nullptr;

		/// <summary>
		/// Posici�n del descriptor set de la instancia en el array del Material Pool due�o.
		/// </summary>
		uint32_t descriptorSetIndex = 0;

		/// <summary>
		/// True si los bindings han sido establecidos.
		/// </summary>
		bool hasBeenSet = false;
		
		/// <summary>
		/// Buffers.
		/// </summary>
		std::vector<std::vector<SharedPtr<GpuDataBuffer>>> buffers;

	};

}