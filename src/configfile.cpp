/// @ingroup MOD_CONFIGFILE
/// @file configfile.cpp
/// @brief Implementation of the Configuration File Parser methods
/// @author Matheus Pimenta

#include <sstream>
#include <stack>

#include "configfile.hpp"

#include "common.hpp"

using namespace common;

using std::fstream;
using std::stack;
using std::string;
using std::ios;
using std::stringstream;
using std::vector;
using std::list;

// =============================================================================
// Configuration Class Member (Parser)
// =============================================================================

class Parser
{
private:
	// =====================================================================
	// File Parser Class
	// =====================================================================
	
	class FileParser
	{
	private:
		enum State
		{
			WAITING_NAME = 0,
			READING_NAME,
			W8ING_EQLS_OR_BRACE,
			W8ING_OPENING_BRACE,
			WAITING_VALUE,
			READING_VALUE,
			W8ING_NAME_OR_BRACE
		};
		
		// =============================================================
		// File Parser Attributes
		// =============================================================
		
		fstream file_;
		stack< Configuration* > dest_;
		unsigned int line_;
		State state_;
		char input_;
		bool keep_input_;
		stack< string > names_;
		string value_;
		string spaces_;
	public:
		// =============================================================
		// File Parser Methods
		// =============================================================
		
		FileParser (const string& filename, Configuration* dest);
		~FileParser ();
	private:
		void parse ();
		
		void insertConfigVar ();
		void insertDataVar ();
		
		void avoidLine ();
		
		void changeToW8ingNameState ();
		
		void handleLineBreak ();
		void handleWaitingName ();
		void handleReadingName ();
		void handleW8ingEqlsOrBrace ();
		void handleW8ingOpeningBrace ();
		void handleWaitingValue ();
		void handleReadingValue ();
		void handleW8ingNameOrBrace ();
		void handleEof ();
	};
public:
	// =====================================================================
	// Parser Methods
	// =====================================================================
	
	static void parseFile (const string& filename, Configuration* dest);
	
	static void parseName (const string& src);
	
	static string parseInStr (const string& src);
	static string parseInReal (const long double& src);
	static string parseInInt (const long int& src);
	static string parseInChar (const char& src);
	
	static string parseOutStr (const string& src);
	static long double parseOutReal (const string& src);
	static long int parseOutInt (const string& src);
	static char parseOutChar (const string& src);
};

// =============================================================================
// Configuration Class Member (Variable)
// =============================================================================

template <class T>
Configuration::Variable< T >::Variable (const string& name, const T& value) :
name_ ( name ), value_ ( value )
{
}

template <class T>
Configuration::Variable< T >::~Variable ()
{
}

template <class T>
const string& Configuration::Variable< T >::name () const
{
	return name_;
}

template <class T>
const T& Configuration::Variable< T >::value () const
{
	return value_;
}

template <class T>
void Configuration::Variable< T >::set (const T& value)
{
	value_ = value;
}

// =============================================================================
// File Parser Class
// =============================================================================

Parser::FileParser::FileParser (const string& filename, Configuration* dest) :
keep_input_(false)
{
	file_.open ( filename.c_str () );
	
	if ( !file_.is_open () )
		throw ( Configuration::FileNotFound () );
	
	dest_.push ( dest );
	names_.push ( string () );
	
	parse ();
	
	file_.close ();
}

Parser::FileParser::~FileParser ()
{
}

void Parser::FileParser::parse ()
{
	line_ = 1;
	state_ = WAITING_NAME;
	
	// avoiding exceptions for empty files by setting eofbit
	input_ = file_.get ();
	
	while ( !file_.eof () )
	{
		switch ( state_ )
		{
			case WAITING_NAME:
				handleWaitingName ();
				break;
			
			case READING_NAME:
				handleReadingName ();
				break;
			
			case W8ING_EQLS_OR_BRACE:
				handleW8ingEqlsOrBrace ();
				break;
			
			case W8ING_OPENING_BRACE:
				handleW8ingOpeningBrace ();
				break;
			
			case WAITING_VALUE:
				handleWaitingValue ();
				break;
			
			case READING_VALUE:
				handleReadingValue ();
				break;
			
			case W8ING_NAME_OR_BRACE:
				handleW8ingNameOrBrace ();
				break;
			
			default:
				break;
		}
		
		if ( !file_.eof () ) {
			if (keep_input_)
				keep_input_ = false;
			else
				input_ = file_.get ();
		}
		
		if ( file_.eof () )
			handleEof ();
	}
}

