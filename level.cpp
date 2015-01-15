#include "level.h"
#include <iostream>
#include "tinyxml.h"



int Object::GetPropertyInt(std::string name)
{
    return atoi(properties[name].c_str());
}

float Object::GetPropertyFloat(std::string name)
{
    return strtod(properties[name].c_str(), NULL);
}

std::string Object::GetPropertyString(std::string name)
{
    return properties[name];
}

bool Level::LoadFromFile(std::string filename)
{
    TiXmlDocument levelFile(filename.c_str());

    // Загружаем XML-карту
    if(!levelFile.LoadFile())
    {
        std::cout << "Loading level \"" << filename << "\" failed." << std::endl;
        return false;
    }

    // Работаем с контейнером map
    TiXmlElement *map;
    map = levelFile.FirstChildElement("map");

    // Пример карты: <map version="1.0" orientation="orthogonal"
    // width="10" height="10" tilewidth="34" tileheight="34">
    width = atoi(map->Attribute("width"));
    height = atoi(map->Attribute("height"));
    tileWidth = atoi(map->Attribute("tilewidth"));
    tileHeight = atoi(map->Attribute("tileheight"));

    // Берем описание тайлсета и идентификатор первого тайла
    TiXmlElement *tilesetElement;
    tilesetElement = map->FirstChildElement("tileset");
    firstTileID = atoi(tilesetElement->Attribute("firstgid"));

    // source - путь до картинки в контейнере image
    TiXmlElement *image;
    image = tilesetElement->FirstChildElement("image");
    this->imagepath = image->Attribute("source");

    // Получаем количество столбцов и строк тайлсета
	image_width = atoi(image->Attribute("width"));
	image_height = atoi(image->Attribute("height"));
    int columns = image_width / tileWidth;
    int rows = image_height / tileHeight;

    // Вектор из прямоугольников изображений (TextureRect)
    std::vector<SDL_Rect> subRects;

    for(int y = 0; y < rows; y++)
    for(int x = 0; x < columns; x++)
    {
        SDL_Rect rect;
		rect.y = y * tileHeight;
        rect.h = tileHeight;
        rect.x = x * tileWidth;
        rect.w = tileWidth;
        subRects.push_back(rect);
    }

    // Работа со слоями
    TiXmlElement *layerElement;
    layerElement = map->FirstChildElement("layer");
    while(layerElement)
    {
        Layer layer;
        
        // Если присутствует opacity, то задаем прозрачность слоя, иначе он полностью непрозрачен
        if (layerElement->Attribute("opacity") != NULL)
        {
            float opacity = strtod(layerElement->Attribute("opacity"), NULL);
            layer.opacity = int(255 * opacity);
        }
        else
        {
            layer.opacity = 255;
        }

        // Контейнер <data>
        TiXmlElement *layerDataElement;
        layerDataElement = layerElement->FirstChildElement("data");

        if(layerDataElement == NULL)
        {
            std::cout << "Bad map. No layer information found." << std::endl;
        }

        // Контейнер <tile> - описание тайлов каждого слоя
        TiXmlElement *tileElement;
        tileElement = layerDataElement->FirstChildElement("tile");

        if(tileElement == NULL)
        {
            std::cout << "Bad map. No tile information found." << std::endl;
            return false;
        }

        int x = 0;
        int y = 0;

        while(tileElement)
        {
            int tileGID = atoi(tileElement->Attribute("gid"));
            int subRectToUse = tileGID - firstTileID;

            // Устанавливаем TextureRect каждого тайла
            if (subRectToUse >= 0)
            {
				Sprite sprite;
				sprite.setSurfaceSrcRect(subRects[subRectToUse], image_width, image_height);
				SDL_Rect tmp_DstRect; 
				tmp_DstRect.x = x*tileWidth; 
				tmp_DstRect.y = y*tileWidth; 
				tmp_DstRect.h = tileHeight;
				tmp_DstRect.w = tileWidth;
				sprite.setSurfaceDstRect(tmp_DstRect);
				sprite.setOpacuty(layer.opacity);
                layer.tiles.push_back(sprite);
            }

            tileElement = tileElement->NextSiblingElement("tile");

            x++;
            if (x >= width)
            {
                x = 0;
                y++;
                if(y >= height)
                    y = 0;
            }
        }

        layers.push_back(layer);

        layerElement = layerElement->NextSiblingElement("layer");
    }

    // Работа с объектами
    TiXmlElement *objectGroupElement;

    // Если есть слои объектов
    if (map->FirstChildElement("objectgroup") != NULL)
    {
        objectGroupElement = map->FirstChildElement("objectgroup");
        while (objectGroupElement)
        {
            // Контейнер <object>
            TiXmlElement *objectElement;
            objectElement = objectGroupElement->FirstChildElement("object");
           
            while(objectElement)
            {
                // Получаем все данные - тип, имя, позиция, etc
                std::string objectType;
                if (objectElement->Attribute("type") != NULL)
                {
                    objectType = objectElement->Attribute("type");
                }
                std::string objectName;
                if (objectElement->Attribute("name") != NULL)
                {
                    objectName = objectElement->Attribute("name");
                }
                int x = atoi(objectElement->Attribute("x"));
                int y = atoi(objectElement->Attribute("y"));

                int width, height;

                Sprite sprite;
				SDL_Rect zeroRect;
				memset(&zeroRect, 0, sizeof(zeroRect));
                sprite.setSurfaceSrcRect(zeroRect, image_width, image_height);
                sprite.setPosition(x, y);
				zeroRect.h = tileHeight; zeroRect.w = tileWidth;
				sprite.setSurfaceDstRect(zeroRect);

                if (objectElement->Attribute("width") != NULL)
                {
                    width = atoi(objectElement->Attribute("width"));
                    height = atoi(objectElement->Attribute("height"));
                }
                else
                {
                    width = subRects[atoi(objectElement->Attribute("gid")) - firstTileID].w;
                    height = subRects[atoi(objectElement->Attribute("gid")) - firstTileID].h;
                    sprite.setSurfaceSrcRect(subRects[atoi(objectElement->Attribute("gid")) - firstTileID], image_width, image_height);
                }

                // Экземпляр объекта
                Object object;
                object.name = objectName;
                object.type = objectType;
                object.sprite = sprite;

                SDL_Rect objectRect;
                objectRect.y = y;
                objectRect.x = x;
                objectRect.h = height;
                objectRect.w = width;
                object.rect = objectRect;

                // "Переменные" объекта
                TiXmlElement *properties;
                properties = objectElement->FirstChildElement("properties");
                if (properties != NULL)
                {
                    TiXmlElement *prop;
                    prop = properties->FirstChildElement("property");
                    if (prop != NULL)
                    {
                        while(prop)
                        {
                            std::string propertyName = prop->Attribute("name");
                            std::string propertyValue = prop->Attribute("value");

                            object.properties[propertyName] = propertyValue;

                            prop = prop->NextSiblingElement("property");
                        }
                    }
                }

                // Пихаем объект в вектор
                objects.push_back(object);

                objectElement = objectElement->NextSiblingElement("object");
            }
            objectGroupElement = objectGroupElement->NextSiblingElement("objectgroup");
        }
    }
    else
    {
        std::cout << "No object layers found..." << std::endl;
    }

    return true;
}

Object Level::GetFirstObject(std::string name)
{
    // Только первый объект с заданным именем
    for (unsigned i = 0; i < objects.size(); i++)
        if (objects[i].name == name)
            return objects[i];
}

std::vector<Object> Level::GetObjects(std::string name)
{
    // Все объекты с заданным именем
    std::vector<Object> vec;
    for(unsigned i = 0; i < objects.size(); i++)
        if(objects[i].name == name)
            vec.push_back(objects[i]);

    return vec;
}

SDL_Rect Level::GetTileSize() const  {
	SDL_Rect res;
	res.x = tileWidth;
	res.y = tileHeight;
    return res;
}

SDL_Rect Level::GetMapSize() const {
	SDL_Rect res;
	res.x = width;
	res.y = height;
    return res;
}

Level::~Level() {
}