//Zac Caffey and Cameron Ley
//CS 4301
//Compiler Stage 0

#include <ctime>                //This is to allow us to calculate the current time
#include <iomanip>              //This is to enable use of setw()
#include <stage0.h>

using namespace std;

Compiler::Compiler(char **argv) // constructor
{
    sourceFile.open(argv[1]);
    listingFile.open(argv[2]);
    objectFile.open(argv[3]);
}

// ---------------------------------------------------------------------------------

Compiler::~Compiler() // destructor
{
    sourceFile.close();
    listingFile.close();
    objectFile.close();
}

// ---------------------------------------------------------------------------------

void Compiler::createListingHeader()
{
	time_t now = time(0);
	char* time = ctime(&now);
	// outputs our names and the time of compilation
	listingFile << "STAGE0:  Zac Caffey and Cameron Ley       " << time << endl;
	listingFile << "LINE NO.              " << "SOURCE STATEMENT" << endl << endl;
}

// ---------------------------------------------------------------------------------

void Compiler::parser() 
{
	string error;
	nextChar();
  
	// grabs the nextToken and ensures that it is program like it should be
	// TOOK OUT STRING X HERE
	if (nextToken() != "program")
	{
		error = "keyword \"program\" expected";
		processError(error);
	}

	prog();
}

// ---------------------------------------------------------------------------------

void Compiler::createListingTrailer()
{
	//outputs the listing trailer at the end of the .lst file 
	listingFile << endl << "COMPILATION TERMINATED" << right << setw(7) << errorCount << " ERRORS ENCOUNTERED" << endl;
}

// ---------------------------------------------------------------------------------

void Compiler::processError(string err)
{
	// for processing errors outputs at the bottom with the specified error
	listingFile << endl << "Error: Line " << lineNo << ": " << err << endl;
	errorCount += 1;
	listingFile << "\nCOMPILATION TERMINATED      " << errorCount << " ERROR ENCOUNTERED" << endl;
	exit(-1);
}

// ---------------------------------------------------------------------------------

void Compiler::prog()  //token should be "program"
{
	// error is made due to error with a \ within "" 
	string error;
	if (token != "program")
	{
		error = "keyword \"program\" expected"; 
		processError(error);
	}

	progStmt();

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
		// we were not sure whether to add a case here for 017.lst
		error = "keyword \"begin\" expected"; 
		processError(error);
	}

	beginEndStmt();

	if (token[0] != END_OF_FILE)    
	{
		error = "no text may follow \"end\""; 
		processError(error);
	}
} 

// ---------------------------------------------------------------------------------

void Compiler::progStmt()  //token should be "program"
{   
	string x;
	string error;
	if (token != "program")
	{
		error = "keyword \"program\" expected"; 
		processError(error);
	}

	x = nextToken(); 

	if (!isNonKeyId(x))	//throws an error if "program" is not followed by a non_key_id 
	{
		processError("program name expected");
	}
  
	string y = nextToken();
  
	if (y != ";") 
	{
		processError("semicolon expected");
	}
  
	nextToken();
	code("program", x);
	insert(x,PROG_NAME,CONSTANT,x,NO,0);
}

// ---------------------------------------------------------------------------------

void Compiler::consts()  //token should be "const"
{  
	string error;
	if (token != "const")
	{
		error = "keyword \"const\" expected"; 
		processError(error);
	}

	// TOOK STRING X OUT
	if (!isNonKeyId(nextToken()))
	{
		error = "non-keyword identifier must follow \"const\""; 
		processError(error);
	}

	constStmts();
} 

// ---------------------------------------------------------------------------------

void Compiler::vars()  //token should be "var"
{   
    string error;

    if (token != "var")
    { 
		error = "keyword \"var\" expected"; 
		processError(error);
    }

    // TOOK STRING X OUT HERE
    if (!isNonKeyId(nextToken()))
    {
		error = "non-keyword identifier must follow \"var\""; 
		processError(error);
    }

    varStmts();
}

// ---------------------------------------------------------------------------------

void Compiler::beginEndStmt()	//token should be "begin"
{   
    string error;

    if (token != "begin")
    {
		error = "keyword \"begin\" expected";
		processError(error);
    }

    execStmts();

    if (nextToken() != "end")
    {
		error =  "keyword \"end\" expected";
		processError(error);
    }

    if (nextToken() != ".") 
    {
		processError("period expected");
    }

    nextToken();		//I feel like this may be un-needed
    code("end", ".");
}

void execStmts(); // stage 1, production 2
{
    if (isNonKeyId(token) || token == "read" || token == "write")
    {
      execStmt();
	  nextToken();
      execStmts();
    }
	else if (token == "end");

	else
	{
		processError("non-keyword identifier, \"read\", \"write\", or \"begin\" expected");
	}
}

void execStmt(); // stage 1, production 3
{
    if (isNonKeyId(token))
    {
      assignStmt();
    }
    else if (token == "read")
    {
      readStmt();
    }
    else if (token == "write")
    {
      writeStmt();
    }
    else
    {
      processError("non-keyword id, \"read\", or \"write\" expected");
    }
}

