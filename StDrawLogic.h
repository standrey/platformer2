#include <Box2D\Box2D.h>
extern "C" {
	#include <SDL.h>
	#include <SDL_main.h>
	#include <SDL_image.h>
	#include <SDL_opengl.h>
}
#include <iostream>
#include "SharedResource.h"

class StDrawLogic {
private:
	int				m_openGL_texture_id;
	SDL_Rect		m_screenSize;
	SDL_GLContext	m_context;
	SDL_Window *	m_window;
	GLuint			m_backTexture;
	GLuint			m_tilesetTexture;
	SDL_Rect		m_worldSizePixels;
	void			DrawSprite(Sprite * pSpriteToDraw, const SDL_Rect & topLeftViewport);
	GLuint			LoadTilesetTexture(const std::string * );
	void			InitVideoOutput();
public:
					StDrawLogic() {} 
					StDrawLogic(const std::string * tileimage);
	void			DrawBackground();
	void			PrepareBackground(const Level * lvl);
	void			DrawTiles(std::unique_ptr<stSharedRes> & resource);
					~StDrawLogic();
};