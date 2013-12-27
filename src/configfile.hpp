/// @defgroup	MOD_CONFIGFILE "Configuration File Parser"
/// @{
/// 	@brief Module to define and implement a parser for configuration files
/// @}
/// @ingroup MOD_CONFIGFILE
/// @file configfile.hpp
/// @brief All Configuration File Parser classes and methods
/// @author Matheus Pimenta

#ifndef CONFIGFILE_HPP
#define CONFIGFILE_HPP

#include <string>
#include <vector>
#include <list>
#include <fstream>

/// Strong class with a system to manage and access configuration variables
/// for the client application, with the capacity of loading files and parsing
/// its words to generate those variables.
/// @brief Strong class to manage configuration variables
class Configuration
{
public:
	// =====================================================================
	// Configuration Exceptions
	// =====================================================================
	
	/// @brief Exception for parsing files
	class FileNotFound {};
	
	/// @brief Exception for finding variables
	class VarNotFound {};
	
	/// @brief Exception for inserting variables
	class VarAlreadyExisting {};
private:
	// =====================================================================
	// Configuration Class Member (Variable)
	// =====================================================================
	
	/// This class holds data of something like "dynamic type variables" as
	/// the configuration variables.
	/// @tparam T Variable's type.
	/// @brief Class for configuration variables
	template <class T>
	class Variable
	{
	private:
		/// @brief Variable's name
		std::string name_;
		
		/// @brief Variable's value
		T value_;
	public:
		/// Constructor assigning both the name and the value.
		/// @param name Variable's name.
		/// @param value Variable's value.
		/// @brief Constructor
		Variable (const std::string& name, const T& value);
		
		/// @brief Empty destructor
		~Variable ();
		
		/// Returns the variable's name.
		/// @return Variable's name.
		/// @brief Access method to variable's name
		const std::string& name () const;
		
		/// Returns the variable's value.
		/// @return Variable's value.
		/// @brief Access method to variable's value
		const T& value () const;
		
		/// Sets the variable's value.
		/// @param value Variable's new value.
		/// @brief Modifier method to variable's value
		void set (const T& value);
	};
	
	// =====================================================================
	// Configuration Class Members (Specialized variables)
	// =====================================================================
	
	/// Class for configuration variables of configuration type.
	/// @see Variable
	/// @brief Class for configuration type
	typedef Variable< Configuration > ConfigVariable;
	
	/// Class for configuration variables of "primitive" types.
	/// @see Variable
	/// @brief Class for "primitive" types
	typedef Variable< std::string > DataVariable;
	
	// =====================================================================
	// Configuration Attributes
	// =====================================================================
	
	/// Set of all configuration variables.
	/// @see ConfigVariable
	/// @brief Vector of configuration variables
	std::vector< ConfigVariable > cvars;
	
	/// Set of all configuration variables.
	/// @see ConfigVariable
	/// @brief Vector of configuration variables
	std::vector< DataVariable > vars;
public:
	// =====================================================================
	// Configuration Methods
	// =====================================================================
	
	/// @brief Empty constructor
	Configuration ();
	
	/// @brief Empty destructor
	~Configuration ();
public:
	/// Method to load a configuration text file into the configuration
	/// instance, calling the parser's method.
	/// If there is some variable in the file with the same name of any
	/// in the configuration instance, they will replace all of the already
	/// existing variables.
	/// @param filename String with the name of the configuration file.
	/// @see parseFile
	/// @brief Loads a text configuration file
	void readTxt (const std::string& filename);
private:
	/// Method to load a configuration binary file into the configuration
	/// instance.
	/// If there is some variable in the file with the same name of any
	/// in the configuration instance, they will replace all of the already
	/// existing variables.
	/// This method is also recursive to load each of configuration sets of
	/// the file.
	/// @param f Stream of the configuration file.
	/// @param dest Current configuration instance that's being read.
	/// @throw FileNotFound If it was not possible to open the file.
	/// @brief Recursive method to load a binary configuration file
	void readBin_ (std::fstream& f, Configuration& dest);
public:
	/// Method to load a configuration binary file into the configuration
	/// instance.
	/// If there is some variable in the file with the same name of any
	/// in the configuration instance, they will replace all of the already
	/// existing variables.
	/// @param filename String with the name of the configuration file.
	/// @throw FileNotFound If it was not possible to open the file.
	/// @brief Loads a binary configuration file
	void readBin (const std::string& filename);
private:
	/// Writes in a text file, named with "filename", all the variables in
	/// the configuration instance, using recursion to write all
	/// configuration instances within the main ("this" pointer)
	/// configuration instance.
	/// @param f Stream of the configuration file.
	/// @param src Current configuration instance that's being written.
	/// @param tab Number of indentation tabs of the current configuration
	/// instance.
	/// @brief Recursive method to write all variables in a text file
	void writeTxt_ (std::fstream& f, unsigned short tab) const;
public:
	/// Writes in a text file, named with "filename", all the variables in
	/// the configuration instance.
	/// @param filename String with the name of the configuration file.
	/// @brief Writes all variables in a text file
	void writeTxt (const std::string& filename);
private:
	/// Writes in a binary file, named with "filename", all the variables in
	/// the configuration instance, using recursion to write all
	/// configuration instances within the main ("this" pointer)
	/// configuration instance.
	/// @param f Stream of the configuration file.
	/// @param src Current configuration instance that's being written.
	/// @brief Recursive method to write all variables in a binary file
	void writeBin_ (std::fstream& f) const;
public:
	/// Writes in a binary file, named with "filename", all the variables in
	/// the configuration instance.
	/// @param filename String with the name of the configuration file.
	/// @brief Writes all variables in a binary file
	void writeBin (const std::string& filename);
	