void assignStmt(); // stage 1, production 4
{
  string first, second;

  if (!isNonKeyId(token))
  {
    processError("non - keyword identifier expected");
  }

  if (symbolTable.count(token) == 0)
  {
	  processError("reference to undefined variable")
  }

  pushOperand(token);
  nextToken();

  if (nextToken() != ":=")
  {
    processError("':=' expected; found " + token);
  }

  pushOperator(":=");
  nextToken();
  
  if (token && "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token) && token != ";")
  {
	processError("one of \"*\", \"and\", \"div\", \"mod\", \")\", \"+\", \"-\", \";\", \"<\", \"<=\", \"<>\", \"=\", \">\", \">=\", or \"or\" expected")
  }

  express();
  
  second = popOperand();
  first = popOperand();

  code(popOperator(), second, first);

}

void readStmt(); // stage 1, production 5
{
  if (token != "read")
  {	
	processError();
  }

  nextToken();

  if (token != '(')
  {
    processError("'(' expected; found " + token);
  }

  x = ids();
  nextToken();

  if (token != ')')
  {
    processError("')' expected; found " + token);
  }

  code("read", x);
  nextToken();

  if (token != ";")
  {
	processError("';' expected; found " + token);
  }
}

void writeStmt(); // stage 1, production 7
{
  if (token != "write")
  {
	processError();
  }

  nextToken();

  if (token != '(')
  {
    processError();
  }

  x = ids();
  nextToken();

  if (token != ')')
  {
    processError("')' expected; found " + token);
  }

  code("write", x);
  nextToken();

  if (token != ";")
  {
	processError();
  }
}

void express(); // stage 1, production 9
{
  if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token))
  {
	processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
  }

  term();
  nextToken();

  if (token == "=" || token == "<>" || token == "<=" || token == ">=" || token == "<" || token == ">")
  {
    expresses();
  }
}

void expresses(); // stage 1, production 10
{
  string first, second;

  if (token != "=" && token != "<>" && token != "<=" && token != ">=" && token != "<" && token != ">")
  {
	processError("\"=\", \"<>\", \"<=\", \">=\", \"<\", or \">\" expected");  
  }

  pushOperator(token);
  nextToken();

  //error checking here
  if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token))
  {
	processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
  }

  term();

  second = popOperand();
  first = popOperand();

  code(popOperator(), second, first);

  if (token == "=" || token == "<>" || token == "<=" || token == ">=" || token == "<" || token == ">")
  {
	expresses();
  }
}

void term(); // stage 1, production 11
{
  if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token))
  {
	processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
  }

  factor();

  if (token == "-" || token == "+" || token == "or")
  {
  	terms();
  }
}

void terms(); // stage 1, production 12   //need to account for epsilon some how
{
  string first, second;

  if (token != "+" && token != "-" && token != "or")
  {
    processError("\"+\", \"-\", or \"or\" expected");
  }

  pushOperator(token);
  nextToken();
  //error checks here
  if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token))
  {
	processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
  }

  factor();
  second = popOperand();
  first = popOperand();

  code(popOperator(), second, first);

  //error checks here
  if (token == "+" || token == "-" || token == "or")
  {
    terms();
  }
}

void factor(); // stage 1, production 13
{
  if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" token != "-" && !isInteger(token) && !isNonKeyId(token))
  {
	processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
  }
  
  part();

  if (token == "*" && token == "div" && token == "mod" && token == "and")
  {
	factors();
  }
}

void factors(); // stage 1, production 14	// need to account for epsilon move
{
	string first, second;

	if (token != "*" && token != "div" && token != "mod" && token != "and")
	{
	  processError();
	}

	pushOperator(token);
	nextToken();
	
	if (token != "not" && token != "(" && !isBoolean(token) && !isNonKeyId(token) && token != "+" && token != "-" && token != "true" && token != "false")
	{
		processError();
	}

	part();

	second = popOperand();		
	first = popOperand();
	
	code(popOperator(), second, first);

	if (token == "*" || token == "div" || token == "mod" || token == "and")
	{
		factors();
	}
}

void part(); // stage 1, production 15
{
	if (token == "not")
	{
		nextToken();

		if (token == "(")
		{
			nextToken();

			if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" token != "-" && !isInteger(token) && !isNonKeyId(token))
			{
				processError();
			}

			express();
			nextToken();

			if (token != ")")
			{
				processError();
			}

			nextToken();
			code("not", popOperand());
		}
		else if (isBoolean(token))
		{
			if (token == "true")
			{
				pushOperand("false");
			}
			else 
			{
				pushOperand("true");
			}
		}
		else if (isNonKeyId(token))
		{
			code("not", token);
		}
	}

	else if (token == "+")
	{
		nextToken();

		if (token == "(")
		{
			nextToken();

			if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" token != "-" && !isInteger(token) && !isNonKeyId(token))
			{
				processError();
			}

			express();
			nextToken();

			if (token != ")")
			{
				processError();
			}
		}
		else if (isInteger(token))
		{
			pushOperand(token);
		}
		else if (isNonKeyId(token))
		{
			pushOperand(token);
		}
	}

	else if (token == "-")
	{
		string first;
		nextToken();

		if (token == "(")
		{
			nextToken();

			if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" token != "-" && !isInteger(token) && !isNonKeyId(token))
			{
				processError();
			}

			express();
			nextToken();

			if (token != ")")
			{
				processError();
			}

			first = popOperand();
			code("neg", first);
		}
		else if (isInteger(token))
		{
			pushOperand("-" + token);
		}
		else if (isNonKeyId(token))
		{
			code("neg", token)
		}
	}

	else if (isInteger(token) || isBoolean(token) || isNonKeyId(token))
	{
		pushOperand(token);
	}

	else if (token == "(")
	{
		nextToken();

		if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" token != "-" && !isInteger(token) && !isNonKeyId(token))
		{
			processError();
		}

		express();
		nextToken();

		if (token != ")")
		{
			processError();
		}

		nextToken();
	}

	else
	{
		processError();
	}

}

// ---------------------------------------------------------------------------------

