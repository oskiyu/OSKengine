#pragma once

#include "ApiCall.h"

#include "Vector3.hpp"
#include "Vector4.hpp"

#include "DynamicArray.hpp"
#include "UniquePtr.hpp"
#include "GpuBuffer.h"
#include "Color.hpp"
#include "RenderpassType.h"

#include "IGpuObject.h"

// Para `GpuImageRange = {}`.
#include "IGpuImage.h"

// Para `ResourceQueueTransfer::Empty()`.
#include "ResourceQueueTransfer.h"

// Para `GpuImageRange::{}`.
#include "GpuImageRange.h"

#include "Viewport.h"

#include <string>


namespace OSK::GRAPHICS {

	// Materiales.
	class Material;
	class IMaterialSlot;
	class MaterialInstance;

	// Pipelines.
	class IGraphicsPipeline;
	class IRaytracingPipeline;
	class IComputePipeline;

	// Imágenes.
	class GpuImage;
	enum class GpuImageLayout;
	class CopyImageInfo;
	class RenderTarget;

	// Buffers.
	struct GpuBufferRange;

	// Barriers.
	struct GpuBarrierInfo;

	// Otros.
	class ICommandPool;


	struct RenderPassImageInfo {
		GpuImage* targetImage = nullptr;
		UIndex32 arrayLevel = 0;
		bool clear = true;
	};

	enum class RenderpassAutoSync {
		ON,
		OFF
	};


	/// @brief Una lista de comandos contiene una serie de comandos que serán
	/// enviados a la GPU para su ejecución.
	/// La lista es creada por una pool de comandos, y se introduce
	/// en una cola de comandos para su ejecución.
	/// 
	/// @note Los comandos serán ejecutados una vez se envíe la lista
	/// a la GPU.
	class OSKAPI_CALL ICommandList : public IGpuObject {

	public:

		~ICommandList() override;

		OSK_DEFINE_AS(ICommandList);


		/// @brief Vacía la lista de comandos.
		/// 
		/// @pre Debe estar cerrado.
		/// @note Debe llamarse antes de comenzar a grabar comandos.
		/// 
		/// @throws CommandListResetException Si hubo algún error en el API de bajo nivel.
		virtual void Reset() = 0;

		/// @brief Comienza a grabar comandos en la lista.
		/// @pre Debe estar cerrado.
		/// 
		/// @throws CommandListStartException Si hubo algún error en el API de bajo nivel.
		virtual void Start() = 0;

		/// @brief Cierra la lista de comandos, y la deja lista para ser enviada
		/// a la GPU.
		/// 
		/// @pre Debe estar abierto.
		/// 
		/// @throws CommandListEndException Si hubo algún error en el API de bajo nivel.
		virtual void Close() = 0;


		/// @brief Establece un barrier que sincroniza la ejecución de comandos.
		/// También cambia el layout de la imagen.
		/// 
		/// @see GpuImageBarrierInfo.
		/// 
		/// @param image Imagen a la que se le cambiará el layout.
		/// @param previousLayout Layout anterior.
		/// @param nextLayout Nuevo layout.
		/// @param previous Stage previo.
		/// @param next Stage siguiente.
		/// @param range Información sobre que subrecursos de la imagen serán afectados.
		/// @param queueTranfer Información sobre la transferencia de la imagen de una cola a otra.
		///
		/// @note Se debe cambiar el layout de la imagen antes de ejecutar un comando sobre ella,
		/// si su layout actual no coincide con el necesario.
		/// 
		/// @pre La lista de comandos debe estar abierta.
		/// @pre previousLayout debe ser el layout de la imagen antes de este comando.
		/// 
		/// @post Los subrecursos especificados por imageInfo estarán en el nuevo layout.
		virtual void SetGpuImageBarrier(
			GpuImage* image, 
			GpuImageLayout previousLayout, 
			GpuImageLayout nextLayout,
			GpuBarrierInfo previous, 
			GpuBarrierInfo next, 
			const GpuImageRange& range = {},
			const ResourceQueueTransferInfo queueTranfer = ResourceQueueTransferInfo::Empty()) = 0;

