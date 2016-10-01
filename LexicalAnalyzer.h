#pragma once
#include <fstream>
#include "TokenPatterns.h"
#include "Structures.h"


using namespace std;

class LexicalAnalyzer{
public:
	LexicalAnalyzer();
	LexicalAnalyzer(const wstring& _filePath);
	~LexicalAnalyzer();

	LexicalPackage GetLexicalPackege();
private:
	TokenTable tokenTable_;
	SentenceTable sentenceTable_;
	ErrorList errorList_;
	vector<wstring> rows_;
private:
	void CreateTables(wistream& _inStream);
	void CreateTokenFile(const wstring& _fileName);
	void CreateSentenceFile(const wstring& _fileName);
	list<wstring> Line2Lexemes(const wstring &_line);
	bool IsConstHex(wstring _str);
	bool IsConstString(wstring _str);
};