void Compiler::constStmts() //this is used to process our constStmts. We include special cases for 020.lst and 049.lst
{ 

	string x, y, error;

	if (!isNonKeyId(token))
	{
		processError("non-keyword identifier expected");
	}
  
	// for 020.lst
	if (token.back() == '_')
	{
		error = "illegal character to end a var";
		processError(error); 
	}
  
	// for 049.lst
	for (unsigned int i = 0; i < token.size(); i++)
	{
		if (token[i] == '_' && token[i+1] == '_')
		{
			processError("'_' must be followed by a letter or number");
		}
	}
  
	x = token;
  
	if (nextToken() != "=")
	{
		error = "\"=\" expected";
		processError(error);
	}

	y = nextToken();
  
	if (!(isNonKeyId(y)) && y != "+" && y != "-" && y != "not" && !(isBoolean(y)) && !(isInteger(y)))
	{
		error = "token to right of \"=\" illegal";
		processError(error);
	}

	if (y == "+" || y == "-")
	{
		string temp = nextToken();
		if (!isInteger(temp))     
		{
			processError("integer expected after sign");
		}

		y = y + token;
	}

	if (y == "not")
	{
		if (!(isBoolean(nextToken())))  
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

	if (nextToken() != ";")
	{
		processError("semicolon expected");
	}
	
	storeTypes temp = whichType(y);
	
	if (temp != INTEGER && temp != BOOLEAN)       
	{
		error = "data type of token on the right-hand side must be INTEGER or BOOLEAN";
		processError(error);
	}

	insert(x,whichType(y),CONSTANT,whichValue(y),YES,1);        
	x = nextToken();

	if (x != "begin" && x != "var" && !(isNonKeyId(x)))       
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

void Compiler::varStmts() //token should be NON_KEY_ID
{
	string x,y, error;
	if (!isNonKeyId(token))
	{
		processError("non-keyword identifier expected");
	}

	// for 020.lst
	if (token.back() == '_')
	{
		error = "illegal character to end a var";
		processError(error); 
	}
   
	// for 049.lst 
	for (unsigned int i = 0; i < token.size(); i++)
	{
		if (token[i] == '_' && token[i+1] == '_')
		{
			processError("'_' must be followed by a letter or number");
		}
	}

	x = ids();

	if (token != ":")
	{
		error = "\":\" expected";
		processError(error);
	}

	if (nextToken() != "integer" && token != "boolean")	//catches any illegal type that might follow ':' 
	{
		error = "illegal type follows \":\"";
		processError(error);
	}

	y = token;

	if (nextToken() != ";")
	{
		processError("semicolon expected");
	}
	
	if (y == "integer")	//if token is integer we proceed to make an insertion into our symbolTable
	{
		insert(x,INTEGER,VARIABLE,"1",YES,1);
	}
	else	//if token is boolean we proceed to make an insertion into our symbolTable
	{
		insert(x,BOOLEAN,VARIABLE,"1",YES,1);    
	}
	 
	if (nextToken() != "begin" && !(isNonKeyId(token)))   
	{
		error = "non-keyword identifier or \"begin\" expected";
		processError(error);
	}

	if (isNonKeyId(token))
	{
		varStmts();
	}
}

// ---------------------------------------------------------------------------------

string Compiler::ids() //token should be NON_KEY_ID
{
	string temp,tempString;

	if (!(isNonKeyId(token)))    //use to catch when token is not a NON_KEY_ID
	{
		processError("non-keyword identifier expected");
	}

	tempString = token;
	temp = token;

	if (nextToken() == ",")
	{
		if (!(isNonKeyId(nextToken())))    //used to catch when nextToken() is not a NON_KEY_ID)
		{
			processError("non-keyword identifier expected");
		}
		
		tempString = temp + "," + ids();
	}
	
	return tempString;
}

// ---------------------------------------------------------------------------------

bool Compiler::isKeyword(string s) const //determines if s is a keyword
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

bool Compiler::isSpecialSymbol(char c) const //determines if c is a special symbol
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

bool Compiler::isNonKeyId(string s) const //determines if s is a non_key_id
{
	if(!isKeyword(s) && !isInteger(s) && !isSpecialSymbol(s[0]))
	{
		return true;
	}
	
	return false;
}

// ---------------------------------------------------------------------------------

bool Compiler::isInteger(string s) const //determines if s is an integer	
{
	try
	{
		stoi(s);
	}
	catch (invalid_argument&)
	{
		return false;
	}
	
	return true;
}

// ---------------------------------------------------------------------------------

bool Compiler::isBoolean(string s) const // determines if s is a boolean

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

bool Compiler::isLiteral(string s) const //determines if s is a literal
{
	//bool integer = isInteger(s);
	if (isInteger(s) || s == "true" || s == "false" || s == "not" || s == "+" || s == "-")
	{    
		return true;
	}
	else
	{
		return false;
	}
}

// ---------------------------------------------------------------------------------

string Compiler::genInternalName(storeTypes stype) const	//generate the internal name when called upon
{
	string internal;
	static int countI = 0, countB = 0;
	//use case statements "INTEGER", "BOOLEAN", "PROG"
	switch(stype)
	{
		case PROG_NAME:
		{
			internal = "P0";
			break;
		}
		
		case INTEGER:
		{
			internal = "I" + to_string(countI);
			++countI;
			break;
		}
		
		case BOOLEAN:
		{
			internal = "B" + to_string(countB);
			++countB;
			break;
		}
	}
	
	return internal;
}

// ---------------------------------------------------------------------------------


 
void Compiler::insert(string externalName,storeTypes inType, modes inMode, string inValue, allocation inAlloc, int inUnits)	//creates symbol table entries for each identifier in list of external names important to note that multiply inserted names are illegal
{
	string name;

	string::iterator itr = externalName.begin();		

	while (itr < externalName.end())
	{
		name = "";
		
		while (itr < externalName.end() && *itr != ',' )
		{
			name = name + *itr;
			++itr;
		}

		if (!name.empty())				
		{
			if (symbolTable.count(name) > 0)
			{
				processError("symbol " + name + " is multiply defined");
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
					symbolTable.insert(pair<string, SymbolTableEntry>(name.substr(0, 15), SymbolTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits)));
				}
			}
		}
		
		if (symbolTable.size() > 256)
		{
			processError("symbolTable overflow");
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
}

// ---------------------------------------------------------------------------------

storeTypes Compiler::whichType(string name)	//Determines that type of a value using the name parameter
{
	map<string,SymbolTableEntry>::iterator itr = symbolTable.find(name);

	storeTypes type;

	if (isLiteral(name))
	{
		if (isBoolean(name))
		{
			type = BOOLEAN;
		}
		else
		{
			type = INTEGER;
		}
	}
	else 
	{
		if (symbolTable.count(name) > 0)
		{
			type = itr->second.getDataType();
		}
		else
		{
			processError("reference to undefined constant");
		}
	}
	
	return type;
}

// ---------------------------------------------------------------------------------

string Compiler::whichValue(string name) //tells which value a name has
{
	map<string,SymbolTableEntry>::iterator itr = symbolTable.find(name);
  
	string value;

	if (isLiteral(name)) 
	{
		// this is for the .asm file output to output '-1' or '0' instead of 'true' or 'false'
		if (name == "false")
		{
			value = "0";
		}
		else if (name == "true")
		{
			value = "-1";
		}
		else 
		{
			value = name;
		}
	}

	else 
	{
		if (itr->second.getValue() != "" && symbolTable.count(name) > 0)
		{
			value = itr->second.getValue();
		}
		else
		{
			processError("reference to undefined constant");
		}
	}
	return value;
}

// ---------------------------------------------------------------------------------

void Compiler::code(string op, string operand1, string operand2)	//Calls emitPrologue when our op is "program" and calls emitEpilogue when our op is "end"
{
  if (op == "program")
  {
	emitPrologue(operand1);
  }

	else if (op == "end")
	{
		emitEpilogue();
	}

	else if (op == "read")
  {
    emitReadCode(/*need something here*/);
  }

  else if (op == "write")
  {
    emitWriteCode(/*need something here*/);
  }

  else if (op == "=")
  {
    emitAssignCode(operand1, operand2);
  }

  else if (op == "+") // this must be binary '+'
  {
    emitAdditionCode(operand1, operand2));
  }

  else if (op == "-") // this must be binary '-'
  {
    emitSubtractionCode(operand1, operand2));
  }

  else if (op == "neg") // this must be unary '-'
  {
    emitNegationCode(operand1, op);
  }

  else if (op == "not")
  {
    emitNotCode(operand1, op);
  }

  else if (op == "*")
  {
    emitMultiplicationCode(operand1, operand2));
  }

  else if (op == "div")
  {
    emitDivisionCode(operand1, operand2));
  }
  else if (op == "mod")
  {
    emitModuloCode(operand1, operand2));
  }

  else if (op == "and")
  {
    emitAndCode(operand1, operand2));
  }
  else if (op == "or")
  {
    emitOrCode(operand1, operand2));
  }

  else if (op == "<")
  {
    emitLessThanCode(operand1, operand2));
  }
  else if (op == "<=")
  {
    emitLessThanOrEqualToCode(operand1, operand2));
  }

  else if (op == ">")
  {
    emitGreaterThanCode(operand1, operand2));
  }

  else if (op == ">=")
  {
    emitGreaterThanOrEqualToCode(operand1, operand2));
  }

  else if (op == "!=")
  {
    emitInequalityCode(operand1, operand2));
  }
  else if (op == "==")
  {
 	emitEqualityCode(operand1, operand2));
  }
  else if (op == ":=")
  {
 	emitAssignCode(operand1, operand2));
  }
 else
 {
 	processError("compiler error since function code should not be called with illegal arguments");
 }
}

