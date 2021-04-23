#include <Jinx.hpp>

#include "FileIO.hpp"
#include <iostream>

struct JinxClass {
	Jinx::ScriptPtr Script;

	void UpdateScript() {
		if (Script.get())
			Script->Execute();
	}
};

int main() {
	auto scriptSource = FileIO::ReadFromFile("weapon.jclass");

	auto jinxRuntime = Jinx::CreateRuntime();

	JinxClass weapon0;
	weapon0.Script = jinxRuntime->CreateScript(scriptSource.c_str());
	weapon0.UpdateScript();

	std::cout << weapon0.Script->GetVariable("damage").GetNumber() << std::endl;

	return 0;
}