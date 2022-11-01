#ifndef __PROGTEST__
#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <iterator>

//filesystem
#include <experimental/filesystem>
//setprecision modifiers
#include <iomanip>
//timestamp to string conversion
#include <ctime>
//get timestamps from files in second version
//#include <sys/stat.h>
//#include <sys/types.h>


using namespace std;
namespace fs = std::experimental::filesystem;


///Generic command class with execute method
class CCommand {
public:

	///Constructor requires path because every command needs a path
	CCommand(const fs::path & p);

	virtual ~CCommand();

	///Virtual method that each command has to implement
	virtual bool execute() const = 0;

	///Getter for command path
	string getPath() const;

	///Getter for command name
	virtual string getName() const = 0;

protected:

	///Path of the command is constant because it cannot be changed
	const fs::path m_cmdPath;
};

///Class for the command "perm" to get permissions of given path
class CPerm : public CCommand {
public:

	///Constructor requires path of the file we want permissions of
	CPerm(const fs::path & p);

	virtual ~CPerm();

	///Prints the permission
	virtual bool execute() const override;

	///Getter for command name
	virtual string getName() const override;
};

///Class for the command "size" to get info about filesystem
class CFileSysInfo : public CCommand {
public:

	///Constructor requires path of the disk we want size of
	CFileSysInfo(const fs::path & p);

	virtual ~CFileSysInfo();

	///Prints the size of disk
	virtual bool execute() const override;

	///Getter for command name
	virtual string getName() const override;
};

///Class for the command "size" to get size of given path
class CSize : public CCommand {
public:

	///Constructor requires path of the file we want size of
	CSize(const fs::path & p);

	virtual ~CSize();

	///Prints the size of path
	virtual bool execute() const override;

	///Getter for command name
	virtual string getName() const override;
};

///Class for the command "time" to get time info about path
class CTime : public CCommand {
public:

	///Constructor requires path of the file we want time info about
	CTime(const fs::path & p);

	virtual ~CTime();

	///Prints the time info
	virtual bool execute() const override;

	///Getter for command name
	virtual string getName() const override;
};

///Class for the command "tree" to print file hierarchy
class CTree : public CCommand {
public:

	///Constructor requires path of the directory and depth
	CTree(const fs::path & p, const size_t depth = 0);

	virtual ~CTree();

	///Prints the tree hierarchy with custom depth
	virtual bool execute() const override;

	///Getter for command name
	virtual string getName() const override;

protected:

	///Specifies the depth of recursion when printing tree
	size_t m_depth;
};

class CFileManager;

///Class for the command "file" to get type and format of file
class CFile : public CCommand {
public:

	///Constructor requires path of the file we want format of
	CFile(const fs::path & p, CFileManager * ptr);

	virtual ~CFile();

	///Prints the format
	virtual bool execute() const override;

	///Getter for command name
	virtual string getName() const override;

protected:

	CFileManager * m_ptrCache;
};

///Class for the command "find" to find file format in a folder with custom depth
class CSearch : public CCommand {
public:

	///Constructor requires path of the directory, depth and formats to look for
	CSearch(const fs::path & p, const size_t depth, const string & args);

	virtual ~CSearch();

	///Finds all files of specific format in path with custom depth
	virtual bool execute() const override;

	///Getter for command name
	virtual string getName() const override;

protected:

	///Specifies the depth of recursion when printing tree
	size_t m_depth;
	///Specifies the format we should look for
	string m_fileTypes;
};

#endif /* __PROGTEST__ */