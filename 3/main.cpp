#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <sstream>
using namespace std;
#endif /* __PROGTEST__ */

using namespace std;

struct STransaction {
	STransaction() : m_recipientId(nullptr), m_key(nullptr), m_amount(0) {};
	~STransaction() {
		delete[] m_recipientId;
		delete[] m_key;
	};

	char * m_recipientId;
	char * m_key;
	int m_amount;
};

struct CAccount {
	CAccount( const char * accountId = nullptr, int balance = 0) : m_initialAmount(0), m_accountId(nullptr), m_transactionCnt(0), m_transAlloc(0), m_balance(0), m_transactions(nullptr) {};
	~CAccount() {
		if (m_transactionCnt > 0 && m_transactions != nullptr)
			delete[] m_transactions;
		delete[] m_accountId;
	};

	int Balance() {
		return m_balance;
	}

    friend ostream & operator<<( ostream & op, const CAccount & a) {
    	op << a.m_accountId << ':' << '\n' << "   " << a.m_initialAmount << '\n';
    	for (int i = 0; i < a.m_transactionCnt; ++i) {
    		if (a.m_transactions[i].m_amount < 0)
    			op << " - " << abs(a.m_transactions[i].m_amount) << ", to: " << a.m_transactions[i].m_recipientId << ", sign: " << a.m_transactions[i].m_key << '\n';
    		else
    			op << " + " << abs(a.m_transactions[i].m_amount) << ", from: " << a.m_transactions[i].m_recipientId << ", sign: " << a.m_transactions[i].m_key << '\n';
    	}
    	op << " = " << a.m_balance << endl;

    	return op;
    }

	int m_initialAmount;
  	char * m_accountId;
  	int m_transactionCnt;
  	int m_transAlloc;
  	int m_balance;
  	STransaction * m_transactions;

  	void addTransaction( const char * recipient, int amount, const char * key ) {
  		m_balance += amount;
		int recipLen = strlen(recipient);
		int keyLen = strlen(key);

		if (m_transactionCnt == 0) {
			m_transAlloc = 50;
			m_transactions = new STransaction[m_transAlloc];
			m_transactions[m_transactionCnt].m_amount = amount;
			m_transactions[m_transactionCnt].m_recipientId = new char[recipLen + 1];
			m_transactions[m_transactionCnt].m_key = new char[keyLen + 1];

			strcpy(m_transactions[m_transactionCnt].m_recipientId, recipient);
			strcpy(m_transactions[m_transactionCnt].m_key, key);

			m_transactions[m_transactionCnt].m_recipientId[recipLen] = '\0';
			m_transactions[m_transactionCnt++].m_key[keyLen] = '\0';
			return;
		}

		if (m_transactionCnt >= m_transAlloc) {
			m_transAlloc += 5;
			STransaction * tmp = new STransaction[m_transAlloc];
			for (int i = 0; i < m_transactionCnt; ++i) {
				tmp[i].m_amount = m_transactions[i].m_amount;
				int recipLen = strlen(m_transactions[i].m_recipientId);
				int keyLen = strlen(m_transactions[i].m_key);
				tmp[i].m_recipientId = new char[recipLen + 1];
				tmp[i].m_key = new char[keyLen + 1];
				strcpy(tmp[i].m_recipientId, m_transactions[i].m_recipientId);
				strcpy(tmp[i].m_key, m_transactions[i].m_key);
			}

			delete[] m_transactions;
			m_transactions = tmp;
		}

		m_transactions[m_transactionCnt].m_amount = amount;
		m_transactions[m_transactionCnt].m_recipientId = new char[recipLen + 1];
		m_transactions[m_transactionCnt].m_key = new char[keyLen + 1];

		strcpy(m_transactions[m_transactionCnt].m_recipientId, recipient);
		strcpy(m_transactions[m_transactionCnt].m_key, key);

		m_transactions[m_transactionCnt].m_recipientId[recipLen] = '\0';
		m_transactions[m_transactionCnt++].m_key[keyLen] = '\0';

  	};
};

class CBank
{
  public:
    // default constructor
    CBank( void ) : m_accountCnt(0), m_accountAllocated(0), m_accounts(nullptr), m_copyAmount(new int[1]) {
    	*m_copyAmount = 1;
    }

