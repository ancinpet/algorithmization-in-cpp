#ifndef __PROGTEST__
#include "explorer.hpp"


///My implementation of fs::canonical()
void normalizePath(string & toNormalize) {
	size_t pos;

	//Empty path is already normalized
	if (toNormalize.empty()) {
		return;
	}

	//Replaces all sequences of "/" with single "/"
	while ((pos = toNormalize.find("//")) != string::npos) {
		toNormalize.erase(pos, 1);
	}

	//Removes all "./" sequences as they do nothing
	while ((pos = toNormalize.find("/./")) != string::npos) {
		toNormalize.erase(pos + 1, 2);
	}

	//Every "../" makes the path go up one directory
	while ((pos = toNormalize.find("../")) != string::npos) {
		size_t ppos;

		//This scenario can happen only when the absolute path is "/../"
		//and in this case, shell ignores the "../" since you cannot go
		//above "/" so just delete the "../" sequence
		if (pos < 2) {
			toNormalize.erase(pos, 3);
		}

		//Searches backwards starting before "/../" untill it finds "/"
		//and then deletes everything inbetween
		if ((ppos = toNormalize.find_last_of("/", pos - 2)) != string::npos) {
			//From (ppos + 1) delete ((pos + 2) - (ppos + 1) + 1) characters
			//I chose this over iterators since it cannot go out of bounds
			toNormalize.erase(ppos + 1, pos + 2 - ppos);
		}
	}
}

///Check if given path is valid
bool isValid(const fs::path & path) {
	bool validPath = false;
	string tmp(path);
	//Using my normalizePath function because fs::canonical throws exceptions
	//on predictable results (canonical of "/root/../root") throws exception
	//because there are no permissions for "/root/.." but it can be removed
	normalizePath(tmp);

	//Try to see if path exists, throws exception if we don't have permission
	try {
		validPath = fs::exists(tmp);
	}
	catch (fs::filesystem_error & e) {
		//cout << e.what() << endl;
		cout << "Permission denied!\n";
		return false;
	}

	return validPath;
}

///It allows to move within the file system and get information about files and directories.
CBrowser::CBrowser(const fs::path & p): m_path(p) {}
CBrowser::~CBrowser() {}

fs::path CBrowser::getPath() const {
	return m_path;
}

bool CBrowser::setPath(const string & pathToSet) {
	string tmp(pathToSet);
	normalizePath(tmp);

	//Checks if path is valid existing directory and sets it
	if (isValid(tmp) && fs::is_directory(tmp)) {
		m_path = fs::canonical(tmp);
		return true;
	}

	return false;
}

bool CBrowser::appendPath(const string & pathToAppend) {
	string tmp(m_path / pathToAppend);
	normalizePath(tmp);

	//Checks if appended path is valid existing directory and sets it
	if (isValid(tmp) && fs::is_directory(tmp)) {
		m_path = fs::canonical(tmp);
		return true;
	}

	return false;
}

void CBrowser::printPath() const {
	cout << "Current path is " << m_path << endl;
}

#endif /* __PROGTEST__ */