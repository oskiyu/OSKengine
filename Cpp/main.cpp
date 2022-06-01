#include <iostream>

#include <spirv_glsl.hpp>
#include <spirv_hlsl.hpp>

#include <OSKengine/FileIO.h>

#include <vector>
#include <utility>

void PrintInfo(const std::string& input) {
	const auto& code = OSK::IO::FileIO::ReadBinaryFromFile(input);
	spirv_cross::Compiler compiler((const uint32_t*)code.GetData(), code.GetSize() / 4);
	spirv_cross::ShaderResources resources = compiler.get_shader_resources();

	for (auto& i : resources.sampled_images) {
		std::cout << "set:" << compiler.get_decoration(i.id, spv::DecorationDescriptorSet) << std::endl;
		std::cout << "binding:" << compiler.get_decoration(i.id, spv::DecorationBinding) << std::endl;
		std::cout << "sampler id:" << compiler.get_name(i.id) << std::endl;
	}

	for (auto& i : resources.push_constant_buffers) {
		std::cout << "set:" << compiler.get_decoration(i.id, spv::DecorationDescriptorSet) << std::endl;
		std::cout << "binding:" << compiler.get_decoration(i.id, spv::DecorationBinding) << std::endl;
		std::cout << "sampler id:" << compiler.get_name(i.id) << std::endl;

		const spirv_cross::SPIRType& type = compiler.get_type(i.base_type_id);
		std::cout << "size: " << compiler.get_declared_struct_size(type) << std::endl;
	}

	for (auto& i : resources.uniform_buffers) {
		std::cout << "set:" << compiler.get_decoration(i.id, spv::DecorationDescriptorSet) << std::endl;
		std::cout << "binding:" << compiler.get_decoration(i.id, spv::DecorationBinding) << std::endl;
		std::cout << "sampler id:" << i.name << std::endl;

		const spirv_cross::SPIRType& type = compiler.get_type(i.base_type_id);
		std::cout << "size: " << compiler.get_declared_struct_size(type) << std::endl;
	}
}

void SpirvToHlsl(const std::string& input, const std::string& output) {
	const auto& code = OSK::IO::FileIO::ReadBinaryFromFile(input);
	spirv_cross::Compiler compiler((const uint32_t*)code.GetData(), code.GetSize() / 4);

	spirv_cross::CompilerHLSL compHlsl((const uint32_t*)code.GetData(), code.GetSize() / 4);
	spirv_cross::CompilerHLSL::Options hlslOptions{};
	hlslOptions.shader_model = 60;
	compHlsl.set_hlsl_options(hlslOptions);
	compHlsl.add_vertex_attribute_remap({ 0, "POSITION" });
	compHlsl.add_vertex_attribute_remap({ 1, "NORMAL" });
	compHlsl.add_vertex_attribute_remap({ 2, "COLOR" });
	compHlsl.add_vertex_attribute_remap({ 3, "TEXCOORD" });
	OSK::IO::FileIO::WriteFile(output, compHlsl.compile());
}

int main()  {
	SpirvToHlsl("vert.spv", "vert.hlsl");
	SpirvToHlsl("frag.spv", "frag.hlsl");

	return 0;
}
