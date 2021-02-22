#include "Material.h"

#include "VulkanRenderer.h"

using namespace OSK;

constexpr uint32_t MAX_SETS = 20;

void Material::SetRenderer(RenderAPI* renderer) {
	this->renderer = renderer;
}

void Material::SetShaders(const Shader& shader) {
	GPipeline = renderer->CreateNewGraphicsPipeline(shader.VertexPath, shader.FragmentPath);
}

void Material::AddBinding(ShaderBindingType type, ShaderStage stage) {
	if (!Layout)
		Layout = renderer->CreateNewDescriptorLayout();

	VkDescriptorType vtype;
	VkShaderStageFlags vstage;

	if (type == ShaderBindingType::BUFFER)
		vtype = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	else
		vtype = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

	if (stage == ShaderStage::VERTEX)
		vstage = VK_SHADER_STAGE_VERTEX_BIT;
	else
		vstage = VK_SHADER_STAGE_FRAGMENT_BIT;

	Layout->AddBinding(bindingCount, vtype, vstage);
	bindingCount++;
}

void Material::AddVertexBinding(ShaderBindingType type) {
	AddBinding(type, ShaderStage::VERTEX);
}

void Material::AddFragmentBinding(ShaderBindingType type) {
	AddBinding(type, ShaderStage::FRAGMENT);
}

void Material::SetRenderTarget(RenderTarget* target) {
	GPipeline->SetViewport({ 0, 0, (float)target->Size.X, (float)target->Size.Y });
	RTarget = target;
}

void Material::Create() {
	Layout->Create(MAX_SETS);

	GPipeline->SetRasterizer(VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_FRONT_BIT, VK_FRONT_FACE_CLOCKWISE);
	GPipeline->SetMSAA(VK_FALSE, VK_SAMPLE_COUNT_1_BIT);
	GPipeline->SetDepthStencil(true);
	GPipeline->SetPushConstants(VK_SHADER_STAGE_VERTEX_BIT, sizeof(PushConst3D));
	GPipeline->SetLayout(&Layout->VulkanDescriptorSetLayout);
	GPipeline->Create(RTarget->VRenderpass);
}

void Material::CreateInstance(MaterialInstance* instance) {
	if (instance->Descriptor)
		delete instance->Descriptor;

	instance->Descriptor = renderer->CreateNewDescriptorSet();

	instance->Descriptor->SetDescriptorLayout(Layout);
	//instance->Descriptor->SetDescriptorLayout
}