void Parser::FileParser::insertConfigVar ()
{
	Configuration* tmp = dest_.top ();
	dest_.pop ();
	names_.pop ();
	
	try {
		dest_.top ()->insertConfig ( names_.top (), *tmp );
	}
	catch (Configuration::VarAlreadyExisting& e)
	{
		string tmp ( "Configuration variable redefined at line " );
		tmp += ( '0' + line_ );
		throw ( mexception ( tmp ) );
	}
	
	delete tmp;
}

void Parser::FileParser::insertDataVar ()
{
	try {
		dest_.top ()->insertStr ( names_.top (), value_ );
	}
	catch (Configuration::VarAlreadyExisting& e)
	{
		string tmp ( "Data variable redefined at line " );
		tmp += ( '0' + line_ );
		throw ( mexception ( tmp ) );
	}
}

void Parser::FileParser::avoidLine ()
{
	for ( bool end = false; ( !file_.eof () ) && ( !end ); )
	{
		input_ = file_.get ();
		
		if ( ( input_ == '\r' ) || ( input_ == '\n' ) )
		{
			end = true;
			handleLineBreak ();
		}
	}
}

void Parser::FileParser::changeToW8ingNameState ()
{
	if ( dest_.size () == 1 )
		state_ = WAITING_NAME;
	else
		state_ = W8ING_NAME_OR_BRACE;
}

void Parser::FileParser::handleLineBreak ()
{
	line_++;
	
	// avoiding Windows/DOS LF, or setting eofbit
	input_ = file_.get ();
	if ( !file_.eof () )
	{
		if ( input_ != '\n' )
			keep_input_ = true;
	}
}

void Parser::FileParser::handleWaitingName ()
{
	if (	( input_ == '_' ) ||
		( ( input_ >= 'A' ) && ( input_ <= 'Z' ) ) ||
		( ( input_ >= 'a' ) && ( input_ <= 'z' ) )	)
	{
		names_.top () = input_;
		state_ = READING_NAME;
	}
	else if ( input_ == '#' )
		avoidLine ();
	else if ( ( input_ == '\r' ) || ( input_ == '\n' ) )
		handleLineBreak ();
	else if ( ( input_ != ' ' ) && ( input_ != '\t' ) )
	{
		string tmp ( "FParserVarNameInvalidToken line " );
		tmp += ( '0' + line_ );
		throw ( mexception ( tmp ) );
	}
}

void Parser::FileParser::handleReadingName ()
{
	if (	( input_ == '_' ) ||
		( ( input_ >= '0' ) && ( input_ <= '9' ) ) ||
		( ( input_ >= 'A' ) && ( input_ <= 'Z' ) ) ||
		( ( input_ >= 'a' ) && ( input_ <= 'z' ) )	)
		names_.top () += input_;
	else if ( ( input_ == ' ' ) || ( input_ == '\t' ) )
		state_ = W8ING_EQLS_OR_BRACE;
	else if ( ( input_ == '\r' ) || ( input_ == '\n' ) )
	{
		handleLineBreak ();
		state_ = W8ING_OPENING_BRACE;
	}
	else if ( input_ == '=' )
		state_ = WAITING_VALUE;
	else if ( input_ == '{' )
	{
		dest_.push ( new Configuration () );
		names_.push ( string () );
		state_ = W8ING_NAME_OR_BRACE;
	}
	else
	{
		string tmp ( "FParserVarNameInvalidToken line " );
		tmp += ( '0' + line_ );
		throw ( mexception ( tmp ) );
	}
}

