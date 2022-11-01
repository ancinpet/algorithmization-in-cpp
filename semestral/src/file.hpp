#pragma once

#ifndef __PROGTEST__
#include <iostream>
#include <set>
#include <string>
//filestream
#include <fstream>
//filesystem
#include <experimental/filesystem>

#define TYPEFILE "/tmp/fsdata8976451474654654"


using namespace std;
namespace fs = std::experimental::filesystem;


///Enum with file types, otr is for other file, err is for error
enum fileType {txt, jpeg, png, bin, snd, img, sz, gzip, zip, otr, err};
///Array for getting string from enum, can be indexed with enum because enum is integer starting with 0
const string fileTypeDef[] = {"txt", "jpeg", "png", "bin", "snd", "img", "7z", "gzip", "zip", "other", "error"};

///Function to get type of file from path
fileType getFileType(const string & path);

///Function that gets output string from shell command
string shellToString(const string & command);

///Function to get string of file format
pair<string, string> getFormatString(fileType format);

///Generic file class
class CGenericFile {
public:

	///Constructor requires path because every file needs a path
	CGenericFile(const fs::path & p);

	///Virtual destructor
	virtual ~CGenericFile();

	///Virtual method for printing information about the file
	virtual void info() const = 0;

	///Getter for file path
	string getPath() const;

	///Check if path is still valid
	bool validPath() const;

	///Checks if file is still valid
	virtual bool valid() const = 0;

protected:

	///Path of the file
	const fs::path m_filePath;
};

///Other file
class COtherFile : public CGenericFile {
public:

	///Constructor requires path of the file we want
	COtherFile(const fs::path & p);

	///Virtual destructor
	virtual ~COtherFile();

	///Prints the info
	virtual void info() const override;

	///Checks if file is valid
	virtual bool valid() const override;
};

///Text file
class CTextFile : public CGenericFile {
public:

	///Constructor requires path of the file we want
	CTextFile(const fs::path & p);

	///Virtual destructor
	virtual ~CTextFile();

	///Prints the info
	virtual void info() const override;

	///Checks if file is valid
	virtual bool valid() const override;
};

///Image file
class CImageFile : public CGenericFile {
public:

	///Constructor requires path of the file we want
	CImageFile(const fs::path & p, const string & format);

	///Virtual destructor
	virtual ~CImageFile();

	///Prints the info
	virtual void info() const override;

	///Checks if file is valid
	virtual bool valid() const override;

protected:

	///String holds image format (jpeg, png, unknown)
	string m_format;
};

///Binary file
class CBinaryFile : public CGenericFile {
public:

	///Constructor requires path of the file we want
	CBinaryFile(const fs::path & p);

	///Virtual destructor
	virtual ~CBinaryFile();

	///Prints the info
	virtual void info() const override;

	///Checks if file is valid
	virtual bool valid() const override;
};

///Sound file
class CSoundFile : public CGenericFile {
public:

	///Constructor requires path of the file we want
	CSoundFile(const fs::path & p);

	///Virtual destructor
	virtual ~CSoundFile();

	///Prints the info
	virtual void info() const override;

	///Checks if file is valid
	virtual bool valid() const override;
};

///Compressed file
class CZipFile : public CGenericFile {
public:

	///Constructor requires path of the file we want
	CZipFile(const fs::path & p, const string & format);

	///Virtual destructor
	virtual ~CZipFile();

	///Prints the info
	virtual void info() const override;

	///Checks if file is valid
	virtual bool valid() const override;

protected:

	///String holds compression format (zip, gzip, 7z)
	string m_format;
};

///Comparator for comparing file pointers inside file manager
///Using their path converted to string so that we can store them in set
///and make sure there aren't duplicate files
struct fileCmp {
    bool operator() (const CGenericFile * lhs, const CGenericFile * rhs) const {
         return string(lhs->getPath()) > string(rhs->getPath());
    }
};

///File manager
class CFileManager {
public:

	///Constructor makes new instance of file manager
	CFileManager();

	///Destructor that deletes all files
	~CFileManager();

	///Prints information about all files inside
	void print();

	///Removes everything from the cache
	void clear();

	///Adds file to the cache
	void add(CGenericFile * toAdd);

protected:

	///Vector of files
	set<CGenericFile*, fileCmp> m_cache;
};


#endif /* __PROGTEST__ */