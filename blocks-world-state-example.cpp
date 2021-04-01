#include <iostream>
#include <vector>
#include <set>
#include <functional>
#include <list>
#include <limits>
#include <time.h>
using namespace std;
#include "blocks-world-state.cpp"

class Ida_BlocksWorld : public State
{

public:
  Ida_BlocksWorld(int blocks, int stacks);

  vector<Move> GetPossibleMoves();

  void GetNeighboors(list<Ida_BlocksWorld>& path, vector<Ida_BlocksWorld>& neighboors);

  int simpleHeuristic();

  void explore(int depth, int maxDepth);
};

bool findState(const list<Ida_BlocksWorld> &path, const Ida_BlocksWorld &state);

Ida_BlocksWorld::Ida_BlocksWorld(int blocks, int stacks)
    : State::State(blocks, stacks){};

vector<State::Move> Ida_BlocksWorld::GetPossibleMoves()
{
  vector<State::Move> moves;

  for (int i = 0; i < nbStacks; i++)
  {
    if (top[i] != -1) // if the stack isn't empty
    {
      for (int j = 0; j < nbStacks; ++j)
      {
        if (j != i)
        {
          State::Move mv = make_pair(i, j);
          moves.push_back(mv);
        }
      }
    }
  }

  return moves;
}

void Ida_BlocksWorld::GetNeighboors(list<Ida_BlocksWorld>& path, vector<Ida_BlocksWorld>& neighboors)
{
  vector<State::Move> moves = GetPossibleMoves();
  for (State::Move mv : moves)
  {
    this->doMove(mv);
    if (! findState(path, *this)) neighboors.push_back(*this);
    this->undoMove(mv);
  }
}

int Ida_BlocksWorld::simpleHeuristic()
{
  int sum = 0;
  for (int i = 0; i < nbStacks - 1; ++i)
  {
    if (top[i] != -1)
    {
      int element = top[i];
      ++sum;
      while (next[element] != -1)
      {
        ++sum;
        element = next[element];
      }
    }
  }
  int lastStackIndex = nbStacks - 1;
  int element = top[lastStackIndex];
  int expectedElement = sum;
  while (element != -1)
  {
    if (element != expectedElement)
    {
      ++sum;
    }
    element = next[element];
  }
  return sum;
}

int simpleHeuristic (const Ida_BlocksWorld& state) {
  int nextElement = state.top[state.nbStacks - 1] - 1;
  int sum = 0;
  int outTheLastStack = 0;
  for (int i = 0; i < state.nbStacks - 1; ++i)
  {
    int nbVisitedBlocksInStack = 0;
    int toMove = 0;
    int max = -1;
    if (state.top[i] != -1)
    {
      int element = state.top[i];
      while (element != -1) {
        ++nbVisitedBlocksInStack;
        if (state.next[element] != -1 && element < state.next[element]) {
          sum += nbVisitedBlocksInStack;
          nbVisitedBlocksInStack = 0;
        }
        // if (element > max) {
        //   max = element;
        //   toMove = nbVisitedBlocksInStack;
        // }
        ++sum;
        ++outTheLastStack;
        element = state.next[element]; 
      }
    }
  }
  int lastStackIndex = state.nbStacks - 1;
  int element = state.top[lastStackIndex]; 
  int expectedElement = outTheLastStack;
  int nbLastStackElements = 0;
  while (element != -1)
  {
    ++nbLastStackElements;
    if (element != expectedElement) {
      sum += 2*nbLastStackElements;
      nbLastStackElements = 0;
    }

    element = state.next[element];
    ++expectedElement;
  }
  return sum;
}

int zero (const Ida_BlocksWorld& state) {
  return 0;
}

// This heuristic calculates the number of misplaced blocks +
//  for each block A out of the final state, if it is above 
//  a block of a greater value, we add 1 move
//  For each misplaced block in the last stack, add 2 moves
int heuristic3(const Ida_BlocksWorld & state) {
  int sum = 0;
  const int & nbStacks = state.nbStacks;
  const int & nbBlocks = state.nbBlocs;
  const int* next = state.next;
  const int* top = state.top;
  int outOfLastStack = 0;
  // For each stack except the last stack
  for (int i = 0; i < nbStacks-1; ++i) {
    int element = top[i];
    while (element != -1) {
      ++sum;
      ++outOfLastStack;
      if (next[element] != -1 && element < next[element]) {
        ++sum;
      }
      element = next[element];
    }
  }

  // For each element in the last stack
  int lastStackIndex = nbStacks-1;
  int element = top[lastStackIndex];
  int expectedElement = outOfLastStack;
  int nbLastStack = 0;
  while (element != -1) {
    ++nbLastStack;
    if (element != expectedElement) {
      sum+= 2*nbLastStack;
      nbLastStack = 0;
    }
    ++expectedElement;
    element = next[element];
  }

  return sum;

} 

int heuristic4(const Ida_BlocksWorld & state) {
  int sum = 0;
  const int & nbStacks = state.nbStacks;
  const int & nbBlocks = state.nbBlocs;
  const int* next = state.next;
  const int* top = state.top;
  int outOfLastStack = 0;
  int tempElement = 0;
  set<int> countElements;
  // For each stack except the last stack
  for (int i = 0; i < nbStacks-1; ++i) {
    int element = top[i];
    while (element != -1) {
      ++sum;
      ++outOfLastStack;
      if (next[element] != -1 && element < next[element]) {
        tempElement = top[i];
        while (tempElement != next[element]) {
          if (tempElement < next[element] && countElements.find(tempElement) == countElements.end()) {
            ++sum;
            countElements.insert(tempElement);
          }
          tempElement = next[tempElement];
        }
      }
      element = next[element];
    }
  }

  // For each element in the last stack
  int lastStackIndex = nbStacks-1;
  int element = top[lastStackIndex];
  int expectedElement = outOfLastStack;
  int nbLastStack = 0;
  while (element != -1) {
    ++nbLastStack;
    if (element != expectedElement) {
      sum+= 2*nbLastStack;
      nbLastStack = 0;
    }
    ++expectedElement;
    element = next[element];
  }

  return sum;

} 