void Parser::FileParser::handleW8ingEqlsOrBrace ()
{
	if ( input_ == '=' )
		state_ = WAITING_VALUE;
	else if ( input_ == '{' )
	{
		dest_.push ( new Configuration () );
		names_.push ( string () );
		state_ = W8ING_NAME_OR_BRACE;
	}
	else if ( input_ == '#' )
	{
		avoidLine ();
		state_ = W8ING_OPENING_BRACE;
	}
	else if ( ( input_ == '\r' ) || ( input_ == '\n' ) )
	{
		handleLineBreak ();
		state_ = W8ING_OPENING_BRACE;
	}
	else if ( ( input_ != ' ' ) && ( input_ != '\t' ) )
	{
		string tmp ( "FParserValAssignmentMissing line " );
		tmp += ( '0' + line_ );
		throw ( mexception ( tmp ) );
	}
}

void Parser::FileParser::handleW8ingOpeningBrace ()
{
	if ( input_ == '{' )
	{
		dest_.push ( new Configuration () );
		names_.push ( string () );
		state_ = W8ING_NAME_OR_BRACE;
	}
	else if ( input_ == '#' )
		avoidLine ();
	else if ( ( input_ == '\r' ) || ( input_ == '\n' ) )
		handleLineBreak ();
	else if ( ( input_ != ' ' ) && ( input_ != '\t' ) )
	{
		string tmp ( "FParserValAssignmentMissing line " );
		tmp += ( '0' + line_ );
		throw ( mexception ( tmp ) );
	}
}

void Parser::FileParser::handleWaitingValue ()
{
	if ( input_ == '#' )
	{
		value_ = "";
		insertDataVar ();
		
		avoidLine ();
		
		changeToW8ingNameState ();
	}
	else if ( ( input_ == '\r' ) || ( input_ == '\n' ) )
	{
		value_ = "";
		insertDataVar ();
		
		handleLineBreak ();
		
		changeToW8ingNameState ();
	}
	else if ( ( input_ != ' ' ) && ( input_ != '\t' ) )
	{
		if ( ( input_ == '}' ) && ( dest_.size () > 1 ) )
		{
			value_ = "";
			insertDataVar ();
			
			insertConfigVar ();
			
			changeToW8ingNameState ();
		}
		else
		{
			spaces_ = "";
			value_ = input_;
			
			state_ = READING_VALUE;
		}
	}
}

void Parser::FileParser::handleReadingValue ()
{
	if ( ( input_ == ' ' ) || ( input_ == '\t' ) )
	{
		if ( spaces_.size () > 0 )
			spaces_ += input_;
		else
			spaces_ = input_;
	}
	else if ( ( input_ == '\r' ) || ( input_ == '\n' ) )
	{
		insertDataVar ();
		
		handleLineBreak ();
		
		changeToW8ingNameState ();
	}
	else if ( input_ == '#' )
	{
		if ( spaces_.size () > 0 )
		{
			insertDataVar ();
			
			avoidLine ();
			
			changeToW8ingNameState ();
		}
		else
			value_ += input_;
	}
	else if ( input_ == '}' )
	{
		if ( spaces_.size () > 0 )
		{
			insertDataVar ();
			
			insertConfigVar ();
			
			avoidLine ();
			
			changeToW8ingNameState ();
		}
		else
			value_ += input_;
	}
	else
	{
		if ( spaces_.size () > 0 )
		{
			value_ += spaces_;
			spaces_ = "";
		}
		value_ += input_;
	}
}

void Parser::FileParser::handleW8ingNameOrBrace ()
{
	if ( input_ == '}' )
	{
		insertConfigVar ();
		
		changeToW8ingNameState ();
	}
	else
		handleWaitingName ();
}

void Parser::FileParser::handleEof ()
{
	string tmp;
	
	switch ( state_ )
	{
		case READING_NAME:
		case W8ING_EQLS_OR_BRACE:
		case W8ING_OPENING_BRACE:
			tmp = "FParserValAssignmentMissing line ";
			tmp += ( '0' + line_ );
			throw ( mexception ( tmp ) );
			break;
		
		case WAITING_VALUE:
			if ( dest_.size () > 1 )
			{
				tmp = "FParserClosingBraceMissing line ";
				tmp += ( '0' + line_ );
				throw ( mexception ( tmp ) );
			}
			value_ = "";
			insertDataVar ();
			break;
		
		case READING_VALUE:
			if ( dest_.size () > 1 )
			{
				tmp = "FParserClosingBraceMissing line ";
				tmp += ( '0' + line_ );
				throw ( mexception ( tmp ) );
			}
			insertDataVar ();
			break;
		
		case W8ING_NAME_OR_BRACE:
			tmp = "FParserClosingBraceMissing line ";
			tmp += ( '0' + line_ );
			throw ( mexception ( tmp ) );
			break;
		
		default:
			break;
	}
}

