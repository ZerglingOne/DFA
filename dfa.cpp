#include "dfa.h"

using namespace std;

vector<State> buildFA(vector<string> language)
{
  vector<State> theFA;
  int fa_index, M2_Q0, messageEnd, temp_index;
  string holder, transition_function;
  State empty_state;
  bool add_flag = true;
  char finalState = 6;
  for(int i = 0; i < 20; i++)
    empty_state.nextIndices[i] = -1;
   
  /* Variable Descriptions
     theFA: is a vector of States defined as a struct in fa.h, it is ultimately the returned data of this function.
     fa_index: is used later for the current node being built and pushed into the vector.
     M2_Q0: does not need to exist, but I used it to help keep track of where I was in the FA. It is the first
            state of the second machine.
     messageEnd: Is a lot like M2_Q0, it denotes where </DOC> is located.
     temp_index: is used later when I needed a temporary index because I use a lot of loops at that point.
     holder: is just a string that is used to build static parts of the FA. Parts that never change.
     empty_state: is an empty state, that as you can see in the for loop directly above this, has all of its
                  indices that represent indices in the vector set to -1 because -1 is an invalid index. it is
		  used to push states into theFA.
     add_flag: Is used later when the FA is building itself. It's used to denote whether or not the FA should add
               a rule or follow an arrow.
     finalState: Is an arbitrary character that is unlikely to ever be encountered (it's the bell character) in
                 any situation. It represents anything that is "not a letter" and is used to denote the final
                 machine state in M2, anything that could trigger a final state has this in its nextStates 
		 string.
  */ 

  
  holder = "<DOC><DOCID></DOCID>Subject\n\n";//the FA needs to read these, in this order
                                             //to go into the spam processing portion
                                             //they are part of the FA though. This is Machine 1
  //Begin M1
  for(int i = 0; i < holder.size(); i++)//set all of the things in holder to their own states.
    {
      theFA.push_back(empty_state);
      theFA[i].name = "Q" + itoa(i);
      theFA[i].contains = holder[i];
      theFA[i].nextStates += holder[i+1];
      theFA[i].nextIndices[0] = i+1;
    }
 
  fa_index = holder.size() - 1;//begin keeping track of where we're adding.
  M2_Q0 = fa_index;
  theFA[M2_Q0].nextStates = " <";
  theFA[M2_Q0].nextIndices[0] = 29;
  theFA[M2_Q0].nextIndices[1] = 30;//a little bit of hardwiring because this has to be set in stone.
  
  theFA.push_back(empty_state);
  fa_index++;
  theFA[fa_index].name = "Q" + itoa(fa_index); 
  theFA[fa_index].contains = ' '; /*This space is -incredibly- important. In the example language we have, 
				    one of the words is "win" if this space were not here, twin, twins, twinkie, 
				    swing, and others would be considered part of the spam filter. So it's very 
				    important that it's here.*/
  
  //End M1
  holder = "</DOC>";
  messageEnd = fa_index+1;
 
  for(int i = 0; i < holder.size() - 1; i++) 
   {
     theFA.push_back(empty_state);
     fa_index++;
     theFA[fa_index].name = "Q" + itoa(fa_index);
     theFA[fa_index].contains = holder[i];
     theFA[fa_index].nextStates += holder[i+1];
     theFA[fa_index].nextIndices[0] = theFA.size();
   }
  theFA.push_back(empty_state);
  fa_index++;
  theFA[fa_index].name = "Q" + itoa(fa_index);
  theFA[fa_index].contains = '>';
  theFA[fa_index].nextStates += '<';
  theFA[fa_index].nextIndices[0] = 0;//wrap back to the beginning of the FA because you're at the end of an email.
  
  
  //building spam detection part of FA

  //Begin M2
  //M2 uses many nested for loops to create the rest of the FA. It traverses itself while reading from a language
  //and pushes letters into the "nextStates" strings and ints into the "nextIndices" array that correspond where the 
  //states are stored. Think of nextIndices as the arrows, and the nextStates as the letters that indicate what the 
  //machine reads.
  for(int i = 0; i < language.size(); i++)//while there are strings left in the language.
    {
      temp_index = M2_Q0 + 1;//setting temp index to the start of language inputs for spam filtering.
      
      for(int j = 0; j < language[i].size(); j++)//while there are letters left to check in a string.
	{
	  if(i != 0)//needed this because it was producing weird results.
	    {
	      add_flag = true;//this flag tells the program to add a rule to the FA if it's not there already.
	      for(int k = 0; k < theFA[temp_index].nextStates.size(); k++)//while there are next states to be checked
		{
		  if(language[i][j] == theFA[temp_index].nextStates[k])//if the character already matches a next state
		    {
		      add_flag = false; //don't add a rule
		      temp_index = theFA[temp_index].nextIndices[k];//and follow the arrow 
		    }
		}
	    }
	  
	  if(add_flag)//this conditional checks if add flag is true, if it is, it adds a rule to the FA to follow.
	    {         //it adds to the next states of the current index, adds a new state to the FA, and advances.
	      fa_index++;
	      theFA[temp_index].nextStates += language[i][j];
	      theFA[temp_index].nextIndices[theFA[temp_index].nextStates.size() -1] = theFA.size();
	      theFA.push_back(empty_state);
	      theFA[fa_index].contains = language[i][j];
	      theFA[fa_index].name = "Q" + itoa(fa_index);
	      temp_index = fa_index;
	      if(j == language[i].size() - 1)
		{
		  theFA[fa_index].nextStates += finalState;//if it reaches the end of a spam case, it adds the final
		}                                          //state character to the next states string.
	    }
	}
    }  
  theFA.push_back(empty_state);//building final state here.
  theFA[theFA.size() - 1].name = "qf";
  theFA[theFA.size() - 1].contains = finalState;
  theFA[theFA.size() - 1].nextStates = "<";
  theFA[theFA.size() - 1].nextIndices[0] = messageEnd;
  for(int i = 0; i < theFA.size(); i++)//looks for things with "finalState" in their nextState and makes a rule for them.
   {
     for(int j = 0; j < theFA[i].nextStates.size(); j++)
       {
	 if(theFA[i].nextStates[j] == finalState)
	   theFA[i].nextIndices[j] = (theFA.size() - 1);//it's easy, because the final state is stored in the final index
       }
   }
  return theFA;  
}

