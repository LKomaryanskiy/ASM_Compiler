#pragma once
#include <stack>
#include <sstream>
#include <fstream>
#include "Structures.h"
#include "TokenPatterns.h"

class Pass {
protected:
	LexicalPackage package_;
	VariableTable variableTable_;
	LabelTable labelTable_;
	ConstantTable constantTable_;
	SegmentTable segmentTable_;

	virtual void MakePass() = 0;
	virtual void CreateFile(wstring fileName_) = 0;

	int CalculateExppression(list<wstring> _expression);
	int SimpleOpeartion(wstring _firstOperand, wstring _secondOperand, wstring _operation);
	wstring ToUpperWstring(wstring str_);
	wstring ToHex4(int value);
	wstring ToHex2(int value);
};