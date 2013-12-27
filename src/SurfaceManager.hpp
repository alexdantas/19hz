#ifndef SURFACEMANAGER_HPP
#define SURFACEMANAGER_HPP

#include <map>
#include <string>

#include "SDL.h"
#include "SDL_ttf.h"

class SurfaceManager
{
private:
	static SurfaceManager* instance_;
	
	std::map< SDL_Surface*, int > surfaces;
	
	int cloned_xblit;
	int cloned_yblit;
	SDL_Surface* cloned;
	
	SurfaceManager();
	~SurfaceManager();
public:
	static SurfaceManager* instance();
	static void close();
	
	SDL_Surface* loadIMG(const std::string& filename);
	SDL_Surface* rotozoom(
		SDL_Surface* src, float angle, float zoomx = 1, float zoomy = 1
	);
	SDL_Surface* clip(SDL_Surface* src, SDL_Rect* srcrect);
	
	SDL_Surface* renderSolid(
		TTF_Font* font, const std::string& text, const SDL_Color& color
	);
	SDL_Surface* renderBlended(
		TTF_Font* font, const std::string& text, const SDL_Color& color
	);
	SDL_Surface* renderShaded(
		TTF_Font* font, const std::string& text,
		const SDL_Color& color, const SDL_Color& bg
	);
	
	void clone(int x, int y, SDL_Surface* src, SDL_Rect* srcrect = 0);
	
	SDL_Surface* getCloned(int& x, int& y);
	
	void free(SDL_Surface* surface);
};

#endif
