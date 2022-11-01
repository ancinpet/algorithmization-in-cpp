#pragma once

#ifndef __PROGTEST__
#include <iostream>
#include <vector>
#include <string>
//filesystem
#include <experimental/filesystem>

#include "command.hpp"


using namespace std;
namespace fs = std::experimental::filesystem;


///Class that keeps record of executed commands
class CCommHistory {
public:

	///Construtor for history manager
	CCommHistory();

	///Destructor for cleaning up allocated memory
	~CCommHistory();

	///Prints the command history
	///Allows you to specify the amount of records you want, defaults to everything
	void print(size_t recordAmount = 0);

	///Calls the command given by index, returns true if it succeeded
	bool call(size_t index) const;

	///Adds command to history
	void add(CCommand * toAdd);

	///Returns amount of records
	size_t getRecordAmount() const;

protected:

	///Vector of commands
	vector<CCommand*> m_history;
};

#endif /* __PROGTEST__ */