		/// @brief Establece un barrier que sincroniza la ejecución de comandos.
		/// Cambia el layout de la imagen.
		/// 
		/// @param image Imagen a la que se le cambiará el layout.
		/// @param nextLayout Nuevo layout.
		/// @param previous Stage previo.
		/// @param next Stage siguiente.
		/// @param range Información sobre que subrecursos de la imagen serán afectados.
		/// @param queueTranfer Información sobre la transferencia de la imagen de una cola a otra.
		/// 
		/// @note Se debe cambiar el layout de la imagen antes de ejecutar un comando sobre ella,
		/// si su layout actual no coincide con el necesario.
		/// 
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @post Los subrecursos especificados por imageInfo estarán en el nuevo layout.
		void SetGpuImageBarrier(
			GpuImage* image,
			GpuImageLayout nextLayout,
			GpuBarrierInfo previous,
			GpuBarrierInfo next,
			const GpuImageRange& range = {},
			const ResourceQueueTransferInfo queueTranfer = ResourceQueueTransferInfo::Empty());

		/// @brief Establece un barrier que sincroniza la ejecución de comandos.
		/// Cambia el layout de la imagen.
		/// 
		/// @param image Imagen a la que se le cambiará el layout.
		/// @param nextLayout Nuevo layout.
		/// @param next Stage siguiente.
		/// @param range Información sobre que subrecursos de la imagen serán afectados.
		/// @param queueTranfer Información sobre la transferencia de la imagen de una cola a otra.
		/// 
		/// @note Se debe cambiar el layout de la imagen antes de ejecutar un comando sobre ella,
		/// si su layout actual no coincide con el necesario.
		/// 
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @post Los subrecursos especificados por imageInfo estarán en el nuevo layout.
		void SetGpuImageBarrier(
			GpuImage* image,
			GpuImageLayout nextLayout,
			GpuBarrierInfo next,
			const GpuImageRange& range = {},
			const ResourceQueueTransferInfo queueTranfer = ResourceQueueTransferInfo::Empty());


		/// @brief Establece un barrier que sincroniza la ejecución de comandos que
		/// usan un buffer.
		/// @param buffer Buffer usado.
		/// @param range Rango del buffer usado.
		/// @param previous Stage previo.
		/// @param next Stage siguiente.
		/// @param queueTranfer Información sobre la transferencia de la imagen de una cola a otra.
		/// 
		/// @pre La lista de comandos debe estar abierta.
		virtual void SetGpuBufferBarrier(
			GpuBuffer* buffer,
			const GpuBufferRange& range,
			GpuBarrierInfo previous,
			GpuBarrierInfo next,
			const ResourceQueueTransferInfo queueTranfer = ResourceQueueTransferInfo::Empty()) = 0;

		/// @brief Establece un barrier que sincroniza la ejecución de comandos que
		/// usan un buffer.
		/// @param buffer Buffer usado.
		/// @param range Rango del buffer usado.
		/// @param next Stage siguiente.
		/// @param queueTranfer Información sobre la transferencia de la imagen de una cola a otra.
		/// 
		/// @pre La lista de comandos debe estar abierta.
		void SetGpuBufferBarrier(
			GpuBuffer* buffer,
			const GpuBufferRange& range,
			GpuBarrierInfo next,
			const ResourceQueueTransferInfo queueTranfer = ResourceQueueTransferInfo::Empty());

#pragma region Pases

		/// @brief Comienza el renderizado a un render target.
		/// @param renderTarget Render target sobre el que se renderizará.
		/// @param color Color con el que limpiará la imagen.
		/// @param autoSync True para introducir barriers automáticamente,
		/// false en caso contrario.
		/// 
		/// @note Por defecto, se limpiará la imagen con color negro.
		/// @note Se encarga de establecer el layout de las imágenes de color y de profundidad.
		/// 
		/// @pre No debe haber ningún renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @post Las imágenes de color se encontrarán en `GpuImageLayout::COLOR_ATTACHMENT`.
		/// @post La imagen de profundidad se encontrarán en `GpuImageLayout::DEPTH_STENCIL_TARGET`.
		/// 
		/// @note El tipo de comando es de GRÁFICOS.
		/// @pre El pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		/// @pre La cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		void BeginGraphicsRenderpass(
			RenderTarget* renderTarget,
			const Color& color = Color::Black,
			bool autoSync = true);

