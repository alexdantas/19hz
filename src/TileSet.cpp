#include "TileSet.hpp"

using std::string;
using std::vector;

TileSet::TileSet (const string& filename, int tile_w, int tile_h)
{
	this->tile_w = tile_w;
	this->tile_h = tile_h;
	
	tileset = new Sprite( filename );
	
	tiles = NULL;
	
	dstrect = new SDL_Rect;
	
	singlefile = true;
	
	rows = tileset->srcH() / tile_h;
	cols = tileset->srcW() / tile_w;
}

TileSet::TileSet (int rows, int cols, const string& filename)
{
	this->rows = rows;
	this->cols = cols;
	
	tileset = new Sprite( filename );
	
	tiles = NULL;
	
	dstrect = new SDL_Rect;
	
	singlefile = true;
	
	tile_w = tileset->srcW() / cols;
	tile_h = tileset->srcH() / rows;
}

TileSet::TileSet (int tile_w, int tile_h)
{
	this->tile_w = tile_w;
	this->tile_h = tile_h;
	rows = 0;
	cols = 0;
	
	tileset = NULL;
	
	tiles = new vector< Sprite* >;
	
	dstrect = NULL;
	
	singlefile = false;
}

TileSet::~TileSet ()
{
	if ( tiles )
	{
		for (
			vector< Sprite* >::iterator it = tiles->begin ();
			it != tiles->end ();
			++it
		)
		{
			delete ( *it );
		}
		
		delete tiles;
	}
	else
	{
		delete tileset;
		delete dstrect;
	}
}

void TileSet::addTile (const string& filename)
{
	if( tiles )
		tiles->push_back( new Sprite( filename ) );
}

void TileSet::render (int index, int x, int y)
{
	if ( tiles )
		(*tiles)[ index ]->render( x, y );
	else
	{
		tileset->clip(
			( index % cols ) * tile_w,
			( index / cols ) * tile_h,
			tile_w,
			tile_h
		);
		tileset->render( x, y );
	}
}

int TileSet::tileW() const
{
	return tile_w;
}

int TileSet::tileH() const
{
	return tile_h;
}