    // copy constructor
    CBank( const CBank &obj ) : m_accountCnt(obj.m_accountCnt), m_accountAllocated(obj.m_accountAllocated) {
    	m_copyAmount = obj.m_copyAmount;
    	m_accounts = obj.m_accounts;
    	++(*m_copyAmount);

    	/* DEEP COPY

    	m_accounts = new CAccount[m_accountAllocated];

		for (int i = 0; i < m_accountCnt; ++i) {
			m_accounts[i].m_balance = obj.m_accounts[i].m_balance;
			m_accounts[i].m_initialAmount = obj.m_accounts[i].m_initialAmount;
			m_accounts[i].m_transactionCnt = obj.m_accounts[i].m_transactionCnt;
			m_accounts[i].m_transAlloc = obj.m_accounts[i].m_transAlloc;

			int accLen = strlen(obj.m_accounts[i].m_accountId);
			m_accounts[i].m_accountId = new char[accLen + 1];
			strcpy(m_accounts[i].m_accountId, obj.m_accounts[i].m_accountId);
			m_accounts[i].m_transactions = new STransaction[m_accounts[i].m_transAlloc];

			for (int j = 0; j < obj.m_accounts[i].m_transactionCnt; ++j) {

				m_accounts[i].m_transactions[j].m_amount = obj.m_accounts[i].m_transactions[j].m_amount;

				int recLen = strlen(obj.m_accounts[i].m_transactions[j].m_recipientId);
				m_accounts[i].m_transactions[j].m_recipientId = new char[recLen + 1];
				strcpy(m_accounts[i].m_transactions[j].m_recipientId, obj.m_accounts[i].m_transactions[j].m_recipientId);

				recLen = strlen(obj.m_accounts[i].m_transactions[j].m_key);
				m_accounts[i].m_transactions[j].m_key = new char[recLen + 1];
				strcpy(m_accounts[i].m_transactions[j].m_key, obj.m_accounts[i].m_transactions[j].m_key);
			}
		}*/
    }

    // destructor
    ~CBank( void ) {
    	--(*m_copyAmount);

    	if (*m_copyAmount < 1) {
			delete[] m_accounts;
			delete[] m_copyAmount;
    	}
    }

    // operator =
  	CBank& operator=( const CBank &obj ) {
  		if (this == &obj)
  			return *this;

    	--(*m_copyAmount);

    	if (*m_copyAmount < 1) {
			delete[] m_accounts;
			delete[] m_copyAmount;
    	}

		m_accountCnt = obj.m_accountCnt;
		m_accountAllocated = obj.m_accountAllocated;
    	m_copyAmount = obj.m_copyAmount;
    	m_accounts = obj.m_accounts;
    	++(*m_copyAmount);

  		/* DEEP COPY

  		delete[] m_accounts;

		m_accountCnt = obj.m_accountCnt;
		m_accountAllocated = obj.m_accountAllocated;

    	m_accounts = new CAccount[m_accountAllocated];

		for (int i = 0; i < m_accountCnt; ++i) {
			m_accounts[i].m_balance = obj.m_accounts[i].m_balance;
			m_accounts[i].m_initialAmount = obj.m_accounts[i].m_initialAmount;
			m_accounts[i].m_transactionCnt = obj.m_accounts[i].m_transactionCnt;
			m_accounts[i].m_transAlloc = obj.m_accounts[i].m_transAlloc;

			int accLen = strlen(obj.m_accounts[i].m_accountId);
			m_accounts[i].m_accountId = new char[accLen + 1];
			strcpy(m_accounts[i].m_accountId, obj.m_accounts[i].m_accountId);
			m_accounts[i].m_transactions = new STransaction[m_accounts[i].m_transAlloc];

			for (int j = 0; j < obj.m_accounts[i].m_transactionCnt; ++j) {

				m_accounts[i].m_transactions[j].m_amount = obj.m_accounts[i].m_transactions[j].m_amount;

				int recLen = strlen(obj.m_accounts[i].m_transactions[j].m_recipientId);
				m_accounts[i].m_transactions[j].m_recipientId = new char[recLen + 1];
				strcpy(m_accounts[i].m_transactions[j].m_recipientId, obj.m_accounts[i].m_transactions[j].m_recipientId);

				recLen = strlen(obj.m_accounts[i].m_transactions[j].m_key);
				m_accounts[i].m_transactions[j].m_key = new char[recLen + 1];
				strcpy(m_accounts[i].m_transactions[j].m_key, obj.m_accounts[i].m_transactions[j].m_key);
			}
		}*/

		return *this;
  	}

  	void separate( void ) {

  		/*

			COPY ON WRITE, SEPARATES 2 INSTANCES FROM EACH OTHER

  		*/

  		if (*m_copyAmount == 1) {
  			return;
  		}

  		--(*m_copyAmount);
  		m_copyAmount = new int[1];
  		*m_copyAmount = 1;

  		CAccount * data = m_accounts;

  		m_accounts = new CAccount[m_accountAllocated];

		for (int i = 0; i < m_accountCnt; ++i) {
			m_accounts[i].m_balance = data[i].m_balance;
			m_accounts[i].m_initialAmount = data[i].m_initialAmount;
			m_accounts[i].m_transactionCnt = data[i].m_transactionCnt;
			m_accounts[i].m_transAlloc = data[i].m_transAlloc;

			int accLen = strlen(data[i].m_accountId);
			m_accounts[i].m_accountId = new char[accLen + 1];
			strcpy(m_accounts[i].m_accountId, data[i].m_accountId);
			m_accounts[i].m_transactions = new STransaction[m_accounts[i].m_transAlloc];

			for (int j = 0; j < data[i].m_transactionCnt; ++j) {

				m_accounts[i].m_transactions[j].m_amount = data[i].m_transactions[j].m_amount;

				int recLen = strlen(data[i].m_transactions[j].m_recipientId);
				m_accounts[i].m_transactions[j].m_recipientId = new char[recLen + 1];
				strcpy(m_accounts[i].m_transactions[j].m_recipientId, data[i].m_transactions[j].m_recipientId);

				recLen = strlen(data[i].m_transactions[j].m_key);
				m_accounts[i].m_transactions[j].m_key = new char[recLen + 1];
				strcpy(m_accounts[i].m_transactions[j].m_key, data[i].m_transactions[j].m_key);
			}
		}

  		data = nullptr;
  	}

