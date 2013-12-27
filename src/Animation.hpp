#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include <string>

#include "Sprite.hpp"

class Animation : public Sprite
{
private:
	float frame;
	float fps;
	int rows;
	int cols;
	bool matrix;
public:
	int line;
	
	Animation (
		const std::string& filename,
		int frame,
		int fps,
		int rows,
		int cols,
		bool matrix = false,
		int line = 0
	);
	
	void update();
private:
	void update_();
public:
	int frameAmount() const;
	int getFrame() const;
	int getFPS() const;
	int getTimeSize() const;
	
	void setFrame(int frame);
	void setFPS(int fps);
};

#endif
