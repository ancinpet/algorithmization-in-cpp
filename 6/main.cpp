#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cassert>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <algorithm>
using namespace std;
#endif /* __PROGTEST_ */

#define SUITCASE_WEIGHT 2000
#define BACKPACK_WEIGHT 1000
#define KNIFE_WEIGHT 100
#define CLOTHES_WEIGHT 500
#define SHOES_WEIGHT 750
#define MOBILE_WEIGHT 150

class CThing {
public:
  CThing( int weight ) : m_weight(weight) {}
  virtual ~CThing() {}

  virtual void print( ostream & os ) const = 0;

  virtual bool isDangerous() const {
  	return false;
  }

  int m_weight;
};

class CKnife : public CThing {
public:
  CKnife( int bladeLength ) : CThing(KNIFE_WEIGHT), m_bladeLenght(bladeLength) {}
  virtual ~CKnife() {}

  virtual void print( ostream & os ) const override {
  	os << "Knife, blade: " << m_bladeLenght << " cm\n";
  }

  virtual bool isDangerous() const override {
  	return m_bladeLenght > 7;
  }

  bool isEqual( const CKnife & x ) const {
  	return m_bladeLenght == x.m_bladeLenght;
  }

  int m_bladeLenght;
};

class CClothes : public CThing {
public:
  CClothes( string desc ) : CThing(CLOTHES_WEIGHT), m_desc(desc) {}
  virtual ~CClothes() {}

  virtual void print( ostream & os ) const override {
  	os << "Clothes (" << m_desc << ")\n";
  }

  bool isEqual( const CClothes & x ) const {
  	return m_desc == x.m_desc;
  }

  string m_desc;
};

class CShoes : public CThing {
public:
  CShoes() : CThing(SHOES_WEIGHT) {}
  virtual ~CShoes() {}

  virtual void print( ostream & os ) const override {
  	os << "Shoes\n";
  }

};

class CMobile : public CThing {
public:
  CMobile( string manufacturer, string model ) : CThing(MOBILE_WEIGHT), m_type(make_pair(manufacturer, model)) {}
  virtual ~CMobile() {}

  virtual void print( ostream & os ) const override {
  	os << "Mobile " << m_type.second << " by: " << m_type.first << "\n";
  }

  virtual bool isDangerous() const override {
  	return m_type.second == "Galaxy Note S7" && m_type.first == "Samsung"; 
  }
  
  bool isEqual( const CMobile & x ) const {
  	return m_type.second == x.m_type.second && m_type.first == x.m_type.first;
  }

  pair<string, string> m_type;
};
  
class CStorage {
public:
  CStorage() : m_weight(0) {}

  CStorage( const CStorage & x ) {
  	if ( this == &x )
  		return;

  	for ( auto res : x.m_items ) {
  		switch( res->m_weight ) {
  			case KNIFE_WEIGHT:
  				m_items.push_back( new CKnife(*dynamic_cast<CKnife*>(res)) );
  				m_weight += KNIFE_WEIGHT;

  			break;

  			case CLOTHES_WEIGHT:
  				m_items.push_back( new CClothes(*dynamic_cast<CClothes*>(res)) );
  				m_weight += CLOTHES_WEIGHT;

  			break;

  			case SHOES_WEIGHT:
  				m_items.push_back( new CShoes(*dynamic_cast<CShoes*>(res)) );
  				m_weight += SHOES_WEIGHT;

  			break;

  			case MOBILE_WEIGHT:
  				m_items.push_back( new CMobile(*dynamic_cast<CMobile*>(res)) );
  				m_weight += MOBILE_WEIGHT;

  			break;
  		}
  	}

  }

  ~CStorage() {
  	for ( auto res : m_items )
  		delete res;
  }

  CStorage & operator=( const CStorage & x ) {
  	if ( this == &x )
  		return *this;

  	for ( auto res : m_items )
  		delete res;
  	m_items.clear();

  	for ( auto res : x.m_items ) {
  		switch( res->m_weight ) {
  			case KNIFE_WEIGHT:
  				m_items.push_back( new CKnife(*dynamic_cast<CKnife*>(res)) );
  				m_weight += KNIFE_WEIGHT;

  			break;

  			case CLOTHES_WEIGHT:
  				m_items.push_back( new CClothes(*dynamic_cast<CClothes*>(res)) );
  				m_weight += CLOTHES_WEIGHT;

  			break;

  			case SHOES_WEIGHT:
  				m_items.push_back( new CShoes(*dynamic_cast<CShoes*>(res)) );
  				m_weight += SHOES_WEIGHT;

  			break;

  			case MOBILE_WEIGHT:
  				m_items.push_back( new CMobile(*dynamic_cast<CMobile*>(res)) );
  				m_weight += MOBILE_WEIGHT;

  			break;
  		}
  	}

  	this->m_weight = x.m_weight;

  	return *this;
  }