// ---------------------------------------------------------------------------------

void Compiler::emit(string label, string instruction, string operands, string comment)	//For formatting our emit statements in the objectFile 
{
	//Turn on left justification in objectFile 
	objectFile.setf(ios_base::left);
	//Output label in a field of width 8 
	objectFile << left << setw(8) << label;             
	//Output instruction in a field of width 8 
	objectFile << left << setw(8) << instruction;
	//Output the operands in a field of width 24     
	objectFile << left << setw(24) << operands;
	//Output the comment 
	objectFile << comment << "\n";
}

// ---------------------------------------------------------------------------------

void Compiler::emitPrologue(string progName, string operand2)	//Output identifying comments at beginning of objectFile and Output the %INCLUDE directives
{
	time_t now = time(0);
	char* time = ctime(&now);
	objectFile << "; Zac Caffey and Cameron Ley" << right << setw(6) << "" << time;
	objectFile << "%INCLUDE \"Along32.inc\"\n" << "%INCLUDE \"Macros_Along.inc\"\n" << endl;
	emit("SECTION", ".text");
	emit("global", "_start", "", "; program " + progName);
	objectFile << "\n";
	emit("_start:");
}

// ---------------------------------------------------------------------------------

void Compiler::emitEpilogue(string operand1, string operand2)	//emits our epilogue to the ASM file and calls emitStorage()
{
	emit("","Exit", "{0}");
	objectFile << "\n";
	emitStorage();
}

// ---------------------------------------------------------------------------------

