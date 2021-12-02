//Zac Caffey and Cameron Ley
//CS 4301
//Compiler Stage 2

#include <ctime>                //This is to allow us to calculate the current time
#include <iomanip>              //This is to enable use of setw()
#include <stage2.h>
#include <iostream>

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
	listingFile << "STAGE1:  Zac Caffey and Cameron Ley       " << time << endl;
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

//process begin-end statement
void Compiler::beginEndStmt()	//stage 1 production 1
{   
    string error;

    if (token != "begin")
    {
		error = "keyword \"begin\" expected";
		processError(error);
    }

    nextToken();
	
	if (isNonKeyId(token) || token == "read" || token == "write" || token == ";" || token == "begin") 
	{
		execStmts();	//make call to execStmts
	}

    if (token != "end")
    {
		error =  "keyword \"end\" expected";
		processError(error);
    }

    if (nextToken() != ".") 
    {
		processError("period expected");
    }

    nextToken();
    code("end", ".");	//code "end"
}

//process execution statements
void Compiler::execStmts()	//stage 1 production 2
{  
    if (isNonKeyId(token) || token == "read" || token == "write" || token == "begin")
	{
		execStmt();
		nextToken();
		execStmts();
	}
	
	else if (token == "end");

	else
	{
		processError("non-keyword identifier, \"read\", \"write\", or \"begin\" expected");		//error here
	}
}

//process execution statement
void Compiler::execStmt()	//stage 1 production 3
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

//process assign statement
void Compiler::assignStmt()	//stage 1 production 4
{
    string first, second;

    if (!isNonKeyId(token))
    {
      processError("non - keyword identifier expected");
    }
  
    if (symbolTable.count(token) == 0)	//if the token is not defined in our symbol table
    {
	  processError("reference to undefined variable");
    }

    pushOperand(token);
	nextToken();

	if (token != ":=")
	{
		processError("':=' expected; found " + token);
	}

	pushOperator(":=");	//push the operator ":=" onto the stack
	nextToken();
  
	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token) && token != ";")
	{
		processError("expected non_key_id, integer, \"not\", \"true\", \"false\", '(', '+', or '-'");
	}

	express();
  
	second = popOperand();
	first = popOperand();

	code(popOperator(), second, first);

}

//process read statement
void Compiler::readStmt()	//stage 1 production 5
{
	string x;
	if (token != "read")
	{	
		processError("read expected; found " + token);
	}

	nextToken();

	if (token != "(")
	{
		processError("'(' expected; found " + token);
	}

	nextToken();
	x = ids();
  
	if (token != ")")
	{
		processError("',' or ')' expected; found " + token);
	}

	code("read", x);
	nextToken();

	if (token != ";")
	{
		processError("';' expected; found " + token);
	}
}

//process write statement
void Compiler::writeStmt()	//stage 1 production 7
{
	string x;
	if (token != "write")
	{
		processError("write expected; found " + token);
	}

	nextToken();

	if (token != "(")
	{
		processError("'(' expected after \"write\"");
	}

	nextToken();
	x = ids();

	if (token != ")")
	{
		processError("',' or ')' expected; found " + token);
	}

	code("write", x);
	nextToken();

	if (token != ";")
	{
	processError("';' expected");
	}
}
void Compiler::ifStmt() // stage 2, production 3
{
	string temp;

	if (token != "if")
	{
		processError("error");
	}

	nextToken();

	//prereq for calling express
	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token) && token != ";")
  	{
		processError("one of \"*\", \"and\", \"div\", \"mod\", \")\", \"+\", \"-\", \";\", \"<\", \"<=\", \"<>\", \"=\", \">\", \">=\", or \"or\" expected");
  	}
	  
	express();
	nextToken();

	if (token != "then")
	{
		processError("error");
	}

	temp = popOperand();
	code("then", temp);
	nextToken();

	if (isNonKeyId(token) || token == "read" || token == "write" || token == "if" || token == "while" || token == "repeat" || token == ";" || token == "begin")
	{
		execStmt();
		nextToken();	//not sure if this is necessary
		elsePt();
	}
}
 
// ---------------------------------------------------------------------------------

void Compiler::elsePt() // stage 2, production 4
{
	if (token == "else")
	{
		code("else", popOperand());
		nextToken();

		if (isNonKeyId(token) || token == "read" || token == "write" || token == "if" || token == "while" || token == "repeat" || token == ";" || token == "begin")
		{
			execStmt();
		}

		code("post_if", popOperand());
	}
	else
	{
		code("post_if", popOperand());
	}
}

 // ---------------------------------------------------------------------------------

void Compiler::whileStmt() // stage 2, production 5
{
	if (token != "while")
	{
		processError("error");
	}

	code("while");
	nextToken();

	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token) && token != ";")
 	{
		processError("one of \"*\", \"and\", \"div\", \"mod\", \")\", \"+\", \"-\", \";\", \"<\", \"<=\", \"<>\", \"=\", \">\", \">=\", or \"or\" expected");
 	}

	express();
	
	if (nextToken() != "do")
	{
		processError("error");
	}

	code("do", popOperand());
	nextToken();

	if (isNonKeyId(token) || token == "read" || token == "write" || token == "if" || token == "while" || token == "repeat" || token == ";" || token == "begin")
	{
		execStmt();
	}

	code("post_while", popOperand(), popOperand());
}

 // ---------------------------------------------------------------------------------

void Compiler::repeatStmt() // stage 2, production 6
{
	if (token != "repeat")
	{
		processError("error");
	}

	code("repeat");
	nextToken();

	if (!isNonKeyId(token) && token != "read" && token != "write")
	{
		processError("error");
	}

	execStmts();

	if (nextToken() != "until")
	{
		processError("error");
	}

	express();
	code("until", popOperand(), popOperand());
	
	if (nextToken() != ";")
	{
		processError("error");
	}
}

 // ---------------------------------------------------------------------------------

void Compiler::nullStmt() // stage 2, production 7
{
	if (token != ";")
	{
		processError("error");
	}
}

// {'not','true','false','(','+','-', INTEGER, NON_KEY_ID}
void Compiler::express()	//stage 1 production 9
{
	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token))
	{
		processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", non - keyword identifier or integer expected");
	}

	term();

	if (token == "<>" || token == "=" || token == "<=" || token == ">=" || token == "<" || token == ">")
	{
		expresses();
	}
}

// {'<>','=','<=','>=','<','>'}
void Compiler::expresses()	//stage 1 production 10
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

// {'not','true','false','(','+','-',INTEGER,NON_KEY_ID}
void Compiler::term()	//stage 1 production 11
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

// {'-','+','or'}
void Compiler::terms() //stage 1 production 12
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

