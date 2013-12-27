#include "common.hpp"

#include "Text.hpp"

#include "SDLBase.hpp"
#include "SurfaceManager.hpp"

using namespace common;

using std::string;

Text::Text(
	const string& fontfile,
	const string& text,
	int ptsize,
	int style,
	const SDL_Color& color,
	render_types render_type,
	const SDL_Color& bg
) : font(NULL), surface(NULL), text(text), style(style),
color(color), render_type(render_type), bg(bg)
{
	if( !fontfile.size() )
		this->fontfile = RootPath::get( "ttf/default.ttf" );
	else
		this->fontfile = RootPath::get( fontfile );
	
	font = TTF_OpenFont( this->fontfile.c_str(), ptsize );
	if( !font )
		throw( mexception( "TTF_OpenFont error" ) );
	TTF_SetFontStyle( font, style );
	
	render_text_();
}

Text::~Text()
{
	TTF_CloseFont( font );
	SurfaceManager::instance()->free( surface );
}

int Text::w() const
{
	return surface->w;
}

int Text::h() const
{
	return surface->h;
}

void Text::render (int x, int y, bool center, bool surface_manager)
{
	if ((center) && (surface)) {
		x -= surface->w/2;
		y -= surface->h/2;
	}
	
	if( surface_manager )
		SurfaceManager::instance()->clone( x, y, surface );
	else
	{
		SDL_Rect dstrect;
		
		dstrect.x = x;
		dstrect.y = y;
		
		SDLBase::renderSurface ( surface, NULL, &dstrect );
	}
}

void Text::render_text()
{
	SurfaceManager::instance()->free( surface );
	render_text_();
}

void Text::render_text_()
{
	switch( render_type )
	{
	case solid:
		surface = SurfaceManager::instance()->renderSolid(
			font, text, color
		);
		break;
		
	case blended:
		surface = SurfaceManager::instance()->renderBlended(
			font, text, color
		);
		break;
		
	case shaded:
		surface = SurfaceManager::instance()->renderShaded(
			font, text, color, bg
		);
		break;
		
	default:
		break;
	}
}

void Text::setStyle(int style)
{
	this->style = style;
	TTF_SetFontStyle( font, style );
	
	render_text();
}

void Text::setSize(int ptsize)
{
	TTF_CloseFont( font );
	font = TTF_OpenFont( fontfile.c_str(), ptsize );
	TTF_SetFontStyle( font, style );
	
	render_text();
}

void Text::setText(const string& text)
{
	this->text = text;
	render_text();
}

void Text::setColor(const SDL_Color& color)
{
	this->color = color;
	render_text();
}

void Text::setRenderType(render_types render_type)
{
	this->render_type = render_type;
	render_text();
}

void Text::setBGColor(const SDL_Color& bg)
{
	this->bg = bg;
	render_text();
}

void Text::swapColors()
{
	SDL_Color tmp = color;
	color = bg;
	bg = tmp;
	
	render_text();
}

void Text::calcSize(const string& text, int& w, int& h)
{
	TTF_SizeText( font, text.c_str(), &w, &h );
}
