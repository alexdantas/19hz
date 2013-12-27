#ifndef INPUTSTRING_HPP
#define INPUTSTRING_HPP

#include <vector>
#include <string>

#include "observer.hpp"

#include "Stopwatch.hpp"

class InputString {
SUBJECT
public:
	enum {
		UPDATE = 0,
		ENTER,
		
		// don't change this
		LASTEVENT
	};
private:
	static std::vector< bool > available_input;
	
	std::string buf;
	int max_size;
	Stopwatch stopwatch;
public:
	bool enabled;
	
	InputString(const std::string& str = "");
	~InputString();
	
	int getMaxSize() const;
	void setMaxSize(int max_size);
	
	const std::string& get() const;
	void set(const std::string& input, bool broadcast = true);
	
	void clear();
	
	void update();
private:
	void initAvailableInput();
	void handleKeyDown(const observer::Event& event, bool& stop);
	void handleKeyUp(const observer::Event& event, bool& stop);
};

#endif