// =============================================================================
// Configuration Parser Methods
// =============================================================================

void Parser::parseFile (const string& filename, Configuration* dest)
{
	FileParser ( filename, dest );
}

void Parser::parseName (const string& src)
{
	size_t src_size = src.size ();
	
	if ( src_size == 0 )
		throw ( mexception ( "StrParserInvalidName" ) );
	else if ( !(	( src[0] == '_' ) ||
			( ( src[0] >= 'A' ) && ( src[0] <= 'Z' ) ) ||
			( ( src[0] >= 'a' ) && ( src[0] <= 'z' ) )	) )
		throw ( mexception ( "StrParserInvalidName" ) );
	
	for ( size_t i = 1; i < src_size; i++ )
	{
		
		if ( !(	( src[i] == '_' ) ||
			( ( src[i] >= '0' ) && ( src[i] <= '9' ) ) ||
			( ( src[i] >= 'A' ) && ( src[i] <= 'Z' ) ) ||
			( ( src[i] >= 'a' ) && ( src[i] <= 'z' ) )	) )
			throw ( mexception ( "StrParserInvalidName" ) );
	}
}

string Parser::parseInStr (const string& src)
{
	string s = "";
	
	stringstream ss;
	ss << src;
	char input = ss.get ();
	
	while ( !ss.eof () )
	{
		if (	( input != '#' ) &&
			( input != '}' )	)
		{
			switch ( input )
			{
				case '\t': s += "\\t"; break;
				case '\r': s += "\\r"; break;
				case '\n': s += "\\n"; break;
				case '\0': s += "\\0"; break;
				default:   s += input; break;
			}
		}
		else if ( input == '#' )
		{
			if ( s[ s.size () - 1 ] != '\\' )
				s += "\\#";
			else
				s += '#';
		}
		else
		{
			if ( s[ s.size () - 1 ] != '\\' )
				s += "\\}";
			else
				s += '}';
		}
		
		input = ss.get ();
	}
	
	return s;
}

string Parser::parseInReal (const long double& src)
{
	stringstream ss;
	ss << src;
	return ss.str ();
}

string Parser::parseInInt (const long int& src)
{
	stringstream ss;
	ss << src;
	return ss.str ();
}

string Parser::parseInChar (const char& src)
{
	string s;
	
	switch ( src )
	{
		case '\t': s = "\\t"; break;
		case '\r': s = "\\r"; break;
		case '\n': s = "\\n"; break;
		case '\0': s = "\\0"; break;
		case '#':  s = "\\#"; break;
		case '}':  s = "\\}"; break;
		default:   s = src;   break;
	}
	
	return s;
}

string Parser::parseOutStr (const string& src)
{
	string s = "";
	
	stringstream ss;
	ss << src;
	char input = ss.get ();
	bool keep_input = false;
	char tmp;
	
	while ( !ss.eof () )
	{
		if ( input != '\\' )
			s += input;
		else
		{
			tmp = ss.get ();
			if ( ss.eof () )
				s += input;
			else if ( tmp == 't' )
				s += '\t';
			else if ( tmp == 'r' )
				s += '\r';
			else if ( tmp == 'n' )
				s += '\n';
			else if ( tmp == '0' )
				s += '\0';
			else if ( tmp == '#' )
				s += '#';
			else if ( tmp == '}' )
				s += '}';
			else
			{
				s += input;
				keep_input = true;
			}
		}
		
		if ( !ss.eof () ) {
			if (keep_input)
				keep_input = false;
			else
				input = ss.get ();
		}
	}
	
	return s;
}

long double Parser::parseOutReal (const string& src)
{
	stringstream ss;
	long double tmp;
	
	ss << src;
	ss >> tmp;
	
	if ( ( !ss.eof () ) || ( ss.fail () ) )
		throw ( mexception ( "StrParserInvConversionReal" ) );
	
	return tmp;
}

