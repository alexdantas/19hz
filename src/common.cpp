#include <iostream>
#include <fstream>
#include <ctime>

#include "common.hpp"

using std::string;
using std::fstream;

using namespace common;

// =============================================================================
// string functions
// =============================================================================

string toLower(const string& src) {
	string buf;
	
	for (int i = 0; i < int(src.size()); ++i) {
		if ((src[i] >= 'A') && (src[i] <= 'Z'))
			buf += char(src[i] - 'A' + 'a');
		else
			buf += src[i];
	}
	
	return buf;
}

string toUpper(const string& src) {
	string buf;
	
	for (int i = 0; i < int(src.size()); ++i) {
		if ((src[i] >= 'a') && (src[i] <= 'z'))
			buf += char(src[i] - 'a' + 'A');
		else
			buf += src[i];
	}
	
	return buf;
}

// =============================================================================
// MainArgs Class
// =============================================================================

// Static vars
int MainArgs::argc = 0;
char** MainArgs::argv;

void MainArgs::init(int argc_, char** argv_) {
	if (argc)
		throw mexception("Trying to set MainArgs more than once");
	argc = argc_;
	argv = argv_;
}

int MainArgs::size() {
	if (!argc)
		throw mexception("Trying to access MainArgs without initialization");
	return argc - 1;
}

int MainArgs::find(const string& what) {
	if (!argc)
		throw mexception("Trying to access MainArgs without initialization");
	
	for (int i = 1; i < argc; ++i) {
		if (what == argv[i])
			return i;
	}
	return 0;
}

// =============================================================================
// mexception Class
// =============================================================================

mexception::mexception(const string& what) throw() : what_(what) {}
mexception::~mexception() throw() {}

const char* mexception::what() const throw() {
	return what_.c_str();
}

void mexception::logerr() const throw() {
	std::cout << "Logging error: " << what_ << std::endl;
	
	fstream f(RootPath::get("ErrorLog.txt").c_str(), fstream::out | fstream::app);
	time_t rawtime = time(NULL);
	
	f << "- Error: ";
	f << what_;
	f << ". Occured on ";
	f << ctime(&rawtime);
	
	f.close();
}

// =============================================================================
// mexception Class
// =============================================================================

Quit::Quit() : mexception("Quit") {}

// =============================================================================
// RootPath Class
// =============================================================================

// Static vars
bool RootPath::isset = false;
string RootPath::path = "";

void RootPath::init(const string& path_) {
	if (isset)
		throw mexception("Trying to set root path more than once");
	isset = true;
	path = path_;
}

const string& RootPath::get() { return path; }

string RootPath::get(const string& path_) {
	if (path.find(path_) == path.npos)
		return (path + path_);
	return path_;
}
