//Zac Caffey and Cameron Ley
//CS 4301
//Compiler Stage 0

#include <fstream>
#include <ostream>
#include <string>
#include <ctime>
#include "stage0main.C"


Compiler(char **argv) // constructor - Z (needs to declare sourceFile, listingFile, and objectFile. Also need to fix the issue with using argv. might just be a result of the prior error)
{
    sourceFile.open(argv[1]);
    listingFile.open(argv[2]);
    objectFile.open(argv[3]);
}

~Compiler() // destructor - Z
{
    sourceFile.close();
    listingFile.close();
    objectFile.close();
}

void createListingHeader() // - Z (needs to be formatted)
{
	time_t now = time(0);
	char* time = ctime(&now);
	cout << "STAGE0:\t Zac Caffey and Cameron Ley, " << time << endl << endl;
	cout << "LINE NO:\t\t\t\t" << "SOURCE STATEMENT" << endl << endl;
 //print "STAGE0:", name(s), DATE, TIME OF DAY
 //print "LINE NO:", "SOURCE STATEMENT"
 //line numbers and source statements should be aligned under the headings
}

void parser()
{
nextChar()
 //ch must be initialized to the first character of the source file
 if (nextToken() != "program")
 processError(keyword "program" expected)
 //a call to nextToken() has two effects
 // (1) the variable, token, is assigned the value of the next token
 // (2) the next token is read from the source file in order to make
 // the assignment. The value returned by nextToken() is also
 // the next token.
prog()
 //parser implements the grammar rules, calling first rule
}

void createListingTrailer() // - Z
{
    cout << "COMPILATION TERMINATED, # ERRORS ENCOUNTERED" << endl;
}

void processError(string err)   // - Z (not sure if this is done correctly. May need to adjust the error message) (ALMOST POSITIVE THIS WILL NEED TO BE ADJUSTED)
{
    ofstream cout("error has occured");
    exit(0);
}

void prog()  //token should be "program" - C
{

    if (token != "program") 
		processError("keyword \"program\" expected");
    progStmt(); 
    if (token == "const") 
        consts(); 
    if (token == "var") 
        vars(); 
    if (token != "begin") 
	 	processError("keyword \"begin\" expected");
    beginEndStmt();
    if (token != END_OF_FILE) 
		processError('no text may follow "end"');
} 

void progStmt()  //token should be "program" - C
{   
  string x = "";
  if (token != "program") 
	  processError("keyword \"program\" expected");
  //Initialize so that we don't mess up nextToken
  x = NextToken(); 
  if (!isNonKeyId(x)) 
		processError("program name expected");
  if (x != ";") 
		processError("semicolon expected");
	// Going to check this one too -C
  x = nextToken(); 
	// Not Sure about this one -C
  code("program", x);
	// Not Sure about this one -C 
  insert(x,PROG_NAME,CONSTANT,x,NO,0);
}

void consts()  //token should be "const" - C
{   
	if (token != "const") 
		processError("keyword \"const\" expected");
	//Initialize so that we don't mess up nextToken
	string x = NextToken(); 
	if (!isNonKeyId(x)) 
		processError("non-keyword identifier must follow \"const\"");
	constStmts();
} 

void vars()  //token should be "var" - C
{   
    if (token != "var") 
		processError("keyword \"var\" expected");
	//Initialize so that we don't mess up nextToken
    string x = NextToken(); 
    if (!isNonKeyId(x))  
		processError("non-keyword identifier must follow \"var\"");
    varStmts();
}

void beginEndStmt()  //token should be "begin" - C
{   
    if (token != "begin") 
		processError("keyword \"begin\" expected");
	//Initialize so that we don't mess up nextToken
    string x = NextToken(); 
    if (x != "end") 
		processError("keyword \"end\" expected");
    if (x != ".") 
		processError("period expected");
    x = nextToken();
	// Unsure About this as well -C
    code("end", ".");
}