	/// Clear all variables in the configuration instance.
	/// @brief Clear all variables
	void clear ();
	
	/// Returns the amount of variables in the configuration instance.
	/// @return The amount of variables in the configuration instance.
	/// @brief Returns the number of all existing variables
	size_t size () const;
private:
	/// Makes a binary search for any variable, named with the "name"
	/// parameter, assigning the position to the storage pointed by "mid"
	/// parameter, returning code of success.
	/// @param mid Storage which will hold the returned position.
	/// @param vec Reference to the std::vector in which the var. will be sought.
	/// @param name String with the variable's name.
	/// @throw VarNotFound If the variable is not present in the
	/// configuration instance.
	/// @brief Searches a variable in the configuration instance
	template <class T>
	void find (long int& mid, const std::vector< T >& vec, const std::string& name) const;
public:
	/// Method to insert a new variable of configuration type in the
	/// configuration instance, checking the validity of the name with the
	/// parser's method.
	/// @param name String with the variable's name.
	/// @param val ConfigVariable's value.
	/// @throw VarAlreadyExisting
	/// If new variable's name is found in another variable.
	/// @throw ConfInsertingItself
	/// If the configuration instance is trying to insert itself.
	/// @see parseName
	/// @brief Inserts a new config. variable in the configuration instance
	void insertConfig (const std::string& name, const Configuration& val);
	
	/// Method to insert a new variable of std::string type in the
	/// configuration instance, checking the validity of the name with the
	/// parser's method, and converting special characters (like '\r', '\n',
	/// etc.) to two characters: '\\' with 'r', for instance (also with a
	/// parser's method).
	/// @param name String with the variable's name.
	/// @param val DataVariable's value.
	/// @throw VarAlreadyExisting
	/// If new variable's name is found in another variable.
	/// @see parseName
	/// @see parseInStr
	/// @brief Inserts a new std::string variable in the configuration instance
	void insertStr (const std::string& name, const std::string& val);
	
	/// Method to insert a new variable of real number type in the
	/// configuration instance, checking the validity of the name with the
	/// parser's method.
	/// @param name String with the variable's name.
	/// @param val DataVariable's value.
	/// @throw VarAlreadyExisting
	/// If new variable's name is found in another variable.
	/// @see parseName
	/// @see parseInReal
	/// @brief Inserts a new real num variable in the configuration instance
	void insertReal (const std::string& name, const long double& val);
	
	/// Method to insert a new variable of integer number type in the
	/// configuration instance, checking the validity of the name with the
	/// parser's method.
	/// @param name String with the variable's name.
	/// @param val DataVariable's value.
	/// @throw VarAlreadyExisting
	/// If new variable's name is found in another variable.
	/// @see parseName
	/// @see parseInInt
	/// @brief Inserts a new int num. variable in the configuration instance
	void insertInt (const std::string& name, const long int& val);
	
	/// Method to insert a new variable of char type in the
	/// configuration instance, checking the validity of the name with the
	/// parser's method, and converting special characters (like '\r', '\n',
	/// etc.) to two characters: '\\' with 'r', for instance (also with a
	/// parser's method).
	/// @param name String with the variable's name.
	/// @param val DataVariable's value.
	/// @throw VarAlreadyExisting
	/// If new variable's name is found in another variable.
	/// @see parseName
	/// @see parseInChar
	/// @brief Inserts a new char variable in the configuration instance
	void insertChar (const std::string& name, const char& val);
	
