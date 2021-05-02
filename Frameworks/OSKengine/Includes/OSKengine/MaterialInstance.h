#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include "DescriptorSet.h"
#include "Texture.h"

#include "SharedPtr.hpp"

namespace OSK {

	class Material;
	class MaterialPool;

	//Instancia de un material.

	/// <summary>
	/// Instancia de un material.
	/// Puede definir los buffers y las texturas para uno o varios modelos / sprites.
	/// </summary>
	class OSKAPI_CALL MaterialInstance {

		friend class Material;
		friend class MaterialPool;

	public:

		/// <summary>
		/// Elimina la instancia.
		/// <summary/>
		~MaterialInstance();

		/// <summary>
		/// Elimina la instancia.
		/// El descriptor set de la instancia queda liberado,
		/// y puede ser usado por otra instancia.
		/// </summary>
		void Free();

		/// <summary>
		/// Establece la textura del próximo binding.
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
		/// Establece los buffers del próximo binding.
		/// </summary>
		/// <param name="buffers">Buffers.</param>
		void SetBuffer(std::vector<GPUDataBuffer>& buffers);

		/// <summary>
		/// Establece los buffers del binding dado.
		/// </summary>
		/// <param name="buffers">Buffers.</param>
		/// <param name="binding">Binding.</param>
		void SetBuffer(std::vector<GPUDataBuffer>& buffers, uint32_t binding);

		/// <summary>
		/// Establece los buffers del binding dado.
		/// </summary>
		/// <param name="name">Binding.</param>
		/// <param name="buffers">Buffers.</param>
		void SetBuffer(const std::string& name, std::vector<GPUDataBuffer>& buffers);

		/// <summary>
		/// Establece los buffers del próximo binding.
		/// </summary>
		/// <param name="buffers">Buffers.</param>
		void SetDynamicBuffer(std::vector<GPUDataBuffer>& buffers);

		/// <summary>
		/// Establece los buffers del binding dado.
		/// </summary>
		/// <param name="buffers">Buffers.</param>
		/// <param name="binding">Binding.</param>
		void SetDynamicBuffer(std::vector<GPUDataBuffer>& buffers, uint32_t binding);

		/// <summary>
		/// Establece los buffers del binding dado.
		/// </summary>
		/// <param name="name">Binding.</param>
		/// <param name="buffers">Buffers.</param>
		void SetDynamicBuffer(const std::string& name, std::vector<GPUDataBuffer>& buffers);

		/// <summary>
		/// Envia el valor de los bindings a la GPU.
		/// Puede usarse para cambiar ciertos bindings en tiempo de ejecución.
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
		Material* OwnerMaterial = nullptr;

		/// <summary>
		/// Material pool al que pertenece la instancia.
		/// </summary>
		MaterialPool* OwnerPool = nullptr;

		/// <summary>
		/// Posición del descriptor set de la instancia en el array del Material Pool dueño.
		/// </summary>
		uint32_t DSet = 0;

		/// <summary>
		/// True si los bindings han sido establecidos.
		/// </summary>
		bool hasBeenSet = false;
		
	};

}
