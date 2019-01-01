#include <iostream>
#include <vector>
#include <cstdlib>

using namespace std;

struct State//state struct, used to build nodes of the FA.
{
  string name;
  char contains; //contains this character or what was read to get here.
  string nextStates; //string of viable states attainable from this node.
  int nextIndices[20];//where the states are stored.
};

int intPow(int, int);//exponential function for just ints.

string itoa(int);//integer to string

vector<State> buildFA(vector<string>); //builds the FA.

bool traverseFA(char, vector<State>, int&, string&); //traverses the FA

bool notALetter(char); //checks if passed char is a letter.

