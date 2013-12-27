/// @ingroup MOD_SDLBASE
/// @file SDLBase.cpp
/// @brief Implementations of all methods of the SDLBase class
/// @author Matheus Pimenta

#include <sstream>

#include "SDL_image.h"
#include "SDL_rotozoom.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"

#include "common.hpp"
#include "configfile.hpp"

#include "SDLBase.hpp"

#define SDL_WIDTH		800
#define SDL_HEIGHT		600
#define SDL_BPP			32
#define SDL_TITLE		"Game"
#define SDL_ICON		""
#define SDL_FPS 		30
#define SDL_STACK_RGB	0x000000
#define SDL_STACK_A		127

using namespace common;
using namespace lalge;

using std::string;

SDL_Surface* SDLBase::screen_ = NULL;
SDL_Surface* SDLBase::bright = NULL;
SDL_Surface* SDLBase::fade = NULL;
SDL_Surface* SDLBase::stack_screen = NULL;
int SDLBase::dt_ = 0;
int SDLBase::fps = 0;
bool SDLBase::has_audio = true;

struct StackScreen
{
	string path;
	int rgb;
	int a;
};

void readSDLConf(
	int& w, int& h, int& bpp,
	string& title, string& icon,
	int& fps,
	StackScreen& stack
)
{
	Configuration tmp;
	
	try {
		tmp.readTxt( RootPath::get( "conf/SDL.conf" ) );
		
		try {
			w = tmp.getInt( "w" );
		} catch (Configuration::VarNotFound& e) {
			w = SDL_WIDTH;
		}
		
		try {
			h = tmp.getInt( "h" );
		} catch (Configuration::VarNotFound& e) {
			h = SDL_HEIGHT;
		}
		
		try {
			bpp = tmp.getInt( "bpp" );
		} catch (Configuration::VarNotFound& e) {
			bpp = SDL_BPP;
		}
		
		try {
			title = tmp.getStr( "title" );
		} catch (Configuration::VarNotFound& e) {
			title = SDL_TITLE;
		}
		
		try {
			icon = RootPath::get( tmp.getStr( "icon" ) );
		} catch (Configuration::VarNotFound& e) {
			icon = RootPath::get( SDL_ICON );
		}
		
		try {
			fps = tmp.getInt( "fps" );
		} catch (Configuration::VarNotFound& e) {
			fps = SDL_FPS;
		}
		
		try {
			stack.path = RootPath::get( tmp.getStr( "stack" ) );
			
			try {
				stack.a = tmp.getInt( "stack_a" );
			} catch(Configuration::VarNotFound& e) {
				stack.a = SDL_STACK_A;
			}
			
		} catch(Configuration::VarNotFound& e) {
			
			try {
				string stmp = tmp.getStr( "stack_rgb" );
				std::stringstream ss;
				ss << std::hex << stmp;
				ss >> stack.rgb;
			} catch(Configuration::VarNotFound& e) {
				stack.rgb = SDL_STACK_RGB;
			}
			
			try {
				stack.a = tmp.getInt( "stack_a" );
			} catch(Configuration::VarNotFound& e) {
				stack.a = SDL_STACK_A;
			}
			
		}
		
	} catch (Configuration::FileNotFound& e) {
		
		w = SDL_WIDTH;
		h = SDL_HEIGHT;
		bpp = SDL_BPP;
		title = SDL_TITLE;
		icon = SDL_ICON;
		fps = SDL_FPS;
		stack.rgb = SDL_STACK_RGB;
		stack.a = SDL_STACK_A;
	}
}

