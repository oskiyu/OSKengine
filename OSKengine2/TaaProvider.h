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
		/// @param resolution Resolución, en píxeles.
		/// @param inputImages Imágenes de entrada para TAA.
		/// @param motionImages Imágenes de velocidad.
		/// 
		/// @pre Las imágenes @p inputImages deben tener la misma resolución que @p resolution.
		/// @pre Las imágenes @p motionImages deben tener la misma resolución que @p resolution.
		/// 
		/// @pre Las imágenes @p motionImages deben producir los vectores de movimiento normalizados (NDC).
		void InitializeTaa(
			const Vector2ui& resolution,
			const GpuImage* inputImage,
			const GpuImage* motionImages);

		/// @brief Cambia de resolución el sistema de Temporal Anti-Aliasing.
		/// Además, reestablece las imágenes de entrada.
		/// @param resolution Resolución, en píxeles.
		/// @param inputImages Imágenes de entrada para TAA.
		/// @param motionImages Imágenes de velocidad.
		/// 
		/// @pre Las imágenes @p inputImages deben tener la misma resolución que @p resolution.
		/// @pre Las imágenes @p motionImages deben tener la misma resolución que @p resolution.
		void ResizeTaa(
			const Vector2ui& resolution,
			const GpuImage* inputImages,
			const GpuImage* motionImages);


		/// @brief Establace el índice máximo de jitter.
		/// EL jitter en cada frame estará entre 1 y maxJitter (ambos incluidos).
		/// Si TAA está desactivado, el jitter será 0.
		/// @param maxJitterIndex Ínidice máximo de jitter.
		inline void SetMaxJitter(UIndex32 maxJitterIndex) { m_maxJitterIndex = maxJitterIndex; }

		/// @return Índice de jitter de este frame.
		/// Si está desactivado, será 0.
		inline UIndex32 GetCurrentFrameJitterIndex() const {
			return m_isActive ? m_currentFrameJitterIndex : 0; 
		}


		inline bool IsActive() const { return m_isActive; }
		inline void SetActivationStatus(bool isActive) { m_isActive = isActive; }
		inline void Activate() { SetActivationStatus(false); }
		inline void Dectivate() { SetActivationStatus(false); }
		inline void ToggleActivation() { SetActivationStatus(!IsActive()); }


		/// @brief Ejecuta el sistema de Temporal Anti-Aliasing.
		/// @param commandList Lista de comandos sobre la que se va a ejecutar.
		/// 
		/// @pre Debe haberse inicializado correctamente el sistema TAA mediante
		/// InitializeTaa.
		void ExecuteTaa(ICommandList* commandList);

		/// @return Render target sobre el que se ejecuta el TAA.
		inline ComputeRenderTarget& GetTaaOutput() { return m_taaSharpenedRenderTarget; }

		/// @return Render target sobre el que se ejecuta el TAA.
		inline const ComputeRenderTarget& GetTaaOutput() const { return m_taaSharpenedRenderTarget; }

	private:

		/// @brief True si se usa TAA, falso en caso contrario.
		bool m_isActive = true;

		/// @brief Número máximo del índice de jitter.
		UIndex32 m_maxJitterIndex = 4;
		/// @brief Índice del jitter en el frame actual.
		UIndex32 m_currentFrameJitterIndex = 0;

		/// @brief Carga los materiales de TAA (tanto del pase de acumulación
		/// temporal como el pase de afilado de imagen).
		void LoadTaaMaterials();

		/// @brief Establece / actualiza ambos materiales de TAA.
		/// @param inputImages Imágenes de entrada de TAA.
		/// @param motionImages Imágenes de vectores de movimiento.
		void SetupTaaMaterials(
			const GpuImage* inputImage,
			const GpuImage* motionImage);

		/// @brief Ejecuta el pase de acumulación temporal.
		/// @param commandList Lista de comandos.
		void ExecuteTaaFirstPass(ICommandList* commandList);

		/// @brief Copia la imagen TAA a la imagen histórica.
		/// @param commandList Lista de comandos.
		void CopyToHistoricalImage(ICommandList* commandList);

		/// @brief Ejecuta el pase de afilado de imagen.
		/// @param commandList Lista de comandos.
		void ExecuteTaaSharpening(ICommandList* commandList);


		/// @brief Contiene la imagen histórica.
		ComputeRenderTarget m_taaHistoricalImage;

		/// @brief Render target para el pase de acumulación temporal de TAA.
		ComputeRenderTarget m_taaRenderTarget;

		/// @brief Render target para el pase de image sharpening de TAA.
		ComputeRenderTarget m_taaSharpenedRenderTarget;


		/// @brief Instancia del material para el pase de acumulación
		/// temporal de TAA.
		UniquePtr<MaterialInstance> m_taaMaterialInstance;

		/// @brief Instancia del material para el pase de image sharpening
		/// de TAA.
		UniquePtr<MaterialInstance> m_taaSharpenMaterialInstance;

	};

}
