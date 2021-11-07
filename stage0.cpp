//Zac Caffey and Cameron Ley
//CS 4301
//Compiler Stage 0

//#include <fstream>              //enable input streams            (These are already defined in .h)
//#include <ostream>              //Enable output streams
//#include <string>               //For use with strings
#include <ctime>                //This is to allow us to calculate the current time
#include <iomanip>              //This is to enable use of setw()
#include <vector>
#include <stdlib.h>
#include "stage0.h"

//using namespace std;

Compiler::Compiler(char **argv) // constructor - Z (needs to declare sourceFile, listingFile, and objectFile. Also need to fix the issue with using argv. might just be a result of the prior error)
{
    ifstream sourceFile(argv[1]);
    ofstream listingFile(argv[2]);  
    ofstream objectFile(argv[3]);
}

// ---------------------------------------------------------------------------------

Compiler::~Compiler() // destructor - Z
{
    sourceFile.close();
    listingFile.close();
    objectFile.close();
}

// ---------------------------------------------------------------------------------

void Compiler::createListingHeader() // - Z (needs to be formatted)
{
	time_t now = time(0);
	char* time = ctime(&now);
	listingFile << "STAGE0:\t Zac Caffey and Cameron Ley, " << time << endl;
	listingFile << "LINE NO:" << right << setw(14) << "SOURCE STATEMENT" << endl << endl;
 //print "STAGE0:", name(s), DATE, TIME OF DAY
 //print "LINE NO:", "SOURCE STATEMENT"
 //line numbers and source statements should be aligned under the headings
}

// ---------------------------------------------------------------------------------

void Compiler::parser()
{
  string error;
  nextChar();
  //ch must be initialized to the first character of the source file
  if (nextToken() != "program")
  {
    error = "keyword \"program\" expected";
    processError(error);
  }
  //a call to nextToken() has two effects
  // (1) the variable, token, is assigned the value of the next token
  // (2) the next token is read from the source file in order to make
  // the assignment. The value returned by nextToken() is also
  // the next token.
  prog();
  //parser implements the grammar rules, calling first rule
}

// ---------------------------------------------------------------------------------

void Compiler::createListingTrailer() // - Z
{
    cout << "COMPILATION TERMINATED," << errorCount << "ERRORS ENCOUNTERED" << endl;
}

// ---------------------------------------------------------------------------------

void Compiler::processError(string err)   // - Z (not sure if this is done correctly. May need to adjust the error message) (ALMOST POSITIVE THIS WILL NEED TO BE ADJUSTED)
{
    listingFile << err;
	  errorCount += 1;
	  listingFile << "\nCOMPILATION TERMINATED      " << errorCount << " ERROR ENCOUNTERED" << endl;
    exit(-1);
}

// ---------------------------------------------------------------------------------

void Compiler::prog()  //token should be "program" - C test
{
    string error;
    if (token != "program")
    {
      error = "keyword \"program\" expected"; 
		  processError(error);
    }

    progStmt(); 
    nextToken();

    if (token == "const") 
    {
      consts(); 
    }
    if (token == "var")
    { 
      vars(); 
    }
    if (token != "begin")
    {
      error = "keyword \"begin\" expected"; 
	 	  processError(error);
    }

    beginEndStmt();
    nextToken();

    if (token != "$")       // might need to check this. getting error because END_OF_FILE is a char and token is a string
    {
      error = "no text may follow \"end\""; 
		  processError(error);
    }
} 

// ---------------------------------------------------------------------------------

void Compiler::progStmt()  //token should be "program" - C
{   
  string x = "";
  string error;
  if (token != "program")
  {
    error = "keyword \"program\" expected"; 
	  processError(error);
  }
  //Initialize so that we don't mess up nextToken
  x = nextToken(); 
  if (!isNonKeyId(x)) 
  {
		processError("program name expected");
  }
  if (x != ";") 
  {
		processError("semicolon expected");
  }

  x = nextToken(); 
  code("program", x);
  insert(x,PROG_NAME,CONSTANT,x,NO,0);
}

