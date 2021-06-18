#include "MyGame.h"

int main() {
	OSK::Logger::Start();

	{
		UniquePtr<MyGame> game = new MyGame();

		game->Run();
	}

	OSK::Logger::Close();

	return 0;
}