#pragma once

#include "OSKsettings.h"
#include "OSKmacros.h"
#include "OSKtypes.h"
#include "Log.h"

#include <vector>
#include <vulkan/vulkan.h>
#include <string>

namespace OSK {

	/// <summary>
	/// Tipos de binding del material.
	/// </summary>
	enum class MaterialBindingType {

		/// <summary>
		/// Buffer de datos: cada instancia tiene su propio buffer. (VK: uniform buffer).
		/// </summary>
		DATA_BUFFER,

		/// <summary>
		/// Buffer de datos compartido por varias instancias. (VK: dynamic uniform buffer).
		/// </summary>
		DYNAMIC_DATA_BUFFER,

		/// <summary>
		/// Textura 2D.
		/// </summary>
		TEXTURE

	};


	/// <summary>
	/// Stage del binding: en que shader se accede.
	/// </summary>
	enum class MaterialBindingShaderStage {

		/// <summary>
		/// Vertex stage.
		/// </summary>
		VERTEX,

		/// <summary>
		/// Fragment stage.
		/// </summary>
		FRAGMENT

	};


	/// <summary>
	/// Información de un binding de un material: tipo y shader al que se enlaza.
	/// </summary>
	struct OSKAPI_CALL MaterialBinding {

		/// <summary>
		/// Tipo de binding.
		/// </summary>
		MaterialBindingType type;

		/// <summary>
		/// Stage del binding.
		/// </summary>
		MaterialBindingShaderStage stage;

		/// <summary>
		/// Nombre del binding.
		/// </summary>
		std::string bindingName = "";

	};


	/// <summary>
	/// Una lista de bindings crean un layout.
	/// </summary>
	typedef	std::vector<MaterialBinding> MaterialBindingLayout;

	
	/// <summary>
	/// Devuelve el descriptor type en formato Vulkan.
	/// </summary>
	/// <param name="type">Tipo en formato OSK.</param>
	/// <returns>Tipo en formato Vulkan.</returns>
	VkDescriptorType GetVulkanBindingType(MaterialBindingType type);

	/// <summary>
	/// Devuelve el shader stage en formato Vulkan.
	/// </summary>
	/// <param name="type">Formato OSK.</param>
	/// <returns>Formato Vulkan.</returns>
	VkShaderStageFlagBits GetVulkanShaderBinding(MaterialBindingShaderStage type);

}