// ---------------------------------------------------------------------------------

void Compiler::consts()  //token should be "const" - C
{  
  string error; 
	if (token != "const")
  {
    error = "keyword \"const\" expected"; 
		processError(error);
  }
	//Initialize so that we don't mess up nextToken
	string x = nextToken(); 
	if (!isNonKeyId(x))
  {
    error = "non-keyword identifier must follow \"const\""; 
		processError(error);
  }

	constStmts();
} 

// ---------------------------------------------------------------------------------

void Compiler::vars()  //token should be "var" - C
{   
    string error;
    if (token != "var")
    { 
		  error = "keyword \"var\" expected"; 
		  processError(error);
    }
	  //Initialize so that we don't mess up nextToken
    string x = nextToken(); 
    if (!isNonKeyId(x))
    {
      error = "non-keyword identifier must follow \"var\""; 
		  processError(error);
    }

    varStmts();
}

// ---------------------------------------------------------------------------------

void Compiler::beginEndStmt()  //token should be "begin" - C
{   
    string error;
    if (token != "begin")
    {
      error = "keyword \"begin\" expected";
		  processError(error);
    }
	  //Initialize so that we don't mess up nextToken
    string x = nextToken();                                                           
    if (x != "end")
    {
      error =  "keyword \"end\" expected";
		  processError(error);
    }
    if (x != ".") 
    {
		  processError("period expected");
    }
    x = nextToken();

    code("end", ".");
}

// ---------------------------------------------------------------------------------

void Compiler::constStmts() //token should be NON_KEY_ID - Z (this will need some work. not done right now)
{ 
  string x, y, error;

  if (!isNonKeyId(token))
  {
    processError("non-keyword identifier expected");
  }

  x = token;

  if (nextToken() != "=")
  {
    error = "\"=\" expected";
    processError(error);
  }

  y = nextToken();

  if (!(isNonKeyId(y)) || y != "+" || y != "-" || y != "not" || y != "true" || y != "false" || isInteger(y)) //y is not one of "+","-","not",NON_KEY_ID,"true","false",INTEGER)  need to see how we will compare these as y is a string  compare to a signal character
  {
    error = "token to right of \"=\" illegal";
    processError(error);
  }

  if (y == "+" || y == "-")
  {
    if (!(isInteger(next)))     //not sure if i need "" around integer since it is an enumerated type
    {
      processError("integer expected after sign");
    }

    y = y + token;
  }

  if (y == "not")
  {
    if (!(isBoolean(next)))   //not sure if i need "" around boolean since it is an enumerated type
    {
      error = "boolean expected after \"not\"";
      processError(error);
    }
      
    if (token == "true")
    {
      y = "false";
    }
    else
    {
      y = "true";
    }
  }

  if (next != ";")
  {
    processError("semicolon expected");
  }

  if (!(isInteger(y)) || !(isBoolean(y)))       //the data type of y is not INTEGER or BOOLEAN
  {
    error = "data type of token on the right-hand side must be INTEGER or BOOLEAN";
    processError(error);
  }

  insert(x,whichType(y),CONSTANT,whichValue(y),YES,1);        // dont think this is complete but will check later (be very careful when placing function calls inside a function call MOTL SAYS TO NOT DO THIS AT ALL)
  x = nextToken();

  if (x != "begin" || x != "var" || !(isNonKeyId(x)))         //x is not one of "begin","var",NON_KEY_ID)
  {
    error = "non-keyword identifier, \"begin\", or \"var\" expected";               
    processError(error);
  }

  if (isNonKeyId(x))
  {
    constStmts();
  }
}

// ---------------------------------------------------------------------------------

