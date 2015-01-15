#include <Box2D\Box2D.h>
extern "C" {
	#include <SDL.h>
}

class StInput {
public:
	bool ProcessInput(b2Body* playerBody);
};