long int Parser::parseOutInt (const string& src)
{
	stringstream ss;
	long int tmp;
	
	ss << src;
	ss >> tmp;
	
	if ( ( !ss.eof () ) || ( ss.fail () ) )
		throw ( mexception ( "StrParserInvConversionInt" ) );
	
	return tmp;
}

char Parser::parseOutChar (const string& src)
{
	if ( src.size () != 1 )
	{
		if ( src.size () != 2 )
			throw ( mexception ( "StrParserInvConversionChar" ) );
		else if ( src[0] == '\\' )
		{
			if ( src[1] == 't' )
				return '\t';
			else if ( src[1] == 'r' )
				return '\r';
			else if ( src[1] == 'n' )
				return '\n';
			else if ( src[1] == '0' )
				return '\0';
			else if ( src[1] == '#' )
				return '#';
			else if ( src[1] == '}' )
				return '}';
			else
				throw ( mexception ( "StrParserInvConversionChar" ) );
		}
		else
			throw ( mexception ( "StrParserInvConversionChar" ) );
	}
	
	return src[0];
}

// =============================================================================
// Configuration Methods
// =============================================================================

Configuration::Configuration ()
{
}

Configuration::~Configuration ()
{
}

void Configuration::readTxt (const string& filename)
{
	Parser::parseFile ( filename, this );
}

void Configuration::readBin_ (fstream& f, Configuration& dest)
{
	size_t tmp1, tmp2;
	char* s;
	string name;
	Configuration buf;
	string value;
	
	// reading all Configuration type variables
	f.read ( (char*) &tmp1, sizeof ( tmp1 ) );
	for ( size_t i = 0; i < tmp1; i++ )
	{
		// reading the size and then the name itself
		f.read ( (char*) &tmp2, sizeof ( tmp2 ) );
		s = new char[ tmp2 + 1 ];
		f.read ( s, tmp2 );
		s[ tmp2 ] = '\0';
		name = s;
		delete[] s;
		
		readBin_ ( f, buf );
		
		try {
			dest.insertConfig ( name, buf );
		}
		catch (VarAlreadyExisting& e)
		{
			dest.setConfig ( name, buf );
		}
	}
	
	// reading all data variables
	f.read ( (char*) &tmp1, sizeof ( tmp1 ) );
	for ( size_t i = 0; i < tmp1; i++ )
	{
		// reading the size and then the name itself
		f.read ( (char*) &tmp2, sizeof ( tmp2 ) );
		s = new char[ tmp2 + 1 ];
		f.read ( s, tmp2 );
		s[ tmp2 ] = '\0';
		name = s;
		delete[] s;
		
		// reading the size and then the value itself of data variable
		f.read ( (char*) &tmp2, sizeof ( tmp2 ) );
		s = new char[ tmp2 + 1 ];
		f.read ( s, tmp2 );
		s[ tmp2 ] = '\0';
		value = s;
		delete[] s;
		
		try {
			dest.insertStr ( name, value );
		}
		catch (VarAlreadyExisting& e)
		{
			dest.setStr ( name, value );
		}
	}
}

void Configuration::readBin (const string& filename)
{
	fstream f ( filename.c_str (), fstream::in | fstream::binary );
	
	if ( !f.is_open () )
		throw ( FileNotFound () );
	
	readBin_ ( f, *this );
	
	f.close ();
}

void Configuration::writeTxt_ (fstream& f, unsigned short tab) const
{
	string inden = "";
	for ( unsigned short i = 0; i < tab; i++ )
		inden += '\t';
	
	// writing all the Configuration type variables
	for ( size_t i = 0; i < cvars.size (); i++ )
	{
		f << inden;
		f << cvars[i].name ();
		f << "\r\n";
		f << inden;
		f << "{\r\n";
		cvars[i].value ().writeTxt_ ( f, tab + 1 );
		f << inden;
		f << "}\r\n";
		
		// extra line break to separate blocks, if the last block
		// written isn't the last variable of its configuration instance
		if ( !(	( i + 1 == cvars.size () ) &&
			( !vars.size () )	) )
		{
			f << inden;
			f << "\r\n";
		}
	}
	
	// writing all the data variables
	for ( size_t i = 0; i < vars.size (); i++ )
	{
		f << inden;
		f << vars[i].name ();
		f << " = ";
		f << vars[i].value ();
		f << "\r\n";
	}
}

