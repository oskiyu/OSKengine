#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vulkan/vulkan.h>
#include <string>

#include "Vertex.h"
#include "Vector4.hpp"
#include <vector>

#include "OSKtypes.h"
#include "Texture.h"
#include "DescriptorSet.h"

namespace OSK::VULKAN {
	class Renderpass;
}


namespace OSK {

	class DescriptorSet;
	
	//Represnta un graphics pipeline.

	/// <summary>
	/// El graphics pipeline es el encargado de transformar los modelos 3D en una imagen final.
	/// Contiene informaci�n y ajustes que cambian este proceso de renerizado.
	/// Debe usarse con shaders para que funcione.
	/// </summary>
	class OSKAPI_CALL GraphicsPipeline {

		friend class RenderAPI;
		friend class RenderizableScene;
		friend class DescriptorSet;
		friend class Model;
		friend class RenderSystem3D;

	public:

		/// <summary>
		/// Destruye el graphics pipeline.
		/// </summary>
		~GraphicsPipeline();

		/// <summary>
		/// Establece el viewport del pipeline: �rea que se va a renderizar.
		/// </summary>
		/// <param name="size">Tama�o de la imagen rederizada del pipeline.</param>
		/// <param name="depthMinMax">M�nimo y m�ximo de la profundidad de la imagen.</param>
		void SetViewport(const Vector4& size, const Vector2& depthMinMax = { 0.0f, 1.0f });

		/// <summary>
		/// Establece las opciones de rasterizado.
		/// </summary>
		/// <param name="renderObjectsOutsideRange">True si se van a renderizar elementos que no saldr�n en la imagen final.</param>
		/// <param name="polygonMode">Modo de renderizado de los pol�gonos (fill, lines, etc.).</param>
		/// <param name="cullMode">Modo de culling de los tri�ngulos. Culling: no renderizar caras que no est�n de frente a la c�mara.</param>
		/// <param name="frontFaceType">C�al es el frente del tri�ngulo.</param>
		void SetRasterizer(VkBool32 renderObjectsOutsideRange, VkPolygonMode polygonMode, VkCullModeFlagBits cullMode, VkFrontFace frontFaceType);

		/// <summary>
		/// Establece si se va a usar MSAA  o no.
		/// </summary>
		/// <param name="use">True si se va a usar.</param>
		/// <param name="samples">N�mero de samples de MSAA.</param>
		void SetMSAA(VkBool32 use, VkSampleCountFlagBits samples);

		/// <summary>
		/// Establece si se va a usar informaci�n de depth/stencil.
		/// </summary>
		/// <param name="use">True si se va a usar depth o stencil.</param>
		void SetDepthStencil(bool use); 

		/// <summary>
		/// Establece la configuraci�n de push constants.
		/// Un push constant representa informaci�n que se env�a a la GPU una vez por entidad.
		/// </summary>
		/// <param name="shaderStage">Shader en el que se va a usar el push constant.</param>
		/// <param name="size">Tama�o en bytes del push constant.</param>
		/// <param name="offset">Offset en bytes (si se usan varios push constants).</param>
		void SetPushConstants(VkShaderStageFlagBits shaderStage, uint32_t size, uint32_t offset = 0);

		/// <summary>
		/// Establece el layout de los bindings que va a seguir el pipeline.
		/// </summary>
		/// <param name="layout">Descriptor layout.</param>
		void SetLayout(VkDescriptorSetLayout* layout);

		/// <summary>
		/// Crea el pipeline.
		/// <br/>
		/// <br/>
		/// Deben llamarse a las siguientes funciones antes: <br/>
		/// -SetViewport().<br/>
		/// -SetLayout().<br/>
		/// </summary>
		/// <param name="renderpass">Renderpass sobre el que se va a usar el pipeline.</param>
		void Create(VULKAN::Renderpass* renderpass);

		/// <summary>
		/// Enlaza este pipeline para su uso.
		/// </summary>
		/// <param name="commandBuffer">Commandbuffer.</param>
		void Bind(VkCommandBuffer commandBuffer) const;

		/// <summary>
		/// Recarga los shaders.
		/// </summary>
		void ReloadShaders();


	private:

		/// <summary>
		/// Crea un pipeline vac�o.
		/// </summary>
		/// <param name="logicalDevice">Logical device de el renderizador.</param>
		/// <param name="vertexPath">Archivo del shader de v�rtices (.spv).</param>
		/// <param name="indexPath">Archivo del shader de fragmentos (.spv).</param>
		GraphicsPipeline(VkDevice logicalDevice, const std::string& vertexPath, const std::string& indexPath);