void Compiler::varStmts() //token should be NON_KEY_ID - Z (started this but not done)
{
 string x,y, next, error;
 if ((isNonKeyId(token)))
 {
  processError("non-keyword identifier expected");
 }

 x = ids();

 if (token != ":")
 {
  error = "\":\" expected";
  processError(error);
 }

 next = nextChar();
  
 if (!(isInteger(next)) || !(isBoolean(next))) //thinking the correct use of getDataType might actually be getDataType(nextToken? We shall see)
 {
  error = "illegal type follows \":\"";
  processError(error);
 }

 y = token;

 if (next != ";")
 {
  processError("semicolon expected");
 }

 insert(x,whichType(y),VARIABLE,"",YES,1);     //this isnt going to work ********************************** second argument must be of type storeTypes and y is not
 string z = nextToken();

 if (z != "begin" || !(isNonKeyId(z)))    //is not one of "begin",NON_KEY_ID)
 {
  error = "non-keyword identifier or \"begin\" expected";
  processError(error);
 }

 if (isNonKeyId(token))   //token is a NON_KEY_ID)
 {
  varStmts();
 }

}

// ---------------------------------------------------------------------------------

string Compiler::ids() //token should be NON_KEY_ID - Z
{
 string temp,tempString, next;

 if (!(isNonKeyId(token)))    //token is not a NON_KEY_ID
 {
  processError("non-keyword identifier expected");
 }

 tempString = token;
 temp = token;
 next = nextToken();

 if (next == ",")
 {
  if (!(isNonKeyId(next)))    //nextToken() is not a NON_KEY_ID)
  {
    processError("non-keyword identifier expected");
  }

  tempString = temp + "," + ids();
 }

 return tempString;
}

// ---------------------------------------------------------------------------------
bool Compiler::isKeyword(string s) const // determines if s is a keyword
{
  if (s == "program" || s == "const" || s == "var" || s == "integer" || s == "boolean" || s == "begin" || s == "end" || s == "true" || s == "false" || s == "not")
  {
    return true;
  }
  else 
  {
    return false;
  }
}

// ---------------------------------------------------------------------------------

bool Compiler::isSpecialSymbol(char c) const // determines if c is a special symbol
{
  if (c == ':' || c == ',' || c == ';' || c == '=' || c == '+' || c == '-' || c == '.')
  {
    return true;
  }
  else 
  {
    return false;
  }
}

// ---------------------------------------------------------------------------------

bool Compiler::isNonKeyId(string s) const // determines if s is a non_key_id  // This needs to be redone - Z
{
    if(!isKeyword(s) && !isInteger(s))
    {
      if (s != ":" || s != "," || s != ";" || s != "=" || s != "+" || s != "-" || s != ".")
      { 
        return true;
      }
    }

    return false;
}

// ---------------------------------------------------------------------------------

bool Compiler::isInteger(string s) const // determines if s is an integer
{
    if(s == "0" || s == "1" || s == "2" || s == "3" || s == "4" || s == "5" || s == "6" || s == "7" || s == "8" || s == "9")
    {
      return true;
    }
    else
    {
      return false;
    }
}

// ---------------------------------------------------------------------------------

bool Compiler::isBoolean(string s) const // determines if s is a boolean - Cam
{
  if (s == "true" || s == "false")
  {
    return true;
  }
  else 
  {
    return false;
  }
}

// ---------------------------------------------------------------------------------

bool Compiler::isLiteral(string s) const // determines if s is a literal - Z
{
  bool integer = isInteger(s);

  if (integer || s == "true" || s == "false" || s == "not" || s == "+" || s == "-")     //this doesnt seem like it will be right to me but I am not sure
  {
    return true;
  }
  else
  {
    return false;
  }
}

// ---------------------------------------------------------------------------------