// {'not','true','false','(','+','-',INTEGER,NON_KEY_ID}
void Compiler::factor()	//stage 1 production 13
{
	// FACTOR {'not','true','false','(','+','-',INTEGER,NON_KEY_ID}
	if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token))
	{
		processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", INTEGER, or NON_KEY_ID expected");
	}
	
	// PART 
	part();

	// FACTORS {'*','div','mod','and'}
	if (token == "*" || token == "div" || token == "mod" || token == "and")
	{
		factors();
	}

	//{'<>','=','<=','>=','<','>',')',';','-','+','or'}
	else if (token == "<>" || token == "=" || token == "<=" || token == ">=" || token == "<" || token == ">" || token == ")" || token == ";" || token == "-" || token == "+" || token == "or" || token == "begin")
	{

	}

	else 
	{
		processError("expected '(', integer, or non_key_id");
	}
}

// {'*','div','mod','and'}
void Compiler::factors()	//stage 1 production 14
{
	string first, second;

	if (token != "*" && token != "div" && token != "mod" && token != "and")
	{
	  processError("\"*\", \"div\", \"mod\", or \"and\" expected");
	}

	pushOperator(token);
	nextToken();
	
	if (token != "not" && token != "(" && !isBoolean(token) && !isNonKeyId(token) && token != "+" && token != "-" && token != "true" && token != "false")
	{
		processError("expected '(', boolean, or non-keyword id");
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

// {"not", '+', '-', '(', INTEGER, BOOLEAN, NON_KEYID}
void Compiler::part()	//stage 1 production 15
{
	string x = "";
	if (token == "not")
	{
		nextToken();
		
		if (token == "(") 
		{
			nextToken();
			
			if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token))
			{
				processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
			}
			
			express();
			
			if (token != ")")
			{
				processError(") expected; found " + token);
			}
			
			nextToken();
			code("not", popOperand());
		}

		else if (isBoolean(token)) 
		{
			if (token == "true") 
			{
				pushOperand("false");
				nextToken();
			}
			else 
			{
				pushOperand("true");
				nextToken();
			}
		}

		else if (isNonKeyId(token)) 
		{
			code("not", token);
			nextToken();
		}
	}

	else if (token == "+")
	{
		nextToken();
		if (token == "(") 
		{
			nextToken();
			
			if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token))
			{
				processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
			}
			
			express();
			
			if (token != ")")
			{
				processError("expected ')'; found " + token);
			}
			
			nextToken();
		}
		
		else if (isInteger(token) || isNonKeyId(token)) 
		{
			pushOperand(token);
			nextToken();
		}

		else 
		{
			processError("expected '(', integer, or non-keyword id; found " + token);
		}
	}

	else if (token == "-")
	{
		nextToken();
		
		if (token == "(") 
		{
			nextToken();
			
			if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token))
			{
				processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
			}
			
			express();
			
			if (token != ")")
			{
				processError("expected ')'; found " + token);
			}
			
			nextToken();
			code("neg", popOperand());
		}
		
		else if (isInteger(token)) 
		{
			pushOperand("-" + token);
			nextToken();
		}
		
		else if (isNonKeyId(token)) 
		{
			code("neg", token);
			nextToken();
		}
	}

	else if (token == "(") 
	{
		nextToken();
		
		if (token != "not" && token != "true" && token != "false" && token != "(" && token != "+" && token != "-" && !isInteger(token) && !isNonKeyId(token))
		{
			processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, or non - keyword identifier expected");
		}
		
		express();
		
		if (token != ")") 
		{
			processError(") expected; found " + token);
		}
		
		nextToken();
	}

	else if (isInteger(token) || isBoolean(token) || isNonKeyId(token)) 
	{
		pushOperand(token);
		nextToken();
	}

	else 
	{
		processError("\"not\", \"true\", \"false\", \"(\", \"+\", \"-\", integer, boolean, or non - keyword identifier expected");
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
	if (s == "program" || s == "const" || s == "var" || s == "integer" || s == "boolean" || s == "begin" || s == "end" || s == "true" || s == "false" || s == "not" || s == "mod" || s == "div" || s == "and" || s == "or" || s == "read" || s == "write")
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
	if (c == ':' || c == ',' || c == ';' || c == '=' || c == '+' || c == '-' || c == '.' || c == '*' || c == '<' || c == '>' || c == '(' || c == ')')
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

bool Compiler::isLiteral(string s) const //determines if s is a literal
{
	//bool integer = isInteger(s);
	if (isInteger(s) || isBoolean(s) || s.front() == '+' || s.front() == '-')// s[0] == "+" || s[0] == "-")
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
	static int countI = 0, countB = 0, countU = 0;
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
		case UNKNOWN:
		{
			internal = "U" + to_string(countU);
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
			else if (isKeyword(name) && name != "true" && name != "false")
			{
				processError("illegal use of keyword");
			}
			else 
			{
				if (isupper(name[0]) || name == "true" || name == "false")
				{
					if (name == "true")
					{
						symbolTable.insert(pair<string, SymbolTableEntry>(name.substr(0, 15),
						SymbolTableEntry("TRUE", inType, inMode, inValue, inAlloc, inUnits)));
					}
					else if (name == "false")
					{
						symbolTable.insert(pair<string, SymbolTableEntry>(name.substr(0, 15),
						SymbolTableEntry("FALSE", inType, inMode, inValue, inAlloc, inUnits)));
					}
					else 
					{
						symbolTable.insert(pair<string, SymbolTableEntry>(name.substr(0, 15),
						SymbolTableEntry(name, inType, inMode, inValue, inAlloc, inUnits)));
					}
				}
				else
				{
					symbolTable.insert(pair<string, SymbolTableEntry>(name.substr(0, 15),
					SymbolTableEntry(genInternalName(inType), inType, inMode, inValue, inAlloc, inUnits)));
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
		emitReadCode(operand1, "");
	}

	else if (op == "write")
	{
		emitWriteCode(operand1, "");
	}

	else if (op == "+") // this must be binary '+'
	{
		emitAdditionCode(operand1, operand2);
	}

	else if (op == "-") // this must be binary '-'
	{
		emitSubtractionCode(operand1, operand2);
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
		emitMultiplicationCode(operand1, operand2);
	}

	else if (op == "div")
	{
		emitDivisionCode(operand1, operand2);
	}
	
	else if (op == "mod")
	{
		emitModuloCode(operand1, operand2);
	}

	else if (op == "and")
	{
		emitAndCode(operand1, operand2);
	}
	
	else if (op == "or")
	{
		emitOrCode(operand1, operand2);
	}

	else if (op == "<")
	{
		emitLessThanCode(operand1, operand2);
	}
	
	else if (op == "<=")
	{
		emitLessThanOrEqualToCode(operand1, operand2);
	}

	else if (op == ">")
	{
		emitGreaterThanCode(operand1, operand2);
	}

	else if (op == ">=")
	{
		emitGreaterThanOrEqualToCode(operand1, operand2);
	}

	else if (op == "<>")
	{
		emitInequalityCode(operand1, operand2);
	}
	
	else if (op == "=")
	{
		emitEqualityCode(operand1, operand2);
	}
	
	else if (op == ":=")
	{
		emitAssignCode(operand1, operand2);
	}
	else
	{
		processError("compiler error; function code called with illegal arguments");
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
	objectFile << comment << endl;
}

// ---------------------------------------------------------------------------------

void Compiler::emitPrologue(string progName, string operand2)	//Output identifying comments at beginning of objectFile and Output the %INCLUDE directives
{
	time_t now = time(0);
	char* time = ctime(&now);
	objectFile << "; Zac Caffey and Cameron Ley" << right << setw(6) << "" << time;
	objectFile << "%INCLUDE \"Along32.inc\"\n" << "%INCLUDE \"Macros_Along.inc\"\n" << endl;
	emit("SECTION", ".text");
	emit("global", "_start", "", "; program " + progName.substr(0, 15));
	objectFile << endl;
	emit("_start:");
}

// ---------------------------------------------------------------------------------

void Compiler::emitEpilogue(string operand1, string operand2)	//emits our epilogue to the ASM file and calls emitStorage()
{
	emit("","Exit", "{0}");
	objectFile << endl;
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

// ---------------------------------------------------------------------------------

void Compiler::emitReadCode(string operand, string)
{
	string name;
	unsigned int size = operand.size();
	//while (name is broken from list (operand) and put in name != "")
	for (unsigned int loopC = 0; loopC < size; ++loopC) 
	{
				
		if (operand[loopC] != ',' && loopC < size) 
		{
			name += operand[loopC];
			continue;
		}
		//if name is not empty
		if (name != "") 
		{
			//if name is not in symbol table
			if (symbolTable.count(name) == 0)
			{
				//processError(reference to undefined symbol)
				processError("reference to undefined symbol " + name);
			}

			//if data type of name is not INTEGER
			if (symbolTable.at(name).getDataType() != INTEGER)
			{
				//processError(can't read variables of this type)
				processError("can't read variables of this type");
			}

			//if storage mode of name is not VARIABLE
			if (symbolTable.at(name).getMode() != VARIABLE)
			{
				//processError(attempting to read to a read-only location)
				processError("attempting to read to a read-only location");
			}
			
			//emit code to call the Irvine ReadInt function
			emit("", "call", "ReadInt", "; read int; value placed in eax");
			//emit code to store the contents of the A register at name
			emit("", "mov", "[" + symbolTable.at(name).getInternalName() + "],eax", "; store eax at " + name);
			//set the contentsOfAReg = name
			contentsOfAReg = symbolTable.at(name).getInternalName();
		}

		name = "";
	}

	// check if anything is left inside name
	if (name != "") 
	{
		//if name is not in symbol table
		if (symbolTable.count(name) == 0)
		{
			//processError(reference to undefined symbol)
			processError("reference to undefined symbol " + name);
		}

		//if data type of name is not INTEGER
		if (symbolTable.at(name).getDataType() != INTEGER)
		{
			//processError(can't read variables of this type)
			processError("can't read variables of this type");
		}

		//if storage mode of name is not VARIABLE
		if (symbolTable.at(name).getMode() != VARIABLE)
		{
			//processError(attempting to read to a read-only location)
			processError("attempting to read to a read-only location");
		}
		
		//emit code to call the Irvine ReadInt function
		emit("", "call", "ReadInt", "; read int; value placed in eax");
		//emit code to store the contents of the A register at name
		emit("", "mov", "[" + symbolTable.at(name).getInternalName() + "],eax", "; store eax at " + name);
		//set the contentsOfAReg = name
		contentsOfAReg = symbolTable.at(name).getInternalName();
	}
}

// --------------------------------------------------------------------------------- 

void Compiler::emitWriteCode(string operand, string)
{
	string name;
	static bool definedStorage = false;
	unsigned int size = operand.size();

	//while (name is broken from list (operand) and put in name != "")
	for (unsigned int loopC = 0; loopC < size; ++loopC) 
	{

		if (operand[loopC] != ',' && loopC < size)
		{
			name += operand[loopC];
			continue;
		}

		if (name != "") 
		{
			//if name is not in symbol table
			if (symbolTable.count(name) == 0)
			{
				//processError(reference to undefined symbol)
				processError("reference to undefined symbol " + name);
			}

			//if name is not in the A register
			if (symbolTable.at(name).getInternalName() != contentsOfAReg) 
			{
				//emit the code to load name in the A register
				emit("", "mov", "eax,[" + symbolTable.at(name).getInternalName() + "]", "; load " + name + " in eax");
				//set the contentsOfAReg = name
				contentsOfAReg = symbolTable.at(name).getInternalName();
			}

			//if data type of name is INTEGER
			if (symbolTable.at(name).getDataType() == storeTypes::INTEGER)
			{
				//emit code to call the Irvine WriteInt function
				emit("", "call", "WriteInt", "; write int in eax to standard out");
			}

			//data type is BOOLEAN
			else 
			{
				//emit code to compare the A register to 0
				emit("", "cmp", "eax,0", "; compare to 0");
				//acquire a new label Ln
				string firstL = getLabel();
				//emit code to jump if equal to the acquired label Ln
				emit("", "je", "." + firstL, "; jump if equal to print FALSE");
				//emit code to load address of TRUE literal in the D register
				emit("", "mov", "edx,TRUELIT", "; load address of TRUE literal in edx");
				//acquire a second label L(n + 1)
				string secondL = getLabel();
				//emit code to unconditionally jump to label L(n + 1)
				emit("", "jmp", "." + secondL, "; unconditionally jump to ." + secondL);
				//emit code to label the next line with the first acquired label Ln
				emit("." + firstL + ":");
				//emit code to load address of FALSE literal in the D register
				emit("", "mov", "edx,FALSLIT", "; load address of FALSE literal in edx");
				//emit code to label the next line with the second acquired label L(n + 1)
				emit("." + secondL + ":");
				//emit code to call the Irvine WriteString function
				emit("", "call", "WriteString", "; write string to standard out");

				//if static variable definedStorage is false
				if (definedStorage == false) 
				{
					//set definedStorage to true
					definedStorage = true;
					//output an endl to objectFile
					objectFile << endl;
					//emit code to begin a .data SECTION
					emit("SECTION", ".data");
					//emit code to create label TRUELIT, instruction db, operands 'TRUE',0
					emit("TRUELIT", "db", "'TRUE',0", "; literal string TRUE");
					//emit code to create label FALSELIT, instruction db, operands 'FALSE',0
					emit("FALSLIT", "db", "'FALSE',0", "; literal string FALSE");
					//output an endl to objectFile
					objectFile << endl;
					//emit code to resume .text SECTION
					emit("SECTION", ".text");
				}

			}
			
			//emit code to call the Irvine Crlf function
			emit("", "call", "Crlf", "; write \\r\\n to standard out");
		}
		name = "";
	}
	//check if anything is left inside name
	if (name != "") 
	{
			//if name is not in symbol table
			if (symbolTable.count(name) == 0)
			{
				//processError(reference to undefined symbol)
				processError("reference to undefined symbol " + name);
			}

			//if name is not in the A register
			if (symbolTable.at(name).getInternalName() != contentsOfAReg) 
			{
				//emit the code to load name in the A register
				emit("", "mov", "eax,[" + symbolTable.at(name).getInternalName() + "]", "; load " + name + " in eax");
				//set the contentsOfAReg = name
				contentsOfAReg = symbolTable.at(name).getInternalName();
			}

			//if data type of name is INTEGER
			if (symbolTable.at(name).getDataType() == storeTypes::INTEGER)
			{
				//emit code to call the Irvine WriteInt function
				emit("", "call", "WriteInt", "; write int in eax to standard out");
			}

			//data type is BOOLEAN
			else 
			{
				//emit code to compare the A register to 0
				emit("", "cmp", "eax,0", "; compare to 0");
				//acquire a new label Ln
				string firstL = getLabel();
				//emit code to jump if equal to the acquired label Ln
				emit("", "je", "." + firstL, "; jump if equal to print FALSE");
				//emit code to load address of TRUE literal in the D register
				emit("", "mov", "edx,TRUELIT", "; load address of TRUE literal in edx");
				//acquire a second label L(n + 1)
				string secondL = getLabel();
				//emit code to unconditionally jump to label L(n + 1)
				emit("", "jmp", "." + secondL, "; unconditionally jump to ." + secondL);
				//emit code to label the next line with the first acquired label Ln
				emit("." + firstL + ":");
				//emit code to load address of FALSE literal in the D register
				emit("", "mov", "edx,FALSLIT", "; load address of FALSE literal in edx");
				//emit code to label the next line with the second acquired label L(n + 1)
				emit("." + secondL + ":");
				//emit code to call the Irvine WriteString function
				emit("", "call", "WriteString", "; write string to standard out");

				//if static variable definedStorage is false
				if (definedStorage == false) 
				{
					//set definedStorage to true
					definedStorage = true;
					//output an endl to objectFile
					objectFile << endl;
					//emit code to begin a .data SECTION
					emit("SECTION", ".data");
					//emit code to create label TRUELIT, instruction db, operands 'TRUE',0
					emit("TRUELIT", "db", "'TRUE',0", "; literal string TRUE");
					//emit code to create label FALSELIT, instruction db, operands 'FALSE',0
					emit("FALSLIT", "db", "'FALSE',0", "; literal string FALSE");
					//output an endl to objectFile
					objectFile << endl;
					//emit code to resume .text SECTION
					emit("SECTION", ".text");
				}

			}
			
			//emit code to call the Irvine Crlf function
			emit("", "call", "Crlf", "; write \\r\\n to standard out");
		}
	}
}
// --------------------------------------------------------------------------------- 

void Compiler::emitAssignCode(string operand1, string operand2) // op2 = op1
{	
	// check that neither operand is empty
	if (symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}

	//if types of operands are not the same
	//processError(incompatible types)
	if (symbolTable.at(operand1).getDataType() !=symbolTable.at(operand2).getDataType())
	{
		processError("incompatible types for operator ':='");
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
		emit("","mov","eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand1);
	}
	//emit code to store the contents of that register into the memory location pointed to by operand2
	emit("","mov","[" + symbolTable.at(operand2).getInternalName() + "],eax", "; " + operand2 + " = AReg");
	//set the contentsOfAReg = operand2
	contentsOfAReg = symbolTable.at(operand2).getInternalName();

	//if operand1 is a temp then free its name for reuse
	if (isTemporary(operand1))
	{
		freeTemp();
	}
	//operand2 can never be a temporary since it is to the left of ':='
}

// --------------------------------------------------------------------------------- 

void Compiler::emitAdditionCode(string operand1, string operand2) // op2 + op1
{
	// check that neither operand is empty
	if (symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	// check that neither operand is empty
	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}

	//if type of either operand is not integer
	if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
	{
		processError("binary '+' requires integer operands");
	}

	//if the A Register holds a temp not operand1 nor operand2
	if (symbolTable.at(operand1).getInternalName() != contentsOfAReg && symbolTable.at(operand2).getInternalName() != contentsOfAReg && isTemporary(contentsOfAReg))
	{
		//emit code to store that temp into memory (store contentsofareg? - Z)
		//store contentsofAReg into eax by emitting assembly code
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");		//do we need commas in here?
		
		//change the allocate entry for the temp in the symbol table to yes
		symbolTable.at(contentsOfAReg).setAlloc(YES);

		//deassign it
		contentsOfAReg = "";
	}

	//if the A register holds a non-temp not operand1 nor operand2
	if (symbolTable.at(operand1).getInternalName() != contentsOfAReg && symbolTable.at(operand2).getInternalName() != contentsOfAReg && !isTemporary(contentsOfAReg))
	{
		//deassign it
		contentsOfAReg = "";
	}

	//if neither operand is in the A register then
	if (symbolTable.at(operand1).getInternalName() != contentsOfAReg && symbolTable.at(operand2).getInternalName() != contentsOfAReg)		
	{
		//emit code to load operand2 into the A register
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);		//CHECK THIS
		
		// set A reg == operand 2 
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}
	
	//FIXED 
	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		//emit code to perform register-memory addition with operand 1
		emit("", "add", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " + " + operand1);
	}
	else
	{
		//emit code to perform register-memory addition with operand 2
		emit("", "add", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand1 + " + " + operand2);
	}
	
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

// --------------------------------------------------------------------------------- 

void Compiler::emitSubtractionCode(string operand1, string operand2) // op2 - op1
{
	 // check that neither operand is empty
	if (symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	// check that neither operand is empty
	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}

	//if type of either operand is not integer
	if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
	{
		processError("illegal type. binary '-' requires integer operands");
	}

	//if the A Register holds a temp not operand2
	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		//emit code to store that temp into memory (store contentsofareg? - Z)
		//store contentsofAReg into eax by emitting assembly code
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		//change the allocate entry for the temp in the symbol table to yes
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		//deassign it
		contentsOfAReg = "";
	}

	//if the A register holds a non-temp not operand1 nor operand2
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		contentsOfAReg = "";
	}

	//if operand2 is NOT in the A register then
	if (contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		//emit code to load operand2 into the A register
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		// A Reg == operand2
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	//emit code to perform register-memory subtraction with operand1
	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("", "sub", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " - " + operand1);
	}

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

// ---------------------------------------------------------------------------------

void Compiler::emitMultiplicationCode(string operand1, string operand2) // op2 * op1
{
	 // check that neither operand is empty
	if (symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
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
	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov","[" + contentsOfAReg + "],eax","; deassign AReg");
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		contentsOfAReg = "";
	}

	// if the A register holds a non-temp not operand2 then deassign it
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		contentsOfAReg = "";
	}
	
	
	// if operand2 is not in the A register
	// emit instruction to do a register-memory load of operand2 into the A register
	if (symbolTable.at(operand1).getInternalName() != contentsOfAReg && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		emit("","mov","eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}
	

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("", "imul", "dword [" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " * " + operand1);
	}
	
	else 
	{
		emit("", "imul", "dword [" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand1 + " * " + operand2);	
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

// --------------------------------------------------------------------------------- 

void Compiler::emitDivisionCode(string operand1, string operand2) // op2 / op1
{
	 // check that neither operand is empty
	if (symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}

	//if type of either operand is not integer
	if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
	{
		processError("binary 'div' requires integer operands");
	}

	//if the A Register holds a temp not operand2
	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		//emit code to store that temp into memory
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		//change the allocate entry for it in the symbol table to yes
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		//deassign it
		contentsOfAReg = "";
	}

	//if the A register holds a non-temp not operand2
	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		//deassign it
		contentsOfAReg = "";
	}

	//if operand2 is not in the A register
	if (symbolTable.at(operand2).getInternalName() != contentsOfAReg)
	{
		//emit instruction to do a register-memory load of operand2 into the A register
		emit("","mov","eax,[" + symbolTable.at(operand2).getInternalName() + "]","; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}
	
	//emit code to extend sign of dividend from the A register to edx:eax
	emit("", "cdq", "", "; sign extend dividend from eax to edx:eax");

	//emit code to perform a register-memory division
	//emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
	emit("", "idiv", "dword [" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " div " + operand1);	//not sure if this is right. we will need to add a comment here as well

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

// --------------------------------------------------------------------------------- 

void Compiler::emitModuloCode(string operand1, string operand2) // op2 % op1
{

	// check that neither operand is empty
	if (symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}

	//if type of either operand is not integer
	if (symbolTable.at(operand1).getDataType() != INTEGER || symbolTable.at(operand2).getDataType() != INTEGER)
	{
		processError("binary 'mod' requires integer operands");
	}

	//if the A Register holds a temp not operand2
	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		//emit code to store that temp into memory
		emit("", "mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		//change the allocate entry for it in the symbol table to yes
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		//deassign it
		contentsOfAReg = "";
	}

	//if the A register holds a non-temp not operand2
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		//deassign it
		contentsOfAReg = "";
	}

	//if operand2 is not in the A register
	if (symbolTable.at(operand2).getInternalName() != contentsOfAReg)
	{
		//emit instruction to do a register-memory load of operand2 into the A register
		emit("","mov","eax,[" + symbolTable.at(operand2).getInternalName() + "]","; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}
		
	//emit code to extend sign of dividend from the A register to edx:eax
	emit("", "cdq", "", "; sign extend dividend from eax to edx:eax");

	//emit code to perform a register-memory division
	emit("", "idiv", "dword [" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " div "  + operand1);	//not sure if this is right. we will need to add a comment here as well
	emit("", "xchg", "eax,edx", "; exchange quotient and remainder");

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

// --------------------------------------------------------------------------------- 

void Compiler::emitNegationCode(string operand1, string) // -op1
{
	 // check that neither operand is empty
	if (symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}

	//if type of either operand is not boolean
	if (symbolTable.at(operand1).getDataType() != INTEGER)
	{
		//processError(illegal type)
		processError("illegal type");
	}
	//if the A Register holds a temp not operand1
	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		//emit code to store that temp into memory
		emit("","mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		//change the allocate entry for the temp in the symbol table to yes
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		//deassign it
		contentsOfAReg = "";
	}
	//if the A register holds a non-temp not operand1 then deassign it
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		//deassign it
		contentsOfAReg = "";
	} 

	//if neither operand is in the A register then
	if (contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		//emit code to load operand1 into the A register
		emit("","mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand1);
		contentsOfAReg = symbolTable.at(operand1).getInternalName();
	}

	//emit code to perform register-memory NOT
	emit("","neg", "eax", "; AReg = -AReg");
	
	//deassign all temporaries involved in the and operation and free those names for reuse
	if (isTemporary(operand1))
	{
		freeTemp();
	}

	//A Register = next available temporary name and change type of its symbol table entry to INTEGER
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(INTEGER);

	//push the name of the result onto operandStk
	pushOperand(contentsOfAReg);
}

// --------------------------------------------------------------------------------- 

void Compiler::emitNotCode(string operand1, string) // !op1
{
	// check that neither operand is empty
	if (symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}

	//if type of either operand is not boolean
	if (symbolTable.at(operand1).getDataType() != BOOLEAN)
	{
		//processError(illegal type)
		processError("illegal type");
	}
	//if the A Register holds a temp not operand1
	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		//emit code to store that temp into memory
		emit("","mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		//change the allocate entry for the temp in the symbol table to yes
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		//deassign it
		contentsOfAReg = "";
	}
	//if the A register holds a non-temp not operand1 then deassign it
	if (!isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		//deassign it
		contentsOfAReg = "";
	} 

	//if neither operand is in the A register then
	if (contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		//emit code to load operand1 into the A register
		emit("","mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand1);
		contentsOfAReg = symbolTable.at(operand1).getInternalName();
	}

	//emit code to perform register-memory NOT
	emit("","not", "eax", "; AReg = !AReg");
	
	//deassign all temporaries involved in the and operation and free those names for reuse
	if (isTemporary(operand1))
	{
		freeTemp();
	}

	//A Register = next available temporary name and change type of its symbol table entry to boolean
	contentsOfAReg = getTemp();
	symbolTable.at(contentsOfAReg).setDataType(BOOLEAN);

	//push the name of the result onto operandStk
	pushOperand(contentsOfAReg);
}

// --------------------------------------------------------------------------------- 

void Compiler::emitAndCode(string operand1, string operand2) // op2 && op1
 {
	 // check that neither operand is empty
	if (symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}

	//if type of either operand is not boolean
	if (symbolTable.at(operand1).getDataType() != BOOLEAN || symbolTable.at(operand2).getDataType() != BOOLEAN)
	{
		//processError(illegal type)
		processError("binary 'and' requires boolean operands");
	}
	//if the A Register holds a temp not operand1 nor operand2
	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		//emit code to store that temp into memory
		emit("","mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
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
		emit("","mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	//emit code to perform register-memory and
	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","and", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " and " + operand1);
	}
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","and", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand1 + " and " + operand2);
	}

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

// --------------------------------------------------------------------------------- 

void Compiler::emitOrCode(string operand1, string operand2) // op2 || op1
{
	 // check that neither operand is empty
	if (symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}

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
		emit("","mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
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
		emit("","mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	//emit code to perform register-memory OR
	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","or", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + operand2 + " or " + operand1);
	}
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","or", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand1 + " or " + operand2);
	}

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

// --------------------------------------------------------------------------------- 

void Compiler::emitEqualityCode(string operand1, string operand2) // op2 == op1
{
	// check that neither operand is empty
	if (symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}

	//if types of operands are not the same
	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
	{
		processError("incompatible types");
	}

	//if the A Register holds a temp not operand1 nor operand2 then 
	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		//emit code to store that temp into memory
		emit("","mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
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
		emit("","mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	//emit code to perform a register-memory compare
	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
	}

	//emit code to jump if equal to the next available Ln (call getLabel)
	string newLabel = getLabel();

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","je", "." + newLabel, "; if " + operand2 + " = " + operand1 + " then jump to set eax to TRUE");
	}
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","je", "." + newLabel, "; if " + operand2 + " = " + operand1 + " then jump to set eax to TRUE");
	}

	//emit code to load FALSE into the A register
	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

	//insert FALSE in symbol table with value 0 and external name false
	if (symbolTable.count("false") == 0)
	{
		insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
		symbolTable.at("false").setInternalName("FALSE");
	}

	string secondLabel = getLabel();
	//emit code to perform an unconditional jump to the next label (call getLabel should be L(n+1))
	emit("","jmp","." + secondLabel, "; unconditionally jump");
	
	emit("." + newLabel + ":");
	//emit code to load TRUE into A register
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	//insert TRUE in symbol table with value -1 and external name true
	if (symbolTable.count("true") == 0)
	{
		insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
		symbolTable.at("true").setInternalName("TRUE");
	}

	//emit code to label the next instruction with the second acquired label L(n+1)
	emit("." + secondLabel + ":");

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

// --------------------------------------------------------------------------------- 

void Compiler::emitInequalityCode(string operand1, string operand2) // op2 != op1
{
	// check that neither operand is empty
	if (symbolTable.count(operand1) == 0)
	{
	    processError("reference to undefined symbol " + operand1);
	}
	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}
	
	//if types of operands are not the same
	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
	{
		processError("incompatible types for operator '<>'");
	}

	//if the A Register holds a temp not operand1 nor operand2 then 
	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName()) 
	{
		//emit code to store that temp into memory
		emit("","mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
		//change the allocate entry for it in the symbol table to yes
		symbolTable.at(contentsOfAReg).setAlloc(YES);
		//deassign it
		contentsOfAReg = "";
	}

	//if the A register holds a non-temp not operand2 nor operand1 then deassign it
	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		//deassign it 
		contentsOfAReg = "";
	}

	// if neither operand is in the A register then
	if (contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName()) 
	{
		////emit code to load operand2 into the A register
		emit("", "mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	//emit code to perform a register-memory compare
	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("", "cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("", "cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
	}

	//emit code to jump if equal to the next available Ln (call getLabel)
	string label_1 = getLabel(), label_2 = getLabel();

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("", "jne", "." + label_1, "; if " + operand2 + " <> " + operand1 + " then jump to set eax to TRUE");
	}
	else
	{
		emit("", "jne", "." + label_1, "; if " + operand2 + " <> " + operand1 + " then jump to set eax to TRUE");
	}

	//emit code to load FALSE into the A register
	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

	//insert FALSE in symbol table with value 0 and external name false
	if (symbolTable.count("false") == 0) 
	{
		insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
		symbolTable.at("false").setInternalName("FALSE");
	}
	
	//emit code to perform an unconditional jump to the next label (call getLabel should be L(n+1))
	emit("", "jmp", "." + label_2, "; unconditionally jump");

	emit("." + label_1 + ":");
	//emit code to load TRUE into A register
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	//insert TRUE in symbol table with value -1 and external name true
	if (symbolTable.count("true") == 0) 
	{
		insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
		symbolTable.at("true").setInternalName("TRUE");
	}

	//emit code to label the next instruction with the second acquired label L(n+1)
	emit("." + label_2 + ":");

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

// ---------------------------------------------------------------------------------
void Compiler::emitLessThanCode(string operand1, string operand2) // op2 < op1
{
	 // check that neither operand is empty
	if (symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}

	//if types of operands are not the same
	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
	{
		processError("incompatible types");
	}

	//if the A Register holds a temp not operand1 nor operand2 then 
	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		//emit code to store that temp into memory
		emit("","mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
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
		emit("","mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	//emit code to perform a register-memory compare
	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}
	
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
	}

	//emit code to jump if NOT equal to the next available Ln (call getLabel)
	string newLabel = getLabel();

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","jl", "." + newLabel, "; if " + operand2 + " < " + operand1 + " then jump to set eax to TRUE");
	}
	
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","jl", "." + newLabel, "; if " + operand2 + " < " + operand1 + " then jump to set eax to TRUE");
	}

	//emit code to load FALSE into the A register
	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

	//insert FALSE in symbol table with value 0 and external name false
	if (symbolTable.count("false") == 0)
	{
		insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
	}

	string secondLabel = getLabel();
	//emit code to perform an unconditional jump to the next label (call getLabel should be L(n+1))
	emit("","jmp","." + secondLabel, "; unconditionally jump");
	
	emit("." + newLabel + ":");
	
	//emit code to load TRUE into A register
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	//insert TRUE in symbol table with value -1 and external name true
	if (symbolTable.count("true") == 0)
	{
		insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
	}

	//emit code to label the next instruction with the second acquired label L(n+1)
	emit("." + secondLabel + ":");

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

// ---------------------------------------------------------------------------------

void Compiler::emitLessThanOrEqualToCode(string operand1, string operand2) // op2 <= op1
{
	 // check that neither operand is empty
	if (symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}

	//if types of operands are not the same
	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
	{
		processError("incompatible types");
	}

	//if the A Register holds a temp not operand1 nor operand2 then 
	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		//emit code to store that temp into memory
		emit("","mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
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
		emit("","mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	//emit code to perform a register-memory compare
	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}
	
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
	}

	//emit code to jump if NOT equal to the next available Ln (call getLabel)
	string newLabel = getLabel();

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","jle", "." + newLabel, "; if " + operand2 + " <= " + operand1 + " then jump to set eax to TRUE");
	}
	
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","jle", "." + newLabel, "; if " + operand2 + " <= " + operand1 + " then jump to set eax to TRUE");
	}

	//emit code to load FALSE into the A register
	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

	//insert FALSE in symbol table with value 0 and external name false
	if (symbolTable.count("false") == 0)
	{
		insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
	}

	string secondLabel = getLabel();
	//emit code to perform an unconditional jump to the next label (call getLabel should be L(n+1))
	emit("","jmp","." + secondLabel, "; unconditionally jump");
	
	emit("." + newLabel + ":");
	//emit code to load TRUE into A register
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	//insert TRUE in symbol table with value -1 and external name true
	if (symbolTable.count("true") == 0)
	{
		insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
	}

	//emit code to label the next instruction with the second acquired label L(n+1)
	emit("." + secondLabel + ":");

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

// ---------------------------------------------------------------------------------

void Compiler::emitGreaterThanCode(string operand1, string operand2) // op2 > op1
{
	// check that neither operand is empty
	if (symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}

	//if types of operands are not the same
	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
	{
		processError("incompatible types");
	}

	//if the A Register holds a temp not operand1 nor operand2 then 
	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		//emit code to store that temp into memory
		emit("","mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
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
		emit("","mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	//emit code to perform a register-memory compare
	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}
	
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
	}

	//emit code to jump if NOT equal to the next available Ln (call getLabel)
	string newLabel = getLabel();

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","jg", "." + newLabel, "; if " + operand2 + " > " + operand1 + " then jump to set eax to TRUE");
	}
	
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","jg", "." + newLabel, "; if " + operand1 + " > " + operand2 + " then jump to set eax to TRUE");
	}

	//emit code to load FALSE into the A register
	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

	//insert FALSE in symbol table with value 0 and external name false
	if (symbolTable.count("false") == 0)
	{
		insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
	}

	string secondLabel = getLabel();
	//emit code to perform an unconditional jump to the next label (call getLabel should be L(n+1))
	emit("","jmp","." + secondLabel, "; unconditionally jump");
	
	emit("." + newLabel + ":");
	//emit code to load TRUE into A register
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	//insert TRUE in symbol table with value -1 and external name true
	if (symbolTable.count("true") == 0)
	{
		insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
	}

	//emit code to label the next instruction with the second acquired label L(n+1)
	emit("." + secondLabel + ":");

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

// ---------------------------------------------------------------------------------

void Compiler::emitGreaterThanOrEqualToCode(string operand1, string operand2) // op2 >= op1
{
	 // check that neither operand is empty
	if (symbolTable.count(operand1) == 0)
	{
		processError("reference to undefined symbol " + operand1);
	}
	
	else if (symbolTable.count(operand2) == 0)
	{
		processError("reference to undefined symbol " + operand2);
	}

	 //if types of operands are not the same
	if (symbolTable.at(operand1).getDataType() != symbolTable.at(operand2).getDataType())
	{
		processError("incompatible types");
	}

	//if the A Register holds a temp not operand1 nor operand2 then 
	if (isTemporary(contentsOfAReg) && contentsOfAReg != symbolTable.at(operand1).getInternalName() && contentsOfAReg != symbolTable.at(operand2).getInternalName())
	{
		//emit code to store that temp into memory
		emit("","mov", "[" + contentsOfAReg + "],eax", "; deassign AReg");
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
		emit("","mov", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; AReg = " + operand2);
		contentsOfAReg = symbolTable.at(operand2).getInternalName();
	}

	//emit code to perform a register-memory compare
	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; compare " + operand2 + " and " + operand1);
	}
	
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","cmp", "eax,[" + symbolTable.at(operand2).getInternalName() + "]", "; compare " + operand1 + " and " + operand2);
	}

	//emit code to jump if NOT equal to the next available Ln (call getLabel)
	string newLabel = getLabel();

	if (contentsOfAReg == symbolTable.at(operand2).getInternalName())
	{
		emit("","jge", "." + newLabel, "; if " + operand2 + " >= " + operand1 + " then jump to set eax to TRUE");
	}
	
	else if (contentsOfAReg == symbolTable.at(operand1).getInternalName())
	{
		emit("","jge", "." + newLabel, "; if " + operand2 + " >= " + operand1 + " then jump to set eax to TRUE");
	}

	//emit code to load FALSE into the A register
	emit("", "mov", "eax,[FALSE]", "; else set eax to FALSE");

	//insert FALSE in symbol table with value 0 and external name false
	if (symbolTable.count("false") == 0)
	{
		insert("false", BOOLEAN, CONSTANT, "0", YES, 1);
		symbolTable.at("false").setInternalName("FALSE");
	}

	string secondLabel = getLabel();
	//emit code to perform an unconditional jump to the next label (call getLabel should be L(n+1))
	emit("","jmp","." + secondLabel, "; unconditionally jump");
	
	emit("." + newLabel + ":");
	//emit code to load TRUE into A register
	emit("", "mov", "eax,[TRUE]", "; set eax to TRUE");

	//insert TRUE in symbol table with value -1 and external name true
	if (symbolTable.count("true") == 0)
	{
		insert("true", BOOLEAN, CONSTANT, "-1", YES, 1);
		symbolTable.at("true").setInternalName("TRUE");
	}

	//emit code to label the next instruction with the second acquired label L(n+1)
	emit("." + secondLabel + ":");

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

// ---------------------------------------------------------------------------------

// Emit functions for Stage 2
// emit code which follows 'then' and statement predicate
void Compiler::emitThenCode(string operand1, string = "")
{
	/*
	if the type of operand1 is not boolean
	processError(if predicate must be of type boolean)
	assign next label to tempLabel
	if operand1 is not in the A register then
	emit instruction to move operand1 to the A register
	emit instruction to compare the A register to zero (false)
	emit code to branch to tempLabel if the compare indicates equality
	push tempLabel onto operandStk so that it can be referenced when emitElseCode() or
	emitPostIfCode() is called
	if operand1 is a temp then
	free operand's name for reuse
	deassign operands from all registers
	*/

	string tempLabel;

	if (symbolTable.at(operand1).getDataType != BOOLEAN)
	{
		processError("the predicate of \"if\" must be of type BOOLEAN");
	}

	//assign next label to tempLabel
	tempLabel = getLabel();

	if (contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + symbolTable.at(operand1).getInternalName());	// instruction to move operand1 to the A register
		emit("", "cmp", "eax, 0", "compare eax to zero");	// instruction to compare the A register to zero (false)
		emit("", "je", "." + tempLabel, "; if " + tempLabel + " is false then jump to end of if");	// code to branch to tempLabel if the compare indicates equality
	}

	// push tempLabel onto operandStk
	pushOperand(tempLabel);

	// if operand1 is a temp
	if (isTemporary(operand1))
	{
		// free operand's name for reuse (is this right?)
		freeTemp();
	}

	// deassign operands from all registers (is this right?)
	contentsOfAReg = "";
}

// ---------------------------------------------------------------------------------

// emit code which follows 'else' clause of 'if' statement
void Compiler::emitElseCode(string operand1, string = "")
{
	string tempLabel;

	// assign next label to tempLabel
	tempLabel = getLabel();

	// emit instruction to branch unconditionally to tempLabel
	emit("", "jmp", "." + tempLabel, "; unconditional jump");

	// emit instruction to label this point of object code with the argument operand1
	emit ("." + tempLabel + ":", "", "", "; else");	

	// push tempLabel onto operandStk
	pushOperand(tempLabel);

	// deassign operands from all registers
	contentsOfAReg = "";
}

// ---------------------------------------------------------------------------------

// emit code which follows end of 'if' statement 
void Compiler::emitPostIfCode(string operand1, string = "")
{
	//emit instruction to label this point of object code with the argument operand1
	emit ("." + tempLabel + ":", "", "", "; if");	

 	//deassign operands from all registers
	contentsOfAReg = "";
}

// ---------------------------------------------------------------------------------

// emit code following 'while'
void Compiler::emitWhileCode(string = "", string = "")
{
	string tempLabel;

	// assign next label to tempLabel
	tempLabel = getLabel();

	// emit instruction to label this point of object code as tempLabel
	emit ("." + tempLabel + ":", "", "", "; while");	
	
	// push tempLabel onto operandStk
	pushOperand(tempLabel);

	// deassign operands from all registers
	contentsOfAReg = "";
}

// ---------------------------------------------------------------------------------

// emit code following 'do'
void Compiler::emitDoCode(string operand1, string = "")
{
	string tempLabel;

	// if the type of operand1 is not boolean
	if (symbolTable.at(operand1).getDataType() != BOOLEAN)
	{
		processError("while predicate must be of type boolean");
	}

	// assign next label to tempLabel
	tempLabel = getLabel();

	// if operand1 is not in the A register then
	if (contentsOfAReg != symbolTable.at(operand1).getInternalName())
	{
		emit("", "mov", "eax,[" + symbolTable.at(operand1).getInternalName() + "]", "; AReg = " + symbolTable.at(operand1).getInternalName());	// instruction to move operand1 to the A register
		emit("", "cmp", "eax, 0", "compare eax to zero");	// instruction to compare the A register to zero (false)
		emit("", "je", "." + tempLabel, "; if " + tempLabel + " is false then jump to end of if");	// code to branch to tempLabel if the compare indicates equality
	}

	// push tempLabel onto operandStk
	pushOperand(tempLabel);

	// if operand1 is a temp
	if (isTemporary(operand1))
	{
		// free operand's name for reuse (is this right?)
		freeTemp();
	}

	// deassign operands from all registers (is this right?)
	contentsOfAReg = "";
}

// ---------------------------------------------------------------------------------

// emit code at end of 'while' loop;
// operand2 is the label of the beginning of the loop
// operand1 is the label which should follow the end of the loop
void emitPostWhileCode(string operand1, string operand2)
{
	/*
	emit instruction which branches unconditionally to the beginning of the loop, i.e., to the
	value of operand2
	emit instruction which labels this point of the object code with the argument operand1
	deassign operands from all registers
	*/
}

// ---------------------------------------------------------------------------------

// emit code which follows 'repeat'
void emitRepeatCode(string = "", string = "")
{
	/*
	string tempLabel
	assign next label to tempLabel
	emit instruction to label this point in the object code with the value of tempLabel
	push tempLabel onto operandStk
	deassign operands from all registers
	*/
}

// ---------------------------------------------------------------------------------

// emit code which follows 'until' and the predicate of loop
// operand1 is the value of the predicate
// operand2 is the label which points to the beginning of the loop
void emitUntilCode(string operand1, string operand2)
{
	/*
	if the type of operand1 is not boolean
	processError(if predicate must be of type boolean)
	if operand1 is not in the A register then
	emit instruction to move operand1 to the A register
	emit instruction to compare the A register to zero (false)
	emit code to branch to operand2 if the compare indicates equality
	if operand1 is a temp then
	free operand1's name for reuse
	deassign operands from all registers
	*/
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
			
			if ((token == "<" && ch == '=') || (token == "<" && ch == '>') || (token == ">" && ch == '=')) 
			{
				token += ch;
				nextChar();
			}
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

// ---------------------------------------------------------------------------------

void Compiler::pushOperator(string name) //push name onto operatorStk
{
 	operatorStk.push(name);
}

// ---------------------------------------------------------------------------------

void Compiler::pushOperand(string name) //push name onto operandStk
 //if name is a literal, also create a symbol table entry for it
{
 	if (symbolTable.count(name) == 0)																					
	{
		if (isInteger(name) || name == "true" || name == "false")
		{
 		  insert(name, whichType(name), CONSTANT, whichValue(name), YES, 1);		//insert symbol table entry, call whichType to determine the data type of the literal
		  // may want to be like this instead insert(x,whichType(y),CONSTANT,whichValue(y),YES,1); 																																	
		}
	}
	
	operandStk.push(name);	
}

// ---------------------------------------------------------------------------------

string Compiler::popOperator() //pop name from operatorStk
{
	string top;
 
	if (!operatorStk.empty())
	{
		top = operatorStk.top();
		operatorStk.pop();
	}
 
	else
	{
		processError("compiler error; operator stack underflow");
	}
 
	return top;
}

// ---------------------------------------------------------------------------------

string Compiler::popOperand() //pop name from operandStk
{
	string top;
 
	if (!operandStk.empty())
	{
		top = operandStk.top();
		operandStk.pop();
	}
 
	else
	{
		processError("compiler error; operand stack underflow");
	}
 
	return top;
}

// ---------------------------------------------------------------------------------

void Compiler::freeTemp()
{
	currentTempNo--;
	if (currentTempNo < -1)
	{
		processError("compiler error, currentTempNo should be >= –1");
	}
}

// ---------------------------------------------------------------------------------

string Compiler::getTemp()
{
	string temp;
	currentTempNo++;

	temp = "T" + to_string(currentTempNo);

	if (currentTempNo > maxTempNo)
	{
		insert(temp, UNKNOWN, VARIABLE, "1", NO, 1);
		symbolTable.at(temp).setInternalName(temp);
		maxTempNo++;
	}
	
	return temp;
}

// ---------------------------------------------------------------------------------

string Compiler::getLabel()
{
	string label;
	static int count = 0;
	
	label = "L" + to_string(count);
	
	count++;

	return label;
}

// ---------------------------------------------------------------------------------

// if s[0] == 'T' then s is temporary
bool Compiler::isTemporary(string s) const 
{
	if (s[0] == 'T') 
	{
		return true;
	}
	return false;
}