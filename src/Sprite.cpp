/// @ingroup MOD_SPRITE
/// @file Sprite.cpp
/// @brief Implementations of all methods of the Sprite class
/// @author Matheus Pimenta

#include "Sprite.hpp"

#include <iostream>

#include "SDLBase.hpp"
#include "SurfaceManager.hpp"

using std::string;

Sprite::Sprite() : src(0), rotozoomed(0), angle_(0), zoomx(1), zoomy(1) {}

Sprite::Sprite(const string& filename) : rotozoomed(0), angle_(0), zoomx(1), zoomy(1){
	load_(filename);

}

Sprite::~Sprite() {
	unload();
}

void Sprite::load (const string& filename)
{
	unload ();
	
	load_ ( filename );
}

void Sprite::load_ (const string& filename)
{
	src = SurfaceManager::instance()->loadIMG( filename );
	
	srcrect_.x = 0;
	srcrect_.y = 0;
	
	srcrect_.w = src->w;
	srcrect_.h = src->h;
}

void Sprite::unload ()
{
	if ( src )
	{
		SurfaceManager::instance()->free( src );
		src = NULL;
		
		restore ();
	}
}

void Sprite::update() {}

void Sprite::clip(int x, int y, int w, int h)
{
	srcrect_.x = x;
	srcrect_.y = y;
	srcrect_.w = w;
	srcrect_.h = h;
	
	if ( rotozoomed )
		rotozoom ();
}

const SDL_Rect& Sprite::srcrect () const
{
	return srcrect_;
}

int Sprite::srcW () const
{
	return src->w;
}

int Sprite::srcH () const
{
	return src->h;
}

int Sprite::rectW () const
{
	return ((!rotozoomed) ? srcrect_.w : rotozoomed->w);
}

int Sprite::rectH () const
{
	return ((!rotozoomed) ? srcrect_.h : rotozoomed->h);
}

void Sprite::render (int x, int y, bool center, bool surface_manager) {
	// calculate x and y
	
	if (!center) {
		if (rotozoomed) {	// not center and rotozoomed
			x += (srcrect_.w - rotozoomed->w)/2;
			y += (srcrect_.h - rotozoomed->h)/2;
		}
	}
	else {
		if (!rotozoomed) {	// center but not rotozoomed
			x -= srcrect_.w/2;
			y -= srcrect_.h/2;
		}
		else {	// center and rotozoomed
			x -= rotozoomed->w/2;
			y -= rotozoomed->h/2;
		}
	}
	
	// render for surface manager
	if (surface_manager) {
		if (rotozoomed)
			SurfaceManager::instance()->clone(x, y, rotozoomed);
		else if (src)
			SurfaceManager::instance()->clone(x, y, src, &srcrect_);
	}
	// normal render
	else {
		SDL_Rect dstrect;
		dstrect.x = x;
		dstrect.y = y;
		if (rotozoomed)
			SDLBase::renderSurface(rotozoomed, NULL, &dstrect);
		else if (src)
			SDLBase::renderSurface(src, &srcrect_, &dstrect);
	}
}

void Sprite::rotozoom (float angle, float zoomx, float zoomy, bool force)
{
	if (	( force ) ||
			(	( angle != angle_ ) ||
				( zoomx != this->zoomx ) ||
				( zoomy != this->zoomy )	)	)
	{
		angle_ = angle;
		this->zoomx = zoomx;
		this->zoomy = zoomy;
		
		rotozoom ();
	}
}

void Sprite::rotozoom ()
{
	restore ();
	
	if (	( !srcrect_.x ) &&
			( !srcrect_.y ) &&
			( srcrect_.w == src->w ) &&
			( srcrect_.h == src->h )	)
	{
		rotozoomed = SurfaceManager::instance()->rotozoom(src, angle_, zoomx, zoomy);
	}
	else
	{
		SDL_Surface* tmp = SurfaceManager::instance()->clip( src, &srcrect_ );
		rotozoomed = SurfaceManager::instance()->rotozoom(
			tmp, angle_, zoomx, zoomy
		);
		SurfaceManager::instance()->free( tmp );
	}
}

