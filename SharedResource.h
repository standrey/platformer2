#ifndef SHARED_RES_H
#define SHARED_RES_H

#include <Box2D\Box2D.h>
extern "C" {
	#include <SDL.h>
}
#include <vector>
#include "level.h"

struct stSharedRes {
	//������� �������� ������ ��� ������ ������
	std::vector<Object> coin;
	//��������� �� ��������������� ������ �� box2d ����
	std::vector<b2Body*> coinBody;

	std::vector<Object> enemy;
	std::vector<b2Body*> enemyBody;

	Object player;
	b2Body* playerBody;
};

#endif