		/// @brief Comienza el renderizado a las imágenes dadas.
		/// @param colorImages Imágenes de color sobre las que se renderizará.
		/// @param depthImage Imagen de profundidad.
		/// @param color Color con el que limpiará la imagen.
		/// @param autoSync True para introducir barriers automáticamente,
		/// false en caso contrario.
		/// 
		/// @note Por defecto, se limpiará la imagen con color negro.
		/// @note Se encarga de establecer el layout de las imágenes de color y de profundidad.
		/// 
		/// @pre No debe haber ningún renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @post Las imágenes de color se encontrarán en GpuImageLayout::COLOR_ATTACHMENT.
		/// @post La imagen de profundidad se encontrarán en GpuImageLayout::DEPTH_STENCIL_TARGET.
		/// 
		/// @note El tipo de comando es de GRÁFICOS.
		/// @pre El pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		/// @pre La cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		virtual void BeginGraphicsRenderpass(
			DynamicArray<RenderPassImageInfo> colorImages,
			RenderPassImageInfo depthImage,
			const Color& color = Color::Black,
			bool autoSync = true) = 0;

		/// @brief Limpia el contenido de una imagen fuera de un renderpass.
		/// @param image Imagen a limpiar.
		/// @param range Rango de la imagen que será limpiada.
		/// @param color Color con el que se limpiará (por defecto, negro).
		/// 
		/// @pre La lista de comandos debe estar abierta.
		/// @pre La lista de comandos no debe tener ningún renderpass activo.
		/// @pre @p range.channel debe ser SampledChannel::COLOR.
		/// 
		/// @post El layout de la imagen después de limpiarse será TRANSFER_DESTINATION.
		/// 
		/// @note El tipo de comando es de GRÁFICOS.
		/// @pre El pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		/// @pre La cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		virtual void ClearImage(
			GpuImage* image,
			const GpuImageRange& range = {},
			const Color& color = Color::Black) = 0;

		/// @brief Finaliza el renderizado a un render target.
		/// @param autoSync True para introducir barriers automáticamente,
		/// false en caso contrario.
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		/// @post Aplica a los color targets un GpuImageBarrier con nuevo layout = SHADER_READ_ONLY para 
		/// fragment shader.
		/// 
		/// @note El tipo de comando es de GRÁFICOS.
		/// @pre El pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		/// @pre La cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		virtual void EndGraphicsRenderpass(bool autoSync = true) = 0;


		/// @brief Establece el viewport a renderizar.
		/// El viewport describe el área de la patnalla que se va a renderizar.
		/// Además, establece la profundida mínima y máxima.
		/// @param viewport Información del viewport.
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @note El tipo de comando es de GRÁFICOS.
		/// @pre El pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		/// @pre La cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		virtual void SetViewport(const Viewport& viewport);

		/// @return Viewport usado en un momento dado.
		const Viewport& GetCurrentViewport() const;


		/// @brief Establece qué área del renderizado será visible en la textura final.
		/// @param scissor Área de renderizado.
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @note El tipo de comando es de GRÁFICOS.
		/// @pre El pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		/// @pre La cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		virtual void SetScissor(const Vector4ui& scissor) = 0;

#pragma endregion