void SDLBase::init()
{
	int w, h, bpp;
	string title, icon;
	int fps;
	StackScreen stack;
	
	readSDLConf( w, h, bpp, title, icon, fps, stack );
	
	if ( screen_ )
		throw ( mexception ( "SDL already on" ) );
	
	if ( SDL_Init ( SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_TIMER ) )
		throw ( mexception ( "SDL_Init error" ) );
	
	SDL_WM_SetCaption ( title.c_str(), title.c_str() );
	
	{
		SDL_Surface* tmp = IMG_Load( icon.c_str() );
		if ( tmp )
		{
			SDL_WM_SetIcon ( tmp, NULL );
			SDL_FreeSurface ( tmp );
		}
	}
	
	screen_ = SDL_SetVideoMode ( w, h, bpp, SDL_SWSURFACE );
	if ( !screen_ )
		throw ( mexception ( "SDL_SetVideoMode error" ) );
	
	bright = SDL_CreateRGBSurface( SDL_SWSURFACE, w, h, bpp, 0, 0, 0, 0 );
	
	fade = SDL_CreateRGBSurface( SDL_SWSURFACE, w, h, bpp, 0, 0, 0, 0 );
	setBrightness( 1 );
	
	stack_screen = SDL_CreateRGBSurface( SDL_SWSURFACE, w, h, bpp, 0, 0, 0, 0 );
	SDL_SetAlpha( stack_screen, SDL_SRCALPHA, stack.a );
	if( !stack.path.size() )
		SDL_FillRect( stack_screen, 0, stack.rgb );
	else
	{
		SDL_Surface* sstmp = loadIMG( stack.path );
		SDL_BlitSurface( sstmp, 0, stack_screen, 0 );
		SDL_FreeSurface( sstmp );
	}
	
	SDLBase::fps = fps;
	
	if( TTF_Init() )
		throw( mexception( "TTF_Init error" ) );
	
	if( Mix_OpenAudio(
		MIX_DEFAULT_FREQUENCY,
		MIX_DEFAULT_FORMAT,
		MIX_DEFAULT_CHANNELS,
		1024
	) )
	{
		has_audio = false;
		mexception("Mix_OpenAudio error").logerr();
	}
}

void SDLBase::close()
{
	Mix_CloseAudio();
	
	TTF_Quit();
	
	if ( !screen_ )
		throw ( mexception ( "SDL already off" ) );
	
	SDL_FreeSurface( bright );
	SDL_FreeSurface( fade );
	
	SDL_Quit ();
	screen_ = NULL;
}

SDL_Surface* SDLBase::screen ()
{
	if ( !screen_ )
		throw ( mexception ( "SDL still off" ) );
	
	return screen_;
}

SDL_Surface* SDLBase::loadIMG (const string& filename)
{
	if ( !screen_ )
		throw ( mexception ( "SDL still off" ) );
	
	SDL_Surface* tmp = NULL;
	SDL_Surface* ret = NULL;
	
	tmp = IMG_Load ( RootPath::get( filename ).c_str () );
	if ( !tmp )
		throw ( mexception ( "IMG_Load error" ) );
	
	if ( tmp->format->Amask )
		ret = SDL_DisplayFormatAlpha ( tmp );
	else
		ret = SDL_DisplayFormat ( tmp );
	if ( !ret )
		throw ( mexception ( "SDL display format conversion error" ) );
	
	SDL_FreeSurface ( tmp );
	
	return ret;
}

void SDLBase::renderSurface(
	SDL_Surface* src,
	SDL_Rect* srcrect,
	SDL_Rect* dstrect
)
{
	SDL_BlitSurface ( src, srcrect, screen_, dstrect );
}

void SDLBase::delayFrame ()
{
	static int t = 0;
	int frame_size = 1000 / fps;
	
	dt_ = SDL_GetTicks () - t;
	
	if ( dt_ < frame_size )
		SDL_Delay ( frame_size - dt_ );
	
	t = SDL_GetTicks ();
	
	dt_ = ( ( dt_ < frame_size ) ? frame_size : dt_ );
}

int SDLBase::dt ()
{
	return 1000 / fps;
}

float SDLBase::FPS ()
{
	return ( (float) 1000 / dt_ );
}

void SDLBase::setFPS(int fps)
{
	if( fps )
		SDLBase::fps = fps;
}

void SDLBase::updateScreen ()
{
	SDL_BlitSurface( bright, 0, screen_, 0 );
	SDL_Flip ( screen_ );
}

SDL_Surface* SDLBase::rotozoom (
	SDL_Surface* src, float angle, float zoomx, float zoomy
)
{
	if ( !( ( src ) && ( ( zoomx > 0 ) && ( zoomy > 0 ) ) ) )
		throw ( mexception ( "Invalid SDLBase::rotozoom call" ) );
	
	return rotozoomSurfaceXY ( src, angle, zoomx, zoomy, 1 );
}

