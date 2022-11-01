#pragma once
#ifndef __PROGTEST__

#include <iostream>
#include <string>
//filesystem
#include <experimental/filesystem>

#include "explorer.hpp"
#include "command.hpp"
#include "history.hpp"
#include "file.hpp"


using namespace std;
namespace fs = std::experimental::filesystem;


///Function which decides if the path is relative or absolute and sets the path
bool setPath(const string & toSet, CBrowser & whereSet) {
	if (toSet.empty()) {
		return false;
	}

	//If the path starts with /, try to set absolute path
	if (toSet[0] == '/') {
		return whereSet.setPath(toSet);
	}

	//Else try to set relative path
	return whereSet.appendPath(toSet);
}

///Function to remove all redundant whitespaces
void removeWS(string & toRemove) {
	//Go through the given string and check if current char is space
	for (size_t i = 0; i < toRemove.size(); ++i) {
		if (toRemove[i] != ' ') {
			continue;
		}

		//If the current char is the first char of string
		if (i == 0) {
			toRemove.erase(toRemove.begin() + i);
			--i;
		//If the current char is the last char of string
		} else if (i == toRemove.size() - 1) {
			toRemove.erase(toRemove.begin() + i);
		//If there is a space at least on one side
		} else if (toRemove[i - 1] == ' ' || toRemove[i + 1] == ' ') {
			toRemove.erase(toRemove.begin() + i);
			--i;
		}
	}
}

///Function to print out invalid input
inline void invalidInput(const string & cmd) {
	cout << "Invalid command: " << cmd << endl;
}

///Function to print out invalid path
inline void invalidPath(const string & path) {
	string tmp(path);
	normalizePath(tmp);
	cout << "Error: invalid path (" << tmp << ")" << endl;
}

///Function to print out help
inline void printHelp() {
	cout << "COMMANDS\n" << string(20, '-') << '\n'
		 << "exit                        - exit the application\n"
		 << "clear                       - clear file cache\n"
		 << "info                        - print info about files from cache\n"
		 << "cd [path]                   - print directory [change directory]\n"
		 << "perm [path]                 - print permissions if possible\n"
		 << "file path                   - print file format and add to file cache\n"
		 << "size [path]                 - print size if possible\n"
		 << "time [path]                 - print modify time if possible\n"
		 << "tree                        - print all files in current folder\n"
		 << "tree depth [path]           - print all files with recursive depth\n"
		 << "note: path must be directory, if depth is negative, maximum depth will be used\n"
		 << "find depth format [format]* - finds format[s] in current directory and subdirectories\n"
		 << "note: supported formats are: txt, jpeg, png, bin, snd, img, 7z, gzip, zip, other\n"
		 << "history                     - prints history of commands"
		 << " (doesn't include cd, info, clear, call, history)\n"
		 << "call [index]                - runs last command [command with index]\n"
		 << string(20, '-') << endl;
}

#endif /* __PROGTEST__ */