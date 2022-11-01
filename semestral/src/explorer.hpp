#ifndef __PROGTEST__
#pragma once

#include <iostream>
#include <string>
//filesystem
#include <experimental/filesystem>


using namespace std;
namespace fs = std::experimental::filesystem;


///Function that normalizes given path, similar to shell
void normalizePath(string & toNormalize);

///Function to check if any path is valid
bool isValid(const fs::path & path);

///Class for manipulating the file system.
class CBrowser {
public:

	///Constructor has to be provided path when initializing
	CBrowser(const fs::path & p);

	///Destructor
	~CBrowser();

	///Gets the path
	fs::path getPath() const;

	///If the path exists on the file system, sets it and returns true
	bool setPath(const string & path);

	///Appender that always normalized the path
	bool appendPath(const string & pathToAppend);

	///Prints current path
	void printPath() const;

protected:

	///Holds the current path
	fs::path m_path;
};

#endif /* __PROGTEST__ */