void Configuration::writeTxt (const string& filename)
{
	fstream f ( filename.c_str (), fstream::out );
	
	writeTxt_ ( f, 0 );
	f << "\r\n";
	
	f.close ();
}

void Configuration::writeBin_ (fstream& f) const
{
	size_t tmp1, tmp2;
	
	// writing all the Configuration type variables
	tmp1 = cvars.size ();
	f.write ( (const char*) &tmp1, sizeof ( tmp1 ) );
	for ( size_t i = 0; i < tmp1; i++ )
	{
		// writing the size and the name itself
		tmp2 = cvars[i].name ().size ();
		f.write ( (const char*) &tmp2, sizeof ( tmp2 ) );
		f.write ( cvars[i].name ().c_str (), tmp2 );
		
		cvars[i].value ().writeBin_ ( f );
	}
	
	// writing all the data variables
	tmp1 = vars.size ();
	f.write ( (const char*) &tmp1, sizeof ( tmp1 ) );
	for ( size_t i = 0; i < tmp1; i++ )
	{
		// writing the size and the name itself
		tmp2 = vars[i].name ().size ();
		f.write ( (const char*) &tmp2, sizeof ( tmp2 ) );
		f.write ( vars[i].name ().c_str (), tmp2 );
		
		// writing the size and the value itself
		tmp2 = vars[i].value ().size ();
		f.write ( (const char*) &tmp2, sizeof ( tmp2 ) );
		f.write ( vars[i].value ().c_str (), tmp2 );
	}
}

void Configuration::writeBin (const string& filename)
{
	fstream f ( filename.c_str (), fstream::out | fstream::binary );
	
	writeBin_ ( f );
	
	f.close ();
}

void Configuration::clear ()
{
	cvars.clear ();
	vars.clear ();
}

size_t Configuration::size () const
{
	return ( cvars.size () + vars.size () );
}

template <class T>
void Configuration::find (long int& mid, const vector< T >& vec, const string& name) const
{
	long int beg = 0;
	long int end = ( ( (long int) vec.size () ) - 1 );
	bool found = false;
	
	mid = 0;
	
	while ( ( beg <= end ) && ( !found ) )
	{
		mid = ( beg + end ) / 2;
		
		if ( vec[ mid ].name () < name )
		{
			beg = mid + 1;
			mid = beg;
		}
		else if ( vec[ mid ].name () > name )
			end = mid - 1;
		else
			found = true;
	}
	
	if ( !found )
		throw ( VarNotFound () );
}

void Configuration::insertConfig (const string& name, const Configuration& val)
{
	long int i;
	
	if ( &val == this )
		throw ( mexception ( "Config. inserting itself" ) );
	
	try {
		find ( i, cvars, name );
		
		throw ( VarAlreadyExisting () );
	}
	catch (VarNotFound& e)
	{
		Parser::parseName ( name );
		
		cvars.insert
		(
			cvars.begin () + i,
			ConfigVariable ( name, val )
		);
	}
}

void Configuration::insertStr (const string& name, const string& val)
{
	long int i;
	
	try {
		find ( i, vars, name );
		
		throw ( VarAlreadyExisting () );
	}
	catch (VarNotFound& e)
	{
		Parser::parseName ( name );
		
		vars.insert
		(
			vars.begin () + i,
			DataVariable ( name, Parser::parseInStr ( val ) )
		);
	}
}

void Configuration::insertReal (const string& name, const long double& val)
{
	long int i;
	
	try {
		find ( i, vars, name );
		
		throw ( VarAlreadyExisting () );
	}
	catch (VarNotFound& e)
	{
		Parser::parseName ( name );
		
		vars.insert
		(
			vars.begin () + i,
			DataVariable ( name, Parser::parseInReal ( val ) )
		);
	}
}