		/// <summary>
		/// Pipeline nativo.
		/// </summary>
		VkPipeline vulkanPipeline;

		/// <summary>
		/// Layout nativo del pipeline.
		/// </summary>
		VkPipelineLayout vulkanPipelineLayout;

		/// <summary>
		/// True si se ha establecido informaci�n del viewport.
		/// </summary>
		bool viewportHasBeenSet = false;

		/// <summary>
		/// True si se ha establecido informaci�n del rasterizador.
		/// </summary>
		bool rasterizerHasBeenSet = false;

		/// <summary>
		/// True si se ha establecido informaci�n de MSAA.
		/// </summary>
		bool msaaHasBeenSet = false;

		/// <summary>
		/// True si se ha establecido informaci�n de depth / stencil.
		/// </summary>
		bool depthStencilHasBeenSet = false;

		/// <summary>
		/// True si se ha establecido informaci�n de push constants.
		/// </summary>
		bool pushConstantsHaveBeenSet = false;

		/// <summary>
		/// True si se ha establecido informaci�n del layout.
		/// </summary>
		bool layoutHasBeenSet = false;

		/// <summary>
		/// Archivo de shader de v�rtices.
		/// </summary>
		std::string vertexPath;

		/// <summary>
		/// Archivo de shader de fragmentos.
		/// </summary>
		std::string fragmentPath;

		/// <summary>
		/// Crea un vulkan shader �module a partir del c�digo binario de un shader.
		/// </summary>
		/// <param name="code">C�digo SPRI-V.</param>
		/// <returns>Shader module nativo.</returns>
		VkShaderModule createShaderModule(const std::vector<char>& code) const;

		/// <summary>
		/// Carga los shaders.
		/// </summary>
		void loadShaders();

		/// <summary>
		/// Descarga los shaders.
		/// </summary>
		void clearShaders();

		/// <summary>
		/// Logical device del renderizador.
		/// </summary>
		VkDevice logicalDevice;


		/// <summary>
		/// Stages que se van a usar en el pipeline.
		/// </summary>
		VkPipelineShaderStageCreateInfo shaderStages[2] = {};

		/// <summary>
		/// Inputs al shader de v�rtices.
		/// </summary>
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};

		/// <summary>
		/// Viewport a usar.
		/// </summary>
		VkViewport viewport{};

		/// <summary>
		/// Scissor a usar: https://vulkan-tutorial.com/Drawing_a_triangle/Graphics_pipeline_basics/Fixed_functions.
		/// </summary>
		VkRect2D scissor{};

		/// <summary>
		/// Create info del viewport.
		/// </summary>
		VkPipelineViewportStateCreateInfo viewportState{};

		/// <summary>
		/// Create info del rasterizador.
		/// </summary>
		VkPipelineRasterizationStateCreateInfo rasterizer{};

		/// <summary>
		/// Create info del MSAA.
		/// </summary>
		VkPipelineMultisampleStateCreateInfo multisampling{};

		/// <summary>
		/// Create info del blending de colores.
		/// </summary>
		std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments{};

		/// <summary>
		/// Create info del blending de colores
		/// </summary>
		VkPipelineColorBlendStateCreateInfo colorBlending{};

		/// <summary>
		/// Create info del depth / stencil.
		/// </summary>
		VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo{};

		/// <summary>
		/// Informaci�n de los push constants.
		/// </summary>
		std::vector<VkPushConstantRange> pushConstRanges{};

		/// <summary>
		/// Create info con el layout.
		/// </summary>
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};

		/// <summary>
		/// Layout de descriptor sets.
		/// </summary>
		VkDescriptorSetLayout* descriptorSetLayout;

		/// <summary>
		/// Pipeline create info.
		/// </summary>
		VkGraphicsPipelineCreateInfo pipelineInfo{};

		/// <summary>
		/// M�dulo del shader de v�rtices.
		/// </summary>
		VkShaderModule vertexShaderModule;

		/// <summary>
		/// M�dulo del shader de fragmentos.
		/// </summary>
		VkShaderModule fragmentShaderModule;

		/// <summary>
		/// Describe la informaci�n que tienen los v�rtices.
		/// </summary>
		VkVertexInputBindingDescription vertexBindingDesc;

		/// <summary>
		/// Describe la informaci�n que tienen los v�rtices.
		/// </summary>
		std::array<VkVertexInputAttributeDescription, OSK_VERTEX_ATTRIBUTES_COUNT> vertexInputDesc;

		/// <summary>
		/// Renderpass target.
		/// </summary>
		VULKAN::Renderpass* targetRenderpass = nullptr;

	};

}