#pragma region Binding

		/// @brief Establece el material que se va a usar a la hora de renderizar los próximos comandos.
		///
		/// @note Los slots que se enlacen después de este comando deben ser compatibles
		/// con este material.
		/// @note Los buffers de vértices que se enlacen después de este comando deben tener
		/// el tipo de vértice del material.
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre El material debe tener registrado el renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @warning Para materiales de rasterizado, debe estar activo el renderpass
		/// con el que se vaya a renderizar usando este material.
		/// 
		/// @pre Si es un material gráfico, el pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		/// @pre Si es un material gráfico, la cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		/// 
		/// @pre Si es un material de computación, el pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::COMPUTE`.
		/// @pre Si es un material de computación, la cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::COMPUTE`.
		void BindMaterial(const Material& material);


		/// @brief Establece el vertex buffer que se va a usar en los próximos renderizados.
		/// @param buffer Buffer con los vértices.
		/// @param view View que representa el rango a enlazar.
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @note Los materiales que se usen después de este comando deben tener
		/// el mismo tipo de vértice.
		/// 
		/// @note El tipo de comando es de GRÁFICOS.
		/// @pre El pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		/// @pre La cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		virtual void BindVertexBufferRange(const GpuBuffer& buffer, const VertexBufferView& view) = 0;

		/// @brief Establece el vertex buffer que se va a usar en los próximos renderizados.
		/// @param buffer Buffer con los vértices.
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		/// @pre El buffer debe tener inicializado su view de vértices.
		/// 
		/// @note Los materiales que se usen después de este comando deben tener
		/// el mismo tipo de vértice.
		/// 
		/// @throws InvalidVertexBufferException Si el buffer no tiene inicializado su view de vértices.
		/// 
		/// @note El tipo de comando es de GRÁFICOS.
		/// @pre El pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		/// @pre La cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		void BindVertexBuffer(const GpuBuffer& buffer);

		/// @brief Establece el index buffer que se va a usar en los próximos renderizados.
		/// @param buffer Buffer con los índices.
		/// @param view View que representa el rango a enlazar.
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @note El tipo de comando es de GRÁFICOS.
		/// @pre El pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		/// @pre La cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		virtual void BindIndexBufferRange(const GpuBuffer& buffer, const IndexBufferView& view) = 0;

		/// @brief Establece el index buffer que se va a usar en los próximos renderizados.
		/// @param buffer Buffer con los índices.
		/// 
		/// @pre El buffer debe tener inicializado su view de índices.
		/// @pre Debe haber un renderpass activo.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @throws InvalidIndexBufferException Si el buffer no tiene inicializado su view de índices.
		/// 
		/// @note El tipo de comando es de GRÁFICOS.
		/// @pre El pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		/// @pre La cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		void BindIndexBuffer(const GpuBuffer& buffer);


		/// @brief Establece todos los material slots de la instancia de material.
		/// @param instance Instancia del material.
		/// 
		/// @pre Debe haber un material enlazado.
		/// @pre El slot debe ser compatible con el material enlazado.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @pre El pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS` o `CommandsSupport::COMPUTE`.
		/// @pre La cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS` o `CommandsSupport::COMPUTE`.
		virtual void BindMaterialInstance(const MaterialInstance& instance);

		/// @brief Establece un material slot que estará asignado en los próximos comandos de renderizado.
		/// Pueden haber varios slots asignados en un mismo instante,
		/// siempre que no tengan el mismo id.
		/// @param slot Slot a establecer.
		/// 
		/// @pre Debe haber un renderpass activo, en caso de que sea un material gráfico.
		/// @pre Debe haber un material enlazado.
		/// @pre El slot debe ser compatible con el material enlazado.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @pre El pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS` o `CommandsSupport::COMPUTE`.
		/// @pre La cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS` o `CommandsSupport::COMPUTE`.
		virtual void BindMaterialSlot(const IMaterialSlot& slot) = 0;


		/// @brief Envía datos push constant al shader.
		/// @param pushConstName Nombre de los push const en el shader.
		/// @param data Información a enviar.
		/// @param size Número de bytes a enviar.
		/// 
		/// @pre @p size > 0.
		/// @pre Debe haber un renderpass activo.
		/// @pre Debe haber un material enlazado.
		/// @pre El push constant debe ser compatible con el material enlazado.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @pre El pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS` o `CommandsSupport::COMPUTE`.
		/// @pre La cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS` o `CommandsSupport::COMPUTE`.
		void PushMaterialConstants(
			const std::string& pushConstName,
			const void* data,
			USize32 size);

		/// @brief Envía datos push constant al shader.
		/// @tparam T Tipo de dato.
		/// @param pushConstName Nombre de los push const en el shader.
		/// @param data Datos a enviar.
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre Debe haber un material enlazado.
		/// @pre El push constant debe ser compatible con el material enlazado.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @pre El pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS` o `CommandsSupport::COMPUTE`.
		/// @pre La cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS` o `CommandsSupport::COMPUTE`.
		template <typename T>
		void PushMaterialConstants(const std::string& pushConstName, const T& data) {
			PushMaterialConstants(pushConstName, &data, sizeof(T));
		}

		/// @brief Envía datos push constant al shader.
		/// @param pushConstName Nombre de los push const en el shader.
		/// @param data Información a enviar.
		/// @param size Número de bytes a enviar.
		/// @param offset Offset, dentro de este push constant, con el que se mandarán datos.
		/// 
		/// @pre @p size > 0.
		/// @pre Debe haber un renderpass activo.
		/// @pre Debe haber un material enlazado.
		/// @pre El push constant debe ser compatible con el material enlazado.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @pre El pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS` o `CommandsSupport::COMPUTE`.
		/// @pre La cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS` o `CommandsSupport::COMPUTE`.
		virtual void PushMaterialConstants(
			const std::string& pushConstName,
			const void* data,
			USize32 size,
			USize32 offset) = 0;

