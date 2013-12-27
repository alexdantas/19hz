#include <list>
#include <sstream>

#include "common.hpp"
#include "configfile.hpp"

#include "StateMovie.hpp"

#include "InputManager.hpp"
#include "SDLBase.hpp"

using namespace common;

using std::string;
using std::list;
using std::stringstream;

GAMESTATE_DEF(StateMovie)

StateMovie::Args::Args(const string& movie, const string& nextstate) :
movie(movie), nextstate(nextstate) {}
//path(string("mov/") + name + "/")

StateMovie::StateMovie(ArgsBase* args) {
	// TODO
}

StateMovie::~StateMovie() {
	// TODO
}

/*

void StateMovie::load(StateArgs* args)
{
	InputManager::instance()->connect(
		InputManager::QUIT,
		this,
		&StateMovie::handleQuit
	);
	InputManager::instance()->connect(
		InputManager::KEYDOWN,
		this,
		&StateMovie::handleKeyDown
	);
	
	nextstate = ( (StateArgsMovie*) args )->nextstate;
	pause_able = ( (StateArgsMovie*) args )->pause_able;
	skip_able = ( (StateArgsMovie*) args )->skip_able;
	skip_fade = ( (StateArgsMovie*) args )->skip_fade;
	
	frames = 0;
	sizes = 0;
	amount = 0;
	index = 0;
	
	bgm = 0;
	
	loadData( ( (StateArgsMovie*) args )->path );
	
	stopwatch.start();
	if( bgm )
		bgm->play( 1 );
}

void StateMovie::unload()
{
	InputManager::instance()->disconnect( this );
	
	if( frames )
		delete[] frames;
	if( sizes )
		delete[] sizes;
	
	if( bgm )
		delete bgm;
}

*/

void StateMovie::update() {
	if( sizes )
	{
		if( stopwatch.time() >= sizes[ index ] )
		{
			++index;
			stopwatch.start();
		}
	}
	
	if( ( index == amount ) || ( skip_stopwatch.time() >= skip_fade ) )
	{
		if( bgm )
			bgm->stop();
		//args = new StateArgs( nextstate );
	}
	else if( skip_stopwatch.time() >= 0 )
		SDLBase::setFadeAlpha( float( skip_stopwatch.time() ) / skip_fade );
}

void StateMovie::render() {
	frames[ index ].render();
	if( skip_stopwatch.time() >= 0 )
		SDLBase::renderFade();
}

void StateMovie::handleKeyDown(const observer::Event& event, bool& stop)
{
	if(	( inputmanager_event.key.keysym.sym == SDLK_SPACE ) &&
		( skip_able ) &&
		( skip_stopwatch.time() < 0 )	)
	{
		skip_stopwatch.start();
		if( bgm )
			bgm->stop( skip_fade );
	}
	else if(	( inputmanager_event.key.keysym.sym == SDLK_KP_ENTER ) ||
				( inputmanager_event.key.keysym.sym == SDLK_RETURN )	)
	{
		if( ( pause_able ) && ( skip_stopwatch.time() < 0 ) )
		{
			//args = new StateArgs(
			//	GameStates::PAUSE_MANAGER, GameStates::STACK_UP
			//);
		}
	}
}

void StateMovie::loadData(const string& path)
{
	Configuration tmp;
	string file_pattern, file_extension;
	list< long int > tmp_sizes;
	int one_size = 0;
	
	tmp.readTxt( RootPath::get( path + "movie.conf" ) );
	
	file_pattern = tmp.getStr( "file_pattern" );
	file_extension = tmp.getStr( "file_extension" );
	
	try {
		bgm = new Audio( path + tmp.getStr( "bgm_file" ) );
	} catch (mexception&) {
	}
	
	// get the amount, and the one_size if it's there
	try {
		amount = tmp.getInt( "amount" );
		one_size = tmp.getInt( "size" );
		
		if( !one_size )
			throw( mexception( "Movie with frames without a size won't be loaded" ) );
		
	} catch(Configuration::VarNotFound& e) {
		tmp_sizes = tmp.getIntList( file_pattern );
		amount = tmp_sizes.size();
		
		if( !amount )
			throw( mexception( "Movie with no frames won't be loaded" ) );
		
	}
	
	frames = new Sprite[ amount ];
	sizes = new int[ amount ];
	
	// loading all frames and setting all sizes
	int i = 0;
	list< long int >::iterator it = tmp_sizes.begin();
	for( int j = 1; i < amount; ++j )
	{
		stringstream ss;
		ss << j;
		string stmp( file_pattern + ss.str() );
		
		try {
			if( one_size )
				sizes[i] = one_size;
			else
			{
				tmp.getInt( stmp );
				sizes[i] = ( *it );
			}
			
			frames[i].load( path + ( stmp + file_extension ) );
			
			++i;
			++it;
			
		} catch(Configuration::VarNotFound& e) {
		}
	}
}
