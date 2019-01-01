#include <fstream>
#include "dfa.h"

using namespace std;

int main()
{
  vector<string> lang = {"free access", "free software", "free vacation", "win", "winner", "winners", "winnings"};
  vector<string> messages;
  vector<State> myFA;
  string message, clean_display;
  vector<bool> flagVector;
  bool flagUp = false;
  int fa_index = 0, formatCounter = 0;
  char input_hold;
  ifstream input;
  /* Variable Descriptions:
     lang: A string vector that holds all of the spam cases. It can be changed to whatever you want and will
     generate a FA for any input language.
     messages: Is a string vector used for the very end to display which messages are spam.
     myFA is a State FA used to hold the result of the buildFA funciton in fa.cpp
     message: is a string that is passed by reference to the traverseFA function in fa.cpp.
     flagVector: is a boolean vector used to mark messages as spam. If the flag at a particular index is set
     it will register that message as spam. it runs concurrently alongside the messages vector.
     flagUp: Is a boolean variable that sets a flag in the transition in main.
     fa_index: Is an int passed to traverseFA by reference. You can think of it as what state you're in currently.
     input_hold: is a variable I use to read into from the file. It's done this way so there aren't input.get()
     instructions everywhere.
     input: is an input file stream used to read from the file.
  */
  input.open("messagefile.txt", ifstream::in);//open file.

  myFA = buildFA(lang);//build the FA.

  while(!input.eof())//loop until end of file.
    {
      input_hold = input.get();
      clean_display = "";
      
      if(input_hold == '\n')
	clean_display = "\\n";
      else if(input_hold == ' ')
	clean_display = "sp";
      else
	clean_display += input_hold;
	
      if(flagUp)//if spam detected.
	{
	  cout << "\u03B4(" << myFA[fa_index].name << "," << clean_display << ")\u21d2";//writes transition function;
	  traverseFA(input_hold, myFA, fa_index, message);//keep going but don't read the return into any variable.
	  cout << "{" << myFA[fa_index].name << "}|";
	  formatCounter++;
	  if(formatCounter == 5)
	    {
	      cout << endl;
	      formatCounter = 0;
	    }
	}
      
      

      else
	{
	  cout << "\u03B4(" << myFA[fa_index].name << "," << clean_display << ")\u21d2";//also writes transition function
	  flagUp = traverseFA(input_hold, myFA, fa_index, message);//if no spam yet, read return value into flagUp.
	  cout << "{" << myFA[fa_index].name << "}|";
	  formatCounter++;
	  if(formatCounter == 5)
	    {
	      cout << endl;
	      formatCounter = 0;
	    }
	}
      
  
      if(fa_index == 34 && !flagUp)//if you're at the very end and the flag isn't set.
	{
	  messages.push_back(message);
	  flagVector.push_back(false);//push false into the flag vector.
	  message = "";
	}
      if(fa_index == 34 && flagUp)
	{
	  messages.push_back(message);
	  flagVector.push_back(true);//otherwise push true
	  message = "";
	  flagUp = false;//reset flag for next message.
	}
     
    }
  input.close();

  cout << endl;
  for(int i = 0; i < messages.size(); i++)//for loop adds *SPAM* to the message name.
    {
      if(flagVector[i])
	messages[i] += "*SPAM*";
    }

  for(int i = 0; i < messages.size(); i++)//display the message titles.
    cout << messages[i] << endl;
  
  return 0;
}
