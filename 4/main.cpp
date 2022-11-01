#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <deque>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <memory>
using namespace std;

class CDate
{
  public:
    //---------------------------------------------------------------------------------------------
    CDate( int y, int m, int d ) : m_Year(y), m_Month(m), m_Day(d) {}
    //---------------------------------------------------------------------------------------------
    int Compare( const CDate & x ) const
    {
      if ( m_Year != x.m_Year )
        return m_Year - x.m_Year;

      if ( m_Month != x.m_Month )
        return m_Month - x.m_Month;

      return m_Day - x.m_Day;
    }
    //---------------------------------------------------------------------------------------------
    int Year( void ) const 
    {
      return m_Year;
    }
    //---------------------------------------------------------------------------------------------
    int Month( void ) const 
    {
      return m_Month;
    }
    //---------------------------------------------------------------------------------------------
    int Day( void ) const 
    {
      return m_Day;
    }
    //---------------------------------------------------------------------------------------------
    friend ostream & operator << ( ostream & os, const CDate & x )
    {
      char oldFill = os.fill();
      return os << setfill('0') << setw(4) << x.m_Year << "-" << setw(2) << (int)x.m_Month << "-" << setw(2) << (int)x.m_Day << setfill(oldFill);
    }
    //---------------------------------------------------------------------------------------------
  private:
    int16_t                  m_Year;
    int8_t                   m_Month;
    int8_t                   m_Day;
};
#endif /* __PROGTEST__ */

using namespace std;

class CInvoice;

class CCompany {
  public:
    CCompany( string official, string normalized = "" ) : m_official(official), m_normalized(normalized) { /*m_ptrInvc.resize(20);*/ }

    string m_official;
    string m_normalized;

    mutable set<const CInvoice*> m_invoicePtr;
    //mutable vector<const CInvoice *> m_ptrInvc;
};

void removeRedundantWS( string & companyName ) {

      for (size_t i = 0; i < companyName.size(); ++i) {
        if (companyName[i] != ' ')
          continue;

        if (i == 0 && companyName[i+1] != ' ') {
          companyName.erase(companyName.begin() + i);
          --i;
        } else if (i == companyName.size() - 1 && companyName[i-1] != ' ') {
          companyName.erase(companyName.begin() + i);
          --i;
        } else if ( (i != 0 && companyName[i-1] == ' ') || (i != companyName.size() - 1 && companyName[i+1] == ' ')) {
          companyName.erase(companyName.begin() + i);
          --i;
        }
      }

}

class CInvoice
{
  public:

    CInvoice( const CDate & date, const string & seller, const string & buyer, unsigned int amount, double VAT, bool isIssued = false, bool isAccepted = false, size_t issuedID = 0, size_t acceptedID = 0, const string & sellers = "", const string & buyers = "" ) : 
              m_isIssued(isIssued), m_isAccepted(isAccepted), m_issuedID(issuedID), m_acceptedID(acceptedID), m_buyer(CCompany(buyer, buyers)),
              m_seller(CCompany(seller, sellers)), m_date(date), m_amount(amount), m_vat(VAT) {}

    CDate Date( void ) const { return m_date; }
    string Buyer( void ) const { return m_buyer.m_official; }
    string Seller( void ) const { return m_seller.m_official; }
    int Amount( void ) const { return (int)m_amount; }
    double VAT( void ) const { return m_vat; }

    mutable bool m_isIssued;
    mutable bool m_isAccepted;

    mutable size_t m_issuedID;
    mutable size_t m_acceptedID;

    CCompany m_buyer;
    CCompany m_seller;

    CDate m_date;
    unsigned int m_amount;
    double m_vat;

};

class CSortOpt
{
  public:
    bool operator() (const CInvoice & lhs, const CInvoice & rhs) const {

        int tmp;

        for (auto val = m_sortOrder.begin(); val != m_sortOrder.end(); ++val) {
          switch (val->first) {

          case BY_DATE:

            if ( (tmp = lhs.Date().Compare(rhs.Date())) == 0 )
              break;

            if (val->second) {
              return tmp < 0;
            } else {
              return tmp > 0;
            }
          break;

          case BY_BUYER:

            if ( (tmp = lhs.m_buyer.m_normalized.compare(rhs.m_buyer.m_normalized)) == 0 )
              break;

            if (val->second) {
              return tmp < 0;
            } else {
              return tmp > 0;
            }
          break;

          case BY_SELLER:

            if ( (tmp = lhs.m_seller.m_normalized.compare(rhs.m_seller.m_normalized)) == 0 )
              break;

            if (val->second) {
              return tmp < 0;
            } else {
              return tmp > 0;
            }
          break;

          case BY_AMOUNT:
            if ( lhs.Amount() == rhs.Amount() )
              break;

            if (val->second) {
              return lhs.Amount() < rhs.Amount();
            } else {
              return lhs.Amount() > rhs.Amount();
            }
          break;

          case BY_VAT:
            if ( lhs.VAT() == rhs.VAT() )
              break;

            if (val->second) {
              return lhs.VAT() < rhs.VAT();
            } else {
              return lhs.VAT() > rhs.VAT();
            }
          break;

          }
      }

        int lhsTmp = lhs.m_issuedID > lhs.m_acceptedID ? lhs.m_issuedID : lhs.m_acceptedID;
        int rhsTmp = rhs.m_issuedID > rhs.m_acceptedID ? rhs.m_issuedID : rhs.m_acceptedID;

        return lhsTmp < rhsTmp;
    }

