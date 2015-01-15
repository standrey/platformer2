#include "StDrawLogic.h"

StDrawLogic::StDrawLogic(const std::string * imagepath)  {
	m_openGL_texture_id = 1;
	//создаём окно
	InitVideoOutput();
	//загружаем тайлсет
	m_tilesetTexture = this->LoadTilesetTexture(imagepath);
}

void StDrawLogic::InitVideoOutput() {

	m_screenSize.w = 800;
	m_screenSize.h = 600;

	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		std::cout << "Error: Could not init SDL library! Error: " << SDL_GetError() << std::endl;
		exit(1);
	}
	
    m_window = SDL_CreateWindow("SDL2 Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, m_screenSize.w, m_screenSize.h, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (m_window == nullptr){
		std::cout << "Error: Could not create window! Error: " << SDL_GetError() << std::endl;
		exit(1);
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetSwapInterval(1);
	m_context = SDL_GL_CreateContext(m_window);
	int res = SDL_GL_MakeCurrent(m_window, m_context);
	if (res != 0) {
		SDL_DestroyWindow(m_window);
		exit(1);
	}
	
	if (SDL_GL_SetSwapInterval(1) < 0) {
		std::cout << "Warning: Unable to set vsync! SDL error: " << SDL_GetError() << "\n";
	}

	 //Initialize PNG loading 
	int imgFlags = IMG_INIT_PNG; 
	if( !( IMG_Init( imgFlags ) & imgFlags ) )
		std::cout << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError();

	glViewport( 0, 0, m_screenSize.w, m_screenSize.h );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, m_screenSize.w, 0, m_screenSize.h, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

GLuint StDrawLogic::LoadTilesetTexture(const std::string * tileset_path) {
	SDL_Surface *loadedSurface = IMG_Load(tileset_path->c_str());
	if (loadedSurface == nullptr){
		std::cout << "Warning: IMG_Load Error: " << SDL_GetError() << std::endl;
		return 0;
	}

	SDL_Surface *gScreenSurface = SDL_GetWindowSurface(m_window);
	SDL_Surface *optimizedSurface = SDL_ConvertSurface( loadedSurface, gScreenSurface->format, NULL ); 
	if( optimizedSurface == NULL ) { 
		std::cout << "Warning: Unable to optimize image " << tileset_path << " SDL Error: " <<  SDL_GetError() << "\n"; 
		optimizedSurface = loadedSurface;
	}

	GLuint resTexture;
	glGenTextures(m_openGL_texture_id++,&resTexture);
	glBindTexture(GL_TEXTURE_2D,resTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, optimizedSurface->w, optimizedSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, optimizedSurface->pixels);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    SDL_FreeSurface(loadedSurface);
	SDL_FreeSurface(optimizedSurface);
	return resTexture;
}

void StDrawLogic::DrawSprite(Sprite * pSpriteToDraw, const SDL_Rect & topLeftViewport) {

	//если спрайт для отрисовки не попадает в область экрана , то скипаем его
	if (pSpriteToDraw->getSurfaceDstRect().x < topLeftViewport.x || 
		pSpriteToDraw->getSurfaceDstRect().x > (topLeftViewport.w + topLeftViewport.x))
		return;
		
	//увеличиваем размер тайла при копировании в 4 раза
	SDL_Rect window_dst_rect;
	memcpy(&window_dst_rect, &pSpriteToDraw->getSurfaceDstRect(), sizeof(SDL_Rect));
	
	window_dst_rect.x = (window_dst_rect.x - topLeftViewport.x) * 4;
	window_dst_rect.y *= 4; 
	window_dst_rect.w *= 4; 
	window_dst_rect.h *= 4;
	
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glBindTexture(GL_TEXTURE_2D, m_tilesetTexture);

	glBegin( GL_QUADS );

		glTexCoord2f(pSpriteToDraw->getSurfaceSrcRect().x1, pSpriteToDraw->getSurfaceSrcRect().y1); 
		glVertex3f( window_dst_rect.x, m_screenSize.h - window_dst_rect.y, 0.0f );
		glTexCoord2f(pSpriteToDraw->getSurfaceSrcRect().x2, pSpriteToDraw->getSurfaceSrcRect().y2); 
		glVertex3f( window_dst_rect.x + window_dst_rect.w,  m_screenSize.h - window_dst_rect.y, 0.0f );
		glTexCoord2f(pSpriteToDraw->getSurfaceSrcRect().x3, pSpriteToDraw->getSurfaceSrcRect().y3); 
		glVertex3f( window_dst_rect.x + window_dst_rect.w, m_screenSize.h - (window_dst_rect.y + window_dst_rect.h), 0.0f );
		glTexCoord2f(pSpriteToDraw->getSurfaceSrcRect().x4, pSpriteToDraw->getSurfaceSrcRect().y4); 
		glVertex3f( window_dst_rect.x, m_screenSize.h - (window_dst_rect.y + window_dst_rect.h), 0.0f );
		
	glEnd(); 
	glLoadIdentity();
	glDisable(GL_TEXTURE_2D);
}

void StDrawLogic::DrawTiles(std::unique_ptr<stSharedRes> & resource) {
	glClearColor ( 1.0, 1.0, 1.0, 0.0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	//вычисляем область рисования на основании положения игрока

	SDL_Rect topLeftViewport;
	topLeftViewport.y = 0;
    topLeftViewport.w = m_screenSize.w / 4;
    topLeftViewport.h = m_screenSize.h / 4;

	
	if (resource->playerBody->GetPosition().x <= m_screenSize.w/8)
		topLeftViewport.x = 0;
	else if (resource->playerBody->GetPosition().x > m_worldSizePixels.w - m_screenSize.w/8)
		topLeftViewport.x = m_worldSizePixels.w - m_screenSize.w/4;
	else
		topLeftViewport.x = resource->playerBody->GetPosition().x - m_screenSize.w/8;

	//рисуем игрока
	DrawSprite(&resource->player.sprite, topLeftViewport);
	
	//рисуем монетки
    for(unsigned i = 0; i < resource->coin.size(); i++)
		DrawSprite(&resource->coin[i].sprite, topLeftViewport);

		//рисуем врагов
    for(unsigned i = 0; i < resource->enemy.size(); i++)
		DrawSprite(&resource->enemy[i].sprite, topLeftViewport);
	
	SDL_GL_SwapWindow(m_window);
}

void StDrawLogic::DrawBackground() {
}

void StDrawLogic::PrepareBackground(const Level * lvl)
{
	//получаем размер игрового мира в пикселях
	m_worldSizePixels.w = lvl->GetMapSize().w * lvl->GetTileSize().w;
	m_worldSizePixels.h = lvl->GetMapSize().h * lvl->GetTileSize().h;
	/*
    for(int layer = 0; layer < layers->size(); layer++)
		//SDL_SetTextureAlphaMod(tex, layers[layer].opacity);
        for(int tile = 0; tile < layers[layer].tiles.size(); tile++) 
			SDL_RenderCopy(	renderer, tex, &layers[layer].tiles[tile].getSurfaceSrcRect(), &layers[layer].tiles[tile].getSurfaceDstRect());
	*/
}

StDrawLogic::~StDrawLogic() {
	glDeleteTextures(1, &m_tilesetTexture);
	glDeleteTextures(1, &m_backTexture);
	SDL_GL_DeleteContext(m_context);
	SDL_DestroyWindow(m_window);
}