void Compiler::emitStorage()    //for those entries in the symbolTable that have an allocation of YES and a storage mode of CONSTANT { call emit to output a line to objectFile }
{
	
	map<string,SymbolTableEntry>::iterator itr = symbolTable.begin();
	
	emit("SECTION", ".data");

	for (itr = symbolTable.begin(); itr != symbolTable.end(); ++itr)
	{
                                                                    
		if (itr->second.getAlloc() == YES)
		{
			if (itr->second.getMode() == CONSTANT)
			{
				emit(itr->second.getInternalName(), "dd", itr->second.getValue(), "; " + itr->first);
			}
		}
	}
 
	objectFile << "\n";
	emit("SECTION", ".bss");
 
	for (itr = symbolTable.begin(); itr != symbolTable.end(); ++itr)
	{
                                                                    
		if (itr->second.getAlloc() == YES)
		{
			if (itr->second.getMode() == VARIABLE)
			{
				emit(itr->second.getInternalName(), "resd", itr->second.getValue(), "; " + itr->first);
			}
		}
	}
}

 void emitReadCode(string operand, string = "");
 {
	 string name;
	 int loopC = 0;
	 int size = operand.size();
	 //while (name is broken from list (operand) and put in name != "")
	 while (loopC < size)
	 {
		loopC++;
		if (operand[loopC] != ',' && loopC < size)
		{
			name = name + operand[loopC];
			continue;
		}

		if (name != "")
		{
			//if name is not in symbol table
			if (symbolTable.count(name) == 0)
			{
				processError("reference to undefined symbol");
			}
			//if data type of name is not INTEGER
			if (symbolTable.at(name).getDataType() != INTEGER)
			{
				processError("can't read variables of this type");
			}
			//if storage mode of name is not VARIABLE
			if (symbolTable.at(name).getMode() != VARIABLE)
			{
				processError("attempting to read to a read-only location");
			}
			//emit code to call the Irvine ReadInt function
			emit("","call","ReadInt",";ReadInt function called")
			//emit code to store the contents of the A register at name
			emit("","mov","[" + symbolTable.at(name).getInternalName() + "],eax", ";store contents at " + name);
			//set the contentsOfAReg = name
			contentsOfAReg = symbolTable.at(name).getInternalName();
		}
	 }
 }

 void emitWriteCode(string operand, string = "");
 {
	string name;
	static bool definedStorage = false;
	int loopC = 0;
	int size = operand.size();
	 //while (name is broken from list (operand) and put in name != "")
	 while (loopC < size)
	 {
		loopC++;
		if (operand[loopC] != ',' && loopC < size)
		{
			name = name + operand[loopC];
			continue;
		}

		if (name != "")
		{
			//if name is not in symbol table
			if (symbolTable.count(name) == 0)
			{
				processError("reference to undefined symbol");
			}
			//if name is not in the A register
			//emit the code to load name in the A register
			//set the contentsOfAReg = name
			if (contentsOfAReg != symbolTable.at(name).getInternalName())
			{
				emit("","mov","eax[" + symbolTable.at(name).getInternalName() + "]", ";load " + name +" in A register");
				contentsOfAReg = symbolTable.at(name).getInternalName();
			}
			/*if data type of name is INTEGER
			emit code to call the Irvine WriteInt function
			else // data type is BOOLEAN
			{
			emit code to compare the A register to 0
			acquire a new label Ln
			emit code to jump if equal to the acquired label Ln
			emit code to load address of TRUE literal in the D register
			acquire a second label L(n + 1)
			emit code to unconditionally jump to label L(n + 1)
			emit code to label the next line with the first acquired label Ln
			emit code to load address of FALSE literal in the D register
			emit code to label the next line with the second acquired label L(n + 1)
			emit code to call the Irvine WriteString function*/
			if (symbolTable.at(name).getDataType() == INTEGER)
			{
				emit("","call","WriteInt",";WriteInt function called")
			}
			else // symbolTable.at(name).getDataType() == BOOLEAN
			{
				emit("","cmp","eax,0",";compare A register to 0");
				// need to write getLabel still and check this line
				string newLabel = getLabel();
				emit("","je","." + newLabel, ";jump if equal to the acquired label Ln");
				emit("","mov", "edx,TRUELIT", ";load address of TRUE literal in the D register");
				string secondLabel = getLabel();
				emit("","jmp","." + secondLabel, ";unconditionally jump to label L(n + 1)");
				emit("." + newLabel + ":");
				emit("","mov", "edx,FALSLIT", ";load address of FALSE literal in the D register");
				emit("." + secondLabel + ":");
				emit("","call","WriteString",";WriteString function called")

				/*
				if static variable definedStorage is false
				{
				set definedStorage to true
				output an endl to objectFile
				emit code to begin a .data SECTION
				emit code to create label TRUELIT, instruction db, operands 'TRUE',0
				emit code to create label FALSELIT, instruction db, operands 'FALSE',0
				output an endl to objectFile
				emit code to resume .text SECTION*/
				if (definedStorage == false)
				{
					definedStorage = true;
					objectFile << "\n";
					emit("SECTION", ".data");
					emit("TRUELIT", "db", "'TRUE',0", "; TRUE Literal");
					emit("FALSLIT", "db", "'FALSE',0", "; FALSE Literal");
					objectFile << "\n";
					emit("SECTION", ".text");
				}			
			}
			//emit code to call the Irvine Crlf function
			emit("","call","Crlf","; Crlf function called")
		}
		// This may not be needed - C
		name = "";
	 }

 }

 void emitAssignCode(string operand1, string operand2); // op2 = op1
 {	
	//if types of operands are not the same
	//processError(incompatible types)
	if (symbolTable.at(operand1).getDataType() !=symbolTable.at(operand2).getDataType())
	{
	 processError("incompatible types");
	}
	//if storage mode of operand2 is not VARIABLE
	//processError(symbol on left-hand side of assignment must have a storage mode of VARIABLE)

	if (symbolTable.at(operand2).getMode() != VARIABLE)
	{
	 processError("symbol on left-hand side of assignment must have a storage mode of VARIABLE");
	}

	//if operand1 = operand2 return
	if (operand1 == operand2)
	{
		return;
	}

	//if operand1 is not in the A register then
	if (contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		//emit code to load operand1 into the A register
		emit("","mov","eax[" + symbolTable.at(operand1).getInternalName() + "]", ";load " + operand1 +" in A register");
	}
	//emit code to store the contents of that register into the memory location pointed to by operand2
	emit("","mov","[" + symbolTable.at(operand2).getInternalName() + "],eax", ";store contents at " + operand2);
	//set the contentsOfAReg = operand2
	contentsOfAReg = symbolTable.at(operand2).getInternalName();

	//if operand1 is a temp then free its name for reuse
	if (isTemporary(operand1))
	{
		freeTemp();
	}
	//operand2 can never be a temporary since it is to the left of ':='
 }

 void emitAdditionCode(string operand1, string operand2); // op2 + op1
 {
	 //  Make sure that both operands are defined in the symbol Table
	 /*if (symbolTable.count(operand1) == 0 || symbolTable.count(operand2) == 0)
	 {
		 processError("operands conatin a reference to an undefined symbol");
	 }

	//if type of either operand is not integer
	//processError(illegal type)
	 if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
	 {
		 processError("Illegal type");
	 }

	//if the A Register holds a temp not operand1 nor operand2 then
	//emit code to store that temp into memory
	//change the allocate entry for the temp in the symbol table to yes
	//deassign it
	 if (contentsOfAReg[0] == 'T' && (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName()))
	{
		emit("","mov","[" + contentsOfAReg + "]","eax", "deassign A Register");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}

	// if the A register holds a non-temp not operand1 nor operand2 then deassign it
	if (symbolTable.count(contentsOfAReg) != 0 && contentsOfAReg[0] != 'T' && (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName()))
	{
		contentsOfAReg = "";
	}

	// if neither operand is in the A register then
	// emit code to load operand2 into the A register
	// emit code to perform register-memory addition
	if (symbolTable.at(operand1).getInternalName() != contentsOfAReg && symbolTable.at(operand2).getInternalName() != contentsOfAReg)\
	{
		emit("","mov","[" + symbolTable.at(operand2).getInternalName() + "]","eax", "A Register =" + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("", "add", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand1 + " + " + operand2);
	}
	else if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("", "add", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " + " + operand1);
	}

	// deassign all temporaries involved in the addition and free those names for reuse
	// A Register = next available temporary name and change type of its symbol table entry to integer
	// push the name of the result onto operandStk
	if (isTemporary(operand1))
	{
		freeTemp();
	}
	if (isTemporary(operand2))
	{
		freeTemp();
	}

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	pushOperand(contentsOfAReg);
}
	// CAM ABOVE */

	//make sure that neither of these are undefined
	if (symbolTable.count(operand1) == 0 || symbolTable.count(operand2) == 0)
	{
		processError("operand makes a reference to an undefined symbol");
	}

	//if type of either operand is not integer
	if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
	{
		processError("Illegal data type. Expected INTEGER");
	}

	//if the A Register holds a temp not operand1 nor operand2
	if (symbolTable.at(operand1).genInternalName() != contentsOfAReg && symbolTable.at(operand2).genInternalName() != contentsOfAReg && contentsOfAReg[0] == 'T')
	{
		//emit code to store that temp into memory (store contentsofareg? - Z)
		//store contentsofAReg into eax by emitting assembly code
		emit("", "mov", "[" + contentsOfAReg + "]", "eax", "deassign A register");		//do we need commas in here?
		
		//change the allocate entry for the temp in the symbol table to yes
		symbolTable.at(contentsOfAReg).setAlloc("YES");

		//deassign it
		contentsOfAReg = "";
	}

	//if the A register holds a non-temp not operand1 nor operand2
	if (symbolTable.at(operand1).genInternalName() != contentsOfAReg && symbolTable.at(operand2).genInternalName() != contentsOfAReg && !contentsOfAReg.empty() && contentsOfAReg[0] != 'T')
	{
		//deassign it
		contentsOfAReg = "";
	}

	//if neither operand is in the A register then
	if (symbolTable.at(operand1).genInternalName() != contentsOfAReg && symbolTable.at(operand2).genInternalName() != contentsOfAReg)		
	{
		//emit code to load operand2 into the A register
		emit("", "mov", "eax", "[" + symbolTable.at(operand2).genInternalName() + "]", "A register =" + operand2)		//CHECK THIS

		//emit code to perform register-memory addition

		//deassign all temporaries involved in the addition and free those names for reuse
		if (isTemporary(operand1))
		{
			freeTemp();		
		}
		if (isTemporary(operand2))
		{
			freeTemp();
		}

		//A Register = next available temporary name and change type of its symbol table entry to integer
		contentsOfAReg = getTemp();
		symbolTable.at(contentsOfAReg).setDataType(INTEGER);

		//push the name of the result onto operandStk
		pushOperand(contentsOfAReg);
	}
 }

 void emitSubtractionCode(string operand1, string operand2); // op2 - op1
 {
	 //  Make sure that both operands are defined in the symbol Table
	 if (symbolTable.count(operand1) == 0 || symbolTable.count(operand2) == 0)
	 {
		 processError("operands conatin a reference to an undefined symbol");
	 }

	//if type of either operand is not integer
	//processError(illegal type)
	 if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
	 {
		 processError("Illegal type");
	 }

	//if the A Register holds a temp not operand1 nor operand2 then
	//emit code to store that temp into memory
	//change the allocate entry for the temp in the symbol table to yes
	//deassign it
	 if (contentsOfAReg[0] == 'T' && (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName()))
	{
		emit("","mov","[" + contentsOfAReg + "]","eax", "deassign A Register");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}

	// if the A register holds a non-temp not operand1 nor operand2 then deassign it
	if (symbolTable.count(contentsOfAReg) != 0 && contentsOfAReg[0] != 'T' && (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName()))
	{
		contentsOfAReg = "";
	}

	// if neither operand is in the A register then
	// emit code to load operand2 into the A register
	// emit code to perform register-memory addition
	if (symbolTable.at(operand1).getInternalName() != contentsOfAReg && symbolTable.at(operand2).getInternalName() != contentsOfAReg)
	{
		emit("","mov","[" + symbolTable.at(operand2).getInternalName() + "]","eax", "A Register =" + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("", "sub", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand1 + " - " + operand2);
	}
	else if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("", "sub", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " - " + operand1);
	}

	// deassign all temporaries involved in the addition and free those names for reuse
	// A Register = next available temporary name and change type of its symbol table entry to integer
	// push the name of the result onto operandStk
	if (isTemporary(operand1))
	{
		freeTemp();
	}
	if (isTemporary(operand2))
	{
		freeTemp();
	}

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	pushOperand(contentsOfAReg);
}
 }

 void emitMultiplicationCode(string operand1, string operand2); // op2 * op1
 {
	  //  Make sure that both operands are defined in the symbol Table
	 if (symbolTable.count(operand1) == 0 || symbolTable.count(operand2) == 0)
	 {
		 processError("operands conatin a reference to an undefined symbol");
	 }

	//if type of either operand is not integer
	//processError(illegal type)
	 if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
	 {
		 processError("Illegal type");
	 }

	//if the A Register holds a temp not operand2 then
	//emit code to store that temp into memory
	//change the allocate entry for the temp in the symbol table to yes
	//deassign it
	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov","[" + contentsOfAReg + "]","eax", "deassign A Register");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}

	// if the A register holds a non-temp not operand2 then deassign it
	if (symbolTable.count(contentsOfAReg) != 0 && !isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		contentsOfAReg = "";
	}

	// if operand2 is not in the A register
	// emit instruction to do a register-memory load of operand2 into the A register
	if (symbolTable.at(operand2).getInternalName() != contentsOfAReg)
	{
		emit("","mov","[" + symbolTable.at(operand2).getInternalName() + "]","eax", "A Register =" + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	// emit code to extend sign of dividend from the A register to edx:eax
	// emit code to perform a register-memory division

	// something here

	// deassign all temporaries involved in the addition and free those names for reuse
	// A Register = next available temporary name and change type of its symbol table entry to integer
	// push the name of the result onto operandStk
	if (isTemporary(operand1))
	{
		freeTemp();
	}
	if (isTemporary(operand2))
	{
		freeTemp();
	}

	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);
	pushOperand(contentsOfAReg);
 }

 void emitDivisionCode(string operand1, string operand2); // op2 / op1
 {
	//if type of either operand is not integer
	if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
	{
		processError();
	}

	//if the A Register holds a temp not operand2
	if (contentsOfAReg != symbolTable.at(operand2).genInternalName())
	{
		//emit code to store that temp into memory

		//change the allocate entry for it in the symbol table to yes
		symbolTable.at(contentsOfAReg).setAlloc("YES");

		//deassign it
		contentsOfAReg = "";
	}

	//if the A register holds a non-temp not operand2
	if (!contentsOfAReg.empty() && contentsOfAReg[0] != 'T' && contentsOfAReg != symbolTable.at(operand2).genInternalName())
	{
		//deassign it
		contentsOfAReg = "";
	}

	//if operand2 is not in the A register
	if (symbolTable.at(operand2).genInternalName() != contentsOfAReg)
	{
		//emit instruction to do a register-memory load of operand2 into the A register

		//emit code to extend sign of dividend from the A register to edx:eax

		//emit code to perform a register-memory division

		//deassign all temporaries involved and free those names for reuse
		if (isTemporary(operand1))
		{
			freeTemp();
		}
		if (isTemporary(operand2))
		{
			freeTemp();
		}

		//A Register = next available temporary name and change type of its symbol table entry to integer	(this needs to be looked at further)
		contentsOfAReg = getTemp();
		symbolTable.at(contentsOfAReg).setDataType(INTEGER);

		//push the name of the result onto operandStk	(this needs to be looked at further)
		pushOperand(contentsOfAReg);
	}
 }

 void emitModuloCode(string operand1, string operand2); // op2 % op1
 {
	if (symbolTable.count(operand1) == 0 || symbolTable.count(operand2) == 0)
	{
		processError("operand makes a reference to an undefined symbol");
	}

	//if type of either operand is not integer
	if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
	{
		processError("Illegal data type. Expected INTEGER");
	}

	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov","[" + contentsOfAReg + "]","eax", "Deassign A Register");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}

	// if the A register holds a non-temp not operand2 then deassign it
	if (symbolTable.count(contentsOfAReg) != 0 && !isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		contentsOfAReg = "";
	}



 }

 void emitNegationCode(string operand1, string = ""); // -op1
 {

 }

 void emitNotCode(string operand1, string = ""); // !op1
 {

 }

 void emitAndCode(string operand1, string operand2); // op2 && op1
 {
	 //if type of either operand is not boolean
	if (symbolTable.at(operand1).getDataType() != BOOLEAN || symbolTable.at(operand2).getDataType() != BOOLEAN)
	{
		//processError(illegal type)
		processError("illegal type");
	}
	//if the A Register holds a temp not operand1 nor operand2
	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		//emit code to store that temp into memory
		emit("","mov", "[" + contentsOfAReg + "],eax", ";store temp into memory")
		//change the allocate entry for the temp in the symbol table to yes
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		//deassign it
		contentsOfAReg = "";
	}
	//if the A register holds a non-temp not operand1 nor operand2 then deassign it
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		//deassign it
		contentsOfAReg = "";
	} 

	//if neither operand is in the A register then
	if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		//emit code to load operand2 into the A register
		emit("","mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "],eax", ";load operand2 into A register");
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	//emit code to perform register-memory and
	emit("","and", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", ";A register = " + operand1 + " and " + operand2);

	//deassign all temporaries involved in the and operation and free those names for reuse
	if (isTemporary(operand1))
	{
		freeTemp();
	}
	if (isTemporary(operand2))
	{
		freeTemp();
	}

	//A Register = next available temporary name and change type of its symbol table entry to boolean
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

	//push the name of the result onto operandStk
	pushOperand(contentsOfAReg);
 }

 void emitOrCode(string operand1, string operand2); // op2 || op1
 {

 }

 void emitEqualityCode(string operand1, string operand2); // op2 == op1
 {
	//if types of operands are not the same
	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
	{
		processError("incompatible types");
	}

	//if the A Register holds a temp not operand1 nor operand2 then 
	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		//emit code to store that temp into memory
		emit("","mov", "[" + contentsOfAReg + ,"],eax", ";store temp in register A");
		//change the allocate entry for it in the symbol table to yes
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		//deassign it
		contentsOfAReg = "";
	}

	//if the A register holds a non-temp not operand2 nor operand1 then deassign it
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		//deassign it
		contentsOfAReg = "";
	}

	// if neither operand is in the A register then
	if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		//emit code to load operand2 into the A register
		emit("","mov", "eax,[" + symbolTable.at(operand2).getInternalName() + ,"]", ";load " + operand2 + " into A register");
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	//emit code to perform a register-memory compare
	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " to " + operand1);
	}
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " to " + operand2);
	}

	//emit code to jump if equal to the next available Ln (call getLabel)
	string newLabel = getLabel();

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","je", "." + newLabel + "]", "; jump to " + newLabel " if " + operand2 + " == " + operand1);
	}
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","je", "." + newLabel + "]", "; jump to " + newLabel " if " + operand1 + " == " + operand2);
	}

	//emit code to load FALSE into the A register
	emit("", "mov", "eax,[FALSE]", "; load false into register A")

	//insert FALSE in symbol table with value 0 and external name false
	if (symbolTable.count("false") == 0)
	{
		insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
	}

	string secondLabel = getLabel();
	//emit code to perform an unconditional jump to the next label (call getLabel should be L(n+1))
	emit("","jmp","." + secondLabel, "; unconditional jump to " + secondLabel)
	//emit code to load TRUE into A register
	emit("", "mov", "eax,[TRUE]", "; load true into register A")

	//insert TRUE in symbol table with value -1 and external name true
	if (symbolTable.count("true") == 0)
	{
		insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
	}

	//emit code to label the next instruction with the second acquired label L(n+1)
	emit("." + secondLabel + ":")

	//deassign all temporaries involved and free those names for reuse
	if (isTemporary(operand1))
	{
		freeTemp();
	}
	if (isTemporary(operand2))
	{
		freeTemp();
	}

	//A Register = next available temporary name and change type of its symbol table entry to boolean
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

	//push the name of the result onto operandStk
	pushOperand(contentsOfAReg);
 }

 void emitInequalityCode(string operand1, string operand2); // op2 != op1
 {

 }

 void emitLessThanCode(string operand1, string operand2); // op2 < op1
 {
	 
 }

 void emitLessThanOrEqualToCode(string operand1, string operand2); // op2 <= op1
 {

 }

 void emitGreaterThanCode(string operand1, string operand2); // op2 > op1
 {

 }

 void emitGreaterThanOrEqualToCode(string operand1, string operand2); // op2 >= op1
 {

 }


