#include "StInput.h"

bool StInput::ProcessInput(b2Body* playerBody) {
	SDL_Event e;	
	while(SDL_PollEvent(&e))
	{
		switch(e.type)
		{
		case SDL_QUIT:
				return false;
				break;
		case SDL_KEYDOWN:
			switch (e.key.keysym.sym)
			{
				case SDLK_LEFT:
					playerBody->SetLinearVelocity(b2Vec2(-5.0f, 0.0f));
				break;
				case SDLK_RIGHT:
					playerBody->SetLinearVelocity(b2Vec2(5.0f, 0.0f));
				break;
				case SDLK_UP: 
					{
					b2Vec2 tmp = playerBody->GetLinearVelocity();
					if (tmp.y==0)
						playerBody->SetLinearVelocity(b2Vec2(0.0f, -15.0f));
					break;
					}
				case SDLK_ESCAPE:
					return false;
				break;
			}
			break;
		}
	}
	return true;
}