#pragma comment(lib,"Box2D.lib")
#pragma comment(lib,"SDL2.lib")
#pragma comment(lib,"SDL2main.lib")
#pragma comment(lib,"SDL2_image.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"opengl32.lib")

#include <windows.h>
#include <gl\GLU.h>
#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <map>


#include <Box2D/Box2D.h>
extern "C" {
	#include <SDL.h>
	#include <SDL_main.h>
	#include <SDL_image.h>
	#include <SDL_opengl.h>
}

#include "level.h"
#include "StInput.h"
#include "StGameLogic.h"
#include "StDrawLogic.h"
#include "SharedResource.h"

#define FRAMES_PER_SECOND 60

void InitLevelObjects(	Level & lvl,
						std::unique_ptr<stSharedRes>& gameUnits,
						b2World & world) {
	SDL_Rect tileSize = lvl.GetTileSize();

	//создаём box2d статические объекты по данным из уровня
	std::vector<Object> block = lvl.GetObjects("block");
    for(int i = 0; i < block.size(); i++)
    {
        b2BodyDef bodyDef;
        bodyDef.type = b2_staticBody;
        bodyDef.position.Set(block[i].rect.x + tileSize.x / 2 * (block[i].rect.w / tileSize.x - 1),
            block[i].rect.y + tileSize.y / 2 * (block[i].rect.h / tileSize.y - 1));
        b2Body* body = world.CreateBody(&bodyDef);
        b2PolygonShape shape;
        shape.SetAsBox(block[i].rect.w / 2, block[i].rect.h / 2);
        body->CreateFixture(&shape,1.0f);
    }

	//создаём box2d динамические объекты по данным из уровня (монеты)
	gameUnits->coin = lvl.GetObjects("coin");
    for(int i = 0; i < gameUnits->coin.size(); i++)
    {
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(gameUnits->coin[i].rect.x + tileSize.x / 2 * (gameUnits->coin[i].rect.w / tileSize.x - 1),
            gameUnits->coin[i].rect.y + tileSize.y / 2 * (gameUnits->coin[i].rect.h / tileSize.y - 1));
        bodyDef.fixedRotation = true;
        b2Body* body = world.CreateBody(&bodyDef);
        b2PolygonShape shape;
        shape.SetAsBox(gameUnits->coin[i].rect.w / 2, gameUnits->coin[i].rect.h / 2);
        body->CreateFixture(&shape,1.0f);
        gameUnits->coinBody.push_back(body);
    }

	//создаём box2d динамические объекты по данным из уровня (враги)
	int x = gameUnits->coinBody[0]->GetPosition().x;
    gameUnits->enemy = lvl.GetObjects("enemy");
    for(int i = 0; i < gameUnits->enemy.size(); i++)
    {
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(gameUnits->enemy[i].rect.x +
            tileSize.x / 2 * (gameUnits->enemy[i].rect.w / tileSize.x - 1),
            gameUnits->enemy[i].rect.y + tileSize.y / 2 * (gameUnits->enemy[i].rect.h / tileSize.y - 1));
        bodyDef.fixedRotation = true;
        b2Body* body = world.CreateBody(&bodyDef);
        b2PolygonShape shape;
        shape.SetAsBox(gameUnits->enemy[i].rect.w / 2, gameUnits->enemy[i].rect.h / 2);
        body->CreateFixture(&shape,1.0f);
        gameUnits->enemyBody.push_back(body);
    }

	//создаём игрока
    gameUnits->player = lvl.GetFirstObject("player");
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(gameUnits->player.rect.x, gameUnits->player.rect.y);
    bodyDef.fixedRotation = true;
    gameUnits->playerBody = world.CreateBody(&bodyDef);
    b2PolygonShape shape;
	shape.SetAsBox(gameUnits->player.rect.w / 2, gameUnits->player.rect.h / 2);
    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 10.0f; fixtureDef.friction = 0.3f;
    gameUnits->playerBody->CreateFixture(&fixtureDef);
}

int main(int argv, char * argc[])
{
	srand(SDL_GetTicks());
	
	std::unique_ptr<stSharedRes> gameUnits(new stSharedRes());
    
	Level lvl;
    lvl.LoadFromFile("resources/platformer.tmx");
    b2Vec2 gravity(0.0f, 10.0f);
    b2World world(gravity);
	
	//создаём все необходимые структуры игрового мира из файла
	InitLevelObjects(lvl, gameUnits, world);

	bool isOpen = true;
	Uint32 t = SDL_GetTicks();
	Uint32 framesPS = 0;
	std::shared_ptr<StInput> InputObject(new StInput());
	std::shared_ptr<StGameLogic> gameLogic(new StGameLogic());
	std::shared_ptr<StDrawLogic> drawLogic(new StDrawLogic(&lvl.GetImagepath()));

	drawLogic->PrepareBackground(&lvl);

	const float STEP_TIME = 1.0f/60.0f;
	while(isOpen)
	{	
		Uint32 loopTick = 0;
		
		isOpen = InputObject->ProcessInput(gameUnits->playerBody);
		//std::cout<< "Process input " << SDL_GetTicks() - t << std::endl;
		world.Step(STEP_TIME, 8, 1);
		//std::cout<< "World step " << SDL_GetTicks() - t << std::endl;
		gameLogic->HandleCollisions(gameUnits);
		//std::cout<< "Handle collision " << SDL_GetTicks() - t << std::endl;

		drawLogic->DrawBackground();
		//std::cout<< "Draw background " << SDL_GetTicks() - t << std::endl;

		drawLogic->DrawTiles(gameUnits);
		//std::cout<< "Draw tiles " << SDL_GetTicks() - t << std::endl;

		gameLogic->Step(gameUnits);
		//std::cout<< "World step " << SDL_GetTicks() - t << std::endl;

		++framesPS;

		if (SDL_GetTicks() - t >= 1000) {
			std::cout<< "Frames per second " << framesPS << std::endl;
			t = SDL_GetTicks();
			framesPS = 0;
		}
	}

	SDL_Quit();
	return 0;
}