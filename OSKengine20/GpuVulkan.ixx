#include <vulkan/vulkan.h>

export module OSKengine.Graphics.GpuVulkan;

export import OSKengine.Graphics.IGpu;

export namespace OSK {
	
	class GpuVulkan : public IGpu {

	public:

		/// <summary>
		/// Información interna de una GPU.
		/// </summary>
		struct Info {

			/// <summary>
			/// Propiedades físicas.
			/// </summary>
			VkPhysicalDeviceProperties properties;

			/// <summary>
			/// Cosas que soporta.
			/// </summary>
			VkPhysicalDeviceFeatures features;

			/// <summary>
			/// Máximo nivel de MSAA disponible.
			/// </summary>
			VkSampleCountFlagBits maxMsaaSamples;

			/// <summary>
			/// Información de las memorias disponibles en la GPU.
			/// </summary>
			VkPhysicalDeviceMemoryProperties memoryProperties;

			/// <summary>
			/// Alineamiento m�nimo para dynamic ubos.
			/// </summary>
			size_t minAlignment;

			/// <summary>
			/// True si puede usarse en OSKengine.
			/// </summary>
			bool isSuitable;

		};

		void SetPhysicalDevice(VkPhysicalDevice gpu) {
			physicalDevice = gpu;
		}

		VkPhysicalDevice GetPhysicalDevice() const {
			return physicalDevice;
		}

		void SetInfo(const Info& info) {
			this->info = info;
		}

		const Info& GetInfo() const {
			return info;
		}

		void SetLogicalDevice(VkDevice device) {
			logicalDevice = device;
		}

		VkDevice GetLogicalDevice() const {
			return logicalDevice;
		}

	private:

		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
		Info info;

	};

}