#pragma endregion

#pragma region Drawing

		/// @brief Renderiza los triángulos del vertex e index buffer,
		/// con el material enlazado.
		/// @param numIndices Número de vértices/índices del vertex buffer.
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre Debe haber un material enlazado.
		/// @pre Debe haber un buffer de vértices enlazado.
		/// @pre Debe haber un buffer de índices enlazado.
		/// @pre Debe estar establecido el viewport y el scissor.
		/// @pre Deben estar enlazados los slots y los push constants necesarios
		/// para rellenar el material.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @note El tipo de comando es de GRÁFICOS.
		/// @pre El pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		/// @pre La cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		virtual void DrawSingleInstance(USize32 numIndices) = 0;

		/// <su
		/// @brief 
		/// Renderiza los triángulos del mesh,
		/// con el material enlazado.
		/// @param firstIndex Índice del primer vértice del mesh.
		/// @param numIndices Número de vértices/índices del mesh.
		/// 
		/// @pre Debe haber un renderpass activo.
		/// @pre Debe haber un material enlazado.
		/// @pre Debe haber un buffer de vértices enlazado.
		/// @pre Debe haber un buffer de índices enlazado.
		/// @pre Debe estar establecido el viewport y el scissor.
		/// @pre Deben estar enlazados los slots y los push constants necesarios
		/// para rellenar el material.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @note El tipo de comando es de GRÁFICOS.
		/// @pre El pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		/// @pre La cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::GRAPHICS`.
		virtual void DrawSingleMesh(UIndex32 firstIndex, USize32 numIndices) = 0;

		/// @brief Renderiza varias veces la malla indicada.
		/// @param firstIndex Índice del primer índice.
		/// @param numIndices Número de vértices/índices de la malla.
		/// @param firstInstance Índice de la primera instancia.
		/// @param instanceCount Número de instancias.
		virtual void DrawInstances(UIndex32 firstIndex, USize32 numIndices, UIndex32 firstInstance, USize32 instanceCount) = 0;

#pragma endregion

#pragma region Ray-tracing

		/// <summary>
		/// Ejecuta un trazado de rayos.
		/// </summary>
		/// <param name="raygenEntry">Índice del shader de generación de rayos que se va 
		/// a usar, según su posición en la tabla de shaders.</param>
		/// <param name="closestHitEntry">Índice del shader de colisión de rayos que se va 
		/// a usar, según su posición en la tabla de shaders.</param>
		/// <param name="missEntry">Índice del shader de fallo de rayos que se va 
		/// a usar, según su posición en la tabla de shaders.</param>
		/// <param name="resolution">Número de rayos generados en los ejes X e Y.</param>
		/// 
		/// @pre NO debe haber un renderpass activo.
		/// @pre Debe haber un material de trazado de rayos enlazado.
		/// @pre Deben estar enlazados los slots y los push constants necesarios
		/// para rellenar el material.
		/// @pre La lista de comandos debe estar abierta.
		/// @pre Debe ejecutarse en un equipo compatible con trazado de rayos.
		/// @pre Debe haberse activado la opción de trazado de rayos de OSKengine.
		/// 
		/// @todo Implementación en DX12.
		/// 
		/// @note El tipo de comando es de COMPUTACIÓN.
		/// @pre El pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::COMPUTE`.
		/// @pre La cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::COMPUTE`.
		virtual void TraceRays(
			UIndex32 raygenEntry,
			UIndex32 closestHitEntry,
			UIndex32 missEntry,
			const Vector2ui& resolution) = 0;

#pragma endregion

