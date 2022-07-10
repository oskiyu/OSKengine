#include "GraphicsPipelineOgl.h"

#include "FileIO.h"

#include <string>

using namespace OSK;
using namespace OSK::GRAPHICS;

void GraphicsPipelineOgl::Create(const MaterialLayout* layout, IGpu* device, const PipelineCreateInfo& info, Format format, const VertexInfo& vertexInfo) {
	pipelineInfo = info;

	int result = 0;
	char log[512];

	OglShaderHandler vertexShader = glCreateShader(GL_VERTEX_SHADER);
	auto vertexCode = IO::FileIO::ReadFromFile(info.vertexPath);
	const char* vertexData = vertexCode.c_str();
	glShaderSource(vertexShader, 1, &vertexData, NULL);
	glCompileShader(vertexShader);

	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(vertexShader, 512, NULL, log);
		OSK_ASSERT(false, log);
	}

	OglShaderHandler fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	auto fragmentCode = IO::FileIO::ReadFromFile(info.fragmentPath);
	const char* fragmentData = fragmentCode.c_str();
	glShaderSource(fragmentShader, 1, &fragmentData, NULL);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(fragmentShader, _countof(log), NULL, log);
		OSK_ASSERT(false, log);
	}

	// Shader program:
	pipelineHandler = glCreateProgram();
	glAttachShader(pipelineHandler, vertexShader);
	glAttachShader(pipelineHandler, fragmentShader);
	glLinkProgram(pipelineHandler);

	glGetProgramiv(pipelineHandler, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(pipelineHandler, _countof(log), NULL, log);
		OSK_ASSERT(false, log);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

OglPipelineHandler GraphicsPipelineOgl::GetPipelineHandler() const {
	return pipelineHandler;
}
