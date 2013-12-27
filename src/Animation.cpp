#include <cmath>

#include "common.hpp"

#include "Animation.hpp"

#include "SDLBase.hpp"

#define TINT_DELTA 20

using std::string;

Animation::Animation (
	const string& filename,
	int frame,
	int fps,
	int rows,
	int cols,
	bool matrix,
	int line
) : rows ( rows ), cols ( cols ), matrix ( matrix ), line ( line )
{
	load ( filename );
	
	if ( ( matrix ) && ( frame >= cols ) )
		frame = 0;
	else
		this->frame = float ( frame );
	this->fps = ((fps >= 1) ? fps : 1);
	
	update_ ();
}

void Animation::update ()
{
	frame += fps*float(SDLBase::dt())/1000;
	if (frame >= cols)
		frame -= cols;
	update_();
}

void Animation::update_ ()
{
	clip (
		(int(frame)%cols)*(src->w/cols),
		((!matrix)*(int(frame)/cols)+(matrix*line))*(src->h/rows),
		src->w/cols,
		src->h/rows
	);
}

int Animation::frameAmount () const { return ((matrix*cols) + ((!matrix)*(rows*cols))); }
int Animation::getFrame() const { return frame; }
int Animation::getFPS() const { return fps; }
int Animation::getTimeSize() const { return (frameAmount()*1000)/fps; }

void Animation::setFrame(int frame)
{
	if( ( !matrix ) || ( frame < cols ) )
	{
		this->frame = float ( frame );
		update_();
	}
}

void Animation::setFPS(int fps) {
	this->fps = ((fps >= 1) ? fps : 1);
}