    static const int BY_DATE = 0;
    static const int BY_BUYER = 1;
    static const int BY_SELLER = 2;
    static const int BY_AMOUNT = 3;
    static const int BY_VAT = 4;

    CSortOpt( void ) {}

    CSortOpt & AddKey( int key, bool ascending = true ) {
      m_sortOrder.push_back(make_pair(key, ascending));

      return *this;
    }

  private:
    vector<pair<int, bool>> m_sortOrder;
};


struct companyCompare {
    bool operator() (const CCompany & lhs, const CCompany & rhs) const {
        return lhs.m_normalized < rhs.m_normalized;
    }
};

struct invoiceCompare {

    bool operator() (const CInvoice & lhs, const CInvoice & rhs) const {
        int tmp;

        if ( lhs.Amount() != rhs.Amount() )
          return lhs.Amount() < rhs.Amount();

        if ( (tmp = lhs.Date().Compare(rhs.Date())) != 0 )
          return tmp < 0;

        if ( (tmp = lhs.m_seller.m_normalized.compare(rhs.m_seller.m_normalized)) != 0 )
          return tmp < 0;
        
        if ( (tmp = lhs.m_buyer.m_normalized.compare(rhs.m_buyer.m_normalized)) != 0 )
          return tmp < 0;

        return lhs.VAT() < rhs.VAT();
    }
};

class CVATRegister
{
  public:
    
    CVATRegister( void ) : ID(1) {};

    bool RegisterCompany( const string & name ) {
      string companyName;

      companyName.resize(name.size());
      transform(name.begin(), name.end(), companyName.begin(), ::tolower);
      removeRedundantWS(companyName);

      if (!m_companies.insert(CCompany(name, companyName)).second) {
        return false;
      }

      return true;
    }

    bool AddIssued( const CInvoice & x ) {
      string buyerNormalized;
      string sellerNormalized;

      buyerNormalized.resize(x.m_buyer.m_official.size());
      sellerNormalized.resize(x.m_seller.m_official.size());      

      transform(x.m_buyer.m_official.begin(), x.m_buyer.m_official.end(), buyerNormalized.begin(), ::tolower);
      transform(x.m_seller.m_official.begin(), x.m_seller.m_official.end(), sellerNormalized.begin(), ::tolower);

      removeRedundantWS(buyerNormalized);
      removeRedundantWS(sellerNormalized);

      if (buyerNormalized == sellerNormalized)
        return false;

      auto buyerIterator = m_companies.find(CCompany("", buyerNormalized));
      auto sellerIterator = m_companies.find(CCompany("", sellerNormalized));

      if (buyerIterator == m_companies.end() || sellerIterator == m_companies.end())
        return false;

      auto invoicePair = m_invoices.emplace(x.m_date, sellerIterator->m_official, buyerIterator->m_official, x.m_amount, x.m_vat, true, x.m_isAccepted, ID, x.m_acceptedID, sellerIterator->m_normalized, buyerIterator->m_normalized);

      if (!invoicePair.second) {
        if (invoicePair.first->m_isIssued) {
          return false;
        } else {
          invoicePair.first->m_isIssued = true;
          invoicePair.first->m_issuedID = ID;
        }
      } else {        
        buyerIterator->m_invoicePtr.insert(&*invoicePair.first);
        sellerIterator->m_invoicePtr.insert(&*invoicePair.first);
      }

      ++ID;
      return true;
    }