void constStmts() //token should be NON_KEY_ID - Z (this will need some work. not done right now)
{ 
  string x,y, next;

  if (!isNonKeyId(token))
    processError("non-keyword identifier expected");

  x = token;
  next = nextToken();

  if (next != "=")
    processError('"=" expected');

  y = nextToken();

  if (!(isNonKeyId(y)) || //y is not one of "+","-","not",NON_KEY_ID,"true","false",INTEGER)  (is there any included member functions to call for each of these possibilities?)
    processError("token to right of \"=\" illegal");

  if (y == '+' || y == '-')
  {
    if (next.getDataType() != "Integer")     //not sure if i need "" around integer since it is an enumerated type
      processError("integer expected after sign");

    y = y + token;
  }

  if (y == "not")
  {
    if (next.getDataType() != "BOOLEAN")     //not sure if i need "" around boolean since it is an enumerated type
      processError('boolean expected after “not”');
      
    if (token == "true")
      y = "false";
    else
      y = "true";
  }

  if (next != ";")
    processError("semicolon expected");

  if (y.getDataType() != "Integer" || y.getDataType() != "Boolean")           //the data type of y is not INTEGER or BOOLEAN
    processError("data type of token on the right-hand side must be INTEGER or BOOLEAN");

  insert(x,whichType(y),CONSTANT,whichValue(y),YES,1);        // dont think this is complete but will check later
  x = nextToken();

  if (x != "begin" || x != "var" || !(isNonKeyId(x)))         //x is not one of "begin","var",NON_KEY_ID)               
    processError("non-keyword identifier, \"begin\", or \"var\" expected");
  if (isNonKeyId(x))
    constStmts();
}

void varStmts() //token should be NON_KEY_ID - Z (started this but not done)
{
 string x,y, next;
 if ((isNonKeyId(token)))
  processError("non-keyword identifier expected");

 x = ids();

 if (token != ":")
  processError('":" expected');
  
 if (next.getDataType() != "INTEGER" || next.getDataType() != "BOOLEAN")  //thinking the correct use of getDataType might actually be getDataType(nextToken? We shall see)
  processError('illegal type follows ":"');

 y = token;

 if (next != ";")
  processError("semicolon expected");

 insert(x,y,VARIABLE,"",YES,1);
 string z = nextToken();

 if (z != "begin" || !(isNonKeyId(z)))    //is not one of "begin",NON_KEY_ID)
  processError('non-keyword identifier or "begin" expected');

 if (isNonKeyId(token))   //token is a NON_KEY_ID)
 varStmts();
}

string ids() //token should be NON_KEY_ID - Z
{
 string temp,tempString, next;
 if (!(isNonKeyId(token)))    //token is not a NON_KEY_ID
  processError("non-keyword identifier expected");
 tempString = token;
 temp = token;
 next = nextToken();
 if (next == ",")
 {
  if (!(isNonKeyId(next)))    //nextToken() is not a NON_KEY_ID)
    processError("non-keyword identifier expected");
  tempString = temp + "," + ids();
 }
 return tempString;
}

void insert(string externalName,storeType inType, modes inMode, string inValue,
 allocation inAlloc, int inUnits)
 //create symbol table entry for each identifier in list of external names
 //Multiply inserted names are illegal
{
 string name
 while (name broken from list of external names and put into name != "")
 {
 if (symbolTable[name] is defined)
 processError(multiple name definition)
 else if (name is a keyword)
 processError(illegal use of keyword)
 else //create table entry
 {
 if (name begins with uppercase)
 symbolTable[name]=(name,inType,inMode,inValue,inAlloc,inUnits)
 else
 symbolTable[name]=(genInternalName(inType),inType,inMode,inValue,
 inAlloc,inUnits)
 }
}

storeTypes whichType(string name) //tells which data type a name has
{
 if (name is a literal)
 if (name is a boolean literal)
 data type = BOOLEAN
 else
 data type = INTEGER
 else //name is an identifier and hopefully a constant
 if (symbolTable[name] is defined)
 data type = type of symbolTable[name]
 else
 processError(reference to undefined constant)
 return data type
}
string whichValue(string name) //tells which value a name has
{
 if (name is a literal)
 value = name
 else //name is an identifier and hopefully a constant
 if (symbolTable[name] is defined and has a value)
 value = value of symbolTable[name]
 else
 processError(reference to undefined constant)
 return value
}

void code(string op, string operand1, string operand2)
{
 if (op == "program")
 emitPrologue(operand1)
 else if (op == "end")
 emitEpilogue()
 else
 processError(compiler error since function code should not be called with
 illegal arguments)
}

void emit(string label, string instruction, string operands, string comment)
{
 Turn on left justification in objectFile
 Output label in a field of width 8
 Output instruction in a field of width 8
 Output the operands in a field of width 24
 Output the comment
}
void emitPrologue(string progName, string operand2)
{
 Output identifying comments at beginning of objectFile
 Output the %INCLUDE directives
 emit("SECTION", ".text")
 emit("global", "_start", "", "; program" + progName)
 emit("_start:")
}
void emitEpilogue(string operand1, string operand2)
{
 emit("","Exit", "{0}");
 emitStorage();
}
void emitStorage()
{
 emit("SECTION", ".data")
 for those entries in the symbolTable that have
 an allocation of YES and a storage mode of CONSTANT
 { call emit to output a line to objectFile }
 emit("SECTION", ".bss")
 for those entries in the symbolTable that have
 an allocation of YES and a storage mode of VARIABLE
{ call emit to output a line to objectFile }
}

string nextToken() //returns the next token or end of file marker
{
 token = "";
 while (token == "")
 {
 switch(ch)
 {
 case '{' : //process comment
 while (nextChar() is not one of END_OF_FILE, '}'}
 { //empty body }
if (ch == END_OF_FILE)
 processError(unexpected end of file)
 else
 nextChar()
 case '}' : processError('}' cannot begin token)
 case isspace(ch) : nextChar()
 case isSpecialSymbol(ch): token = ch;
 nextChar()
 case islower(ch) : token = ch;
 while (nextChar() is one of letter, digit, or
 '_' but not END_OF_FILE)
 {
 token+=ch
}
if (ch is END_OF_FILE)
 processError(unexpected end of file)
 case isdigit(ch) : token = ch;
 while (nextChar() is digit but not END_OF_FILE)
 {
 token+=ch
 }
if (ch is END_OF_FILE)
 processError(unexpected end of file)
 case END_OF_FILE : token = ch
 default : processError(illegal symbol)
 }
 return token
}
char nextChar() //returns the next character or end of file marker
{
 read in next character
 if end of file
 ch = END_OF_FILE //use a special character to designate end of file
 else
 ch = next character
 print to listing file (starting new line if necessary)
 return ch;
 }