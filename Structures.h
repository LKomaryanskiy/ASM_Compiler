#pragma once
#include <list>
#include <vector>
#include <string>
#include <map>

using namespace std;

typedef struct {
	int numRowError;
	wstring error;
} Error;

typedef list<Error> ErrorList;

/*structures for lexical analyzer*/
typedef struct {
	int	length;
	wstring value;
	wstring type;
} Token;

typedef vector<Token> TokenTable;

typedef struct {
	int pos;
	int numbers;
}TokenPos;

typedef struct {
	int label;
	TokenPos mnemoCode;
	TokenPos operand1;
	TokenPos operand2;
} Sentence;

typedef vector<Sentence> SentenceTable;

typedef struct {
	SentenceTable sentenceTable;
	TokenTable tokenTable;
	ErrorList errorList;
	vector<wstring> rows;
} LexicalPackage;

/*structures for first pass*/
typedef struct {
	wstring name;
	int length;
	wstring align;
	wstring class_;
} SegmentInfo;

typedef struct {
	wstring value;
	wstring type;
	wstring attr;
} PersonalIdentInfo;

typedef struct {
	wstring value;
	bool definedBeforeUsing;
	wstring attr;
	wstring type;
} LabelInfo;

typedef map<wstring, PersonalIdentInfo> VariableTable;

typedef map<wstring, LabelInfo> LabelTable;

typedef map<wstring, PersonalIdentInfo> ConstantTable;

typedef vector<SegmentInfo> SegmentTable;

typedef struct {
	int offset;
	wstring row;
} FirstPassRow;

typedef vector<FirstPassRow> Result1Pass;

typedef struct {
	bool isCorrect = true;
	bool isRegiser = false;
	bool isConstant = false;
	bool isMemory = false;
	bool hasPrefix = false;
	int size = 0;
} OperandInfo;

typedef struct {
	SentenceTable sentenceTable;
	TokenTable tokenTable;
	ErrorList errorList;
	vector<wstring> rows;
	Result1Pass result1Pass;
	VariableTable variableTable;
	LabelTable labelTable;
	ConstantTable constantTable;
	SegmentTable segmentTable;
} FirstPassPackage;

/*structures for second pass*/
typedef list<wstring> ByteCode;

typedef struct {
	int offset;
	ByteCode byteCode;
	wstring row;
} SecondPassRow;

typedef vector<SecondPassRow> Result2Pass;