#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cmath>
#include <cassert>
#include <iostream> 
#include <iomanip> 
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */

struct SCompany {
  SCompany( void ) : sum(new unsigned int(0)), name(""), id("") {};
  unsigned int * sum;
  string name;
  string id;
};

struct lowerBoundCmpFunc
{
    bool operator ()(SCompany const& a, string const& b) const
    {
      return a.id.compare(b) < 0;
    }
};

struct lowerBoundCmpFuncName
{
    bool operator ()(SCompany const& a, string const& b) const
    {
      return a.name.compare(b) < 0;
    }
};

class CVATRegister
{
  public:
    CVATRegister( void ) {};
    ~CVATRegister( void ) {
       for (unsigned int j = 0; j < byName.size(); ++j) {
          delete byName[j].sum;
       }
    };

    bool NewCompany( const string & name, const string & addr, const string & taxID );
    bool CancelCompany( const string & name, const string & addr );
    bool CancelCompany( const string & taxID );
    bool Invoice( const string & taxID, unsigned int amount );
    bool Invoice( const string & name, const string & addr, unsigned int amount );
    bool Audit( const string & name, const string & addr, unsigned int & sumIncome ) const;
    bool Audit( const string & taxID, unsigned int & sumIncome ) const;
    unsigned int MedianInvoice( void ) const;
  private:
    vector<unsigned int> globalInvoice;
    vector<SCompany> byName;
    vector<SCompany> byId;
};

bool CVATRegister::NewCompany( const string & name, const string & addr, const string & taxID ) {
  string companyName = (name + "-|-" + addr);
  transform(companyName.begin(), companyName.end(), companyName.begin(), ::tolower);

  for (unsigned int i = 0; i < byName.size(); ++i) {
    //duplicita jmena
    if (byName[i].name.compare(companyName) == 0)
      return false;
    //duplicita ID
    if (byName[i].id.compare(taxID) == 0)
      return false;
  }

  SCompany tmp;
  tmp.name = companyName;
  tmp.id = taxID;

  unsigned int i = 0;
  for (i = 0; i < byId.size(); ++i) {

    if (byId[i].id.compare(taxID) > 0)
      break;
  }
  byId.insert(byId.begin() + i, tmp);

  i = 0;
  for (i = 0; i < byName.size(); ++i) {

    if (byName[i].name.compare(companyName) > 0)
      break;
  }
  byName.insert(byName.begin() + i, tmp);

  return true;
}

bool CVATRegister::CancelCompany( const string & name, const string & addr ) {
  string companyName = (name + "-|-" + addr);
  transform(companyName.begin(), companyName.end(), companyName.begin(), ::tolower);

  unsigned int j = 0;
  for (; j < byName.size(); ++j) {

    if (byName[j].name.compare(companyName) == 0)
      break;
  }

  if (j >= byName.size()) {
    //jmeno nenalezeno
    return false;
  }

  unsigned int i = 0;
  for (; i < byId.size(); ++i) {

    if (byId[i].id.compare(byName[j].id) == 0)
      break;
  }

  if (i >= byId.size()) {
    //taxID nenalezeno
    return false;
  }

  delete byId[i].sum;

  byId.erase(byId.begin() + i);
  byName.erase(byName.begin() + j);

  return true;
}

bool CVATRegister::CancelCompany( const string & taxID ) {

  unsigned int i = 0;
  for (; i < byId.size(); ++i) {

    if (byId[i].id.compare(taxID) == 0)
      break;
  }

  if (i >= byId.size()) {
    //taxID nenalezeno
    return false;
  }

  unsigned int j = 0;
  for (; j < byName.size(); ++j) {

    if (byName[j].name.compare(byId[i].name) == 0)
      break;
  }

  if (j >= byName.size()) {
    //jmeno nenalezeno
    return false;
  }

  delete byId[i].sum;

  byId.erase(byId.begin() + i);
  byName.erase(byName.begin() + j);

  return true;
}

bool CVATRegister::Invoice( const string & name, const string & addr, unsigned int amount ) {
  if (byName.size() == 0)
    return false;


  string companyName = (name + "-|-" + addr);
  transform(companyName.begin(), companyName.end(), companyName.begin(), ::tolower);

  auto company = lower_bound(byName.begin(), byName.end(), companyName, lowerBoundCmpFuncName());

  if (size_t(company - byName.begin()) >= byName.size() || company->name != companyName)
      return false;

  *(company->sum) += amount;
  globalInvoice.push_back(amount);

  return true;
}

bool CVATRegister::Invoice( const string & taxID, unsigned int amount ) {
  if (byId.size() == 0)
    return false;

  auto company = lower_bound(byId.begin(), byId.end(), taxID, lowerBoundCmpFunc());

  if (size_t(company - byId.begin()) >= byId.size() || company->id != taxID)
    return false;

  *(company->sum) += amount;
  globalInvoice.push_back(amount);

  return true;
}

bool CVATRegister::Audit( const string & name, const string & addr, unsigned int & sumIncome ) const {
  if (byName.size() == 0)
    return false;

  string companyName = (name + "-|-" + addr);
  transform(companyName.begin(), companyName.end(), companyName.begin(), ::tolower);

  auto company = lower_bound(byName.begin(), byName.end(), companyName, lowerBoundCmpFuncName());

  if (size_t(company - byName.begin()) >= byName.size() || company->name != companyName)
    return false;

  sumIncome = *(company->sum);

  return true;
}

bool CVATRegister::Audit( const string & taxID, unsigned int & sumIncome ) const {
  if (byId.size() == 0)
    return false;

  auto company = lower_bound(byId.begin(), byId.end(), taxID, lowerBoundCmpFunc());

  if (size_t(company - byId.begin()) >= byId.size() || company->id != taxID)
    return false;

  sumIncome = *(company->sum);

  return true;
}

unsigned int CVATRegister::MedianInvoice( void ) const {
  if (globalInvoice.size() == 0) {
    return 0;
  }

    vector<unsigned int> tmp;
    copy(globalInvoice.begin(), globalInvoice.end(), back_inserter(tmp));
    sort(tmp.begin(), tmp.end());

  return tmp[tmp.size() / 2];
}


#ifndef __PROGTEST__
int main ( void )
{

  return 0;
}
#endif /* __PROGTEST__ */