void Sprite::restore()
{
	if( rotozoomed )
	{
		SurfaceManager::instance()->free( rotozoomed );
		rotozoomed = NULL;
	}
}

float Sprite::angle () const
{
	return angle_;
}

void Sprite::setAlpha(float alpha) {
	SDL_SetAlpha(src, SDL_SRCALPHA, alpha * 255);
}

/**
 * Cria um gradiente radial quadratico com uma posicao, raio e cor definidos
 * \param x coordenada x do centro do gradiente
 * \param y coordenada y do centro do gradiente
 * \param radius raio da circunferencia (nenhum pixel fora desse raio eh alterado)
 * \param red/green/blue/alpha componentes da cor do gradiente
 */
void Sprite::gradient(Uint32 x, Uint32 y, int radius, Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha){
	float distance, ratio, circleAlpha, transparency;
	Uint8 r8, g8, b8, a8;
	Uint32 r, g, b, a;
	int xDraw, yDraw;
	for (int i = -radius; i <= radius; i++){
		for (int j = -radius; j <= radius; j++){
			//setPixel(i, j, SDL_MapRGBA(surface->format, r, g, b, a * ( i*xStep * j*yStep ) ) );
			//setPixel(i, j, SDL_MapRGBA(surface->format, r, g, b, a *( sin(i*xStep) * sin(j*yStep) ) ) );
			distance = sqrt(i*i + j*j);
			ratio = 1-(distance/radius);
			xDraw = x+i;
			yDraw = y+j;
			if (i*i+j*j<radius*radius && xDraw>0 && xDraw<src->w && yDraw>0 && yDraw<src->h){
				circleAlpha = ratio*ratio;
				transparency = (1-circleAlpha);
				//std::cout<<circleAlpha<<","<<transparency<<std::endl;
				SDL_GetRGBA(getPixel(xDraw, yDraw), src->format, &r8, &g8, &b8, &a8);
				r = red*circleAlpha + r8*transparency;
				if (r>255) r = 255;
				g = green*circleAlpha + g8*transparency;
				if (g>255) g = 255;
				b = blue*circleAlpha + b8*transparency;
				if (b>255) b = 255;
				a = alpha*circleAlpha + a8*transparency;
				if (a>255) a = 255;
				setPixel(x+i, y+j, SDL_MapRGBA(src->format, r, g, b, a));
			}
		}
	}
}

/**
 * 
 */
void Sprite::tint(float f){
}

//From http://sdl.beuc.net/sdl.wiki/Pixel_Access
Uint32 Sprite::getPixel(int x, int y, SDL_Surface* source){
	if (!source) source = src;
	int bpp = source->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8 *p = (Uint8 *)source->pixels + y * source->pitch + x * bpp;
	
	switch(bpp) {
	case 1:
		return *p;
		break;
		
	case 2:
		return *(Uint16 *)p;
		break;
		
	case 3:
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
		break;
		
	case 4:
		return *(Uint32*)p;
		break;
		
	default:
		break;
	}
	
	return 0;	/* shouldn't happen, but avoids warnings */
}

//From http://sdl.beuc.net/sdl.wiki/Pixel_Access
void Sprite::setPixel(int x, int y, Uint32 pixel, SDL_Surface* source){
	if (!source) source = src;
	if ( x<0 || x>source->w || y<0 || y>source->h )
		return;
	
	int bpp = source->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to set */
	Uint8 *p = (Uint8 *)source->pixels + y * source->pitch + x * bpp;
	
	switch(bpp) {
	case 1:
		*p = pixel;
		break;
		
	case 2:
		*(Uint16 *)p = pixel;
		break;
		
	case 3:
		if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
			p[0] = (pixel >> 16) & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = pixel & 0xff;
		} else {
			p[0] = pixel & 0xff;
			p[1] = (pixel >> 8) & 0xff;
			p[2] = (pixel >> 16) & 0xff;
		}
		break;
		
	case 4:
		*(Uint32 *)p = pixel;
		break;
		
	default:
		break;
	}
}
