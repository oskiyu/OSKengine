#pragma once

namespace OSK::GAME {

	/// @brief Perfil que indica qué sistemas,
	/// componentes y assets por defecto se cargarán
	/// al iniciar el motor.
	enum class DefaultContentProfile {

		/// @brief 
		/// - DeferredRenderSystem (*ECS System*).
		/// - PhysicsSystem (*ECS System*).
		/// - CollisionSystem (*ECS System*).
		/// - PhysicsResolver (*ECS System*).
		/// - TransformApplierSystem (*ECS System*).
		/// - SkyboxRenderSystem (*ECS System*).
		/// - RenderSystem2D (*ECS System*).
		/// 
		/// - TransformComponent3D (*ECS Component*).
		/// - ModelComponent3D (*ECS Component*).
		/// - CameraComponent3D (*ECS Component*).
		/// - TerrainComponent (*ECS Component*).
		/// - CollisionComponent (*ECS Component*).
		/// - PhysicsComponent (*ECS Component*).
		/// - Transform2D (*ECS Component*).
		/// - CameraComponent2D (*ECS Component*).
		/// - Sprite (*ECS Component*).
		/// 
		/// - TextureLoader (*Asset*).
		/// - ModelLoader3D (*Asset*).
		/// - CubemapTextureLoader (*Asset*).
		/// - FontLoader (*Asset*).
		/// - IrradianceMapLoader (*Asset*).
		/// - SpecularMapLoader (*Asset*).
		/// - AudioLoader (*Asset*).
		/// - PreBuiltColliderLoader (*Asset*).
		/// - PreBuiltSplineLoader3D (*Asset*).
		/// 
		/// @pre Deben existir los siguientes materiales:
		/// - (*Para DeferredRenderSystem*) Resources/Materials/PBR/Deferred/deferred_gbuffer_static.json
		/// - (*Para DeferredRenderSystem*) Resources/Materials/PBR/Deferred/deferred_gbuffer_anim.json
		/// - (*Para DeferredRenderSystem*) Resources/Materials/PBR/Deferred/deferred_gbuffer_billboard.json
		/// - (*Para DeferredRenderSystem*) Resources/Materials/ShadowMapping/material_shadows.json
		/// - (*Para DeferredRenderSystem*) Resources/Materials/PBR/Deferred/deferred_resolve.json
		/// 
		/// - (*Para SkyboxRenderSystem*) Resources/Materials/Skybox/material.json
		/// 
		/// - (*Para RenderSystem2D*) Resources/Materials/2D/material_2d.json
		/// 
		/// - (*Para IrradianceMapLoader*) Resources/Materials/AssetsGen/material_irradiance_gen.json
		/// - (*Para IrradianceMapLoader*) Resources/Materials/AssetsGen/material_irradiance_convolution.json
		/// 
		/// - (*Para SpecularMapLoader*) Resources/Materials/AssetsGen/material_specular_gen.json
		/// - (*Para SpecularMapLoader*) Resources/Materials/AssetsGen/material_specular_prefilter.json
		/// - (*Para SpecularMapLoader*) Resources/Materials/AssetsGen/material_specular_lut_gen.json
		/// 
		/// @pre Deben existir los siguientes shaders:
		/// - (*Para DeferredRenderSystem*) Resources/Shaders/VK/PBR/Deferred/GBuffer/vert.spv
		/// - (*Para DeferredRenderSystem*) Resources/Shaders/VK/PBR/Deferred/GBuffer/frag.spv
		/// - (*Para DeferredRenderSystem*) Resources/Shaders/VK/PBR/Deferred/GBuffer/vert.spv
		/// - (*Para DeferredRenderSystem*) Resources/Shaders/VK/PBR/Deferred/GBuffer/shader_billboard.spv
		/// - (*Para DeferredRenderSystem*) Resources/Shaders/VK/PBR/Deferred/GBuffer/anim.spv
		/// - (*Para DeferredRenderSystem*) Resources/Shaders/VK/PBR/Deferred/CompResolve/shader.spv
		/// - (*Para DeferredRenderSystem*) Resources/Shaders/VK/Shadows/vert.spv
		/// 
		/// - (*Para SkyboxRenderSystem*) Resources/Shaders/VK/Skybox/vert.spv
		/// - (*Para SkyboxRenderSystem*) Resources/Shaders/VK/Skybox/frag.spv
		/// 
		/// - (*Para RenderSystem2D*) Resources/Shaders/VK/2D/2D/vert.spv
		/// - (*Para RenderSystem2D*) Resources/Shaders/VK/2D/2D/frag.spv
		/// 
		/// - (*Para IrradianceMapLoader*) Resources/Shaders/VK/AssetsGen/IrradianceGen/CubeGen/vert.spv
		/// - (*Para IrradianceMapLoader*) Resources/Shaders/VK/AssetsGen/IrradianceGen/CubeGen/frag.spv
		/// - (*Para IrradianceMapLoader*) Resources/Shaders/VK/AssetsGen/IrradianceGen/Convolution/vert.spv
		/// - (*Para IrradianceMapLoader*) Resources/Shaders/VK/AssetsGen/IrradianceGen/Convolution/frag.spv
		/// 
		/// - (*Para SpecularMapLoader*) Resources/Shaders/VK/AssetsGen/SpecularGen/CubeGen/vert.spv
		/// - (*Para SpecularMapLoader*) Resources/Shaders/VK/AssetsGen/SpecularGen/CubeGen/frag.spv
		/// - (*Para SpecularMapLoader*) Resources/Shaders/VK/AssetsGen/SpecularGen/LutGen/shader.spv
		/// - (*Para SpecularMapLoader*) Resources/Shaders/VK/AssetsGen/SpecularGen/Prefilter/vert.spv
		/// - (*Para SpecularMapLoader*) Resources/Shaders/VK/AssetsGen/SpecularGen/Prefilter/frag.spv
		/// 
		/// @pre Deben existir los siguientes recursos:
		/// - (*Para SkyboxRenderSystem, IrradianceMapLoader*) Resources/Assets/Models/cube.json
		ALL,

