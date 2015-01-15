#ifndef LEVEL_H
#define LEVEL_H

//#define SFML_DYNAMIC

#include <string>
#include <vector>
#include <map>
extern "C" { 
	#include "SDL.h"
}

struct SDL_FloatRect {
	float x1,y1;
	float x2,y2;
	float x3,y3;
	float x4,y4;
};

class Sprite {
private:
	//координаты назначения сохраняем в координатах окна
	SDL_Rect		m_DstRect;
	//координаты источника сохраняем в текстурных координатах
	SDL_FloatRect	m_SrcRect;
	//позиция спрайта в оконных координатах
	Uint32			m_xPos, m_yPos;
    Uint32			m_opacuty;
public:
	void		setSurfaceSrcRect(SDL_Rect r, int image_width, int image_height) {
		float texelWidth = 1.0f/image_width;
		float texelHeight = 1.0f/image_height;
		m_SrcRect.x1 = r.x * texelWidth;			m_SrcRect.y1 = r.y * texelHeight;
		m_SrcRect.x2 = (r.x + r.w) * texelWidth;	m_SrcRect.y2 = r.y * texelHeight;
		m_SrcRect.x3 = (r.x + r.w) * texelWidth;	m_SrcRect.y3 = (r.y + r.h) * texelHeight;
		m_SrcRect.x4 = r.x * texelWidth;			m_SrcRect.y4 = (r.y + r.h) * texelHeight;
	}
	void			setSurfaceDstRect(SDL_Rect r) {m_DstRect = r;}
	void			setPosition(Uint32 x , Uint32 y) {m_DstRect.x = m_xPos = x; m_DstRect.y = m_yPos = y;}
	void			setOpacuty(Uint32 o) {m_opacuty = o;}
	SDL_FloatRect	getSurfaceSrcRect() {return m_SrcRect;}
	SDL_Rect		getSurfaceDstRect() {return m_DstRect;}
	Uint32			getOpacuty() {return m_opacuty;}
};

struct Object
{
    int GetPropertyInt(std::string name);
    float GetPropertyFloat(std::string name);
    std::string GetPropertyString(std::string name);

    std::string name;
    std::string type;
    SDL_Rect rect;
    std::map<std::string, std::string> properties;
    Sprite sprite;
};

struct Layer
{
    int opacity;
    std::vector<Sprite> tiles;
};

class Level {
public:
    bool				LoadFromFile(std::string filename);
    Object				GetFirstObject(std::string name);
    std::vector<Object> GetObjects(std::string name);
	SDL_Texture*		PrepareDraw(SDL_Renderer*, SDL_Texture *);
	std::string&		GetImagepath() {return imagepath;}
	//return tile size in pixels
	SDL_Rect			GetTileSize() const;
	//return map size in tiles
	SDL_Rect			GetMapSize() const;
	~Level();
private:
    int					width, height, tileWidth, tileHeight, image_height, image_width;
    int					firstTileID;
	SDL_Rect			drawingBounds;
    std::vector<Object> objects;
    std::vector<Layer>	layers;
	std::string			imagepath;
};

#endif

