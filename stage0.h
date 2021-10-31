#ifndef STAGE0_H
#define STAGE0_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

const char END_OF_FILE = '$';      // arbitrary choice

enum storeTypes {INTEGER, BOOLEAN, PROG_NAME};
enum modes {VARIABLE, CONSTANT};
enum allocation {YES, NO};

class SymbolTableEntry
{
public:
  SymbolTableEntry(string in, storeTypes st, modes m,
                   string v, allocation a, int u)
  {
    setInternalName(in);
    setDataType(st);
    setMode(m);
    setValue(v);
    setAlloc(a);
    setUnits(u);
  }    

  string getInternalName() const
  {
    return internalName;
  }

  storeTypes getDataType() const
  {
    return dataType;
  }

  modes getMode() const
  {
    return mode;
  }

  string getValue() const
  {
    return value;
  }

  allocation getAlloc() const
  {
    return alloc;
  }

  int getUnits() const
  {
    return units;
  }

  void setInternalName(string s)
  {
    internalName = s;
  }

  void setDataType(storeTypes st)
  {
    dataType = st;
  }

  void setMode(modes m)
  {
    mode = m;
  }

  void setValue(string s)
  {
    value = s;
  }

  void setAlloc(allocation a)
  {
    alloc = a;
  }

  void setUnits(int i)
  {
    units = i;
  }

private:
  string internalName;
  storeTypes dataType;
  modes mode;
  string value;
  allocation alloc;
  int units;
};

class Compiler
{
public:
  Compiler(char **argv); // constructor
  ~Compiler();           // destructor

  void createListingHeader();
  void parser();
  void createListingTrailer();

  // Methods implementing the grammar productions
  void prog();           // stage 0, production 1
  void progStmt();       // stage 0, production 2
  void consts();         // stage 0, production 3
  void vars();           // stage 0, production 4
  void beginEndStmt();   // stage 0, production 5
  void constStmts();     // stage 0, production 6
  void varStmts();       // stage 0, production 7
  string ids();          // stage 0, production 8

  // Helper functions for the Pascallite lexicon
  bool isKeyword(string s) const;  // determines if s is a keyword
  bool isSpecialSymbol(char c) const; // determines if c is a special symbol
  bool isNonKeyId(string s) const; // determines if s is a non_key_id
  bool isInteger(string s) const;  // determines if s is an integer
  bool isBoolean(string s) const;  // determines if s is a boolean
  bool isLiteral(string s) const;  // determines if s is a literal

  // Action routines
  void insert(string externalName, storeTypes inType, modes inMode,
              string inValue, allocation inAlloc, int inUnits);
  storeTypes whichType(string name); // tells which data type a name has
  string whichValue(string name); // tells which value a name has
  void code(string op, string operand1 = "", string operand2 = "");

  // Emit Functions
  void emit(string label = "", string instruction = "", string operands = "",
            string comment = "");
  void emitPrologue(string progName, string = "");
  void emitEpilogue(string = "", string = "");
  void emitStorage();

  // Lexical routines
  char nextChar(); // returns the next character or END_OF_FILE marker
  string nextToken(); // returns the next token or END_OF_FILE marker

  // Other routines
  string genInternalName(storeTypes stype) const;
  void processError(string err);

private:
  map<string, SymbolTableEntry> symbolTable;
  ifstream sourceFile;
  ofstream listingFile;
  ofstream objectFile;
  string token;          // the next token
  char ch;               // the next character of the source file
  uint errorCount = 0;   // total number of errors encountered
  uint lineNo = 0;       // line numbers for the listing
};

#endif