		/// @brief 
		/// - DeferredRenderSystem (*ECS System*).
		/// - PhysicsSystem (*ECS System*).
		/// - CollisionSystem (*ECS System*).
		/// - PhysicsResolver (*ECS System*).
		/// - TransformApplierSystem (*ECS System*).
		/// - SkyboxRenderSystem (*ECS System*).
		/// 
		/// - TransformComponent3D (*ECS Component*).
		/// - ModelComponent3D (*ECS Component*).
		/// - CameraComponent3D (*ECS Component*).
		/// - TerrainComponent (*ECS Component*).
		/// - CollisionComponent (*ECS Component*).
		/// - PhysicsComponent (*ECS Component*).
		/// - Transform2D (*ECS Component*).
		/// - CameraComponent2D (*ECS Component*).
		/// 
		/// - TextureLoader (*Asset*).
		/// - ModelLoader3D (*Asset*).
		/// - CubemapTextureLoader (*Asset*).
		/// - FontLoader (*Asset*).
		/// - IrradianceMapLoader (*Asset*).
		/// - SpecularMapLoader (*Asset*).
		/// - AudioLoader (*Asset*).
		/// - PreBuiltColliderLoader (*Asset*).
		/// - PreBuiltSplineLoader3D (*Asset*).
		/// 
		/// @pre Deben existir los siguientes materiales:
		/// - (*Para DeferredRenderSystem*) Resources/Materials/PBR/Deferred/deferred_gbuffer_static.json
		/// - (*Para DeferredRenderSystem*) Resources/Materials/PBR/Deferred/deferred_gbuffer_anim.json
		/// - (*Para DeferredRenderSystem*) Resources/Materials/PBR/Deferred/deferred_gbuffer_billboard.json
		/// - (*Para DeferredRenderSystem*) Resources/Materials/ShadowMapping/material_shadows.json
		/// - (*Para DeferredRenderSystem*) Resources/Materials/PBR/Deferred/deferred_resolve.json
		/// 
		/// - (*Para SkyboxRenderSystem*) Resources/Materials/Skybox/material.json
		/// 
		/// - (*Para IrradianceMapLoader*) Resources/Materials/AssetsGen/material_irradiance_gen.json
		/// - (*Para IrradianceMapLoader*) Resources/Materials/AssetsGen/material_irradiance_convolution.json
		/// 
		/// - (*Para SpecularMapLoader*) Resources/Materials/AssetsGen/material_specular_gen.json
		/// - (*Para SpecularMapLoader*) Resources/Materials/AssetsGen/material_specular_prefilter.json
		/// - (*Para SpecularMapLoader*) Resources/Materials/AssetsGen/material_specular_lut_gen.json
		/// 
		/// @pre Deben existir los siguientes shaders:
		/// - (*Para DeferredRenderSystem*) Resources/Shaders/VK/PBR/Deferred/GBuffer/vert.spv
		/// - (*Para DeferredRenderSystem*) Resources/Shaders/VK/PBR/Deferred/GBuffer/frag.spv
		/// - (*Para DeferredRenderSystem*) Resources/Shaders/VK/PBR/Deferred/GBuffer/vert.spv
		/// - (*Para DeferredRenderSystem*) Resources/Shaders/VK/PBR/Deferred/GBuffer/shader_billboard.spv
		/// - (*Para DeferredRenderSystem*) Resources/Shaders/VK/PBR/Deferred/GBuffer/anim.spv
		/// - (*Para DeferredRenderSystem*) Resources/Shaders/VK/PBR/Deferred/CompResolve/shader.spv
		/// - (*Para DeferredRenderSystem*) Resources/Shaders/VK/Shadows/vert.spv
		/// 
		/// - (*Para SkyboxRenderSystem*) Resources/Shaders/VK/Skybox/vert.spv
		/// - (*Para SkyboxRenderSystem*) Resources/Shaders/VK/Skybox/frag.spv
		/// 
		/// - (*Para IrradianceMapLoader*) Resources/Shaders/VK/AssetsGen/IrradianceGen/CubeGen/vert.spv
		/// - (*Para IrradianceMapLoader*) Resources/Shaders/VK/AssetsGen/IrradianceGen/CubeGen/frag.spv
		/// - (*Para IrradianceMapLoader*) Resources/Shaders/VK/AssetsGen/IrradianceGen/Convolution/vert.spv
		/// - (*Para IrradianceMapLoader*) Resources/Shaders/VK/AssetsGen/IrradianceGen/Convolution/frag.spv
		/// 
		/// - (*Para SpecularMapLoader*) Resources/Shaders/VK/AssetsGen/SpecularGen/CubeGen/vert.spv
		/// - (*Para SpecularMapLoader*) Resources/Shaders/VK/AssetsGen/SpecularGen/CubeGen/frag.spv
		/// - (*Para SpecularMapLoader*) Resources/Shaders/VK/AssetsGen/SpecularGen/LutGen/shader.spv
		/// - (*Para SpecularMapLoader*) Resources/Shaders/VK/AssetsGen/SpecularGen/Prefilter/vert.spv
		/// - (*Para SpecularMapLoader*) Resources/Shaders/VK/AssetsGen/SpecularGen/Prefilter/frag.spv
		/// 
		/// @pre Deben existir los siguientes recursos:
		/// - (*Para SkyboxRenderSystem, IrradianceMapLoader*) Resources/Assets/Models/cube.json
		_3D_ONLY,

		/// @brief 
		/// - RenderSystem2D (*ECS System*).
		/// 
		/// - Transform2D (*ECS Component*).
		/// - CameraComponent2D (*ECS Component*).
		/// - Sprite (*ECS Component*).
		/// 
		/// - TextureLoader (*Asset*).
		/// - FontLoader (*Asset*).
		/// - AudioLoader (*Asset*).
		/// 
		/// @pre Deben existir los siguientes materiales:
		/// - (*Para RenderSystem2D*) Resources/Materials/2D/material_2d.json
		/// 
		/// @pre Deben existir los siguientes shaders:
		/// - (*Para RenderSystem2D*) Resources/Shaders/VK/2D/2D/vert.spv
		/// - (*Para RenderSystem2D*) Resources/Shaders/VK/2D/2D/frag.spv
		_2D_ONLY,

		/// @brief 
		/// - Transform2D (*ECS Component*).
		/// - CameraComponent2D (*ECS Component*).
		/// 
		/// - TextureLoader (*Asset*).
		/// - FontLoader (*Asset*).
		MINIMAL

	};

}
