#pragma once
#include <stack>
#include <sstream>
#include <fstream>
#include "Pass.h"
#include "Structures.h"
#include "TokenPatterns.h"

using namespace std;

class FirstPass : public Pass{
public:
	FirstPass();
	FirstPass(LexicalPackage package);
	~FirstPass();

	FirstPassPackage GetFirrstPassPackage();
	virtual void MakePass();
	virtual void CreateFile(wstring fileName_);
private:
	LexicalPackage package_;
	VariableTable variableTable_;
	LabelTable labelTable_;
	ConstantTable constantTable_;
	SegmentTable segmentTable_;
	Result1Pass result1Pass_;
private:
	bool isCorrectExpression(list<Token> _expression);
	OperandInfo GetOperandInfo(list<Token> tokens_);
	bool isRegister(list<Token> tokens_);
	bool isCorrcectConstant(list<Token> tokens_);
	bool isCorrcectMemory(list<Token> tokens_);
	bool hasPrefix(list<Token> tokens_);
	bool isDefinedVariable(wstring varName_);
	bool isDefinedfLabel(wstring varName_);
	bool isDefinedConstant(wstring varName_);
	int getOperandSize(list<Token> tokens_);
	list<wstring> CreateExpression(list<Token> tokens_);
};
