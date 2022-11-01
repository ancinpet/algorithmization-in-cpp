#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cmath>
#include <cfloat>
#include <cassert>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <complex>
#include <string>
#include <vector>
#include <algorithm>
#include <memory>
using namespace std;
#endif /* __PROGTEST__ */

using namespace std;

//ignore this, potentional RIP for me
static string variableName = "x";
static auto currentFlags = cout.flags();

bool doubleCmp( double a, double b )
{
	return fabs(a - b) <= fabs(a*1e-15);
}

ios_base & dummy_polynomial_manipulator ( ios_base & x )
{ 
  return x;
}

ios_base & ( * ( polynomial_variable ( const string & varName ) ) ) ( ios_base & x )
{
  variableName = varName;
  return dummy_polynomial_manipulator;
}

class CPolynomial
{
  public:

    // default constructor
  	CPolynomial() : m_coef({0}) {};

    // operator +
    CPolynomial operator+ (const CPolynomial & poly) const;
    
    // operator -
    CPolynomial operator- (const CPolynomial & poly) const;
    
    // operator * (polynomial, double)
    CPolynomial operator* (const double multiplier) const;
    CPolynomial operator* (const CPolynomial & poly) const;

    // operator ==
  	bool operator== ( const CPolynomial &a) const;

    // operator !=
  	bool operator!= ( const CPolynomial &a) const;

    // operator []
    double & operator[] ( const size_t index );
    const double & operator[] ( const size_t index ) const;
    
    // operator ()
    double operator() ( const double x ) const;
    
    // operator <<
    friend ostream & operator << ( ostream & op, const CPolynomial & a); 
    
    // Degree (), returns unsigned value
  	size_t Degree( void ) const {
      for (size_t i = m_coef.size(); i > 0; --i) {
        if (m_coef[i-1] != 0.0)
          return i - 1;
      }

  		return 0;
  	};

  private:
  	vector<double> m_coef;
};

bool CPolynomial::operator== ( const CPolynomial &a ) const {
	if (a.Degree() != Degree()) {
		return false;
	}

	for (size_t i = 0; i < m_coef.size(); ++i) {
		if (!doubleCmp(a.m_coef[i], m_coef[i])) {
			return false;
		}
	}

	return true;
 }

bool CPolynomial::operator!= ( const CPolynomial &a ) const {
	if (a.Degree() != Degree()) {
		return true;
	}

	for (size_t i = 0; i < m_coef.size(); ++i) {
		if (!doubleCmp(a.m_coef[i], m_coef[i])) {
			return true;
		}
	}

	return false;
 }

ostream & operator << ( ostream & op, const CPolynomial & a) {

  if (variableName == "x") {
    op << boolalpha;
    currentFlags = op.flags();
  }

  if (currentFlags != op.flags()) {
    variableName = "x";
  }


	if (a.Degree() == 0) {
		if (a.m_coef[0] < 0.0)
			op << "- ";
		op << fabs(a.m_coef[0]);
		return op;
	}

	for (size_t i = a.m_coef.size(); i > 0; --i) {
		if (a.m_coef[i-1] == 0.0) {
			continue;
		}

		if (i - 1 != a.Degree()) {
			op << " ";
		}

		if ( a.m_coef[i-1] < 0.0 ) {
			op << "- ";
		} else if ( i - 1 != a.Degree() ) {
			op << "+ ";
		}

		if (i - 1 == 0) {
			op << fabs(a.m_coef[i-1]);
		} else if (!doubleCmp(fabs(a.m_coef[i-1]), 1.0)) {
			op << fabs(a.m_coef[i-1]) << "*" << variableName << "^" << i - 1;
		} else {
			op << variableName << "^" << i - 1;
		}

	}

	return op;
}

double & CPolynomial::operator[] ( size_t index ) {
	if (index >= m_coef.size()) {
		m_coef.resize(index + 1, 0);
  }
	return m_coef[index];
}

const double & CPolynomial::operator[] ( size_t index ) const {
	return m_coef[index];
}

double CPolynomial::operator() ( double x ) const {
	double res = 0;
	for (size_t i = m_coef.size(); i > 0; --i) {
		if ( i - 1 == 0 ) {
			res += m_coef[i - 1];
			break;
		}

		res += m_coef[i - 1] * pow(x, i - 1);
	}

	return res;
}

CPolynomial CPolynomial::operator* (const double multiplier) const {
	CPolynomial tmp;

	for (size_t i = 0; i < m_coef.size(); ++i) {
		tmp[i] = m_coef[i] * multiplier;
	}

	return tmp;
}

CPolynomial CPolynomial::operator* (const CPolynomial & poly) const {
	CPolynomial tmp;
  size_t curDeg = Degree();
  size_t polyDeg = poly.Degree();
  tmp.m_coef.resize(curDeg + polyDeg + 1, 0);

	for (size_t i = 0; i < curDeg + 1; ++i)
    for (size_t j = 0; j < polyDeg + 1; ++j)
      tmp.m_coef[i+j] += m_coef[i] * poly.m_coef[j];

	return tmp;
}