#pragma region Compute

		/// @brief Comienza la ejecución de un material de computación.
		/// @param groupCount Número de grupos lanzados.
		/// 
		/// @pre NO debe haber un renderpass activo.
		/// @pre Debe haber un material de computación enlazado.
		/// @pre Deben estar enlazados los slots y los push constants necesarios
		/// para rellenar el material.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @note El tipo de comando es de COMPUTACIÓN.
		/// @pre El pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::COMPUTE`.
		/// @pre La cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::COMPUTE`.
		virtual void DispatchCompute(const Vector3ui& groupCount) = 0;

#pragma endregion

#pragma region Transfer

		/// @brief Copia la imagen guardada en un buffer a una imagen en la GPU.
		/// Para poder subir una imagen a la GPU, primero debemos subir la
		/// imagen a un buffer intermedio, y después copiamos el contenido
		/// de ese buffer intermedio a la imagen final de la GPU.
		/// 
		/// @param source Buffer con los contenidos de la textura.
		/// @param dest Imagen final.
		/// @param layer Capa a la que se copiará los contenidos. Por defecto: 0.
		/// @param offset Offset, en bytes, respecto al inicio del buffer.
		/// 
		/// @pre El buffer de origen debe haber sido creado con GpuBufferUsage::TRANSFER_SOURCE.
		/// @pre La imagen de destino debe haber sido creada con GpuImageUsage::TRANSFER_DESTINATION.
		/// @pre Si la imagen de destino tiene niveles de mip-map, entonces debe haber sido creada con GpuImageUsage::TRANSFER_SOURCE.
		/// @pre La imagen de destino debe tener el layout GpuImageLayout::TRANSFER_DESTINATION.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @post El layout de la imagen después de efectuarse la copia segirá siendo GpuImageLayout::TRANSFER_DESTINATION.
		/// @post Si la imagen de destino tiene niveles de mip-map, estos estarán correctamente generados.
		/// 
		/// @note Si el número de niveles de mip de @p dest es 1, el tipo de comando es de TRANSFERENCIA.
		/// @pre Si el número de niveles de mip de @p dest es 1, el pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::TRANSFER`.
		/// @pre Si el número de niveles de mip de @p dest es 1, la cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::TRANSFER`.
		/// 
		/// @note Si el número de niveles de mip de @p dest es mayor que 1, el tipo de comando es de TRANSFERENCIA y GRÁFICOS.
		/// @pre Si el número de niveles de mip de @p dest es mayor que 1, el pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::TRANSFER` y `CommandsSupport::GRAPHICS`.
		/// @pre Si el número de niveles de mip de @p dest es mayor que 1, la cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::TRANSFER` y `CommandsSupport::GRAPHICS`.
		virtual void CopyBufferToImage(
			const GpuBuffer& source,
			GpuImage* dest,
			UIndex32 layer = 0,
			USize64 offset = 0) = 0;

		/// @brief Copia el contenido de una imagen a otra.
		/// @param source Imagen con los contenidos a copiar.
		/// @param destination Destino de la copia.
		/// @param copyInfo Configuración de la copia.
		///
		/// @pre Ambas imágenes deben tener el mismo formato.
		/// @pre Las áreas de origen y destino deben ser del mismo tamaño.
		/// @pre La imagen de origen debe haber sido creado con GpuImageUsage::TRANSFER_SOURCE.
		/// @pre La imagen de origen debe tener el layout GpuImageLayout::TRANSFER_SOURCE.
		/// @pre La imagen de destino debe haber sido creada con GpuImageUsage::TRANSFER_DESTINATION.
		/// @pre La imagen de destino debe tener el layout GpuImageLayout::TRANSFER_DESTINATION.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @post El layout de la imagen después de efectuarse la copia segirá siendo GpuImageLayout::TRANSFER_DESTINATION.
		/// 
		/// @note El tipo de comando es de TRANSFERENCIA.
		/// @pre El pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::TRANSFER`.
		/// @pre La cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::TRANSFER`.
		virtual void RawCopyImageToImage(
			const GpuImage& source,
			GpuImage* destination,
			const CopyImageInfo& copyInfo) = 0;

		/// @brief Copia los contenidos de una imagen a otra, realizando operaciones de cambio de resolución y formato.
		/// @param source Imagen de origen.
		/// @param destination Imagen de destino.
		/// @param copyInfo Información de la copia.
		/// @param filter Filtro que se usará en caso de realizar cambio de resolución.
		/// 
		/// @pre La imagen de origen debe haber sido creado con GpuImageUsage::TRANSFER_SOURCE.
		/// @pre La imagen de origen debe tener el layout GpuImageLayout::TRANSFER_SOURCE.
		/// @pre La imagen de destino debe haber sido creada con GpuImageUsage::TRANSFER_DESTINATION.
		/// @pre La imagen de destino debe tener el layout GpuImageLayout::TRANSFER_DESTINATION.
		/// @pre La lista de comandos debe estar abierta.
		/// 
		/// @post El layout de la imagen después de efectuarse la copia segirá siendo GpuImageLayout::TRANSFER_DESTINATION.
		/// 
		/// @note El tipo de comando es de TRANSFERENCIA y GRÁFICOS.
		/// @pre El pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::TRANSFER` y `CommandsSupport::GRAPHICS`.
		/// @pre La cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::TRANSFER` y `CommandsSupport::GRAPHICS`.
		virtual void CopyImageToImage(
			const GpuImage& source,
			GpuImage* destination,
			const CopyImageInfo& copyInfo,
			GpuImageFilteringType filter) = 0;

		/// @brief Copia los contenidos del primer buffer al segundo.
		/// @param source Buffer donde están el contenido que se quiere copiar.
		/// @param dest Buffer en el que se va a poner el contenido copiado.
		/// @param size Tamaño, en bytes, del área de memoria que se va a copiar.
		/// @param sourceOffset Offset del área de memoria en el origen.
		/// @param destOffset Offset del área de memoria en el destino.
		/// 
		/// @note El tipo de comando es de TRANSFERENCIA.
		/// @pre El pool de comandos a partir del cual se ha creado la lista debe 
		/// soportar, al menos, `CommandsSupport::TRANSFER`.
		/// @pre La cola de comandos sobre la que se envía la lista debe 
		/// soportar, al menos, `CommandsSupport::TRANSFER`.
		virtual void CopyBufferToBuffer(
			const GpuBuffer& source,
			GpuBuffer* dest,
			USize64 size,
			USize64 sourceOffset,
			USize64 destOffset) = 0;


		/// @brief Registra un buffer intermedio.
		/// Como la operación de copia de datos de un buffer intermedio a un buffer final
		/// no se realiza hasta que se ejecute la lista de comandos, no podemos
		/// eliminar el buffer intermedio hasta la próxima ejecución de la
		/// lista de comandos.
		/// @param stagingBuffer Staging buffer a enlazar a la lista de comandos.
		/// 
		/// @pre El buffer debe haber sido creado con GpuBufferUsage::TRANSFER_SOURCE.
		/// @pre El buffer debe haber sido creado con GpuSharedMemoryType::GPU_AND_CPU.
		/// @pre La lista de comandos debe estar abierta.
		void RegisterStagingBuffer(OwnedPtr<GpuBuffer> stagingBuffer);

		/// @brief Elimina todos los buffers intermedios que ya no son necesarios.
		/// @pre La lista debe haberse ejecutado antes de llamar a esta función.
		void DeleteAllStagingBuffers();

		void _SetSingleTimeUse();