	/// Returns a configuration of some existing variable in the
	/// configuration instance.
	/// @param name String with the variable's name.
	/// @return The value of the required configuration type variable.
	/// @brief Access method to a configuration type variable
	Configuration getConfig (const std::string& name) const;
private:
	/// Returns the "true" (non-parsed) value of an existing variable.
	/// @param name String with the variable's name.
	/// @return Non-parsed variable's value.
	/// @brief Returns non-parsed variable value
	const std::string& get (const std::string& name) const;
public:
	/// Returns a std::string value of some existing variable in the
	/// configuration instance using the std::string source and the parser's
	/// method.
	/// @param name String with the variable's name.
	/// @return The value of the required variable.
	/// @see get
	/// @see parseOutStr
	/// @brief Access method to a std::string variable
	std::string getStr (const std::string& name) const;
	
	/// Returns a real number (long double) value of some existing variable
	/// in the configuration instance using the std::string source and the
	/// parser's method.
	/// @param name String with the variable's name.
	/// @return The value of the required variable.
	/// @see get
	/// @see parseOutReal
	/// @brief Access method to a real number variable
	long double getReal (const std::string& name) const;
	
	/// Returns an integer number (long int) value of some existing variable
	/// in the configuration instance using the std::string source and the
	/// parser's method.
	/// @param name String with the variable's name.
	/// @return The value of the required variable.
	/// @see get
	/// @see parseOutInt
	/// @brief Access method to an integer number variable
	long int getInt (const std::string& name) const;
	
	/// Returns a character (char) value of some existing variable in the
	/// configuration instance using the std::string source and the parser's
	/// method.
	/// @param name String with the variable's name.
	/// @return The value of the required variable.
	/// @see get
	/// @see parseOutChar
	/// @brief Access method to a character variable
	char getChar (const std::string& name) const;
private:
	template <class var_type>
	class getvar
	{
	public:
		typedef var_type (Configuration::*type)(const std::string& name) const;
	};
	
	template <class var_type, class vec_type>
	std::list< var_type > getList (
		const std::string& name,
		typename getvar< var_type >::type access_method,
		const std::vector< vec_type >& vec
	) const;
public:
	std::list< Configuration > getConfigList (const std::string& name) const;
	
	std::list< std::string > getStrList (const std::string& name) const;
	std::list< long double > getRealList (const std::string& name) const;
	std::list< long int > getIntList (const std::string& name) const;
	std::list< char > getCharList (const std::string& name) const;
	
	/// Sets the value with "val" parameter of config. type variable named
	/// with "name" parameter.
	/// @param name String with the variable's name.
	/// @param val New value to be assigned.
	/// @brief Modifier method to a configuration type variable
	void setConfig (const std::string& name, const Configuration& val);
	
	/// Sets the value with "val" parameter of std::string variable named with
	/// "name" parameter.
	/// @param name String with the variable's name.
	/// @param val New value to be assigned.
	/// @see parseInStr
	/// @brief Modifier method to a std::string variable
	void setStr (const std::string& name, const std::string& val);
	
	/// Sets the value with "val" parameter of real num. variable named with
	/// "name" parameter.
	/// @param name String with the variable's name.
	/// @param val New value to be assigned.
	/// @see parseInReal
	/// @brief Modifier method to a real number variable
	void setReal (const std::string& name, const long double& val);
	
	/// Sets the value with "val" parameter of int num. variable named with
	/// "name" parameter.
	/// @param name String with the variable's name.
	/// @param val New value to be assigned.
	/// @see parseInInt
	/// @brief Modifier method to an integer number variable
	void setInt (const std::string& name, const long int& val);
	
	/// Sets the value with "val" parameter of character variable named with
	/// "name" parameter.
	/// @param name String with the variable's name.
	/// @param val New value to be assigned.
	/// @see parseInChar
	/// @brief Modifier method to a character variable
	void setChar (const std::string& name, const char& val);
	
	/// Erases a configuration type variable named with "name" parameter.
	/// @param name String with the configuration type variable's name.
	/// @brief Erases a configuration type variable
	void eraseSub (const std::string& name);
	
	/// Erases a data variable named with "name" parameter.
	/// @param name String with the data variable's name.
	/// @brief Erases a data variable
	void erase (const std::string& name);
};

#endif