//generate the internal name when called upon
string Compiler::genInternalName(storeTypes stype) const
{
  string internal;
  static int countI = 0, countB = 0;
  //use case statements "INTEGER", "BOOLEAN", "PROG"
  switch(stype)
  {
    case PROG_NAME:
      internal = "P0";
      break;
    case INTEGER:
      internal = "I" + to_string(countI);
      ++countI;
      break;
    case BOOLEAN:
      internal = "B" + to_string(countB);
      ++countB;
      break;
  }

  return internal;
}

// ---------------------------------------------------------------------------------

//create symbol table entry for each identifier in list of external names
//Multiply inserted names are illegal - Z

//Need to create a vector of our external names
// 
void Compiler::insert(string externalName,storeTypes inType, modes inMode, string inValue,
allocation inAlloc, int inUnits)
{

  string name;
  map<string,SymbolTableEntry>::iterator itr2 = symbolTable.begin();

  auto itr = externalName.begin();      //I do not think that auto is required in this statement. It does however make the code more efficient by declaring the type of the iterator to whatever the type of externalName.begin() is

  while (itr < externalName.end())
  {
    name = "";

    while (*itr != ',' && itr < externalName.end())
    {
      name += *itr;
      ++itr;
    }

  if (name != "")                   //meaning that we have grabbed a name from the external names (if symbolTable name is defined)
    {
      if (symbolTable.count(name) != 1)       //multiply defined
      {
        processError("multiple definitions of name");
      }
      else if (isKeyword(name))
      {
        processError("illegal use of keyword");
      }
      else
      {
        if (isupper(name[0]))
        {
          symbolTable.insert(pair<string, SymbolTableEntry>(name.substr(0, 15), SymbolTableEntry(name, inType, inMode, inValue, inAlloc, inUnits)));
        }
        else
        {
          auto internal = genInternalName(inType);
          symbolTable.insert(pair<string, SymbolTableEntry>(name.substr(0, 15), SymbolTableEntry(internal, inType, inMode, inValue, inAlloc, inUnits)));
        }
      }
    }

    if (itr == externalName.end())
    {
      break;
    }
    else
    {
      ++itr;
    }
  }



  /*
 vector<string> externalNames;
 string name = externalName;
 string value = inValue;
 string type = to_string(inType);
 string mode = to_string(inMode);
 string alloc = to_string(inAlloc);
 string units = to_string(inUnits);
 
 map<string,SymbolTableEntry>::iterator itr = symbolTable.find(name);
 symbolTable.insert({itr->first, itr->second});                           //CHECK THESE LINES TO SEE IF IT IS ACTUALLY INSERTING THE EXTERNAL NAME
  
 

 while (name != "")    //need to better understand what is meant by broken
 {
  if (itr != symbolTable.end())   //might need more here                                            
  {
    processError("multiple name definition");
  }
  else if (isKeyword(name))
  {
    processError("illegal use of keyword");
  }
  else //create table entry
  {
    if (isupper(name[0]))
    {
      symbolTable.insert({itr->first, itr->second});     //name     :need to look at these - Z
      itr = symbolTable.find(type); 
      symbolTable.insert({itr->first, itr->second});    //inType
      itr = symbolTable.find(mode);
      symbolTable.insert({itr->first, itr->second});    //inMode
      itr = symbolTable.find(value);
      symbolTable.insert({itr->first, itr->second});    //inValue
      itr = symbolTable.find(alloc);
      symbolTable.insert({itr->first, itr->second});    //inAlloc
      itr = symbolTable.find(units);
      symbolTable.insert({itr->first, itr->second});    //inUnits
    }
    else
    {
      string internal_name = genInternalName(inType);                                                                          //symbolTable.insert(genInternalName(inType),inType,inMode,inValue, inAlloc,inUnits);
      itr = symbolTable.find(internal_name);        //dont think this is correct - Z
      symbolTable.insert({itr->first, itr->second});    //inType
      itr = symbolTable.find(mode);
      symbolTable.insert({itr->first, itr->second});    //inMode
      itr = symbolTable.find(value);
      symbolTable.insert({itr->first, itr->second});    //inValue
      itr = symbolTable.find(alloc);
      symbolTable.insert({itr->first, itr->second});    //inAlloc
      itr = symbolTable.find(units);
      symbolTable.insert({itr->first, itr->second});    //inUnits
    }
  }  
 }
 */
}