  template <typename _T>
  CStorage & Add( const _T & item ) {
  	m_items.push_back( new _T(item) );
  	m_weight += item.m_weight;

  	return *this;
  }

  //couldn't figure out proper solution without dynamic cast, CThing cannot have compare method because it doesn't hold the information
  bool IdenticalContents( const CStorage & x ) const {
  	if ( m_weight != x.m_weight )
  		return false;

  	vector<CThing*> a(m_items);
  	vector<CThing*> b(x.m_items);

  	for ( auto x = a.begin(); x != a.end(); ++x ) {
  		bool keepLooping = true;

  		for ( auto y = b.begin(); y != b.end() && keepLooping; ++y ) {
  			if ( (*x)->m_weight == (*y)->m_weight ) {
  				switch( (*x)->m_weight ) {
  					case KNIFE_WEIGHT:
  						if (dynamic_cast<CKnife*>(*x)->isEqual(*(dynamic_cast<CKnife*>(*y)))) {
  							a.erase(x);
  							--x;
  							b.erase(y);
  							--y;
  							keepLooping = false;
  						}

  						break;

  					case CLOTHES_WEIGHT:
						if (dynamic_cast<CClothes*>(*x)->isEqual(*(dynamic_cast<CClothes*>(*y)))) {
  							a.erase(x);
  							--x;
  							b.erase(y);
  							--y;
  							keepLooping = false;
  						}

  						break;

  					case SHOES_WEIGHT:
  						a.erase(x);
  						--x;
  						b.erase(y);
  						--y;
  						keepLooping = false;

  						break;

  					case MOBILE_WEIGHT:
  						if (dynamic_cast<CMobile*>(*x)->isEqual(*(dynamic_cast<CMobile*>(*y)))) {
  							a.erase(x);
  							--x;
  							b.erase(y);
  							--y;
  							keepLooping = false;
  						}

  						break;
  				}
  			}
  		}
  	}

  	return a.size() == 0 && b.size() == 0;
  }

  int m_weight;
  vector<CThing*> m_items;
};

class CSuitcase : public CStorage {
public:
  CSuitcase( int w, int h, int d ) : m_width(w), m_height(h), m_depth(d) {}

  int Weight() const {
  	return m_weight + SUITCASE_WEIGHT;
  }

  int Count() const {
  	return int(m_items.size());
  }

  bool Danger() const {
  	for( auto res : m_items ) {
  		if (res->isDangerous())
  			return true; 	
  	}

  	return false;
  }

  friend ostream& operator<<( ostream & os, const CSuitcase & x ) {
  	x.print( os );
  	return os;
  }

  void print( ostream & os ) const {
  	os << "Suitcase " << m_width << "x" << m_height << "x" << m_depth << "\n";

  	for ( auto i = m_items.begin(); i != m_items.end(); ++i ) {
  		if ( i - m_items.begin() == int(m_items.size()) - 1 ) {
  			os << "\\-";
  		} else {
  			os << "+-";
  		}

  		(*i)->print( os );
  	}

  }

  int m_width;
  int m_height;
  int m_depth;
};

class CBackpack : public CStorage {
public:
  CBackpack() {}

  int Weight() const {
  	return m_weight + BACKPACK_WEIGHT;
  }

  int Count() const {
  	return int(m_items.size());
  }

  bool Danger() const {
  	for( auto res : m_items ) {
  		if (res->isDangerous())
  			return true; 	
  	}


  	return false;
  }

  friend ostream& operator<<( ostream & os, const CBackpack & x ) {
  	x.print( os );
  	return os;
  }

  void print( ostream & os ) const {
  	os << "Backpack\n";

  	for ( auto i = m_items.begin(); i != m_items.end(); ++i ) {
  		if ( i - m_items.begin() == int(m_items.size()) - 1 ) {
  			os << "\\-";
  		} else {
  			os << "+-";
  		}

  		(*i)->print( os );
  	}

  }

};

