#ifndef __PROGTEST__
#include <cstdlib>
#include <cstdio>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <set>
#include <list>
#include <map>
#include <vector>
#include <queue>
#include <string>
#include <stack>
#include <queue>
#include <deque>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <functional>
using namespace std;
#endif /* __PROGTEST__ */


class OrderingDoesNotExistException
{
};

class DuplicateMatchException
{
};

class CGraph
{
  public:
    CGraph(int numOfVerticles) : m_numOfVerticles(numOfVerticles) {
      m_arrayOfNeighbours.resize(numOfVerticles);
    }

    void addEdge(int src, int dst) {
      m_arrayOfNeighbours[src].push_back(dst);
    }

    int search(int start) {
      int lossCount = -1;
      vector<bool> visited(m_numOfVerticles, false);

      deque<int> queue;

      visited[start] = true;
      queue.push_back(start);

      while(!queue.empty()) {

        start = queue.front();
        ++lossCount;
        queue.pop_front();

        for(auto i = m_arrayOfNeighbours[start].begin(); i != m_arrayOfNeighbours[start].end(); ++i) {

          if(!visited[*i]) {

            visited[*i] = true;
            queue.push_back(*i);
          }
        }
      }
      return lossCount;
    }

  private:
    int m_numOfVerticles;
    vector<list<int>> m_arrayOfNeighbours;
};

template <typename _M>
class CContest
{
  public:
    CContest() : contestID(0) {}
    ~CContest() {}

    CContest & AddMatch( const string & contestant1, const string & contestant2, const _M & result ) {

      if (m_matchMap.count(make_pair(contestant1, contestant2)) > 0 || m_matchMap.count(make_pair(contestant2, contestant1)) > 0)
        throw DuplicateMatchException();
      else {
        m_matchMap.insert(make_pair(make_pair(contestant1, contestant2), result));

        if (indexList.count(contestant1) == 0) {
          indexList.insert(make_pair(contestant1, contestID));
          ++contestID;
        }
        if (indexList.count(contestant2) == 0) {
          indexList.insert(make_pair(contestant2, contestID));
          ++contestID;
        }
      }

      return *this;
    }

    template <typename _T>
    bool IsOrdered ( const _T & comparator ) const {

      CGraph bracket(indexList.size());
      map<int, string> lossBoard;

      for (auto it = m_matchMap.begin(); it != m_matchMap.end(); ++it) {
        int tmp = comparator(it->second);

        if (tmp > 0) {
          bracket.addEdge(indexList.at(it->first.second), indexList.at(it->first.first));
          //cout << "Added edge - " << indexList.at(it->first.first) << " -> " << indexList.at(it->first.second) << endl;
        } else if (tmp < 0) {
          bracket.addEdge(indexList.at(it->first.first), indexList.at(it->first.second));
          //cout << "Added edge - " << indexList.at(it->first.second) << " -> " << indexList.at(it->first.first) << endl;
        } else {
          return false;
        }
        //cout << "fight: " << indexList.at(it->first.first) << " vs " << indexList.at(it->first.second) << " with result: " << comparator(it->second) << endl;
      }

      for (auto & res: indexList) {
        int lossCounter = bracket.search(res.second);
        if (!lossBoard.insert(make_pair(lossCounter, res.first)).second)
          return false;
        //cout << "name: " << res.first << res.second << " - index loss: " << lossCounter << endl;
      }


      return true;
    }

    template <typename _T>
    list<string> Results ( const _T & comparator ) const {

      CGraph bracket(indexList.size());
      map<int, string> lossBoard;

      for (auto it = m_matchMap.begin(); it != m_matchMap.end(); ++it) {
        int tmp = comparator(it->second);

        if (tmp > 0) {
          bracket.addEdge(indexList.at(it->first.second), indexList.at(it->first.first));
          //cout << "Added edge - " << indexList.at(it->first.first) << " -> " << indexList.at(it->first.second) << endl;
        } else if (tmp < 0) {
          bracket.addEdge(indexList.at(it->first.first), indexList.at(it->first.second));
          //cout << "Added edge - " << indexList.at(it->first.second) << " -> " << indexList.at(it->first.first) << endl;
        } else {
          throw OrderingDoesNotExistException();
        }
        //cout << "fight: " << indexList.at(it->first.first) << " vs " << indexList.at(it->first.second) << " with result: " << comparator(it->second) << endl;
      }

      for (auto & res: indexList) {
        int lossCounter = bracket.search(res.second);
        if (!lossBoard.insert(make_pair(lossCounter, res.first)).second)
          throw OrderingDoesNotExistException();
        //cout << "name: " << res.first << res.second << " - index loss: " << lossCounter << endl;
      }
      list<string> tmp;
      for (auto & res: lossBoard) {
        //cout << "name: " << res.second << " losses: " << res.first << endl;
        tmp.push_back(res.second);
      }

      return tmp;
    }
    // Results ( comparator )

  private:
    int contestID;
    map<pair<string, string>, _M> m_matchMap;
    map<string, int> indexList;
};