    bool AddAccepted( const CInvoice & x ) {
      string buyerNormalized;
      string sellerNormalized;

      buyerNormalized.resize(x.m_buyer.m_official.size());
      sellerNormalized.resize(x.m_seller.m_official.size()); 

      transform(x.m_buyer.m_official.begin(), x.m_buyer.m_official.end(), buyerNormalized.begin(), ::tolower);
      transform(x.m_seller.m_official.begin(), x.m_seller.m_official.end(), sellerNormalized.begin(), ::tolower);

      removeRedundantWS(buyerNormalized);
      removeRedundantWS(sellerNormalized);

      if (buyerNormalized == sellerNormalized)
        return false;

      auto buyerIterator = m_companies.find(CCompany("", buyerNormalized));
      auto sellerIterator = m_companies.find(CCompany("", sellerNormalized));

      if (buyerIterator == m_companies.end() || sellerIterator == m_companies.end())
        return false;

      auto invoicePair = m_invoices.emplace(x.m_date, sellerIterator->m_official, buyerIterator->m_official, x.m_amount, x.m_vat, x.m_isIssued, true, x.m_issuedID, ID, sellerIterator->m_normalized, buyerIterator->m_normalized);


      if (!invoicePair.second) {
        if (invoicePair.first->m_isAccepted) {
          return false;
        } else {
          invoicePair.first->m_isAccepted = true;
          invoicePair.first->m_acceptedID = ID;
        }
      } else {
        buyerIterator->m_invoicePtr.insert(&*invoicePair.first);
        sellerIterator->m_invoicePtr.insert(&*invoicePair.first);
      }

      ++ID;
      return true;  
    }

    bool DelIssued( const CInvoice & x ) {
      string buyerNormalized;
      string sellerNormalized;

      buyerNormalized.resize(x.m_buyer.m_official.size());
      sellerNormalized.resize(x.m_seller.m_official.size()); 

      transform(x.m_buyer.m_official.begin(), x.m_buyer.m_official.end(), buyerNormalized.begin(), ::tolower);
      transform(x.m_seller.m_official.begin(), x.m_seller.m_official.end(), sellerNormalized.begin(), ::tolower);

      removeRedundantWS(buyerNormalized);
      removeRedundantWS(sellerNormalized);

      if (buyerNormalized == sellerNormalized)
        return false;

      auto buyerIterator = m_companies.find(CCompany("", buyerNormalized));
      auto sellerIterator = m_companies.find(CCompany("", sellerNormalized));

      if (buyerIterator == m_companies.end() || sellerIterator == m_companies.end())
        return false;

      CInvoice tmp( x.m_date, "", "", x.m_amount, x.m_vat );
      tmp.m_buyer.m_normalized = buyerIterator->m_normalized;
      tmp.m_seller.m_normalized = sellerIterator->m_normalized;

      auto invoiceIterator = m_invoices.find(tmp);

      if (invoiceIterator == m_invoices.end()) {
        return false;
      } else {
        if (invoiceIterator->m_isAccepted && invoiceIterator->m_isIssued) {
          invoiceIterator->m_isIssued = false;
          invoiceIterator->m_issuedID = 0;
        } else if (invoiceIterator->m_isAccepted) {
          return false;
        } else if (invoiceIterator->m_isIssued) {
          buyerIterator->m_invoicePtr.erase(&*invoiceIterator);
          sellerIterator->m_invoicePtr.erase(&*invoiceIterator);
          m_invoices.erase(invoiceIterator);
        } else {
          buyerIterator->m_invoicePtr.erase(&*invoiceIterator);
          sellerIterator->m_invoicePtr.erase(&*invoiceIterator);
          m_invoices.erase(invoiceIterator);          
        }
      }

      return true;
    }

    bool DelAccepted( const CInvoice & x ) {
      string buyerNormalized;
      string sellerNormalized;

      buyerNormalized.resize(x.m_buyer.m_official.size());
      sellerNormalized.resize(x.m_seller.m_official.size()); 

      transform(x.m_buyer.m_official.begin(), x.m_buyer.m_official.end(), buyerNormalized.begin(), ::tolower);
      transform(x.m_seller.m_official.begin(), x.m_seller.m_official.end(), sellerNormalized.begin(), ::tolower);

      removeRedundantWS(buyerNormalized);
      removeRedundantWS(sellerNormalized);

      if (buyerNormalized == sellerNormalized) {
        cout << "0" << endl;
        return false;
      }

      auto buyerIterator = m_companies.find(CCompany("", buyerNormalized));
      auto sellerIterator = m_companies.find(CCompany("", sellerNormalized));

      if (buyerIterator == m_companies.end() || sellerIterator == m_companies.end()) {
        cout << "1" << endl;
        return false;
      }

      CInvoice tmp( x.m_date, "", "", x.m_amount, x.m_vat );
      tmp.m_buyer.m_normalized = buyerIterator->m_normalized;
      tmp.m_seller.m_normalized = sellerIterator->m_normalized;

      auto invoiceIterator = m_invoices.find(tmp);

      if (invoiceIterator == m_invoices.end()) {
        cout << "3" << endl;
        return false;
      } else {
        if (invoiceIterator->m_isAccepted && invoiceIterator->m_isIssued) {
          invoiceIterator->m_isAccepted = false;
          invoiceIterator->m_acceptedID = 0;
        } else if (invoiceIterator->m_isIssued) {
        cout << "2" << endl;
          return false;
        } else if (invoiceIterator->m_isAccepted) {
          buyerIterator->m_invoicePtr.erase(&*invoiceIterator);
          sellerIterator->m_invoicePtr.erase(&*invoiceIterator);
          m_invoices.erase(invoiceIterator);
        } else {
          buyerIterator->m_invoicePtr.erase(&*invoiceIterator);
          sellerIterator->m_invoicePtr.erase(&*invoiceIterator);
          m_invoices.erase(invoiceIterator);          
        }
      }

      return true;
    }