	bool NewAccount ( const char * accID, int initialBalance ) {
		this->separate();

		int size = strlen(accID);

		if (m_accountCnt == 0) {
			m_accountAllocated = 50;
			m_accounts = new CAccount[m_accountAllocated];
			m_accounts[m_accountCnt].m_balance = initialBalance;
			m_accounts[m_accountCnt].m_initialAmount = initialBalance;
			m_accounts[m_accountCnt].m_accountId = new char[size + 1];
			for (int i = 0; i < size; ++i)
				m_accounts[m_accountCnt].m_accountId[i] = accID[i];
			m_accounts[m_accountCnt++].m_accountId[size] = '\0';
			return true;
		}

		for (int i = 0; i < m_accountCnt; ++i)
			if (strcmp(m_accounts[i].m_accountId, accID) == 0)
				return false;

		if (m_accountCnt >= m_accountAllocated) {
			m_accountAllocated += 5;
			CAccount * tmp = new CAccount[m_accountAllocated];
			for (int i = 0; i < m_accountCnt; ++i) {
				tmp[i].m_balance = m_accounts[i].m_balance;
				tmp[i].m_initialAmount = m_accounts[i].m_initialAmount;
				tmp[i].m_transactionCnt = m_accounts[i].m_transactionCnt;

				int accLen = strlen(m_accounts[i].m_accountId);
				tmp[i].m_accountId = new char[accLen + 1];
				strcpy(tmp[i].m_accountId, m_accounts[i].m_accountId);

				for (int j = 0; j < m_accounts[i].m_transactionCnt; ++j) {
					tmp[i].m_transactions[j].m_amount = m_accounts[i].m_transactions[j].m_amount;

					int recLen = strlen(m_accounts[i].m_transactions[j].m_recipientId);
					tmp[i].m_transactions[j].m_recipientId = new char[recLen + 1];
					strcpy(tmp[i].m_transactions[j].m_recipientId, m_accounts[i].m_transactions[j].m_recipientId);

					recLen = strlen(m_accounts[i].m_transactions[j].m_key);
					tmp[i].m_transactions[j].m_key = new char[recLen + 1];
					strcpy(tmp[i].m_transactions[j].m_key, m_accounts[i].m_transactions[j].m_key);
				}
			}

			delete[] m_accounts;
			m_accounts = tmp;
		}

		m_accounts[m_accountCnt].m_balance = initialBalance;
		m_accounts[m_accountCnt].m_initialAmount = initialBalance;
		m_accounts[m_accountCnt].m_accountId = new char[size + 1];
		for (int i = 0; i < size; ++i)
			m_accounts[m_accountCnt].m_accountId[i] = accID[i];
		m_accounts[m_accountCnt++].m_accountId[size] = '\0';

		return true;
	}

	bool Transaction ( const char * debAccID, const char * credAccID, int amount, const char * signature ) {
		this->separate();

		if (strcmp(debAccID, credAccID) == 0 || amount < 0)
			return false;

		int i = 0;
		for ( ; i < m_accountCnt; ++i) {
			if (strcmp(m_accounts[i].m_accountId, debAccID) == 0)
				break;
		}

		if (i >= m_accountCnt)
			return false;

		int j = 0;
		for ( ; j < m_accountCnt; ++j) {
			if (strcmp(m_accounts[j].m_accountId, credAccID) == 0)
				break;
		}

		if (j >= m_accountCnt)
			return false;

		m_accounts[i].addTransaction(credAccID, -1 * amount, signature);
		m_accounts[j].addTransaction(debAccID, amount, signature);

		return true;
	}

	bool TrimAccount ( const char * accID ) {
		this->separate();

		int i = 0;
		for ( ; i < m_accountCnt; ++i) {
			if (strcmp(m_accounts[i].m_accountId, accID) == 0)
				break;
		}

		if (i >= m_accountCnt)
			return false;

		m_accounts[i].m_initialAmount = m_accounts[i].m_balance;
		delete[] m_accounts[i].m_transactions;
		m_accounts[i].m_transactionCnt = 0;
		m_accounts[i].m_transAlloc = 0;
		m_accounts[i].m_transactions = nullptr;

		return true;
	}

    // Account ( accID )
    CAccount & Account( const char * accID ) {
    	int i = 0;
		for ( ; i < m_accountCnt; ++i) {
			if (strcmp(m_accounts[i].m_accountId, accID) == 0)
				break;
		}

		if (i >= m_accountCnt)
			throw "arbitaryException";

		return m_accounts[i];
    }
	
  	int m_accountCnt;
  	int m_accountAllocated;
  	CAccount * m_accounts;
  private:
  	int * m_copyAmount;
};

#ifndef __PROGTEST__
int main ( void )
{

  return 0;
}
#endif /* __PROGTEST__ */