#ifndef __PROGTEST__
struct CMatch
{
  public:
    CMatch( int a, int b ) : m_A( a ), m_B( b ) {}
    
    int m_A;
    int m_B;
};

class HigherScoreThreshold
{
  public:
    HigherScoreThreshold( int diffAtLeast ) : m_DiffAtLeast( diffAtLeast ) {}

    int operator()( const CMatch & x ) const {
      return ( x.m_A > x.m_B + m_DiffAtLeast ) - ( x.m_B > x.m_A + m_DiffAtLeast );
    }

  private:
    int m_DiffAtLeast;    
};

int HigherScore( const CMatch & x ) {
  return ( x.m_A > x.m_B ) - ( x.m_B > x.m_A );
}

int main( void ) {

  CContest<CMatch> x;
  
  x . AddMatch ( "C++", "Pascal", CMatch ( 10, 3 ) )
    . AddMatch ( "C++", "Java", CMatch ( 8, 1 ) )
    . AddMatch ( "Pascal", "Basic", CMatch ( 40, 0 ) )
    . AddMatch ( "Java", "PHP", CMatch ( 6, 2 ) )
    . AddMatch ( "Java", "Pascal", CMatch ( 7, 3 ) )
    . AddMatch ( "PHP", "Basic", CMatch ( 10, 0 ) );
    
  
  assert ( ! x . IsOrdered ( HigherScore ) );
  try
  {
    list<string> res = x . Results ( HigherScore );
    assert ( "Exception missing!" == NULL );
  }
  catch ( const OrderingDoesNotExistException & e )
  {
  }
  catch ( ... )
  {
    assert ( "Invalid exception thrown!" == NULL );
  }

  x . AddMatch ( "PHP", "Pascal", CMatch ( 3, 6 ) ); 

  assert ( x . IsOrdered ( HigherScore ) );
  try
  {
    list<string> res = x . Results ( HigherScore );
    assert ( ( res == list<string>{ "C++", "Java", "Pascal", "PHP", "Basic" } ) );
  }
  catch ( ... )
  {
    assert ( "Unexpected exception!" == NULL );
  }
  
  assert ( ! x . IsOrdered ( HigherScoreThreshold ( 3 ) ) );
  try
  {
    list<string> res = x . Results ( HigherScoreThreshold ( 3 ) );
    assert ( "Exception missing!" == NULL );
  }
  catch ( const OrderingDoesNotExistException & e )
  {
  }
  catch ( ... )
  {
    assert ( "Invalid exception thrown!" == NULL );
  }
  
  assert ( x . IsOrdered ( [] ( const CMatch & x )
  {
    return ( x . m_A < x . m_B ) - ( x . m_B < x . m_A ); 
  } ) );
  try
  {
    list<string> res = x . Results ( [] ( const CMatch & x )
    {
      return ( x . m_A < x . m_B ) - ( x . m_B < x . m_A ); 
    } );
    assert ( ( res == list<string>{ "Basic", "PHP", "Pascal", "Java", "C++" } ) );
  }
  catch ( ... )
  {
    assert ( "Unexpected exception!" == NULL );
  }
  
  CContest<bool>  y;
  
  y . AddMatch ( "Python", "PHP", true )
    . AddMatch ( "PHP", "Perl", true )
    . AddMatch ( "Perl", "Bash", true )
    . AddMatch ( "Bash", "JavaScript", true )
    . AddMatch ( "JavaScript", "VBScript", true );
  
  assert ( y . IsOrdered ( [] ( bool v )
  {
    return v ? 10 : - 10;
  } ) );
  try
  {
    list<string> res = y . Results ( [] ( bool v )
    {
      return v ? 10 : - 10;
    });
    assert ( ( res == list<string>{ "Python", "PHP", "Perl", "Bash", "JavaScript", "VBScript" } ) );
  }
  catch ( ... )
  {
    assert ( "Unexpected exception!" == NULL );
  }
    
  y . AddMatch ( "PHP", "JavaScript", false );
  assert ( !y . IsOrdered ( [] ( bool v )
  {
    return v ? 10 : - 10;
  } ) );
  try
  {
    list<string> res = y . Results ( [] ( bool v )
    {
      return v ? 10 : - 10;
    } );
    assert ( "Exception missing!" == NULL );
  }
  catch ( const OrderingDoesNotExistException & e )
  {
  }
  catch ( ... )
  {
    assert ( "Invalid exception thrown!" == NULL );
  }
  
  try
  {
    y . AddMatch ( "PHP", "JavaScript", false );
    assert ( "Exception missing!" == NULL );
  }
  catch ( const DuplicateMatchException & e )
  {
  }
  catch ( ... )
  {
    assert ( "Invalid exception thrown!" == NULL );
  }
  
  try
  {
    y . AddMatch ( "JavaScript", "PHP", true );
    assert ( "Exception missing!" == NULL );
  }
  catch ( const DuplicateMatchException & e )
  {
  }
  catch ( ... )
  {
    assert ( "Invalid exception thrown!" == NULL );
  }
  
  return 0;
}
#endif /* __PROGTEST__ */