CPolynomial CPolynomial::operator+ (const CPolynomial & poly) const {
  CPolynomial tmp;

  if (m_coef.size() > poly.m_coef.size()) {
    tmp.m_coef.resize(m_coef.size());
    copy ( m_coef.begin(), m_coef.end(), tmp.m_coef.begin());

    for (size_t i = 0; i < poly.m_coef.size(); ++i) {
      tmp[i] += poly.m_coef[i];
    }
  } else {
    tmp.m_coef.resize(poly.m_coef.size());
    copy ( poly.m_coef.begin(), poly.m_coef.end(), tmp.m_coef.begin());

    for (size_t i = 0; i < m_coef.size(); ++i) {
      tmp[i] += m_coef[i];
    }
  }

  return tmp;
}

CPolynomial CPolynomial::operator- (const CPolynomial & poly) const {
  CPolynomial tmp;

  if (m_coef.size() > poly.m_coef.size()) {
    tmp.m_coef.resize(m_coef.size(), 0);
  } else {
    tmp.m_coef.resize(poly.m_coef.size(), 0);
  }

  copy ( m_coef.begin(), m_coef.end(), tmp.m_coef.begin());

  for (size_t i = 0; i < poly.m_coef.size(); ++i) {
    tmp[i] -= poly.m_coef[i];
  }

  return tmp;
}


#ifndef __PROGTEST__

bool dumpMatch( const CPolynomial & x, const vector<double> & ref )
{
	return true; //todo
}

int  main( void )
{

  CPolynomial a, b, c;
  ostringstream out, ku;

  cout << showpos << 1 << -1 << endl;
  cout << 1 << endl;

  cout << b[500] << endl;

  cout << b.Degree() << endl;


  a[0] = -10;
  a[1] = 3.5;
  a[3] = 1;
  assert ( doubleCmp ( a ( 2 ), 5 ) );
  out . str ("");
  out << a;
  cout << out.str() << endl;
  assert ( out . str () == "x^3 + 3.5*x^1 - 10" );
  a = a * -2;
  assert ( a . Degree () == 3
           && dumpMatch ( a, vector<double>{ 20.0, -7.0, -0.0, -2.0 } ) );

  out . str ("");
  out << a;
  assert ( out . str () == "- 2*x^3 - 7*x^1 + 20" );
  out . str ("");
  out << b;
  cout << out.str() << endl;
  assert ( out . str () == "0" );
  b[5] = -1;
  out . str ("");
  out << b;
  cout << out.str() << endl;
  assert ( out . str () == "- x^5" );
  c = a + b;
  assert ( c . Degree () == 5
           && dumpMatch ( c, vector<double>{ 20.0, -7.0, 0.0, -2.0, 0.0, -1.0 } ) );

  out . str ("");
  out << c;
  assert ( out . str () == "- x^5 - 2*x^3 - 7*x^1 + 20" );
  c = a - b;
  assert ( c . Degree () == 5
           && dumpMatch ( c, vector<double>{ 20.0, -7.0, -0.0, -2.0, -0.0, 1.0 } ) );

  out . str ("");
  out << c;
  assert ( out . str () == "x^5 - 2*x^3 - 7*x^1 + 20" );

  out . str ("");
  out << a << endl;
  out << b << endl;
  out << c << endl;
  cout << out . str ();



  c = a * b;
  assert ( c . Degree () == 8
           && dumpMatch ( c, vector<double>{ 0.0, -0.0, 0.0, -0.0, -0.0, -20.0, 7.0, 0.0, 2.0 } ) );

  out . str ("");
  out << c;
  cout << out . str () << endl;
  assert ( out . str () == "2*x^8 + 7*x^6 - 20*x^5" );
  assert ( a != b );
  b[5] = 0;
  assert ( !(a == b) );
  a = a * 0;
  assert ( a . Degree () == 0
           && dumpMatch ( a, vector<double>{ 0.0 } ) );

  assert ( a == b );

  // bonus
  a[2] = 4;
  a[1] = -3;
  b[3] = 7;
  ku . str ("");
  ku << polynomial_variable ( "t" ) << "a=" << a << ", b=" << b;
  cout << ku.str() << endl;
  ku . str ("");
  ku << polynomial_variable ( "t" ) << "a=" << a << ", b=" << b;
  cout << ku.str() << endl;
  out . str ("");
  out << polynomial_variable ( "y" ) << "a=" << a << ", b=" << b;
  cout << out.str() << endl;
  assert ( out . str () == "a=4*y^2 - 3*y^1, b=7*y^3" );

  out . str ("");
  out << polynomial_variable ( "test" ) << c;
  cout << out.str() << endl;
  assert ( out . str () == "2*test^8 + 7*test^6 - 20*test^5" );
  return 0;
}
#endif /* __PROGTEST__ */
