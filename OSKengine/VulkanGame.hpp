#pragma once

#include <iostream>
#include <string>
#include <array>
#include <numeric>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm.hpp>

#include "VulkanGraphicsPipeline.h"
#include "VulkanDescriptorSet.h"
#include "VulkanDevice.h"
#include "VulkanSurface.h";
#include "VulkanSwapchain.h"
#include "VulkanInstance.h"
#include "VulkanRenderpass.h"
#include "VulkanCommandPool.h"
#include "VulkanModel.h"
#include "VulkanUniformBufferObject.h"

#define GLFW_INCLUDE_VULKAN
#include "WindowAPI.h"

namespace OSK::VULKAN {


	class VulkanGame {

	public:

		float CurrentTime;

		float LastTime;

		float DeltaTime;

		VulkanDevice device{};

		VulkanSurface surface{};

		VulkanSwapchain swapchain{};

		VulkanInstance instance{};

		VulkanRenderpass renderpass{};

		VulkanDescriptorSet DescriptorSet{};

		VulkanGraphicsPipeline GraphicsPipeline{};

		WindowAPI* window;
		
		VkQueue GraphicsQueue;

		VulkanCommandPool CommandPool{};

		VkDebugUtilsMessengerEXT DebugMessengerHandler;

		//V
		VulkanModel model{};

		//Establece el CALLBACK de Vulkan para cuando hay un problema ( DEBUG )
		void SetDebugMessenger() {
			//No podemos establecer el CALLBACK sin capas de validación
			if (!instance.UseValidationLayers) {
				return;
			}

			//Estructura con la información del CALLBACK
			VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT; //Tipo de struct
			createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT; //Importancia de los mensajes que va a manejar
			createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT; //Tipos de mensaje que va a manejar
			createInfo.pfnUserCallback = VulkanDebugCallback; //Establecer el CALLBACK
			createInfo.pUserData = NULL;

			//Tratar de establecer el CALLBACK y comprobar si ha sido posible
			VkResult result = CreateDebugUtilsMessengerEXT(instance.Instance, &createInfo, NULL, &DebugMessengerHandler);
			if (result != VK_SUCCESS) {
				Logger::Log(OSK::LogMessageLevels::BAD_ERROR, "debug messenger", __LINE__);
				throw std::runtime_error("ERROR: debug messenger, línea: " + __LINE__); //Error si no se puede establecer el CALLBACK
			}
		}

		//VkResult que ejecutará la función 'vkGetInstanceProcAddr', o return error si no es posible
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* createInfo, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* messenger) {

			//Comrpueba si realmente se puede usar la función par establecer el CALLBACK
			auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"); //func = pointer a la función (nullptr si no existe)

			//Si existe la función, usarla
			if (func != nullptr) {
				return func(instance, createInfo, allocator, messenger);
			}

			//Si no existe la función, return error
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}

		//Callback: función que se ejecuta cuando hay un problema con Vulkan
		static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* data, void* userData) {

			//Comprueba la importancia del mensaje, e imprime el mensaje si es importante
			/*if (severity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
				//Imprime en la consola el mensaje del error
				std::cerr << "Capa de validación: " << data->pMessage << std::endl;
			}*/

			Logger::Log(LogMessageLevels::WARNING, "Vulkan error: " + std::string(data->pMessage) + '\n');

			//return para evitar el 'CL.exe exit with code 2'
			return VK_FALSE;
		}

		void Start() {
			instance.UseValidationLayers = true;
			instance.CreateInstance("VkTest", 0, 0, 0);
#ifdef OSK_DEBUG
			SetDebugMessenger();
#endif
			surface.CreateSurface(window->GetGLFWWindow(), instance);
			device.PickPhysicalDevice(instance, surface);
			device.CreateLogicalDevice(instance, surface, GraphicsQueue);
			CommandPool.CreateCommandPool(device, surface);
			swapchain.CreateSwapchain(device, surface, *window);
			renderpass.CreateRenderpass(device, swapchain);
			GraphicsPipeline.CreateGraphicsPipeline(device, swapchain, renderpass, "shaders/Vk/vert", "shaders/Vk/frag");
			swapchain.CreateFramebuffers(device, renderpass);
			swapchain.CreateUniformBuffers(device);
			swapchain.CreateSyncObjects(device);

			model.LoadModel(device, &CommandPool, GraphicsQueue, "models/cube/cube.obj");
			model.ModelTransform.SetPosition(Vector3(0, 0, 0));
			model.ModelTransform.SetScale(0.01f);
		}

		void Draw() {
			swapchain.DrawModel(&model);
			swapchain.Render(device, renderpass, CommandPool, GraphicsPipeline.GraphicsPipeline, GraphicsPipeline.PipelineLayout);
			swapchain.FinishRender(device, surface.SurfaceQueue, GraphicsQueue);
		}

		void MainLoop() {
			//El bucle se ejecuta mientras la ventana no se cierre
			while (!glfwWindowShouldClose(window->GetGLFWWindow())) {

				CurrentTime = glfwGetTime();
				DeltaTime = CurrentTime - LastTime;

				//Comprueba eventos (input, etc.)
				glfwPollEvents();

				//Renderiza la imagen
				Draw();

/*
				if (NewKS.IsKeyDown(OSK::Key::W))
					Camera->Move(OSK::Directions::FORWARD, DeltaTime);
				if (NewKS.IsKeyDown(OSK::Key::S))
					Camera->Move(OSK::Directions::BACKWARDS, DeltaTime);
				if (NewKS.IsKeyDown(OSK::Key::A))
					Camera->Move(OSK::Directions::LEFT, DeltaTime);
				if (NewKS.IsKeyDown(OSK::Key::D))
					Camera->Move(OSK::Directions::RIGHT, DeltaTime);*/

				//
				/*OskDebugOutput("INFO: ms: " + std::to_string(DeltaTime), __LINE__);
				OskDebugOutput("INFO: FPS: " + std::to_string(GetFPS()), __LINE__);
				*/

				LastTime = CurrentTime;
			}

			//Esperar a que terminen las tareas asincrónicas para continuar
			vkDeviceWaitIdle(device.LogicalDevice);
		}

		void Destroy() {
			DestroySwapchain();

			CommandPool.DestroyCommandPool(device);
			device.Destroy();
			surface.Destroy(instance);
			instance.Destroy();

			window = nullptr;
		}

		void DestroySwapchain() {
			CommandPool.DestroyCommandPool(device);
			GraphicsPipeline.Destroy(device);
			renderpass.Destroy(device);
			swapchain.Destroy(device);
		}

	};

}