// ---------------------------------------------------------------------------------

string Compiler::nextToken()    //returns the next token or end of file marker {            
{
	token = "";	
	while(token == "")
	{
		if (ch == '{')
		{
			while (nextChar() != END_OF_FILE && ch != '}')
			{
				
			}
			if (ch == END_OF_FILE)
			{
				processError("unexpected end of file: '}' expected");
			}
			else 
			{
				nextChar();
			}
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

			//Assignment operation :=
			if (token == ":" && ch == '=') 
			{
				token += ch;
				nextChar();
			}
			
			// Not sure if this is needed - C
			//if ((token == "<" && ch == '=') || (token == "<" && ch == '>') || (token == ">" && ch == '=')) 
			//{
			//	token += ch;
			//	nextChar();
			//}
		}
		else if (islower(ch))
		{
			token = ch;

			while((nextChar() == '_' || islower(ch) || isupper(ch) || isdigit(ch)) && ch != END_OF_FILE)
			{
				token = token + ch;
			}

			if (ch == END_OF_FILE)
			{
				processError("unexpected end of file");
			}
		}
		else if (isdigit(ch))
		{
			token = ch;
			
			while (nextChar() != END_OF_FILE && isdigit(ch))
			{
				token += ch;
			}

			if (ch == END_OF_FILE)
			{
				processError("unexpected end of file");
			}
		}

		else if (ch == END_OF_FILE)
		{
			token = ch;
		}
		else
		{
			processError("illegal symbol");
		}
	}
	
	token = token.substr(0,15);

	return token;
}

