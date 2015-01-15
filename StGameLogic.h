#include <Box2D\Box2D.h>
extern "C" {
	#include <SDL.h>
}
#include "SharedResource.h"

class StGameLogic {
public:
	void HandleCollisions(std::unique_ptr<stSharedRes> & res);
	void Step(std::unique_ptr<stSharedRes> & res);
};