#ifndef __PROGTEST__
int main ( void ) {
  CSuitcase x( 1, 2, 3 );
  CBackpack y;
  ostringstream os;
  
  x . Add ( CKnife ( 7 ) );
  x . Add ( CClothes ( "red T-shirt" ) );
  x . Add ( CClothes ( "black hat" ) );
  x . Add ( CShoes () );
  x . Add ( CClothes ( "green pants" ) );
  x . Add ( CClothes ( "blue jeans" ) );
  x . Add ( CMobile ( "Samsung", "J3" ) );
  x . Add ( CMobile ( "Tamtung", "Galaxy Note S7" ) );

  
  os . str ( "" );
  os << x;
  assert ( os . str () == "Suitcase 1x2x3\n"
           "+-Knife, blade: 7 cm\n"
           "+-Clothes (red T-shirt)\n"
           "+-Clothes (black hat)\n"
           "+-Shoes\n"
           "+-Clothes (green pants)\n"
           "+-Clothes (blue jeans)\n"
           "+-Mobile J3 by: Samsung\n"
           "\\-Mobile Galaxy Note S7 by: Tamtung\n" );
  
  assert ( x . Count () == 8 );
  assert ( x . Weight () == 5150 );
  assert ( !x . Danger () );
  x . Add ( CKnife ( 8 ) );
  os . str ( "" );
  os << x;
  assert ( os . str () == "Suitcase 1x2x3\n"
           "+-Knife, blade: 7 cm\n"
           "+-Clothes (red T-shirt)\n"
           "+-Clothes (black hat)\n"
           "+-Shoes\n"
           "+-Clothes (green pants)\n"
           "+-Clothes (blue jeans)\n"
           "+-Mobile J3 by: Samsung\n"
           "+-Mobile Galaxy Note S7 by: Tamtung\n"
           "\\-Knife, blade: 8 cm\n" );
  assert ( x . Count () == 9 );
  assert ( x . Weight () == 5250 );
  assert ( x . Danger () );
  y . Add ( CKnife ( 7 ) )
    . Add ( CClothes ( "red T-shirt" ) )
    . Add ( CShoes () );
  y . Add ( CMobile ( "Samsung", "Galaxy Note S7" ) );
  y . Add ( CShoes () );
  y . Add ( CClothes ( "blue jeans" ) );
  y . Add ( CClothes ( "black hat" ) );
  y . Add ( CClothes ( "green pants" ) );
  os . str ( "" );
  os << y;
  assert ( os . str () == "Backpack\n"
           "+-Knife, blade: 7 cm\n"
           "+-Clothes (red T-shirt)\n"
           "+-Shoes\n"
           "+-Mobile Galaxy Note S7 by: Samsung\n"
           "+-Shoes\n"
           "+-Clothes (blue jeans)\n"
           "+-Clothes (black hat)\n"
           "\\-Clothes (green pants)\n" );
  assert ( y . Count () == 8 );
  assert ( y . Weight () == 4750 );
  assert ( y . Danger () );
  y . Add ( CMobile ( "Samsung", "J3" ) );
  y . Add ( CMobile ( "Tamtung", "Galaxy Note S7" ) );
  y . Add ( CKnife ( 8 ) );
  os . str ( "" );
  os << y;
  assert ( os . str () == "Backpack\n"
           "+-Knife, blade: 7 cm\n"
           "+-Clothes (red T-shirt)\n"
           "+-Shoes\n"
           "+-Mobile Galaxy Note S7 by: Samsung\n"
           "+-Shoes\n"
           "+-Clothes (blue jeans)\n"
           "+-Clothes (black hat)\n"
           "+-Clothes (green pants)\n"
           "+-Mobile J3 by: Samsung\n"
           "+-Mobile Galaxy Note S7 by: Tamtung\n"
           "\\-Knife, blade: 8 cm\n" );
  assert ( y . Count () == 11 );
  assert ( y . Weight () == 5150 );
  assert ( y . Danger () );
  assert ( !x . IdenticalContents ( y ) );
  assert ( !y . IdenticalContents ( x ) );
  x . Add ( CMobile ( "Samsung", "Galaxy Note S7" ) );
  assert ( !x . IdenticalContents ( y ) );
  assert ( !y . IdenticalContents ( x ) );
  x . Add ( CShoes () );
  assert ( x . IdenticalContents ( y ) );
  assert ( y . IdenticalContents ( x ) );
  assert ( y . IdenticalContents ( y ) );
  assert ( x . IdenticalContents ( x ) );
  
  CSuitcase s(x);
  CBackpack r(y);

  CSuitcase a(5, 6, 7);
  a = x;


  CSuitcase sr(5000, 6, 7);

  x = sr;

  cout << x.Weight() << "asddsa";

  x . Add ( CKnife ( 5000 ) );

  //a . Add ( CKnife ( 8000 ) );

  cout << x.Weight() << endl;
  cout << a.Weight() << endl;

  CBackpack b;
  b = y;

  b = b;


  return 0;
}
#endif /* __PROGTEST__ */