// ---------------------------------------------------------------------------------

char Compiler::nextChar()   //returns the next character or end of file marker
{ 
	// read in next character   
	sourceFile.get(ch);
  
	static char prev = '\n';

	if (sourceFile.eof())
	{
		ch = END_OF_FILE;
	}
	else 
	{
		// print to listing file (starting new line if necessary) 
		if (prev == '\n')
		{
		// ADD A NEW LINE COMPONENT HERE
		lineNo += 1;
		listingFile << right << setw(5) << lineNo << '|';    
		}
		
		listingFile << ch;  
	}
	
	prev = ch;
  
	return ch;
}

void Compiler::pushOperator(string name) //push name onto operatorStk
{
 	operatorStk.push(name);
}

void Compiler::pushOperand(string name) //push name onto operandStk
 //if name is a literal, also create a symbol table entry for it
{
 	if (isLiteral(name) && symbolTable.count(name) == 0)																					//name is a literal and has no symbol table entry
	{
 		symbolTable.insert(pair<string, SymbolTableEntry>(name.substr(0, 15), SymbolTableEntry(name, whichType(name), inMode, inValue, inAlloc, inUnits)));			//insert symbol table entry, call whichType to determine the data type of the literal
		 // may want to be like this instead insert(x,whichType(y),CONSTANT,whichValue(y),YES,1); 
 		operandStk.push(name);																																		//push name onto stack;
	}
}

string Compiler::popOperator() //pop name from operatorStk
{

 if (!operatorStk.empty())
 {
	string top = operatorStk.top();
	operatorStk.pop()
 	return top;			//top element removed from stack;		// ???does this mean we remove the top of the stack and return that element or do we remove the top of the stack and return the stack???
 }
 else
 {
 	processError("compiler error; operator stack underflow");
 }

}

string Compiler::popOperand() //pop name from operandStk
{

 if (!operandStk.empty())
 {
	string top = operandStk.top();
	operandStk.pop();
 	return top;
 else
 {
 	processError("compiler error; operand stack underflow");
 }

}

void freeTemp()
{
 currentTempNo--;
 if (currentTempNo < -1)
 {
   processError("compiler error, currentTempNo should be >= –1");
 }
}

string getTemp()
{
 string temp;
 currentTempNo++;

 temp = "T" + currentTempNo;

 if (currentTempNo > maxTempNo)
 {
  insert(temp, UNKNOWN, VARIABLE, "", NO, 1);
 }

 maxTempNo++;
 return temp;
}

// if s[0] == 'T' then s is temporary
bool Compiler::isTemporary(string s) const 
{
	if (s[0] == 'T') 
	{
		return true;
	}
	return false;
}