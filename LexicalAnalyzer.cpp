#include "LexicalAnalyzer.h"

LexicalAnalyzer::LexicalAnalyzer() {
};

LexicalAnalyzer::LexicalAnalyzer(const wstring& _filePath) {
	wifstream inFile(_filePath);
	if (inFile.is_open()) {
		CreateTables(inFile);
		CreateTokenFile(L"Token.txt");
		CreateSentenceFile(L"Sentence.txt");
	}
};

LexicalAnalyzer::~LexicalAnalyzer() {
};

void LexicalAnalyzer::CreateTables(wistream& _inStream) {
	wchar_t line[256];
	Token token;
	Sentence sentence;
	int num_of_lexems;
	list<wstring> lexems;
	Error error;
	while (!_inStream.eof()) {
		num_of_lexems = 0;
		_inStream.getline(line, 256);
		if (wcslen(line) != 0)
			rows_.push_back(line);
		lexems = Line2Lexemes(line);
		//Token table
		for (auto it = lexems.begin(); it != lexems.end(); it++) {
			token.length = wcslen((*it).c_str());
			token.value = (*it);
			auto fIt = classificationMap.find((*it));
			if (fIt != classificationMap.end()) {
				token.type = classificationMap[(*it)];
			}
			else if ((*it)[0] >= L'0'
				&&(*it)[0] <= L'9') {
				if (IsConstHex((*it))) {
					token.type = L"Const Hex Digit";
				}
				else{
					error.numRowError = -1;
					error.error = L"Incorrect digit";
					errorList_.push_back(error);
				}
			}
			else if (IsConstString((*it))) {
				token.type = L"Const String";
				token.value.erase(0, 1);
				token.value.erase(token.value.length() - 1, 1);
				token.length -= 2;
			}
			else if (it->length() == 1) {
				token.type = L"Single Symbol";
			}
			else {
				if (token.value.find(L'\'') == -1
					&& token.value.find(L'\"') == -1)
					token.type = L"Personal ident";
				else {
					error.numRowError = -1;
					error.error = L"Error symbol";
					errorList_.push_back(error);
				}
			}
			tokenTable_.push_back(token);
			num_of_lexems++;
		}

		if (!lexems.empty()) {
			//Sentence table
			//First element
			int table_elem = tokenTable_.size() - num_of_lexems;
			int elem = 1;
			//If first token is label
			if (table_elem < tokenTable_.size()
				&& tokenTable_[table_elem].type == L"Personal ident") {
				sentence.label = 1;
				table_elem++;
				elem++;
				if (table_elem < tokenTable_.size()
					&& tokenTable_[table_elem].value == L":") {
					table_elem++;
					elem++;
				}
			}
			else if (table_elem < tokenTable_.size()) {
				sentence.label = 0;
			}
			//Mnemocode
			if (table_elem < tokenTable_.size()
				&& (tokenTable_[table_elem].type == L"Machine Command"
					|| tokenTable_[table_elem].type == L"Define Byte"
					|| tokenTable_[table_elem].type == L"Define Word"
					|| tokenTable_[table_elem].type == L"Define Double Word"
					|| tokenTable_[table_elem].type == L"Directive")) {
				sentence.mnemoCode.pos = elem;
				sentence.mnemoCode.numbers = 1;
				elem++;
				table_elem++;
				//operands
				if (table_elem < tokenTable_.size()) {
					//1st operand
					sentence.operand1.pos = elem;
					while (table_elem < tokenTable_.size()
						&& tokenTable_[table_elem].value != L",") {
						elem++;
						table_elem++;
					}
					sentence.operand1.numbers = elem - sentence.operand1.pos;
					elem++;
					table_elem++;

					//2nd operand
					if (table_elem < tokenTable_.size()) {
						sentence.operand2.pos = elem;
						while (table_elem < tokenTable_.size()) {
							elem++;
							table_elem++;
						}
						sentence.operand2.numbers = elem - sentence.operand2.pos;
					}
					else {
						sentence.operand2.pos = elem - 1;
						sentence.operand2.numbers = 0;
					}
				}
				else {
					sentence.operand1.pos = elem;
					sentence.operand1.numbers = 0;
					sentence.operand2.pos = elem;
					sentence.operand2.numbers = 0;
				}
			}
			else {
				sentence.mnemoCode.pos = elem;
				sentence.mnemoCode.numbers = 0;
				sentence.operand1.pos = elem;
				sentence.operand1.numbers = 0;
				sentence.operand2.pos = elem;
				sentence.operand2.numbers = 0;
			}
			sentenceTable_.push_back(sentence);
		}
	}
};

void LexicalAnalyzer::CreateTokenFile(const wstring& _fileName) {
	wofstream outFile(_fileName);
	for (auto it = tokenTable_.begin(); it != tokenTable_.end(); it++) {
		outFile.fill(L'-');

		outFile << L'+';
		outFile.width(10);
		outFile << right << L'+';
		outFile.width(40);
		outFile << L'+';
		outFile.width(40);
		outFile << L'+' << endl;

		outFile.fill(L' ');
		outFile << L'|';
		outFile.width(9);
		outFile << left <<  it->length;
		outFile.width(40 - wcslen(it->type.c_str()));
		outFile << L'|' << it->type;
		outFile.width(40 - wcslen(it->value.c_str()));
		outFile << L'|' << it->value << L'|' << endl;

		outFile.fill(L'-');
		outFile << L'+';
		outFile.width(10);
		outFile << right << L'+';
		outFile.width(40);
		outFile << L'+';
		outFile.width(40);
		outFile << L'+' << endl << endl;
	}
};


