#pragma once

#include <exception>
#include <string>


class Exception : public std::exception
{
public:
	Exception(int __line__, const char* __file__) noexcept;

	const char* what() const noexcept override;

	virtual const char* GetType() const noexcept;
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;
	std::string GetOriginString() const noexcept;

private:
	int line;
	std::string file;

protected:
	mutable std::string whatBuffer;
};