#pragma endregion

#pragma region Resource queue transfer

		/// @brief Transfiere la propiedad de la imagen de una cola GPU a otra.
		/// @param image Imagen cuya propiedad será tranfserida.
		/// @param sourceQueue Cola que era propietaria antes de este comando.
		/// @param targetQueue Cola que será propietaria después de este comando.
		/// 
		/// @pre @p targetQueue debe ser estable.
		/// 
		/// @pre @p sourceQueue debe ser la cola que posee la imagen antes de este comando.
		/// 
		/// @note La transferencia de la propiedad no se completará hasta que no
		/// se complete la ejecución del comando en GPU.
		/// @note Si @p sourceQueue y @p targetQueue representan la misma cola,
		/// no ocurre nada.
		void TransferToQueue(
			GpuImage* image,
			const ICommandQueue& sourceQueue,
			const ICommandQueue& targetQueue);

		/// @brief Transfiere la propiedad de la imagen a otra cola GPU.
		/// @param image Imagen cuya propiedad será tranfserida.
		/// @param targetQueue Cola que será propietaria después de este comando.
		/// 
		/// @pre @p targetQueue debe ser estable.
		/// 
		/// @note Si @p targetQueue representan la cola que ya es propietaria de la imagen,
		/// no ocurre nada.
		void TransferToQueue(
			GpuImage* image,
			const ICommandQueue& targetQueue);


		/// @brief Transfiere la propiedad del buffer de una cola GPU a otra.
		/// @param buffer Buffer cuya propiedad será tranfserida.
		/// @param sourceQueue Cola que era propietaria antes de este comando.
		/// @param targetQueue Cola que será propietaria después de este comando.
		/// 
		/// @pre @p targetQueue debe ser estable.
		/// 
		/// @pre @p sourceQueue debe ser la cola que posee el buffer antes de este comando.
		/// 
		/// @note La transferencia de la propiedad no se completará hasta que no
		/// se complete la ejecución del comando en GPU.
		/// @note Si @p sourceQueue y @p targetQueue representan la misma cola,
		/// no ocurre nada.
		void TransferToQueue(
			GpuBuffer* buffer,
			const ICommandQueue& sourceQueue,
			const ICommandQueue& targetQueue);

		/// @brief Transfiere la propiedad del buffer de una cola GPU a otra.
		/// @param buffer Buffer cuya propiedad será tranfserida.
		/// @param targetQueue Cola que será propietaria después de este comando.
		/// 
		/// @pre @p targetQueue debe ser estable.
		/// 
		/// @note Si @p targetQueue representan la cola que ya es propietaria del buffer,
		/// no ocurre nada.
		void TransferToQueue(
			GpuBuffer* buffer,
			const ICommandQueue& targetQueue);