bool traverseFA(char input, vector<State> elFA, int& index, string& msgnames)
{
  char input_hold = input;
  bool finalFlag = false, equalFlag = false;
  
  /*Variable descriptions:
    Parameters:
               input:
	       is the input character read from the file.
	       elFA:
	       is the FA to traverse
	       index:
	       is the index we are currently at (passed by ref)
	       msgnames:
	       is the name of the message, gotten by traversing the FA. a string passed by ref.
    Variables:
               input_hold: 
	                  a char I use because I really don't like using assignment statements with parameters.
			  it feels messy.
	       finalFlag: 
	                  Is ultimately what's returned from this function. will set if spam is detected.
	       equalFlag: 
	                  used below to see if the input matches a "nextStates" character.
  */
  if(index == 11)//index 11 is <DOCID>*here*</DOCID> this part is used for reading message names.
    {
      if(input_hold == '<');//if we read the <, it does nothing.
      
      else 
	{
	  msgnames += input_hold;//otherwise add the character to the message title.
	}
    }
  
  for(int i = 0; i < elFA[index].nextStates.size(); i++)//checks next states for moving forward.
    {
      for(int j = 0; j < elFA[index].nextStates.size(); j++)//immediately runs through to check 2 things.
	{
	  if(elFA[index].nextStates[j] == 6 && notALetter(input_hold))//if the final state is next, and that we have not
	    {                                                         //read a letter. immediately sets finalFlag to true
	      finalFlag = true;
	      input_hold = 6;
	    }
	}

      if(input_hold == elFA[index].nextStates[i])//follows an arrow.
	{
	  equalFlag = true;
	  index = elFA[index].nextIndices[i];
	}
      
    }
  if(!equalFlag && !finalFlag && index >= 28 && index != 35)//if there's no arrow to follow, go back to initial state
    {                                                       //where we're reading spam from. which is index 28.
	  index = 28;
    }
  return finalFlag;
}

string itoa(int theInt)//integer to string, because it's not ANSI C standard for some reason but atoi is.
{                      //a little tool i wrote for ease of making this program.
  int powTen = 10000, holder1, holder2;
  string returned;
  holder2 = theInt;
  if(theInt == 0)
    return "0";
  else for(powTen; powTen > 0; powTen /= 10)
    {
      if(theInt/powTen > 0)
	{
	  holder1 = holder2/powTen;
	  holder2 -= holder1 * powTen;
	  returned += (holder1 + 48);
	}
      
	  
    }
  return returned;
}

int intPow(int base, int exponent)//simple power function just for integers without getting floats involved.
{
  int holder = 1;
  for(int i = 0; i < exponent; i++)
    holder *= base;

  if(exponent == 0)
    return 1;

  else return holder;
}

bool notALetter(char isit)//this is used to calculate the transition to the final state.
{                         //it has to be here, because otherwise wins, winter, and other similar words would 
  if((isit > 64 && isit < 91) || (isit > 96 && isit < 123))//falsely flag spam.
    return false;
  else
    return true;
}
