#include "StGameLogic.h"
#include <vector>

void StGameLogic::Step(std::unique_ptr<stSharedRes> & gameUnits) {
	for(int i = 0; i < gameUnits->coin.size(); i++)
        gameUnits->coin[i].sprite.setPosition(gameUnits->coinBody[i]->GetPosition().x, gameUnits->coinBody[i]->GetPosition().y);
	for(int i = 0; i < gameUnits->enemy.size(); i++)
		gameUnits->enemy[i].sprite.setPosition(gameUnits->enemyBody[i]->GetPosition().x, gameUnits->enemyBody[i]->GetPosition().y);
	gameUnits->player.sprite.setPosition(gameUnits->playerBody->GetPosition().x, gameUnits->playerBody->GetPosition().y);
}

void StGameLogic::HandleCollisions(std::unique_ptr<stSharedRes> & res) {
		std::vector<int> coinBodyToRemove;
		std::vector<int> enemyBodyToRemove;

		for(b2ContactEdge* ce = res->playerBody->GetContactList(); ce; ce = ce->next)
        {
            b2Contact* c = ce->contact;
		
            for(int i = 0; i < res->coinBody.size(); i++)
                if(c->GetFixtureA() == res->coinBody[i]->GetFixtureList())
                {
					coinBodyToRemove.push_back(i);
                }
            
			for(int i = 0; i < res->enemyBody.size(); i++)
                if(c->GetFixtureA() == res->enemyBody[i]->GetFixtureList())
                {
                    if(res->playerBody->GetPosition().y < res->enemyBody[i]->GetPosition().y)
                    {
                        res->playerBody->SetLinearVelocity(b2Vec2(0.0f, -10.0f));
						enemyBodyToRemove.push_back(i);
                    }
					else
                    {
                        int tmp = (res->playerBody->GetPosition().x < res->enemyBody[i]->GetPosition().x) ? -1 : 1;
                        res->playerBody->SetLinearVelocity(b2Vec2(10.0f * tmp, 0.0f));
                    }
                }
		}

		for (auto cbIndex = coinBodyToRemove.begin(); cbIndex != coinBodyToRemove.end(); cbIndex++)
		{
			res->coinBody[*cbIndex]->DestroyFixture(res->coinBody[*cbIndex]->GetFixtureList());
            res->coin.erase(res->coin.begin() + *cbIndex);
            res->coinBody.erase(res->coinBody.begin() + *cbIndex);
		}
		coinBodyToRemove.clear();

		for (auto ebIndex = enemyBodyToRemove.begin(); ebIndex != enemyBodyToRemove.end(); ebIndex++)
		{
			res->enemyBody[*ebIndex]->DestroyFixture(res->enemyBody[*ebIndex]->GetFixtureList());
            res->enemy.erase(res->enemy.begin() + *ebIndex);
            res->enemyBody.erase(res->enemyBody.begin() + *ebIndex);
		}
		enemyBodyToRemove.clear();

		for(int i = 0; i < res->enemyBody.size(); i++)
			if(res->enemyBody[i]->GetLinearVelocity() == b2Vec2_zero)
			{
				int tmp = (rand() % 2 == 1) ? 1 : -1;
				res->enemyBody[i]->SetLinearVelocity(b2Vec2(5.0f * tmp, 0.0f));
			}
}