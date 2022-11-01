#ifndef __PROGTEST__
#include "main.hpp"


using namespace std;
namespace fs = std::experimental::filesystem;


int main(int argc, const char* argv[]) {

	//Initialization of required instances
	CBrowser browser(fs::current_path());
	CCommHistory history;
	CFileManager globalCache;
	string userInput;

	if (argc > 1) {
		string tmp = argv[1];

		//If path doesn't exist, print error
		if (!setPath(tmp, browser)) {
			cout << "Invalid starting directory parameter: " << tmp << endl;
		}
	}

	while (getline(cin, userInput, '\n')) {
		//Remove all redundant whitespaces
		removeWS(userInput);

		//If input is empty, do nothing
		if (userInput.empty()) {
			continue;
		}
//---------------------Start of parser for user input---------------------//
//----------------------------------exit----------------------------------//
		//If exit is written, quit the program
		if (userInput == "exit") {
			break;
//---------------------------------help----------------------------------//
		//If clear is written, clear file cache
		} else if (userInput == "help") {
			printHelp();
//---------------------------------clear----------------------------------//
		//If clear is written, clear file cache
		} else if (userInput == "clear") {
			globalCache.clear();
			cout << "Cleared the cache" << endl;
//----------------------------------info----------------------------------//
		//If info is written, print info about all files
		} else if (userInput == "info") {
			globalCache.print();
//-----------------------------------cd-----------------------------------//
		//If cd is written, print the current path
		} else if (userInput == "cd") {
			browser.printPath();
		//If cd is at the start of the input, change directory to what is following
		} else if (userInput.find("cd ") == 0) {
			//Remove the "cd " from the command
			userInput.erase(userInput.begin(), userInput.begin() + 3);

			//If path was successfully set, print the current directory, otherwise print error
			if (setPath(userInput, browser)) {
				browser.printPath();
			} else {
				cout << "Error: invalid directory" << endl;
			}
//----------------------------------perm----------------------------------//
		//If perm is written, print permissions of current folder
		} else if (userInput == "perm") {
			CCommand * tmp = new CPerm(browser.getPath());
			history.add(tmp);
			tmp->execute();
		//If perm is at the start of the input, print permissions of what is following
		} else if (userInput.find("perm ") == 0) {
			//Remove the "perm " from the command
			userInput.erase(userInput.begin(), userInput.begin() + 5);

			fs::path tmpPath;
			//If path is absolute
			if (userInput[0] == '/') {
				tmpPath = userInput;
			} else {
				tmpPath = browser.getPath() / userInput;
			}

			//If path is valid, print its permissions
			if (isValid(tmpPath)) {
				CCommand * tmp = new CPerm(fs::canonical(tmpPath));
				history.add(tmp);
				tmp->execute();	
			} else {
				invalidPath(tmpPath);
			}
//----------------------------------file----------------------------------//
		//If file is at the start of the input, print file format of what is following
		} else if (userInput.find("file ") == 0) {
			//Remove the "file " from the command
			userInput.erase(userInput.begin(), userInput.begin() + 5);

			fs::path tmpPath;
			//If path is absolute
			if (userInput[0] == '/') {
				tmpPath = userInput;
			} else {
				tmpPath = browser.getPath() / userInput;
			}

			//If path is valid, print file format and add to file cache
			if (isValid(tmpPath)) {
				CCommand * tmp = new CFile(fs::canonical(tmpPath), &globalCache);
				history.add(tmp);
				tmp->execute();
			} else {
				invalidPath(tmpPath);
			}
//----------------------------------size----------------------------------//
		//If size is written, prints space information about current filesystem
		} else if (userInput == "size") {
			CCommand * tmp = new CFileSysInfo("(of filesystem)");
			history.add(tmp);
			tmp->execute();			
		//If size is at the start of the input, print the size of what is following
		} else if (userInput.find("size ") == 0) {
			//Remove the "size " from the command
			userInput.erase(userInput.begin(), userInput.begin() + 5);

			fs::path tmpPath;
			//If path is absolute
			if (userInput[0] == '/') {
				tmpPath = userInput;
			} else {
				tmpPath = browser.getPath() / userInput;
			}

			//If path is valid, print size of path
			if (isValid(tmpPath)) {
				CCommand * tmp = new CSize(fs::canonical(tmpPath));
				history.add(tmp);
				tmp->execute();	
			} else {
				invalidPath(tmpPath);
			}
//----------------------------------time----------------------------------//
		//If time is written, print the three (one) time(s) of a path
		} else if (userInput == "time") {
			CCommand * tmp = new CTime(browser.getPath());
			history.add(tmp);
			tmp->execute();
		//If time is at the start of the input, print times of what is following
		} else if (userInput.find("time ") == 0) {
			//Remove the "time " from the command
			userInput.erase(userInput.begin(), userInput.begin() + 5);

			fs::path tmpPath;
			//If path is absolute
			if (userInput[0] == '/') {
				tmpPath = userInput;
			} else {
				tmpPath = browser.getPath() / userInput;
			}

			//If path is valid, print mtime of path
			if (isValid(tmpPath)) {
				CCommand * tmp = new CTime(fs::canonical(tmpPath));
				history.add(tmp);
				tmp->execute();	
			} else {
				invalidPath(tmpPath);
			}
//----------------------------------tree----------------------------------//
		//If tree is written, print the hierarchy tree
		} else if (userInput == "tree") {
			CCommand * tmp = new CTree(browser.getPath());
			history.add(tmp);
			tmp->execute();
		//If tree is at the start of the input, print tree using custom depth of custom folder
		} else if (userInput.find("tree ") == 0) {
			//Remove the "tree " from the command and declare temporary variables
			userInput.erase(userInput.begin(), userInput.begin() + 5);
			int tmpNum;
			size_t tmpIndex;
			string secondPath;

			//Try to parse the number from user input
			try {
				//tmpIndex will be set to position after the parsed number
				tmpNum = stoi(userInput, &tmpIndex);
			}
			catch(invalid_argument& e){
				//If it cannot be parsed
				invalidInput("tree " + userInput);
				continue;
			}
			catch(out_of_range& e){
				//If the value is out of range
				cout << "Value is out of range." << endl;
				continue;
			}

			//Get the string after the parsed number
			secondPath = userInput.substr(tmpIndex);

			//If the string is empty, call tree on current path
			if (secondPath.empty()) {
				//If depth is negative, call tree with max_depth
				if (tmpNum < 0) {
					CCommand * tmp = new CTree(browser.getPath(), numeric_limits<size_t>::max());
					history.add(tmp);
					tmp->execute();
				//Else call tree with parsed depth
				} else {
					CCommand * tmp = new CTree(browser.getPath(), (size_t)tmpNum);
					history.add(tmp);
					tmp->execute();
				}
			//If the string is not empty check if the first char is space and if there are
			//at least 2 characters (there can't be two spaces in a row because of normalization)
			} else if (secondPath[0] == ' ' && secondPath.size() > 1) {
				//Remove the space from the string
				secondPath.erase(secondPath.begin());
				//If path is relative, append it to current browser path
				if (secondPath[0] != '/') {
					secondPath = browser.getPath() / secondPath;
				}
				//If the path is valid, we can call the command
				if (isValid(secondPath)) {
					//Make path canonical so we can check for directory without exceptions
					secondPath = fs::canonical(secondPath);
					if (fs::is_directory(secondPath)) {
						//If depth is negative, call with max_depth
						if (tmpNum < 0) {
							CCommand * tmp = new CTree(secondPath, numeric_limits<size_t>::max());
							history.add(tmp);
							tmp->execute();
						//Else call with set depth
						} else {
							CCommand * tmp = new CTree(secondPath, (size_t)tmpNum);
							history.add(tmp);
							tmp->execute();
						}
					} else {
						invalidPath(secondPath + " is not a directory");
					}
				//Path is invalid (doesn't exist or isn't directory), print error
				} else {
					invalidPath(secondPath);
				}
			} else {
				invalidInput("tree " + userInput);
			}
//----------------------------------find----------------------------------//
		//If find is at the start of the input, find following filetype
		} else if (userInput.find("find ") == 0) {
			//Remove the "find " from the command
			userInput.erase(userInput.begin(), userInput.begin() + 5);
			int tmpNum;
			size_t tmpIndex;
			string fileTypes;

			//Try to parse the number from user input
			try {
				//tmpIndex will be set to position after the parsed number
				tmpNum = stoi(userInput, &tmpIndex);
			}
			catch(invalid_argument& e){
				//If it cannot be parsed
				invalidInput("find " + userInput);
				continue;
			}
			catch(out_of_range& e){
				//If the value is out of range
				cout << "Value is out of range." << endl;
				continue;
			}

			//Get the string after the parsed number
			fileTypes = userInput.substr(tmpIndex);

			//If the string is empty, print error
			if (fileTypes.empty()) {
				cout << "Error: file types are not specified." << endl;
			//If the string is not empty check if the first char is space and if there are
			//at least 2 characters (there can't be two spaces in a row because of normalization)
			} else if (fileTypes[0] == ' ' && fileTypes.size() > 1) {
				//Remove the space from the string
				fileTypes.erase(fileTypes.begin());

				//If depth is negative, call with max_depth
				if (tmpNum < 0) {
					CCommand * tmp = new CSearch(browser.getPath(), numeric_limits<size_t>::max(), fileTypes);
					if (tmp->execute()) {
						history.add(tmp);
					} else {
						delete tmp;
					}
				//Else call with set depth
				} else {
					CCommand * tmp = new CSearch(browser.getPath(), (size_t)tmpNum, fileTypes);
					if (tmp->execute()) {
						history.add(tmp);
					} else {
						delete tmp;
					}
				}
				//Path is invalid (doesn't exist or isn't directory), print error
			} else {
				cout << "Error: invalid file types (" << fileTypes << ")" << endl;
			}
//--------------------------------history--------------------------------//
		//If history is written, print current history
		} else if (userInput == "history") {
			history.print();
		//If history is at the start of the input and number follows, print number of last commands
		} else if (userInput.find("history ") == 0) {
			//Remove the "history " from the command
			userInput.erase(userInput.begin(), userInput.begin() + 8);
			int tmpNum;

			//Try to parse the number from user input
			try {
				tmpNum = stoi(userInput, nullptr);
			}
			catch(invalid_argument& e){
				//If it cannot be parsed
				invalidInput("history " + userInput);
				continue;
			}
			catch(out_of_range& e){
				//If the value is out of range
				cout << "Value is out of range." << endl;
				continue;
			}

			history.print(tmpNum);
//--------------------------------call--------------------------------//
		//If call is written, the last command from history gets executed
		} else if (userInput == "call") {
			if (history.getRecordAmount() != 0) {
				history.call(history.getRecordAmount() - 1);
			} else {
				cout << "History is empty." << endl;
			}
		//If call is at the start of the input and number follows, execute the command from history
		} else if (userInput.find("call ") == 0) {
			//Remove the "call " from the command
			userInput.erase(userInput.begin(), userInput.begin() + 5);
			int tmpNum;

			//Try to parse the number from user input
			try {
				tmpNum = stoi(userInput, nullptr);
			}
			catch(invalid_argument& e){
				//If it cannot be converted
				invalidInput("call " + userInput);
				continue;
			}
			catch(out_of_range& e){
				//If the value is out of range
				cout << "Value is out of range." << endl;
				continue;
			}

			if (history.getRecordAmount() != 0) {
				history.call(tmpNum);
			} else {
				cout << "History is empty." << endl;
			}
//---------------------------------error---------------------------------//
		//Command doesn't exist
		} else {
			invalidInput(userInput);
		}
	}
//-----------------------------End of parser-----------------------------//

	return 0;
}
#else
int main() {return 0;}
#endif /* __PROGTEST__ */