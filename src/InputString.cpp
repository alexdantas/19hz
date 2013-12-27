#include "InputString.hpp"

#include "configfile.hpp"
#include "common.hpp"

#include "InputManager.hpp"

#define AVAILABLE_INPUT	\
"._-+1234567890 abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"

using std::vector;
using std::string;

vector< bool > InputString::available_input;

InputString::InputString(const string& str) : buf(str), max_size(0), enabled(false) {
	if (!available_input.size())
		initAvailableInput();
	subject.init(LASTEVENT);
	InputManager::instance()->connect(InputManager::KEYDOWN, this, &InputString::handleKeyDown);
	InputManager::instance()->connect(InputManager::KEYUP, this, &InputString::handleKeyUp);
}

InputString::~InputString() {
	InputManager::instance()->disconnect(this);
}

int InputString::getMaxSize() const { return max_size; }

void InputString::setMaxSize(int max_size) {
	if (max_size < 0)
		return;
	this->max_size = max_size;
	if ((max_size) && (buf.size() > max_size))
		buf.erase(max_size);
}

const string& InputString::get() const { return buf; }

void InputString::set(const std::string& input, bool broadcast) {
	buf = input;
	if ((max_size) && (buf.size() > max_size))
		buf.erase(max_size);
	if (broadcast)
		subject.broadcast(observer::Event(UPDATE));
}

void InputString::clear() {
	buf = "";
}

void InputString::update() {
	if (!enabled)
		return;
	
	if ((InputManager::instance()->keyPressed(SDLK_BACKSPACE)) &&
		(!stopwatch.ispaused()) && (stopwatch.time() >= 500) && (buf.size())) {
		buf.erase(buf.size() - 1);
		subject.broadcast(observer::Event(UPDATE));
	}
}

void InputString::initAvailableInput()
{
	Configuration tmp;
	string stmp;
	
	available_input.resize( SDLK_LAST, false );
	
	try {
		tmp.readTxt(common::RootPath::get("conf/InputString.conf"));
		stmp = tmp.getStr( "available_input" );
		
		if( !stmp.size() )
			stmp = AVAILABLE_INPUT;
		
	} catch(Configuration::FileNotFound& e) {
		stmp = AVAILABLE_INPUT;
	} catch(Configuration::VarNotFound& e) {
		stmp = AVAILABLE_INPUT;
	}
	
	for( int i = 0; i < int( stmp.size() ); ++i )
		available_input[ int( stmp[i] ) ] = true;
}

void InputString::handleKeyDown(const observer::Event& event, bool& stop) {
	if (!enabled)
		return;
	
	int tmp = inputmanager_event.key.keysym.sym;
	bool broadcast = true;
	
	if( ( tmp == SDLK_BACKSPACE ) && ( buf.size() ) ) {
		buf.erase( buf.size() - 1 );
		stopwatch.start();
	}
	else if( ( tmp == SDLK_KP_ENTER ) || ( tmp == SDLK_RETURN ) ) {
		subject.broadcast( observer::Event( ENTER ) );
		broadcast = false;
	}
	else if ((max_size) && (buf.size() == max_size))
		broadcast = false;
	else if ((InputManager::instance()->keyPressed(SDLK_LCTRL)) ||
			(InputManager::instance()->keyPressed(SDLK_LCTRL)))
		broadcast = false;
	else
	{
		if ((tmp == SDLK_COMMA) || (tmp == SDLK_KP_PERIOD))
			tmp = SDLK_PERIOD;
		else if (tmp == SDLK_KP_PLUS)
			tmp = SDLK_PLUS;
		else if (tmp == SDLK_KP_MINUS)
			tmp = SDLK_MINUS;
		else if(	( InputManager::instance()->keyPressed( SDLK_RSHIFT ) ) ||
					( InputManager::instance()->keyPressed( SDLK_LSHIFT ) )	)
		{
			if ((tmp >= SDLK_a) && (tmp <= SDLK_z))
				tmp -= 32;
			else if (tmp == SDLK_MINUS)
				tmp = SDLK_UNDERSCORE;
			else if (tmp == SDLK_EQUALS)
				tmp = SDLK_PLUS;
		}
		else if(	( tmp >= SDLK_KP0 ) &&
					( tmp <= SDLK_KP9 )	)
		{
			tmp += ( '0' - SDLK_KP0 );
		}
		
		if( available_input[tmp] )
			buf += char( tmp );
		else
			broadcast = false;
	}
	
	if( broadcast )
		subject.broadcast( observer::Event( UPDATE ) );
}

void InputString::handleKeyUp(const observer::Event& event, bool& stop) {
	if (!enabled)
		return;
	
	if (inputmanager_event.key.keysym.sym == SDLK_BACKSPACE)
		stopwatch.pause();
}
