#pragma once

#include <array>

#include "ApiCall.h"

#include "RgBufferDependency.h"
#include "RgImageDependency.h"
#include "GdrDeferredPass.h"

#include "ResourcesInFlight.h"
#include "RenderPass.h"


namespace OSK::GRAPHICS {

	class RenderGraph;


	/// @brief Clase de utilidad para manejar
	/// que encapsula la configuración por defecto
	/// del rendergraph.
	class OSKAPI_CALL GdrPipeline {

	public:

		explicit GdrPipeline(RenderGraph* renderGraph);

		/// @brief Actualiza los contenidos de los buffers
		/// de cámaras.
		/// 
		/// @param frameIndex Índice del recurso a escribir.
		/// @param cameraInfo Información de la cámara en este frame.
		/// @param previousCameraInfo Información de la cámara
		/// en el frame anterior.
		/// 
		/// @pre See debe haber registrado los buffers
		/// mediante `RegisterAll``.
		void UpdateCameraBuffers(
			UIndex16 frameIndex,
			const GdrDeferredPass::CameraInfo& cameraInfo,
			const GdrDeferredPass::PreviousCameraInfo& previousCameraInfo);


		/// @brief Registro parcial de lo siguiente:
		/// - Imágenes de color y profundidad del GBuffer.
		/// - Buffers de cámara.
		/// - Buffers de cámara de frame anterior.
		/// - Pase de renderizado diferido.
		void RegisterAll();

		/// @brief Registro completo de lo siguiente:
		/// - Pase de renderizado diferido.
		/// 
		/// @pre Se debe haber hecho el registro previo 
		/// mediante `RegisterAll()`.
		void FinalRegisterAll();

	private:

		/// @brief Registra parcialmente el pase diferido.
		/// @pre Se debe haber registrado previamente el
		/// GBuffer mediante `RegisterGBuffer`.
		/// @pre Se deben haber registrado previamente
		/// los buffers de cámaras actual y previa
		/// mediante `RegisterCameras3D`.
		void RegisterDeferredPass();

		/// @brief Registro final del pase diferido.
		/// @pre Se debe haber registrado parcialmente
		/// el pase mediante `RegisterDeferredPass`.
		void FinalRegisterDeferredPass();

		/// @brief Registra todas las imágenes del GBuffer,
		/// tanto las de color como las de profundidad.
		void RegisterGBuffer();

		/// @brief Registra los buffers de la cámara
		/// y de la cámara del frame anterior.
		/// 
		/// Habrá @p MAX_RESOURCES_IN_FLIGHT buffers.
		void RegisterCameras3D();

		RenderGraph* m_renderGraph = nullptr;

		RenderPassUuid m_deferredPass = RenderPassUuid::CreateEmpty();

		/// @brief Imágenes de renderizado del GBuffer
		/// (sin contar profundidad).
		std::array<GdrImageUuid, 5> m_gBufferUuids;
		GdrImageUuid m_depthImageUuid = GdrImageUuid::CreateEmpty();

		std::array<GdrBufferUuid, MAX_RESOURCES_IN_FLIGHT> m_cameraBuffer;
		std::array<GdrBufferUuid, MAX_RESOURCES_IN_FLIGHT> m_previousCameraBuffer;

	};

}