SDL_Surface* SDLBase::clip (SDL_Surface* src, SDL_Rect* rect)
{
	if ( !( ( src ) && ( rect ) ) )
		throw ( mexception ( "Invalid SDLBase::clip call" ) );
	
	SDL_Surface* tmp = SDL_CreateRGBSurface(
		SDL_SWSURFACE, rect->w, rect->h, screen_->format->BitsPerPixel,
		0, 0, 0, 0
	);
	
	SDL_BlitSurface ( src, rect, tmp, NULL );
	
	return tmp;
}

SDL_Color SDLBase::getColor(int r, int g, int b)
{
	SDL_Color color;
	
	color.r = r;
	color.g = g;
	color.b = b;
	
	return color;
}

void SDLBase::setBrightness(float brightness)
{
	SDL_SetAlpha( bright, SDL_SRCALPHA, ( 1 - brightness ) * 255 );
}

void SDLBase::renderFade()
{
	SDL_BlitSurface( fade, 0, screen_, 0 );
}

void SDLBase::setFadeAlpha(float opacity)
{
	SDL_SetAlpha( fade, SDL_SRCALPHA, opacity * 255 );
}

void SDLBase::renderStackScreen()
{
	SDL_BlitSurface( stack_screen, 0, screen_, 0 );
}

void SDLBase::drawLine(const R2Vector& beg, const R2Vector& end, int rgb, int spacing, int size) {
	try {
		R2Vector lineseg( end - beg );
		R2Vector delta( lineseg.unitvec() );
		R2Vector tmp( beg );
		
		SDL_Rect pixel;
		pixel.w = size;
		pixel.h = size;
		
		int i = 0;
		while( i < lineseg.length() )
		{
			unsigned int j = 0;
			while( ( i < lineseg.length() ) && ( ( j < spacing ) || ( !spacing ) ) )
			{
				pixel.x = tmp.x( 0 );
				pixel.y = tmp.x( 1 );
				
				SDL_FillRect( screen_, &pixel, rgb );
				
				tmp += delta;
				i++;
				
				j++;
			}
			
			tmp += ( delta * spacing );
			i += spacing;
		}
		
	} catch(DirectionNotDefined& e) {
	}
}

void SDLBase::drawMask(const SDL_Rect& rect, int rgb) {
	int x, y, w, h;
	
	//cima
	
	x = 0;
	y = 0;
	w = rect.x;
	h = rect.y;
	drawRectangle(x, y, w, h, rgb);
	
	x = rect.x;
	y = 0;
	w = rect.w;
	h = rect.y;
	drawRectangle(x, y, w, h, rgb);
	
	x = rect.x + rect.w;
	y = 0;
	w = screen_->w - (rect.x + rect.w);
	h = rect.y;
	drawRectangle(x, y, w, h, rgb);
	
	// meio
	
	x = 0;
	y = rect.y;
	w = rect.x;
	h = rect.h;
	drawRectangle(x, y, w, h, rgb);
	
	x = rect.x + rect.w;
	y = rect.y;
	w = screen_->w - (rect.x + rect.w);
	h = rect.h;
	drawRectangle(x, y, w, h, rgb);
	
	// baixo
	
	x = 0;
	y = rect.y + rect.h;
	w = rect.x;
	h = screen_->h - (rect.y + rect.h);
	drawRectangle(x, y, w, h, rgb);
	
	x = rect.x;
	y = rect.y + rect.h;
	w = rect.w;
	h = screen_->h - (rect.y + rect.h);
	drawRectangle(x, y, w, h, rgb);
	
	x = rect.x + rect.w;
	y = rect.y + rect.h;
	w = screen_->w - (rect.x + rect.w);
	h = screen_->h - (rect.y + rect.h);
	drawRectangle(x, y, w, h, rgb);
}

void SDLBase::drawRectangle(int x, int y, int w, int h, int rgb) {
	if (x < 0) {
		w += x;
		x = 0;
	}
	if (y < 0) {
		h += y;
		y = 0;
	}
	
	if (w > 0 && h > 0) {
		SDL_Rect tmp;
		tmp.x = x;
		tmp.y = y;
		tmp.w = w;
		tmp.h = h;
		SDL_FillRect(screen_, &tmp, rgb);
	}
}

bool SDLBase::hasAudio() { return has_audio; }
