#include "PlaceToken.h"

using namespace OSK;
using namespace OSK::SceneSystem::Loader;

PlaceToken::PlaceToken() {
	tokenType = TokenType::PLACE;
}

void PlaceToken::ProcessData(Scanner* sc) {
	className = sc->ProcessText();
	instanceName = sc->ProcessText();

	Vector3TokenConverter vec;
	vec.Process(sc->ProcessDataToken());
	position = vec.GetVector3();

	vec.Process(sc->ProcessDataToken());
	angle = sc->ProcessFloat();
	axis = vec.GetVector3();

	vec.Process(sc->ProcessDataToken());
	scale = vec.GetVector3();
}

std::string PlaceToken::GetClassName() const {
	return className;
}

std::string PlaceToken::GetInstanceName() const {
	return instanceName;
}

Vector3f PlaceToken::GetPosition() const {
	return position;
}
Vector3f PlaceToken::GetAxis() const {
	return axis;
}
float PlaceToken::GetAngle() const {
	return angle;
}
Vector3f PlaceToken::GetScale() const {
	return scale;
}