int heuristic2 (const Ida_BlocksWorld& state) {
  int sum = simpleHeuristic(state);
  int* const &  next = state.next;
  for (int i = 0; i < state.nbStacks - 1; ++i) {
    int element = state.top[i];
    while ( element != -1 && next[element] != -1 ) {
      if ( element < next[element] ) {
        ++sum;
      }
      element = next[element];
    }
  }
  return sum;
}

// void Ida_BlocksWorld::explore(int depth, int maxDepth)
// {
//   vector<Ida_BlocksWorld> neighboors;
//   GetNeighboors();
//   this->display();
//   if (depth == maxDepth)
//     return;
//   for (Ida_BlocksWorld st : neighboors)
//   {
//     st.explore(depth + 1, maxDepth);
//   }
// }

typedef function<int(const Ida_BlocksWorld &state)> Heuristic;

void search(
    Ida_BlocksWorld &currentState,
    int ub,
    int &nub,
    list<Ida_BlocksWorld> &path,
    list<Ida_BlocksWorld> &bestPath,
    Heuristic h,
    int &nbVisitedState)
{

  ++nbVisitedState;

  int g = path.size();
  int f;

  if ( currentState.isFinal() ) {
    bestPath = path;
    return;
  }

  vector<Ida_BlocksWorld> neighboors;
  currentState.GetNeighboors(path, neighboors);

  for (Ida_BlocksWorld state : neighboors) {
    int heuristic = h( state );
    f = g + 1 + heuristic;
    if (f > ub) {
      if (f < nub) {
        nub = f;
      }
    }else {
      path.push_back(state);
      search (state, ub, nub, path, bestPath, h, nbVisitedState);
      path.pop_back();
      if ( !bestPath.empty() ) return;
    }
  }

}

bool findState(const list<Ida_BlocksWorld> &path, const Ida_BlocksWorld &state)
{
  for (const Ida_BlocksWorld &st : path)
  {
    if (state == st)
    {
      return true;
    }
  }
  return false;
}

void ida(
    Ida_BlocksWorld &initialState,
    Heuristic h,
    list<Ida_BlocksWorld>& bestPath,
    int& nbVisitedStates
  )
{
  int ub = 0;
  int nub = h(initialState);
  list<Ida_BlocksWorld> path;

  while ( bestPath.empty() && nub != numeric_limits<int>::max() )
  {
    ub = nub;
    nub = numeric_limits<int>::max();
    search (initialState, ub, nub, path, bestPath, h,nbVisitedStates);
  }
}

int main()
{
  int nbStacks = 4;
  int nbBlocs = 20;
  Ida_BlocksWorld state = Ida_BlocksWorld(nbBlocs, nbStacks);
  state.setInitial();
  cout << "simple heuristic : " << simpleHeuristic(state) << endl;
  cout << "heuristic 2 : " << heuristic2(state) << endl;
  cout << "Initial state s0:" << endl;
  state.display();
  state.doMove(make_pair(2, 1));
  cout << "simple heuristic : " << simpleHeuristic(state) << endl;
  cout << "heuristic 2 : " << heuristic2(state) << endl;
  cout << "State s1 = t(s0,2->1) :" << endl;
  state.display();
  state.doMove(make_pair(0, 2));
  cout << "simple heuristic : " << simpleHeuristic(state) << endl;
  cout << "heuristic 2 : " << heuristic2(state) << endl;
  cout << "State s2 = t(s1,0->2) :" << endl;
  state.display();
  state.doMove(make_pair(1, 2));
  cout << "simple heuristic : " << simpleHeuristic(state) << endl;
  cout << "heuristic 2 : " << heuristic2(state) << endl;
  cout << "State s3 = t(s2,1->2) :" << endl;
  state.display();
  state.doMove(make_pair(1, 2));
  cout << "simple heuristic : " << simpleHeuristic(state) << endl;
  cout << "heuristic 2 : " << heuristic2(state) << endl;
  cout << "State s4 = t(s3,1->2) :" << endl;
  state.display();
  state.doMove(make_pair(0, 2));
  cout << "simple heuristic : " << simpleHeuristic(state) << endl;
  cout << "heuristic 2 : " << heuristic2(state) << endl;
  cout << "State s5 = t(s4,0->2) :" << endl;
  state.display();
  if (state.isFinal())
  {
    cout << "s5 is a final state..." << endl;
  }

  cout << "IDA algorithm =========================" << endl;
  
  time_t startTime = time(&startTime);

  list <Ida_BlocksWorld> path;
  state.setInitial();
  int visitedStates = 0;
  ida (
    state,
    heuristic4,
    path,
    visitedStates
  );
  for (Ida_BlocksWorld state : path) {
    state.display();
    cout << endl;
  }
  cout << "nb visited states : " << visitedStates << endl;
  cout << "path length : " << path.size() << endl;

  time_t endTime = time(&endTime);
  double diff = difftime( endTime, startTime );
  cout << "time : " << diff << endl;
  return 0;
}