void Configuration::insertInt (const string& name, const long int& val)
{
	long int i;
	
	try {
		find ( i, vars, name );
		
		throw ( VarAlreadyExisting () );
	}
	catch (VarNotFound& e)
	{
		Parser::parseName ( name );
		
		vars.insert
		(
			vars.begin () + i,
			DataVariable ( name, Parser::parseInInt ( val ) )
		);
	}
}

void Configuration::insertChar (const string& name, const char& val)
{
	long int i;
	
	try {
		find ( i, vars, name );
		
		throw ( VarAlreadyExisting () );
	}
	catch (VarNotFound& e)
	{
		Parser::parseName ( name );
		
		vars.insert
		(
			vars.begin () + i,
			DataVariable ( name, Parser::parseInChar ( val ) )
		);
	}
}

Configuration Configuration::getConfig (const string& name) const
{
	long int i;
	
	find ( i, cvars, name );
	
	return cvars[i].value ();
}

const string& Configuration::get (const string& name) const
{
	long int i;
	
	find ( i, vars, name );
	
	return vars[i].value ();
}

string Configuration::getStr (const string& name) const
{
	return Parser::parseOutStr ( get ( name ) );
}

long double Configuration::getReal (const string& name) const
{
	return Parser::parseOutReal ( get ( name ) );
}

long int Configuration::getInt (const string& name) const
{
	return Parser::parseOutInt ( get ( name ) );
}

char Configuration::getChar (const string& name) const
{
	return Parser::parseOutChar ( get ( name ) );
}

template <class var_type, class vec_type>
list< var_type > Configuration::getList (
	const string& name,
	typename getvar< var_type >::type access_method,
	const vector< vec_type >& vec
) const
{
	list< var_type > ret;
	
	for ( int i = 0; i < (int) vec.size (); ++i )
	{
		size_t j = 0;
		
		while (	(	( j < vec[i].name ().size () ) &&
				( j < name.size () )	) &&
			( vec[i].name ()[j] == name[j] )	)
		{
			++j;
		}
		
		if ( j == name.size () )
		{
			ret.push_back (
				COMMON_CALLBACK ( *this, access_method ) ( vec[i].name () )
			);
		}
	}
	
	return ret;
}

list< Configuration > Configuration::getConfigList (const string& name) const
{
	return getList< Configuration, ConfigVariable > (
		name,
		&Configuration::getConfig,
		cvars
	);
}

list< string > Configuration::getStrList (const string& name) const
{
	return getList< string, DataVariable > (
		name,
		&Configuration::getStr,
		vars
	);
}

list< long double > Configuration::getRealList (const string& name) const
{
	return getList< long double, DataVariable > (
		name,
		&Configuration::getReal,
		vars
	);
}

list< long int > Configuration::getIntList (const string& name) const
{
	return getList< long int, DataVariable > (
		name,
		&Configuration::getInt,
		vars
	);
}

list< char > Configuration::getCharList (const string& name) const
{
	return getList< char, DataVariable > (
		name,
		&Configuration::getChar,
		vars
	);
}

void Configuration::setConfig (const string& name, const Configuration& val)
{
	long int i;
	
	find ( i, cvars, name );
	
	cvars[i].set ( val );
}

void Configuration::setStr (const string& name, const string& val)
{
	long int i;
	
	find ( i, vars, name );
	
	vars[i].set ( Parser::parseInStr ( val ) );
}

void Configuration::setReal (const string& name, const long double& val)
{
	long int i;
	
	find ( i, vars, name );
	
	vars[i].set ( Parser::parseInReal ( val ) );
}

void Configuration::setInt (const string& name, const long int& val)
{
	long int i;
	
	find ( i, vars, name );
	
	vars[i].set ( Parser::parseInInt ( val ) );
}

void Configuration::setChar (const string& name, const char& val)
{
	long int i;
	
	find ( i, vars, name );
	
	vars[i].set ( Parser::parseInChar ( val ) );
}

void Configuration::eraseSub (const string& name)
{
	long int i;
	
	find ( i, cvars, name );
	
	cvars.erase ( cvars.begin () + i );
}

void Configuration::erase (const string& name)
{
	long int i;
	
	find ( i, vars, name );
	
	vars.erase ( vars.begin () + i );
}