// ---------------------------------------------------------------------------------

storeTypes Compiler::whichType(string name) //tells which data type a name has - Z (not even close to being done)
{
 storeTypes dataType;
 map<string,SymbolTableEntry>::iterator itr = symbolTable.find(name);

 if (isLiteral(name))   //name is a literal)
 {
  if (isBoolean(name))  //name is a boolean literal)
  {
    dataType = BOOLEAN; //data type = "Boolean";    //might need to be uppercase      //idk why setDataType is undefined here. It is in the include
  }
  else
  {
    dataType = INTEGER;   //might need to be uppercase
  }
 }
  /*else //name is an identifier and hopefully a constant
 {
  if (itr != symbolTable.end())     //CHECK THIS
  {
    dataType = itr->second.getDataType(); //    type of symbolTable[name];
  }
  else
  {
    processError("reference to undefined constant");
  }
 }*/
 else //name is an identifier and hopefully a constant
 {
		if (symbolTable.count(name) > 0)
			dataType = symbolTable.at(name).getDataType();
		else
			processError("reference to undefined constant");
 }
  return dataType;
}

// ---------------------------------------------------------------------------------

string Compiler::whichValue(string name) //tells which value a name has
{
  map<string,SymbolTableEntry>::iterator itr = symbolTable.find(name);
  string value;

  if (isLiteral(name)) //name is a literal)
  {
   string value = name;
  }
  else               //name is an identifier and hopefully a constant
  {
   if (itr != symbolTable.end() && itr->second.getValue() != "")  //this must also have contents
   {
    value = itr->second.getValue();       // might need to be first, well
   }
   else
   {
    processError("reference to undefined constant");
   }
  }
  return value;
}

// ---------------------------------------------------------------------------------

void Compiler::code(string op, string operand1, string operand2) // - Z
{
 if (op == "program")
 {
  emitPrologue(operand1);
 }
 else if (op == "end")
 {
  emitEpilogue();
 }
 else
 {
  processError("compiler error since function code should not be called with illegal arguments");
 }
}

// ---------------------------------------------------------------------------------

void Compiler::emit(string label, string instruction, string operands, string comment)  // - C
{
	//Turn on left justification in objectFile 
	objectFile.setf(ios_base::left);
	//Output label in a field of width 8 
	objectFile << setw(8) << label;              //changed from width(x) to setw(x) - Z
	//Output instruction in a field of width 8 
	objectFile << setw(8) << instruction;
	//Output the operands in a field of width 24      //WHY A FIELD WIDTH OF 5? JUST CURIOUS - Z
	objectFile << setw(24) << operands;
	//Output the comment 
	objectFile << comment << "\n";
}

// ---------------------------------------------------------------------------------

void Compiler::emitPrologue(string progName, string operand2)
{
  //Output identifying comments at beginning of objectFile 
  //Output the %INCLUDE directives 
	objectFile << "%INCLUDE \"Along32.inc\"\n" << "%INCLUDE \"Macros_Along.inc\"\n\n";
	emit("SECTION", ".text");
	emit("global", "_start", "", "; program" + progName);
	objectFile << "\n";
	emit("_start:");
}

// ---------------------------------------------------------------------------------

void Compiler::emitEpilogue(string operand1, string operand2)
{
  emit("","Exit", "{0}");
  objectFile << "\n";
  emitStorage();
}

// ---------------------------------------------------------------------------------

