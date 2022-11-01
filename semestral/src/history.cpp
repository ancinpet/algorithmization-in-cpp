#ifndef __PROGTEST__
#include "history.hpp"


//CCommHistory
CCommHistory::CCommHistory() {}

CCommHistory::~CCommHistory() {
	//Free all commands
	for (auto it = m_history.begin(); it != m_history.end(); ++it) {
		delete *it;
	}
}

void CCommHistory::print(size_t recordAmount) {
	//Check if history isn't empty
	if (m_history.empty()) {
		cout << "No commands logged yet." << endl;
		return;
	}

	//If we want more records than there already are just clamp to all
	if (recordAmount > m_history.size()) {
		recordAmount = m_history.size();
	}

	//Show entire history
	if (recordAmount == 0 || recordAmount == m_history.size()) {
		cout << "Showing all " << m_history.size() << " commands.";
		//Not writing newline after the initial text so we can put newlines at the beginning and flush at the end
		for (auto it = m_history.begin(); it != m_history.end(); ++it) {
			cout << "\n[" << it - m_history.begin() << "]: " << (*it)->getName() << " " << (*it)->getPath();
		}
		cout << endl;
	//Show only requested amount of records
	} else {
		cout << "Showing last " << recordAmount << " commands.";
		//Not writing newline after the initial text so we can put newlines at the beginning and flush at the end
		for (auto it = m_history.begin() + m_history.size() - recordAmount; it != m_history.end(); ++it) {
			cout << "\n[" << it - m_history.begin() << "]: " << (*it)->getName() << " " << (*it)->getPath();
		}
		cout << endl;
	}
}

bool CCommHistory::call(size_t index) const {
	//If index is out of bounds, return false
	if (m_history.size() <= index) {
		cout << "Index is out of bounds." << endl;
		return false;
	}

	//If command fails, path is not valid anymore
	if (!fs::exists(m_history[index]->getPath())) {
		cout << "Path is no longer valid." << endl;
		return false;
	}

	return m_history[index]->execute();
}

void CCommHistory::add(CCommand * toAdd) {
	m_history.push_back(toAdd);
}

size_t CCommHistory::getRecordAmount() const {
	return m_history.size();
}

#endif /* __PROGTEST__ */