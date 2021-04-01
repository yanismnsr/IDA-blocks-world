#include <iostream>
#include <utility>
using namespace std;

class State
{
  public:
    
  State(int b, int s);
  // Constructor that creates a state with b blocs and s stacks

  // Copy constructor
  State(const State & s);
  
  virtual void setInitial();
  // Initialise this with the initial state (blocs uniformly distributed)
  
  virtual bool isFinal();
  // Return true if this is a final state (all blocs on the last stack, from 
  // largest to smallest)
  
  typedef pair<int,int> Move;
  // A move is a pair of stacks: the bloc on top of the first stack is moved to
  // the top of the second stack

  virtual void doMove(Move m);
  // Precondition: 0 <= m.first < getNbStacks(), 0 <= m.second < getNbStacks(), 
  //               and !emptyStack(m.first)
  // Apply the doMove (m.first->m.second) to this

  virtual void undoMove(Move m);
  
  virtual void display() const;
  // Display this
  
  virtual int getNbStacks();
  // Return the number of stacks
  
  virtual bool emptyStack(int s);
  // Precondition: 0 <= s < getNbStacks()
  // Return true if stack s has no block

  virtual bool operator == (const State& state) const;
  
  ~State();
  // Destructor
  


  int nbBlocs, nbStacks;
  int* stack; // For each bloc b: stack[b] = stack that contains b
  int* next;  // For each bloc b: if b is the lowest block of its stack,
              // then next[b]=-1, else next[b]=bloc immediately below b
  int* top;   // For each stack s: if s is empty, then top[s]=-1
              // else top[s]=bloc on top of s
};

State::State(int b, int s)
{
  // Constructor that creates a state with b blocs and s stacks
  nbBlocs = b;
  nbStacks = s;
  stack = new int[nbBlocs];
  next = new int[nbBlocs];
  top = new int[nbStacks];
}

State::State(const State & s) {
  nbBlocs = s.nbBlocs;
  nbStacks = s.nbStacks;
  stack = new int[s.nbBlocs];
  next = new int[s.nbBlocs];
  top = new int [nbStacks];
  for (int i = 0; i < nbBlocs; ++i) {
    stack[i] = s.stack[i];
    next[i] = s.next[i];
  } 
  for (int i = 0; i < nbStacks; ++i) {
    top[i] = s.top[i];
  }
}

State::~State()
{
  delete[] next;
  delete[] stack;
  delete[] top;
}

void State::setInitial()
{
  // Initialise this with the initial state (blocs uniformly distributed)
  for (int s=0; s<nbStacks; s++) top[s] = -1;
  for (int b=0; b<nbBlocs; b++)
  {
    int s = b % nbStacks; // Put bloc b on top of stack s
    stack[b] = s;
    next[b] = top[s];
    top[s] = b;
  }
}

bool State::isFinal()
{
  // Return true if this is a final state (all blocs on the last stack, from 
  // largest to smallest)
  for (int s=0; s<nbStacks-1; s++)
  {
    if (top[s] != -1) return false;
  }
  if (top[nbStacks-1] != 0) return false;
  for (int b=0; b<nbBlocs-1; b++)
  {
    if (stack[b] != nbStacks-1) return false;
    if (next[b] != b+1) return false;
  }
  if (next[nbBlocs-1] != -1) return false;
  return true;
}

void State::doMove(Move m)
{
  // Precondition: 0 <= m.first < getNbStacks(), 0 <= m.second < getNbStacks(),
  //               and !emptyStack(m.first)
  // Apply the doMove (s1->s2) to this
  int b1 = top[m.first];
  int bb1 = next[b1];
  int b2 = top[m.second];
  next[b1] = b2;
  top[m.second] = b1;
  top[m.first] = bb1;
  stack[b1] = m.second;
}

void State::undoMove(Move m)
{
  // Precondition: 0 <= m.first < getNbStacks(), 0 <= m.second < getNbStacks(),
  //               and !emptyStack(m.first)
  // Apply the doMove (s1->s2) to this
  int b1 = top[m.second];
  int bb1 = next[b1];
  int b2 = top[m.first];
  next[b1] = b2;
  top[m.first] = b1;
  top[m.second] = bb1;
  stack[b1] = m.first; 
}


bool State::operator == (const State& state) const {
  if (
    nbBlocs != state.nbBlocs ||
    nbStacks != state.nbStacks
  ) return false;

  for (int i = 0; i < nbBlocs; ++i) {
    if (
      stack[i] != state.stack[i] ||
      next[i] != state.next[i]
    ) return false;
  }

  for (int i = 0; i < nbStacks; ++i) {
    if (top[i] != state.top[i]) return false;
  }
  return true;
}


void displayStack(int s, int* next)
{
  // Display all blocs below s (including s), from bottom to top
  if (s >= 0)
  {
      displayStack(next[s], next);
      cout << s << " ";
  }
}


void State::display() const
{
  // Display this
  for (int s=0; s<nbStacks; s++)
  {
    cout << "stack "<< s << ": ";
    displayStack(top[s], next);
    cout << endl;
  }
}

int State::getNbStacks()
{
  return nbStacks;
}

bool State::emptyStack(int s)
{
  // Precondition: 0 <= s < getNbStacks()
  // Return true if stack s has no block
  return top[s] == -1;
}
