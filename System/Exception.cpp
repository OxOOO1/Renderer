#include "Exception.h"
#include <sstream>

Exception::Exception(int __line__, const char * __file__) noexcept 
	: line(__line__), file(__file__)
{

}

const char* Exception::what() const noexcept
{
	std::ostringstream ss;
	ss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = ss.str();
	return whatBuffer.c_str();

}

const char* Exception::GetType() const noexcept
{
	return "Exception";
}

int Exception::GetLine() const noexcept
{
	return line;
}

const std::string & Exception::GetFile() const noexcept
{
	return file;
}

std::string Exception::GetOriginString() const noexcept
{
	std::ostringstream ss;
	ss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return ss.str();
}
