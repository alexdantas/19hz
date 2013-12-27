#include "SDL_ttf.h"

#include "SurfaceManager.hpp"

#include "SDLBase.hpp"

using std::map;
using std::string;

SurfaceManager* SurfaceManager::instance_ = 0;

SurfaceManager::SurfaceManager() : cloned(0)
{
}

SurfaceManager::~SurfaceManager()
{
}

SurfaceManager* SurfaceManager::instance()
{
	if( !instance_ )
		instance_ = new SurfaceManager();
	
	return instance_;
}

void SurfaceManager::close()
{
	if( instance_ )
	{
		delete instance_;
		instance_ = 0;
	}
}

SDL_Surface* SurfaceManager::loadIMG(const string& filename)
{
	SDL_Surface* ret = SDLBase::loadIMG( filename );
	surfaces[ ret ] = 1;
	return ret;
}

SDL_Surface* SurfaceManager::rotozoom(
	SDL_Surface* src, float angle, float zoomx, float zoomy
)
{
	SDL_Surface* ret = SDLBase::rotozoom( src, angle, zoomx, zoomy );
	surfaces[ ret ] = 1;
	return ret;
}

SDL_Surface* SurfaceManager::clip(SDL_Surface* src, SDL_Rect* srcrect)
{
	SDL_Surface* ret = SDLBase::clip( src, srcrect );
	surfaces[ ret ] = 1;
	return ret;
}

SDL_Surface* SurfaceManager::renderSolid(
	TTF_Font* font, const std::string& text, const SDL_Color& color
)
{
	SDL_Surface* ret = TTF_RenderUTF8_Solid( font, text.c_str(), color );
	surfaces[ ret ] = 1;
	return ret;
}

SDL_Surface* SurfaceManager::renderBlended(
	TTF_Font* font, const std::string& text, const SDL_Color& color
)
{
	SDL_Surface* ret = TTF_RenderUTF8_Blended( font, text.c_str(), color );
	surfaces[ ret ] = 1;
	return ret;
}

SDL_Surface* SurfaceManager::renderShaded(
	TTF_Font* font, const std::string& text,
	const SDL_Color& color, const SDL_Color& bg
)
{
	SDL_Surface* ret = TTF_RenderUTF8_Shaded( font, text.c_str(), color, bg );
	surfaces[ ret ] = 1;
	return ret;
}

void SurfaceManager::clone(int x, int y, SDL_Surface* src, SDL_Rect* srcrect)
{
	if( src )
	{
		cloned_xblit = x;
		cloned_yblit = y;
		
		if( srcrect )
			cloned = clip( src, srcrect );
		else
		{
			cloned = src;
			++surfaces[ src ];
		}
	}
}

SDL_Surface* SurfaceManager::getCloned(int& x, int& y)
{
	x = cloned_xblit;
	y = cloned_yblit;
	
	SDL_Surface* ret = cloned;
	cloned = 0;
	return ret;
}

void SurfaceManager::free(SDL_Surface* surface)
{
	if (!surface)
		return;
	
	map< SDL_Surface*, int >::iterator it = surfaces.find( surface );
	
	if( it->second > 1 )
		--it->second;
	else
	{
		SDL_FreeSurface( surface );
		surfaces.erase( it );
	}
}
