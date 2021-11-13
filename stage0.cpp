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

    nextToken();
    code("end", ".");
}

void execStmts(); // stage 1, production 2
{
    if (isNonKeyId(token) || token == "read" || token == "write")
    {
      execStmt();
      execStmts();
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
      processError();
    }
}

void assignStmt(); // stage 1, production 4
{
  if (!isNonKeyId(token))
  {
    processError();
  }

  pushOperand(token);

  if (nextToken() != ":=")
  {
    processError();
  }

  pushOperator(":=");

  express();

  if (nextToken() != ";")
  {
    processError();
  }

  code(popOperator(), popOperand(), popOperand());

}

void readStmt(); // stage 1, production 5
{
  if (nextToken() != '(')
  {
    processError();
  }

  x = ids();

  if (nextToken() != ')')
  {
    processError();
  }

  code("read", x);
}

void writeStmt(); // stage 1, production 7
{
  if (nextToken() != '(')
  {
    processError();
  }

  x = ids();

  if (nextToken() != ')')
  {
    processError();
  }

  code("write", x);
}

void express(); // stage 1, production 9
{
  term();
  expresses();
}

void expresses(); // stage 1, production 10
{
  x = nextToken();

  if (x != "=" || x != "<>" || x != "<=" || x != ">=" || x != "<" || x != ">")    //not sure if I need to use token or nextToken. This is true for most of the code I have written so far - Z
  {
    processError();
  }

  pushOperator(x);

  term();

  code(popOperator(), popOperand(), popOperand())
}

void term(); // stage 1, production 11
{
  factor();
  terms();
}

void terms(); // stage 1, production 12   //need to account for epsilon some how
{
  x = nextToken();

  if (x != "+" || x != "-" || x != "or")
  {
    processError();
  }

  pushOperator(x);
  factor();
  code(popOperator(), popOperand(), popOperand());
  terms();
}

void factor(); // stage 1, production 13
{
  part();
  factors();
}

void factors(); // stage 1, production 14	// need to account for epsilon move
{
	x = nextToken();		//idk if we need to use nextToken or just token here
	if (x != "*" || x != "div" || x != "mod" || x != "and")
	{
		processError();
	}

	pushOperator(x);
	part();
	code(popOperator(), popOperand(), popOperand());
	factors();
}

void part(); // stage 1, production 15
{

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
    emit read code
  }

  else if (op == "write")
  {
    emit write code
  }

  else if (op == "+") // this must be binary '+'
  {
    emit addition code
  }

  else if (op == "-") // this must be binary '-'
  {
    emit subtraction code
  }

  else if (op == "neg") // this must be unary '-'
  {
    emit negation code;
  }

  else if (op == "not")
  {
    emit not code
  }

  else if (op == "*")
  {
    emit multiplication code
  }

  else if (op == "div")
  {
    emit division code
  }
  else if (op == "mod")
  {
    emit modulo code
  }

  else if (op == "and")
  {
    emit and code
  }
 …
 else if (op == "=")
 emit equality code
 else if (op == ":=")
 emit assignment code
 else
 processError(compiler error since function code should not be called with
 illegal arguments)
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

 }

 void emitWriteCode(string operand, string = "");
 {

 }

 void emitAssignCode(string operand1, string operand2); // op2 = op1
 {

 }

 void emitAdditionCode(string operand1, string operand2); // op2 + op1
 {

 }

 void emitSubtractionCode(string operand1, string operand2); // op2 - op1
 {

 }

 void emitMultiplicationCode(string operand1, string operand2); // op2 * op1
 {

 }

 void emitDivisionCode(string operand1, string operand2); // op2 / op1
 {

 }

 void emitModuloCode(string operand1, string operand2); // op2 % op1
 {

 }

 void emitNegationCode(string operand1, string = ""); // -op1
 {

 }

 void emitNotCode(string operand1, string = ""); // !op1
 {

 }

 void emitAndCode(string operand1, string operand2); // op2 && op1
 {
	 
 }

 void emitOrCode(string operand1, string operand2); // op2 || op1
 {

 }

 void emitEqualityCode(string operand1, string operand2); // op2 == op1
 {

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