void LexicalAnalyzer::CreateSentenceFile(const wstring& _fileName) {
	wofstream outFile(_fileName);
	string tmp;
	outFile.fill(L'-');
	outFile << L'+';
	outFile.width(10);
	outFile << right << L'+';
	outFile.width(30);
	outFile << L'+';
	outFile.width(30);
	outFile << L'+';
	outFile.width(30);
	outFile << L'+';
	outFile.width(41);
	outFile << L'+' << endl;

	outFile.fill(L' ');
	outFile << L'|';
	outFile.width(9);
	outFile << left << L"Label";
	outFile.width(15);
	outFile << L'|' << L"Mnemocode field";
	outFile.width(19);
	outFile << L'|' << L"1st operand";
	outFile.width(19);
	outFile << L'|' << L"2nd operand";
	outFile.width(38);
	outFile << L'|' << L"Row" << L'|' << endl;

	outFile.fill(L'-');
	outFile << L'+';
	outFile.width(10);
	outFile << right << L'+';
	outFile.width(30);
	outFile << L'+';
	outFile.width(30);
	outFile << L'+';
	outFile.width(30);
	outFile << L'+';
	outFile.width(41);
	outFile << L'+' << endl;

	int i = 0;
	for (auto it = sentenceTable_.begin(); it != sentenceTable_.end(); it++) {
		outFile.fill(L'-');
		outFile << L'+';
		outFile.width(10);
		outFile << L'+';
		outFile.width(15);
		outFile << L'+';
		outFile.width(15);
		outFile << L'+';
		outFile.width(15);
		outFile << L'+';
		outFile.width(15);
		outFile << L'+';
		outFile.width(15);
		outFile << L'+';
		outFile.width(15);
		outFile << L'+';
		outFile.width(41);
		outFile << L'+';
		outFile << endl;

		outFile.fill(L' ');
		outFile << L'|';
		outFile.width(9);
		outFile << left << it->label;
		tmp = to_string(it->mnemoCode.pos);
		outFile.width(15 - tmp.length());
		outFile << L'|' << it->mnemoCode.pos;
		tmp = to_string(it->mnemoCode.numbers);
		outFile.width(15 - tmp.length());
		outFile << L'|' << it->mnemoCode.numbers;
		tmp = to_string(it->operand1.pos);
		outFile.width(15 - tmp.length());
		outFile << L'|' << it->operand1.pos;
		tmp = to_string(it->operand1.numbers);
		outFile.width(15 - tmp.length());
		outFile << L'|' << it->operand1.numbers;
		tmp = to_string(it->operand2.pos);
		outFile.width(15 - tmp.length());
		outFile << L'|' << it->operand2.pos;
		tmp = to_string(it->operand2.numbers);
		outFile.width(15 - tmp.length());
		outFile << L'|' << it->operand2.numbers;
		outFile.width(40 - rows_[i].length());
		outFile << L'|' << rows_[i];
		outFile << L'|' << endl;

		outFile.fill(L'-');
		outFile << L'+';
		outFile.width(10);
		outFile << right << L'+';
		outFile.width(15);
		outFile << L'+';
		outFile.width(15);
		outFile << L'+';
		outFile.width(15);
		outFile << L'+';
		outFile.width(15);
		outFile << L'+';
		outFile.width(15);
		outFile << L'+';
		outFile.width(15);
		outFile << L'+';
		outFile.width(41);
		outFile << L'+' << endl << endl;
		i++;
	}
};

list<wstring> LexicalAnalyzer::Line2Lexemes(const wstring& _line) {
	list<wstring> retValue_;
	wstring word;
	for (int i = 0; i < wcslen(_line.c_str()); i++) {
		word = L"";
		if (singleSymbol[_line[i]] == L"Single Symbol")
			word += _line[i];
		else if (singleSymbol[_line[i]] != L"Single Symbol"
			&& _line[i] != L' '
			&& _line[i] != L'\t') {
			do {
				word += toupper(_line[i]);
				i++;
			} while (i < wcslen(_line.c_str()) 
					&& _line[i] != L' '
					&& _line[i] != L'\t'
					&& singleSymbol[_line[i]] != L"Single Symbol");
			i--;
		}  
		if (word.size() != 0) 
			retValue_.push_back(word);
	}

	return retValue_;
};

bool LexicalAnalyzer::IsConstHex(wstring _str) {
	if (isdigit(_str[0])) {
		for (auto it = _str.begin(); it != _str.end(); it++) {
			if ((*it) < '0'
				&& (*it) > '9'
				&& (*it) < 'A'
				&& (*it) > 'H') {
				return false;
			}
		}
		return true;
	}
	return false;
};

bool LexicalAnalyzer::IsConstString(wstring _str) {
	if ((_str[0] == '\''
		&& _str[_str.length() - 1] == '\'')
		|| (_str[0] == '"'
			&& _str[_str.length() - 1] == '"')) {
		return true;
	}
	return false;
};

LexicalPackage LexicalAnalyzer::GetLexicalPackege() {
	LexicalPackage retValue_;
	retValue_.errorList = errorList_;
	retValue_.sentenceTable = sentenceTable_;
	retValue_.tokenTable = tokenTable_;
	retValue_.rows = rows_;
	return retValue_;
};