    list<CInvoice> Unmatched( const string & company, const CSortOpt & sortBy ) const {
      string companyName;

      companyName.resize(company.size());
      transform(company.begin(), company.end(), companyName.begin(), ::tolower);
      removeRedundantWS(companyName);

      list<CInvoice> tmp;

      auto companyIt = m_companies.find(CCompany("", companyName));

      if (companyIt == m_companies.end()) {
        
        //cout << "-----------------------------------" << endl;
        return tmp;
      }

      for (auto value = companyIt->m_invoicePtr.begin(); value != companyIt->m_invoicePtr.end(); ++value) {
        auto va = *value;

        if ((!va->m_isIssued || !va->m_isAccepted)) {
          CInvoice toLower = *va;
          toLower.m_buyer.m_normalized.resize(toLower.m_buyer.m_official.size());
          transform(toLower.m_buyer.m_official.begin(), toLower.m_buyer.m_official.end(), toLower.m_buyer.m_normalized.begin(), ::tolower);
          toLower.m_seller.m_normalized.resize(toLower.m_seller.m_official.size());
          transform(toLower.m_seller.m_official.begin(), toLower.m_seller.m_official.end(), toLower.m_seller.m_normalized.begin(), ::tolower);

          tmp.push_back(toLower);       
        }
      }

      tmp.sort(sortBy);

      //for (auto & val : tmp)
      //  cout << val.Date() << " - " << val.Seller() << " - " << val.Buyer() << " - " << val.Amount() << " - " << val.VAT() << endl;
      //cout << "-----------------------------------" << endl;

      return tmp;
    }

    size_t ID;

  private:

    set<CCompany, companyCompare> m_companies;
    set<CInvoice, invoiceCompare> m_invoices;

};


#ifndef __PROGTEST__
bool equalLists ( const list<CInvoice> & a, const list<CInvoice> & b )
{
      return true;
}

int main ( void )
{

  CVATRegister r;
  assert ( r . RegisterCompany ( "first Company" ) );
  assert ( r . RegisterCompany ( "Second     Company" ) );

  assert ( r . AddAccepted ( CInvoice ( CDate ( 2000, 1, 1 ), "First Company", "Second Company", 100, 20 ) ) );
  assert ( r . AddIssued ( CInvoice ( CDate ( 2000, 1, 1 ), "First Company", "Second Company", 100, 20 ) ) );
  assert ( r . DelAccepted ( CInvoice ( CDate ( 2000, 1, 1 ), "First Company", "Second Company", 100, 20 ) ) );
  assert ( r . DelIssued ( CInvoice ( CDate ( 2000, 1, 1 ), "First Company", "Second Company", 100, 20 ) ) );

  assert ( r . AddIssued ( CInvoice ( CDate ( 2000, 1, 1 ), "First Company", "Second Company", 100, 20 ) ) );

  assert ( r . DelIssued ( CInvoice ( CDate ( 2000, 1, 1 ), "First Company", "Second Company", 100, 20 ) ) );

  assert ( r . AddIssued ( CInvoice ( CDate ( 2000, 1, 1 ), "First Company", "Second Company", 100, 20 ) ) );

  assert ( r . DelIssued ( CInvoice ( CDate ( 2000, 1, 1 ), "First Company", "Second Company", 100, 20 ) ) );

  assert ( r . AddAccepted ( CInvoice ( CDate ( 2000, 1, 1 ), "First Company", "Second Company", 100, 20 ) ) );

  assert ( r . DelAccepted ( CInvoice ( CDate ( 2000, 1, 1 ), "First Company", "Second Company", 100, 20 ) ) );

  return 0;
}
#endif /* __PROGTEST__ */