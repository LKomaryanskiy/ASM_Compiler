#pragma once
#include <stack>
#include <sstream>
#include <fstream>
#include "Pass.h"
#include "Structures.h"
#include "TokenPatterns.h"

using namespace std;

class SecondPass : public Pass{
public:
	SecondPass();
	SecondPass(FirstPassPackage &package);
	~SecondPass();

	virtual void MakePass();
	virtual void CreateFile(wstring fileName_);
private:
	Result1Pass result1Pass_;
	Result2Pass result2Pass_;

private:
	ByteCode GetByteCodeByString(wstring string_);
	ByteCode GetByteCode(list<Token> label_,
						 list<Token> mnemocode_,
						 list<Token> firstOperand_,
						 list<Token> secondOperand_, 
						 int currentOffset_);
	int GetRegCode(wstring reg_);
	wstring GetPrefixCode(list<Token> operand_);
	bool hasPrefix(list<Token> tokens_);
	int GetMod(list<Token> operand_);
	int GetRM(list<Token> operand_);
	int getOperandSize(list<Token> tokens_);
	bool isDefinedConstant(wstring varName_);
	int DistanceBetweenLabels(int cOffset, wstring labelOffset);
	list<wstring> CreateExpression(list<Token> tokens_);
};
