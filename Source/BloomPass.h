#pragma once

#include "IPostProcessPass.h"
#include "NumericTypes.h"
#include <array>
#include "UniquePtr.hpp"
#include "MaterialInstance.h"

namespace OSK::GRAPHICS {

	class ICommandList;
	class IGpuStorageBuffer;
	class Material;


	/// @brief Clase auxiliar que permite una capa de bloom / resplandor.
	/// 
	/// Partiendo de una imagen de entrada (preferiblemente
	/// con un formato HDR) genera una nueva imagen con un resplandor
	/// alrededor de las zonas más brillantes de la imagen de entrada.
	/// 
	/// La imagen final contiene la imagen original + resplandor.
	/// 
	/// Usa shaders de computación para ejecutar un algoritmo basado en: 
	/// http://www.iryoku.com/next-generation-post-processing-in-call-of-duty-advanced-warfare
	class OSKAPI_CALL BloomPass : public IPostProcessPass {

	public:

		constexpr static auto DownscaleMaterial = "Resources/Materials/PostProcess/Bloom/downscale.json";
		constexpr static auto UpscaleMaterial = "Resources/Materials/PostProcess/Bloom/upscale.json";
		constexpr static auto ResolveMaterial = "Resources/Materials/PostProcess/Bloom/final.json";

		void SetInput(GpuImage* inputImage);

		void Create(const Vector2ui& size) override;
		void Resize(const Vector2ui& size) override;

		void Execute(ICommandList* computeCmdList) override;

	private:

		/// @brief Establece las imágenes intermedias de bloom en 
		/// los material instance.
		void SetupMaterialInstances();


		/// @brief Copia la imagen de la escena a la cadena de mip-maps
		/// para poder comenzar la ejecución del bloom.
		/// @param computeCmdList Lista de comandos usada.
		void InitialCopy(ICommandList* computeCmdList);

		/// @brief Ejecuta un escalado o desescalado de un nivel de mip a otro.
		/// @param computeCmdList Lista de comandos donde se ejecutará.
		/// @param sourceMipLevel Nivel de mip de origen.
		/// @param destMipLevel Nivel de mip que se escribirá.
		/// 
		/// @pre computeCmdList debe ser una lista de comandos de computación. 
		void ExecuteSinglePass(
			ICommandList* computeCmdList, 
			UIndex32 sourceMipLevel,
			UIndex32 destMipLevel);
		
		/// <summary>
		/// Procesa el proceso de desescalado completo.
		/// </summary>
		/// <param name="computeCmdList">Lista de comandos donde se ejecutará.</param>
		/// <param name="res">Puntero a un vector 2D que contiene la resolución del pase actual.</param>
		/// 
		/// @pre computeCmdList debe ser una lista de comandos de computación. 
		/// 
		/// @post *res será la resolución después de haberse realizado todo el desescalado.
		void DownscaleBloom(ICommandList* computeCmdList);

		/// <summary>
		/// Procesa el proceso de escalado completo.
		/// </summary>
		/// <param name="computeCmdList">Lista de comandos donde se ejecutará.</param>
		/// <param name="res">Puntero a un vector 2D que contiene la resolución del pase actual.</param>
		/// 
		/// @pre computeCmdList debe ser una lista de comandos de computación. 
		/// 
		/// @post *res será la resolución después de haberse realizado todo el escalado.
		void UpscaleBloom(ICommandList* computeCmdList);

		/// @brief Ejecuta el paso final de la ejecución.
		/// @param computeCmdList Lista de comandos.
		void Resolve(ICommandList* computeCmdList);


		/// @brief Calcula el número de pases, de acuerdo al número
		/// de mip-levels del render target.
		/// @return Número de pases de downscale/upscale.
		UIndex32 GetNumPasses() const;


		/// @brief Número máximo de pases de downscale/upscale.
		const static UIndex32 maxNumPasses = 8;

	private:

		/// @brief Instancias de los materiales de downscale.
		/// Hay una instancia por cada paso de downscale, configurada
		/// con los mip-maps necesistados por cada paso.
		std::array<UniquePtr<MaterialInstance>, maxNumPasses> m_downscalingMaterialInstances{};

		/// @brief Instancias de los materiales de upscale.
		/// Hay una instancia por cada paso de upscale, configurada
		/// con los mip-maps necesistados por cada paso.
		/// 
		/// @note Aunque tiene una instancia para el nivel 0, esta no se debe usar:
		/// se debe usar @p resolveInstance.
		std::array<UniquePtr<MaterialInstance>, maxNumPasses> m_upscalingMaterialInstances{};

		/// @brief Instancia del material final.
		UniquePtr<MaterialInstance> m_resolveInstance;

		GpuImage* m_inputImage = nullptr;

		Material* m_downscaleMaterial = nullptr;
		Material* m_upscaleMaterial = nullptr;
		Material* m_resolveMaterial = nullptr;

	};

}