void Compiler::emitStorage()
{
 map<string,SymbolTableEntry>::iterator itr = symbolTable.begin();

/*for those entries in the symbolTable that have
 an allocation of YES and a storage mode of CONSTANT
 { call emit to output a line to objectFile }*/
 /* Proposed Change - Cam
 for (int i = 0; i < symbolTable.size(); i++)
    {
    map<string,SymbolTableEntry>::iterator itr = symbolTable.at(i)
    if (itr->second.getAlloc() == YES)
        {
          if (itr->first.getMode() == CONSTANT || itr->first.getMode() == VARIABLE)
          {
            emit("SECTION", ".data");
          }
        }
    }
 */

 for (itr = symbolTable.begin(); itr != symbolTable.end(); ++itr)
 {
                                                                    //map<string, SymbolTableEntry>::iterator itr = symbolTable.find(i);     //need this to be the address of the first table entry
   if (itr->second.getAlloc() == YES)
   {
     if (itr->second.getMode() == CONSTANT || itr->second.getMode() == VARIABLE)
     {
      emit("SECTION", ".data");
     }
   }
 }

/*
 if (itr->second.getAlloc() == YES && itr->first.getMode() == CONSTANT)      //having a hard time figuring out how to access these different data points
 {
    emit("SECTION", ".data");
 }
for those entries in the symbolTable that have
 an allocation of YES and a storage mode of VARIABLE
{ call emit to output a line to objectFile }*/

/*
 if (itr->second.getAlloc() == YES && itr->first.getMode() == VARIABLE)      //having a hard time figuring out how to access these different data points
 {
    emit("SECTION", ".data");
 }

*/
}


// ---------------------------------------------------------------------------------

string Compiler::nextToken()        //returns the next token or end of file marker { - C   MIGHT WANT TO CHANGE THIS TO A SWITCH STATEMENT TO MAKE THINGS EASIER FOR LATER             
{

	token = "";	
	while(token == "")
	{
		if (ch == '{')
		{
			char next = nextChar();   //need to checkout these two lines (Changed from string to char and lost the error - Z)
			while (next != sourceFile.eof() || next != '}')
			{
				
			}
			if (ch == '$')
				processError("unexpected end of file");
			else 
				nextChar();
		}
		else if (ch == '}')
		{
			processError("'}' cannot begin token");
		}
		else if (isspace(ch))
		{
			nextChar();
		}
		else if (isSpecialSymbol(ch))
		{
			token = ch;
			nextChar();
		}
		else if (islower(ch))
		{
			token = ch;
			char next = nextChar();   //we need to checkout this error    (changed from string to char and lost the error - Z)
			while((isalpha(next) || isdigit(next) || next == '_') || next != sourceFile.eof())
			{
				token = token + ch;
			}
			if (ch == '$')
      {
				processError("unexpected end of file");
      }
		}
		else if (isdigit(ch))
		{
			token = ch;
			char next = nextChar();   //(changed from string to char and lost the error - Z
			while(isdigit(next) && next != sourceFile.eof())
			{
				token = token + ch;
			}
			if (ch == '$')
      {
				processError("unexpected end of file");
      }
		}
		else if (ch == '$')
		{
			token = ch;
		}
		else
		{
			processError("illegal symbol");
		}
	}
	return token;
}

// ---------------------------------------------------------------------------------

char Compiler::nextChar()   //returns the next character or end of file marker - C (needs to be edited)
{ 
  // read in next character 
  sourceFile.get(ch);    //get does not need an argument - Z
  if (sourceFile.eof())
  {
	//use a special character "$" to designate end of file 
    ch = END_OF_FILE;     
    return ch;
  }
  else 
  {
	  sourceFile.get(ch);
  }
	// print to listing file (starting new line if necessary) 
	if (ch == '\n')
	{
    listingFile << endl << lineNo;
		lineNo++;    //(changed from listingFile.write(ch); I think this way will work better - Z)
	}
	listingFile << ch;      //(changed from listingFile.write(ch); I think this way will work better - Z)

  
  return ch; 
}
