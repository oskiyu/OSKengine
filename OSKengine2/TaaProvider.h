#pragma once

#include "UniquePtr.hpp"
#include "RtRenderTarget.h"
#include "MaterialInstance.h"
#include "IRenderSystem.h"
#include "GpuBarrierInfo.h"
#include "GpuImageLayout.h"

namespace OSK::GRAPHICS {

	/// @brief Clase que contiene la funcionalidad del renderizado
	/// de Temporal Anti-Aliasing.
	class OSKAPI_CALL TaaProvider final {

	public:

		/// @brief Inicializa el sistema de Temporal Anti-Aliasing.
		/// @param resolution Resoluci�n, en p�xeles.
		/// @param inputImages Im�genes de entrada para TAA.
		/// @param motionImages Im�genes de velocidad.
		/// 
		/// @pre Las im�genes @p inputImages deben tener la misma resoluci�n que @p resolution.
		/// @pre Las im�genes @p motionImages deben tener la misma resoluci�n que @p resolution.
		/// 
		/// @pre Las im�genes @p motionImages deben producir los vectores de movimiento normalizados (NDC).
		void InitializeTaa(
			const Vector2ui& resolution,
			const GpuImage* inputImages[NUM_RESOURCES_IN_FLIGHT],
			const GpuImage* motionImages[NUM_RESOURCES_IN_FLIGHT]);

		/// @brief Cambia de resoluci�n el sistema de Temporal Anti-Aliasing.
		/// Adem�s, reestablece las im�genes de entrada.
		/// @param resolution Resoluci�n, en p�xeles.
		/// @param inputImages Im�genes de entrada para TAA.
		/// @param motionImages Im�genes de velocidad.
		/// 
		/// @pre Las im�genes @p inputImages deben tener la misma resoluci�n que @p resolution.
		/// @pre Las im�genes @p motionImages deben tener la misma resoluci�n que @p resolution.
		void ResizeTaa(
			const Vector2ui& resolution,
			const GpuImage* inputImages[NUM_RESOURCES_IN_FLIGHT],
			const GpuImage* motionImages[NUM_RESOURCES_IN_FLIGHT]);


		/// @brief Establace el �ndice m�ximo de jitter.
		/// EL jitter en cada frame estar� entre 1 y maxJitter (ambos incluidos).
		/// Si TAA est� desactivado, el jitter ser� 0.
		/// @param maxJitterIndex �nidice m�ximo de jitter.
		inline void SetMaxJitter(TIndex maxJitterIndex) { this->maxJitterIndex = maxJitterIndex; }

		/// @return �ndice de jitter de este frame.
		/// Si est� desactivado, ser� 0.
		inline TIndex GetCurrentFrameJitterIndex() const { 
			return isActive ? currentFrameJitterIndex : 0; 
		}


		inline bool IsActive() const { return isActive; }
		inline void SetActivationStatus(bool isActive) { this->isActive = isActive; }
		inline void Activate() { SetActivationStatus(false); }
		inline void Dectivate() { SetActivationStatus(false); }
		inline void ToggleActivation() { SetActivationStatus(!IsActive()); }


		/// @brief Ejecuta el sistema de Temporal Anti-Aliasing.
		/// @param commandList Lista de comandos sobre la que se va a ejecutar.
		/// 
		/// @pre Debe haberse inicializado correctamente el sistema TAA mediante
		/// InitializeTaa.
		/// 
		/// @pre La imagen de entrada debe tener el layout devuelto por GetTaaSourceLayout().
		/// @pre La imagen de entrada debe ser sincronizada usando el GpuBarrierInfo devuelto
		/// por GetTaaSourceBarrierInfo() como barrier @p next.
		void ExecuteTaa(ICommandList* commandList);

		/// @return Layout que debe tener la imagen de entrada del sistema TAA antes de su ejecuci�n.
		inline constexpr static 
		GpuImageLayout GetTaaSourceLayout() { return GpuImageLayout::SAMPLED; }

		/// @return GpuBarrierInfo que se debe usarse como @p next en la sincronizaci�n de
		/// la imagen de entrada del sistema TAA.
		inline static 
		GpuBarrierInfo GetTaaSourceBarrierInfo() {
			return GpuBarrierInfo(
				GpuBarrierStage::COMPUTE_SHADER,
				GpuBarrierAccessStage::SHADER_WRITE);
		}

		/// @return GpuBarrierInfo que se debe usarse como @p previous en la sincronizaci�n de
		/// la imagen de salida del sistema TAA.
		inline static 
		GpuBarrierInfo GetTaaOutputBarrierInfo() {
			return GpuBarrierInfo(
				GpuBarrierStage::COMPUTE_SHADER,
				GpuBarrierAccessStage::SHADER_WRITE);
		}

		/// @return Render target sobre el que se ejecuta el TAA.
		inline const ComputeRenderTarget& GetTaaOutput() const { return taaSharpenedRenderTarget; }

	private:

		static std::array<const IGpuImageView*, NUM_RESOURCES_IN_FLIGHT> GetViews(
			const GpuImage* image[NUM_RESOURCES_IN_FLIGHT],
			GpuImageViewConfig config);

		/// @brief True si se usa TAA, falso en caso contrario.
		bool isActive = true;

		/// @brief N�mero m�ximo del �ndice de jitter.
		TIndex maxJitterIndex = 4;
		/// @brief �ndice del jitter en el frame actual.
		TIndex currentFrameJitterIndex = 0;

		/// @brief Carga los materiales de TAA (tanto del pase de acumulaci�n
		/// temporal como el pase de afilado de imagen).
		void LoadTaaMaterials();

		/// @brief Establece / actualiza ambos materiales de TAA.
		/// @param inputImages Im�genes de entrada de TAA.
		/// @param motionImages Im�genes de vectores de movimiento.
		void SetupTaaMaterials(
			const GpuImage* inputImages[NUM_RESOURCES_IN_FLIGHT],
			const GpuImage* motionImages[NUM_RESOURCES_IN_FLIGHT]);

		/// @brief Ejecuta el pase de acumulaci�n temporal.
		/// @param commandList Lista de comandos.
		void ExecuteTaaFirstPass(ICommandList* commandList);
		/// @brief Ejecuta el pase de afilado de imagen.
		/// @param commandList Lista de comandos.
		void ExecuteTaaSharpening(ICommandList* commandList);

		/// @brief Render target para el pase de acumulaci�n temporal de TAA.
		ComputeRenderTarget taaRenderTarget;
		/// @brief Render target para el pase de image sharpening de TAA.
		ComputeRenderTarget taaSharpenedRenderTarget;

		/// @brief Instancia del material para el pase de acumulaci�n
		/// temporal de TAA.
		UniquePtr<MaterialInstance> taaMaterialInstance;
		/// @brief Instancia del material para el pase de image sharpening
		/// de TAA.
		UniquePtr<MaterialInstance> taaSharpenMaterialInstance;

	};

}
