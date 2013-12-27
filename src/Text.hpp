#ifndef TEXT_HPP
#define TEXT_HPP

#include "SDL_ttf.h"

class Text {
public:
	enum render_types {
		solid,
		blended,
		shaded
	};
private:
	TTF_Font* font;
	SDL_Surface* surface;
	std::string fontfile;
	std::string text;
	int style;
	SDL_Color color;
	render_types render_type;
	SDL_Color bg;
public:
	Text(
		const std::string& fontfile,
		const std::string& text,
		int ptsize = 10,
		int style = 0,
		const SDL_Color& color = SDL_Color(),
		render_types render_type = solid,
		const SDL_Color& bg = SDL_Color()
	);
	~Text();
	
	int w() const;
	int h() const;
	
	void render(int x = 0, int y = 0, bool center = true, bool surface_manager = false);
private:
	void render_text();
	void render_text_();
public:
	void setStyle(int style);
	void setSize(int ptsize);
	
	void setText(const std::string& text);
	void setColor(const SDL_Color& color);
	
	void setRenderType(render_types render_type);
	void setBGColor(const SDL_Color& bg);
	void swapColors();
	
	void calcSize(const std::string& text, int& w, int& h);
};

#endif