#pragma endregion


		/// @brief Añade un marcador en la lista de comandos que podrá ser vista
		/// en un software de debbuging.
		/// @param mark Título del marcador.
		/// @param color Color del marcador.
		virtual void AddDebugMarker(const std::string& mark, const Color& color) = 0;

		/// @brief Comienza una sección de la lista de comandos que quedará marcada.
		/// @param mark Título de la marca.
		/// @param color Color de la marca.
		virtual void StartDebugSection(const std::string& mark, const Color& color) = 0;

		/// @brief Finaliza la sección actualmente marcada.
		virtual void EndDebugSection() = 0;


		const ICommandPool* GetOwnerPool() const;
		
		/// @return Tipo de cola GPU en la que se puede
		/// insertar esta lista;
		GpuQueueType GetCompatibleQueueType() const;

		UIndex32 _GetCommandListIndex() const;

	protected:

		explicit ICommandList(const ICommandPool* commandPool);

		void _SetSingleTime();

		virtual void BindGraphicsPipeline(const IGraphicsPipeline& computePipeline) = 0;
		virtual void BindComputePipeline(const IComputePipeline& computePipeline) = 0;
		virtual void BindRayTracingPipeline(const IRaytracingPipeline& computePipeline) = 0;


		/// @brief Pipeline que está siendo grabado en un instante determinado.
		/// Sólamente puede estar uno activo.
		union {
			const IGraphicsPipeline* graphics = nullptr;
			const IRaytracingPipeline* raytracing;
			const IComputePipeline* compute;
		} m_currentPipeline{};

		/// @brief Material que está siendo usado en un instante determinado.
		const Material* m_currentlyBoundMaterial = nullptr;


		/// @brief 
		DynamicArray<RenderPassImageInfo> m_currentlyBoundColorImages;

		/// @brief 
		RenderPassImageInfo m_currentlyBoundDepthImage;

		/// @brief 
		RenderpassType m_currentlyBoundRenderTargetType = RenderpassType::INTERMEDIATE;


	private:

		/// @brief Viewport en un momento dado.
		Viewport m_currentViewport = {};

		/// @brief True si es una lista de uso único.
		bool m_isSingleUse = false;

		/// @brief Buffers de un único uso que pueden ser eliminados
		/// tras la ejecución de esta lista de comandos.
		DynamicArray<UniquePtr<GpuBuffer>> m_stagingBuffersToDelete;

		/// @brief Lista con las imágenes que han cambiado de
		/// cola de GPU.
		DynamicArray<GpuImage*> m_imagesToQueueTransfer;

		/// @brief Command pool que creó esta lista de comandos.
		const ICommandPool* m_ownerPool = nullptr;

	};

}
