
/**

Some structures that can be easily needed everywhere.

@file common.hpp
@author Matheus Costa de Sousa Carvalho Pimenta

*/

#ifndef COMMON_HPP
#define COMMON_HPP

#include <iostream>
#include <sstream>
#include <cmath>
#include <cstring>

#define COMMON_CALLBACK(ref,mfunc)	((ref).*(mfunc))

#define COMMON_SHOW(X)				std::cout << #X << ": (" << (X) << ")" << std::endl

#define COMMON_BITSOF(X)			(sizeof(X)*8)

/// Namespace for some common classes and functions.
namespace common {

/// Amount of bits in a char variable.
const int BITSOFCHAR = COMMON_BITSOF(char);

/// Evaluate function for strings.
template <typename T>
bool eval(const std::string& raw, T& buf);

/// Evaluate function to strings.
template <typename T>
std::string eval(const T& raw);

/// Class to manage the main function arguments.
class MainArgs {
private:
	static int argc;
	static char** argv;
public:
	static void init(int argc, char** argv);	///< Set the arguments only once.
	static int size();	///< Returns the amount of arguments passed.
	static int find(const std::string& what);	///< Find an argument.
	
	/// Access to an argument by the name of its predecessor.
	template <typename T> static T get(const std::string& what);
};

/// Inherited exception class from std::exception, to log errors into a ErrorLog.txt file.
class mexception : public std::exception {
private:
	std::string what_;
public:
	mexception(const std::string& what) throw();	///< Constructor to assign the description.
	virtual ~mexception() throw();	///< Empty destructor.
	const char* what() const throw();	///< Returns the description.
	void logerr() const throw();	///< Logs the exception into the ErrorLog.txt file.
};

/// Class to throw quit exceptions.
class Quit : public mexception {
public:
	// Empty constructor.
	Quit();
};

/// Auto pointer Class.
template <class T>
class aptr {
private:
	T* ptr;
public:
	aptr(T* ptr) : ptr(ptr) {}	///< Assignment constructor.
	~aptr() { if (ptr) delete ptr; }	///< Deleting destructor.
	T* operator()() { return ptr; }	///< Returns the value of the pointer.
	T* operator->() { return ptr; }	///< Access to member operator.
	T& operator*() { return *ptr; }	///< Dereference operator.
};

/// Auto pointer Class for arrays.
template <class T>
class arr_aptr {
private:
	T* ptr;
public:
	arr_aptr(T* ptr) : ptr(ptr) {}	///< Assignment constructor.
	~arr_aptr() { if (ptr) delete[] ptr; }	///< Deleting destructor.
	T* operator()() { return ptr; }	///< Returns the value of the pointer.
	T& operator[](int i) { return ptr[i]; }	///< Access to an element of the array.
};

/// Auto pointer Class for arrays.
class RootPath {
private:
	static bool isset;
	static std::string path;
public:
	static void init(const std::string& path_);
	static const std::string& get();
	static std::string get(const std::string& path_);
};

/// Interface to bitarray for unknown size.
class bitarray_base {
public:
	virtual int size() const = 0;
};

/// Implementation for an array of bits to save memory.
template <int n>
class bitarray : public bitarray_base {
public:
	/// Tells the amout of characters necessary to store a bitarray of size n.
	static const int char_amount;
	char* buf;	///< Storage for the bitarray.
	
	bitarray();	///< Allocate the memory and set all positions to false.
	~bitarray();	///< Delete the array.
	
	int size() const;	///< Returns the n template value.
	/// Makes a new bitarray. If expanding, zeroes will be the MSBs.
	/// If shrinking, the MSBs will be lost.
	template <int size_> bitarray<size_> to_size() const;
	std::string to_str() const;	///< Returns the bit array in string format.
	
	bool operator()(int pos) const;	///< Access method.
	void operator()(int pos, bool val);	///< Modifier method.
	
	bitarray<n>& operator<<(int amount);	///< Common left shift operation.
	bitarray<n>& operator>>(int amount);	///< Common right shift operation.
	
	bitarray<n>& operator=(const bitarray<n>& other);	///< Clone operation.
};

// bitarray HEADER end

}	// namespace common end

// =============================================================================
// eval functions
// =============================================================================

template <typename T>
bool common::eval(const std::string& raw, T& buf) {
	if (!raw.size()) {
		buf = T();
		return false;
	}
	
	std::stringstream ss;
	ss << raw;
	ss >> buf;
	return ((!ss.fail()) && (ss.eof()));
}

template <typename T>
std::string common::eval(const T& raw) {
	std::stringstream ss;
	ss << raw;
	return ss.str();
}

// =============================================================================
// MainArgs class
// =============================================================================

template <typename T>
T common::MainArgs::get(const std::string& what) {
	int i = find(what);
	
	// if there is the argument and its successor
	if ((i) && (i + 1 < argc))
		return eval<T>(argv[i + 1]);
	
	return T();
}

// =============================================================================
// bitarray class
// =============================================================================

template <int n>
const int common::bitarray<n>::char_amount = ceil(float(n)/BITSOFCHAR);

template <int n>
common::bitarray<n>::bitarray() {
	if (n <= 0)
		throw mexception("Trying to allocate bit array with invalid size");
	
	buf = new char[char_amount];
	memset(buf, 0, char_amount);
}

template <int n>
common::bitarray<n>::~bitarray() {
	delete[] buf;
}

template <int n>
int common::bitarray<n>::size() const {
	return n;
}

template <int n>
template <int size_>
common::bitarray<size_> common::bitarray<n>::to_size() const {
	bitarray<size_> ret;
	for (int i = 0; (i < bitarray<n>::char_amount) && (i < bitarray<size_>::char_amount); ++i)
		ret.buf[i] = buf[i];
	return ret;
}

template <int n>
std::string common::bitarray<n>::to_str() const {
	std::string ret;
	for (int i = n - 1; i >= 0; --i)
		ret += ((*this)(i) ? '1' : '0');
	return ret;
}

template <int n>
bool common::bitarray<n>::operator()(int pos) const {
	if (pos >= n)
		throw mexception("Trying to get value of invalid position of bit array");
	
	return ((buf[pos/BITSOFCHAR] & char(1 << (BITSOFCHAR-1 - pos%BITSOFCHAR))) != 0);
}

template <int n>
void common::bitarray<n>::operator()(int pos, bool val) {
	if (pos >= n)
		throw mexception("Trying to set value of invalid position of bit array");
	
	if (val)
		buf[pos/BITSOFCHAR] |= char(1 << (BITSOFCHAR-1 - pos%BITSOFCHAR));
	else
		buf[pos/BITSOFCHAR] &= ~char(1 << (BITSOFCHAR-1 - pos%BITSOFCHAR));
}

template <int n>
common::bitarray<n>& common::bitarray<n>::operator<<(int amount) {
	for (int j = 0; j < amount; ++j) {
		for (int i = n - 1; i > 0; --i)
			(*this)(i, (*this)(i - 1));
		(*this)(0, false);
	}
	return *this;
}

template <int n>
common::bitarray<n>& common::bitarray<n>::operator>>(int amount) {
	for (int j = 0; j < amount; ++j) {
		for (int i = 0; i < n - 1; ++i)
			(*this)(i, (*this)(i + 1));
		(*this)(n - 1, false);
	}
	return *this;
}

template <int n>
common::bitarray<n>& common::bitarray<n>::operator=(const bitarray<n>& other) {
	for (int i = 0; i < bitarray<n>::char_amount; ++i)
		buf[i] = other.buf[i];
	return *this;
}

#endif
