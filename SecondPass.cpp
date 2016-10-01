#include "SecondPass.h"

SecondPass::SecondPass(){
};

SecondPass::SecondPass(FirstPassPackage &package){
	this->package_.errorList = package.errorList;
	this->package_.rows = package.rows;
	this->package_.sentenceTable = package.sentenceTable;
	this->package_.tokenTable = package.tokenTable;
	this->constantTable_ = package.constantTable;
	this->labelTable_ = package.labelTable;
	this->segmentTable_ = package.segmentTable;
	this->variableTable_ = package.variableTable;
	this->result1Pass_ = package.result1Pass;
};

SecondPass::~SecondPass(){
}

void SecondPass::MakePass(){
	int i = 0;
	int itRows = 0;
	bool IFValue = false;
	bool IFBlock = false;
	bool activeBlock = true;
	list<Token> firstOperand;
	list<Token> secondOperand;
	list<Token> label;
	list<Token> mnemocode;
	SecondPassRow bufSecondPassRow;
	Error error;
	list<int> errorRows;
	for (auto it = package_.errorList.begin(); it != package_.errorList.end(); it++) {
		errorRows.push_back(it->numRowError);
	}
	for (auto itSentence = package_.sentenceTable.begin(); itSentence != package_.sentenceTable.end(); itSentence++) {
		//Read operands, mnemocode and label
		if (itSentence->label == 1) {
			for (int j = i; j < i + itSentence->mnemoCode.pos - 1; j++) {
				label.push_back(package_.tokenTable[j]);
			}
		}
		for (int j = i + itSentence->mnemoCode.pos - 1; j < i + itSentence->mnemoCode.pos + itSentence->mnemoCode.numbers - 1; j++) {
			mnemocode.push_back(package_.tokenTable[j]);
		}
		if (itSentence->operand1.numbers != 0) {
			for (int j = i + itSentence->operand1.pos - 1; j < i + itSentence->operand1.pos + itSentence->operand1.numbers - 1; j++) {
				firstOperand.push_back(package_.tokenTable[j]);
			}
			if (itSentence->operand2.numbers != 0) {
				for (int j = i + itSentence->operand2.pos - 1; j < i + itSentence->operand2.pos + itSentence->operand2.numbers - 1; j++) {
					secondOperand.push_back(package_.tokenTable[j]);
				}
			}
		}

		if (mnemocode.size() != 0) {
			if (mnemocode.back().value == L"IF") {
				if (CalculateExppression(CreateExpression(firstOperand)) != 0) {
					//If block
					IFBlock = true;
					IFValue = true;
					activeBlock = true;
					i += 2;
					bufSecondPassRow.byteCode.clear();
					bufSecondPassRow.offset = result1Pass_[itRows].offset;
					bufSecondPassRow.row = result1Pass_[itRows].row;
					bufSecondPassRow.byteCode.push_back(L" ");
					result2Pass_.push_back(bufSecondPassRow);
					mnemocode.clear();
					firstOperand.clear();
					itRows++;
					continue;
				}
				else {
					//Else block or ENDIF directive
					IFBlock = true;
					IFValue = false;
					activeBlock = false;
					i += 2;
					mnemocode.clear();
					firstOperand.clear();
					continue;
				}
			}
			else if (mnemocode.back().value == L"ELSE") {
				if (IFBlock) {
					if (IFValue) {
						//Pass this block
						activeBlock = false;
						i++;
						mnemocode.clear();
						firstOperand.clear();
						continue;
					}
					else {
						activeBlock = true;
						i++;
						bufSecondPassRow.byteCode.clear();
						bufSecondPassRow.offset = result1Pass_[itRows].offset;
						bufSecondPassRow.row = result1Pass_[itRows].row;
						bufSecondPassRow.byteCode.push_back(L" ");
						result2Pass_.push_back(bufSecondPassRow);
						mnemocode.clear();
						firstOperand.clear();
						itRows++;
						continue;
					}
				}
				else {
					//error
					error.numRowError = itRows;
					error.error = L"Error";
					package_.errorList.push_back(error);
					return;
				}
			}
			else if (mnemocode.back().value == L"ENDIF") {
				IFBlock = false;
				activeBlock = true;
				i++;
				bufSecondPassRow.byteCode.clear();
				bufSecondPassRow.offset = result1Pass_[itRows].offset;
				bufSecondPassRow.row = result1Pass_[itRows].row;
				bufSecondPassRow.byteCode.push_back(L" ");
				result2Pass_.push_back(bufSecondPassRow);
				mnemocode.clear();
				firstOperand.clear();
				itRows++;
				continue;
			}
		}
		
		if (activeBlock) {
			
			bufSecondPassRow.offset = result1Pass_[itRows].offset;
			bufSecondPassRow.row = result1Pass_[itRows].row;
			auto it = find(errorRows.begin(), errorRows.end(), itRows);
			if (it == errorRows.end()) {
				bufSecondPassRow.byteCode = GetByteCode(label, mnemocode, firstOperand, secondOperand, bufSecondPassRow.offset);
			}
			else {
				bufSecondPassRow.byteCode.push_back(L"");
			}
			result2Pass_.push_back(bufSecondPassRow);
			itRows++;
		}

		if (label.size() != 0) {
			i += itSentence->mnemoCode.pos - itSentence->label;
		}
		i += itSentence->mnemoCode.numbers + itSentence->operand1.numbers + itSentence->operand2.numbers;
		if (secondOperand.size() != 0) {
			i++; //','
		}

		mnemocode.clear();
		firstOperand.clear();
		secondOperand.clear();
		label.clear();
	}
};

void SecondPass::CreateFile(wstring fileName_){
	wofstream file(fileName_);
	for (auto it = result2Pass_.begin(); it != result2Pass_.end(); it++) {
		file << ToHex4(it->offset) << L" ";
		for (auto itList = it->byteCode.begin(); itList != it->byteCode.end(); itList++) {
			file << *itList << L" ";
		}
		file << '\t';
		file << it->row << endl;
	}
	file << endl << endl;
	file.fill(L' ');
	file.width(25);
	file << left << L"Name";
	file.width(10);
	file << left << L"Size";
	file << left << L"Length" << endl;

	for (auto it = segmentTable_.begin(); it != segmentTable_.end(); it++) {
		file.width(25);
		file << left << it->name;
		file.width(10);
		file << left << L"16 Bit";
		file << left << ToUpperWstring(ToHex4(it->length));
		file << endl;
	}

	file << endl << endl;
	file.width(25);
	file << left << L"Name";
	file.width(10);
	file << left << L"Type";
	file.width(15);
	file << left << L"Value";
	file << left << L"Attr" << endl;
	wstring tmp;
	for (auto it = constantTable_.begin(); it != constantTable_.end(); it++) {
		file.width(25);
		file << left << it->first;
		file.width(10);
		file << left << it->second.type;
		file.width(15);
		tmp = it->second.value;
		tmp.erase(tmp.size() - 1, 1);
		while (tmp.size() < 4) {
			tmp = L'0' + tmp;
		}
		file << left << tmp << endl;
	}
	file << endl;
	for (auto it = labelTable_.begin(); it != labelTable_.end(); it++) {
		file.width(25);
		file << left << it->first;
		file.width(10);
		file << left << it->second.type;
		file.width(15);
		file << left << it->second.value;
		file << left << it->second.attr << endl;
	}
	file << endl;
	for (auto it = variableTable_.begin(); it != variableTable_.end(); it++) {
		file.width(25);
		file << left << it->first;
		file.width(10);
		file << left << it->second.type;
		file.width(15);
		file << left << it->second.value;
		file << left << it->second.attr << endl;
	}
	file << endl << endl;
	if (package_.errorList.size() == 0) {
		file << '\t' << L"0 Warning Errors" << endl;
		file << '\t' << L"0 Severe  Errors" << endl;
	}
	else {
		for (auto it = package_.errorList.begin(); it != package_.errorList.end(); it++) {
			file << L"In row:" << '\t' << it->numRowError + 1 << '\t' << it->error << endl;
		}
	}
};

ByteCode SecondPass::GetByteCodeByString(wstring string_){
	list<wstring> retValue_;
	for (auto it = string_.begin(); it != string_.end(); it++) {
		retValue_.push_back(ToHex2(static_cast<int> (*it)));
	}
	return retValue_;
};

ByteCode SecondPass::GetByteCode(list<Token> label_, list<Token> mnemocode_, list<Token> firstOperand_, list<Token> secondOperand_, int currentOffset_)
{
	ByteCode retValue_;
	wstring tmp;
	if (mnemocode_.size() != 0) {
		if (mnemocode_.back().value == L"DB") {
			if (firstOperand_.back().type == L"Const Hex Digit") {
				tmp = firstOperand_.back().value;
				tmp.erase(tmp.size() - 1, 1); //delete 'h' in the end
				retValue_.push_back(tmp);
			}
			else {
				retValue_ = GetByteCodeByString(firstOperand_.back().value);
			}
		}
		else if (mnemocode_.back().value == L"DW") {
			tmp = firstOperand_.back().value;
			tmp.erase(tmp.size() - 1, 1); //delete 'h' in the end
			retValue_.push_back(tmp);
		}
		else if (mnemocode_.back().value == L"DD") {
			tmp = firstOperand_.back().value;
			tmp.erase(tmp.size() - 1, 1); //delete 'h' in the end
			retValue_.push_back(tmp);
		}
		else if (mnemocode_.back().value == L"JNE") {
			if (!labelTable_[firstOperand_.back().value].definedBeforeUsing) {
				retValue_.push_back(L"0F");
				retValue_.push_back(L"85");
				int distance;
				stringstream ss;
				string buf(labelTable_[firstOperand_.back().value].value.begin(), labelTable_[firstOperand_.back().value].value.end());
				ss << buf;
				ss >> hex >> distance;
				distance -= currentOffset_;
				retValue_.push_back(ToUpperWstring(ToHex4(distance)));
				retValue_.push_back(L"R");
			}
			else {
				retValue_.push_back(L"75");
				retValue_.push_back(ToUpperWstring(ToHex2(DistanceBetweenLabels(currentOffset_, labelTable_[firstOperand_.back().value].value))));
			}
		}
		else if (mnemocode_.back().value == L"STI"){ 
			retValue_.push_back(L"FB"); // 1111.1011 
		}
		else if (mnemocode_.back().value == L"INC") {
			if (classificationMap[firstOperand_.back().value] == L"Data Register 8 bit") {
				retValue_.push_back(L"FE");
				retValue_.push_back(ToUpperWstring(ToHex2(0x0C0 + GetRegCode(firstOperand_.back().value)))); // 1111.111w : 1100.0reg
			}
			else {
				retValue_.push_front(ToUpperWstring(ToHex2(0x040 + GetRegCode(firstOperand_.back().value)))); // 0100.0reg
			}
		}
		else if (mnemocode_.back().value == L"DEC") {
			if (hasPrefix(firstOperand_)) {
				retValue_.push_back(GetPrefixCode(firstOperand_) + L":");
			}
			if (firstOperand_.front().value == L"BYTE") {
				retValue_.push_back(L"FE");
			}
			else {
				retValue_.push_back(L"FF");
			}
			retValue_.push_back(ToUpperWstring(ToHex2(GetMod(firstOperand_) * 64 + 8 + GetRM(firstOperand_)))); //1111.111w : mod 001 r/m
			if (CalculateExppression(CreateExpression(firstOperand_)) != 0) {
				if (CalculateExppression(CreateExpression(firstOperand_)) == 1) {
					retValue_.push_back(ToUpperWstring(ToHex2(CalculateExppression(CreateExpression(firstOperand_)))));
				}
				else {
					retValue_.push_back(ToUpperWstring(ToHex4(CalculateExppression(CreateExpression(firstOperand_)))));
				}
			}
		}
		else if (mnemocode_.back().value == L"TEST") {
			if (hasPrefix(firstOperand_)) {
				retValue_.push_back(GetPrefixCode(firstOperand_) + L":");
			}
			if (classificationMap[secondOperand_.back().value] == L"Data Register 16 bit") {
				retValue_.push_back(L"85");
				retValue_.push_back(ToUpperWstring(ToHex2(GetMod(firstOperand_) * 64 + GetRM(firstOperand_))));
				if (CalculateExppression(CreateExpression(firstOperand_)) != 0) {
					retValue_.push_back(ToUpperWstring(ToHex2(CalculateExppression(CreateExpression(firstOperand_)))));
				}
			}
			else {
				retValue_.push_back(L"84");
				retValue_.push_back(ToUpperWstring(ToHex2(GetMod(firstOperand_) * 64 + GetRM(firstOperand_))));
				if (CalculateExppression(CreateExpression(firstOperand_)) != 0) {
					retValue_.push_back(ToUpperWstring(ToHex4(CalculateExppression(CreateExpression(firstOperand_)))));
				}
			}
		}
		else if (mnemocode_.back().value == L"OR") {
			if (classificationMap[firstOperand_.back().value] == L"Data Register 8 bit"){
				if (firstOperand_.back().value == L"AL") {
					retValue_.push_back(L"0C"); //0000.110w : data
				}
				else {
					retValue_.push_back(L"80");
					retValue_.push_back(ToUpperWstring(ToHex2(0x0C0 + 8 + GetRegCode(firstOperand_.back().value)))); //1000.00sw : 1100.1reg : data
				}
				retValue_.push_back(ToUpperWstring(ToHex2(CalculateExppression(CreateExpression(secondOperand_)))));
			}
			else {
				if (firstOperand_.back().value == L"AX") {
					retValue_.push_back(L"0D"); // 0000.110w : data
				}
				else {
					retValue_.push_back(L"81");
					retValue_.push_back(ToUpperWstring(ToHex4(0x0C0 + 8 + GetRegCode(firstOperand_.back().value)))); //1000.00sw : 1100.1reg : data
				}
			}
		}
		else if (mnemocode_.back().value == L"SBB") {
			if (hasPrefix(secondOperand_)) {
				retValue_.push_back(GetPrefixCode(secondOperand_) + L":");
			}
			if (classificationMap[firstOperand_.back().value] == L"Data Register 8 bit") {
				retValue_.push_back(L"1A");
				retValue_.push_back(ToUpperWstring(ToHex2(GetMod(secondOperand_) * 64 + GetRM(secondOperand_))));
				if (CalculateExppression(CreateExpression(secondOperand_)) != 0) {
					retValue_.push_back(ToUpperWstring(ToHex2(CalculateExppression(CreateExpression(secondOperand_)))));
				}
			}
			else {
				retValue_.push_back(L"1B");
				retValue_.push_back(ToUpperWstring(ToHex2(GetMod(secondOperand_) * 64 + GetRM(secondOperand_))));
				if (CalculateExppression(CreateExpression(secondOperand_)) != 0) {
					retValue_.push_back(ToUpperWstring(ToHex4(CalculateExppression(CreateExpression(secondOperand_)))));
				}
			}
		}
		else if (mnemocode_.back().value == L"MOVSX") {
			retValue_.push_back(L"0F");
			if (classificationMap[secondOperand_.front().value] == L"Data Register 16 bit") {
				retValue_.push_back(L"BF");
			}
			else {
				retValue_.push_back(L"BE");
			}
			retValue_.push_back(ToUpperWstring(ToHex2(0xC0 + GetRegCode(firstOperand_.back().value) * 8 + GetRegCode(secondOperand_.back().value)))); //0000.1111 1011.111w mod reg r/m
		}
		else if (mnemocode_.back().value == L"ADD") {
			if (hasPrefix(firstOperand_)) {
				retValue_.push_back(GetPrefixCode(firstOperand_) + L":");
			}
			if (firstOperand_.front().value == L"BYTE") {
				retValue_.push_back(L"80");
				retValue_.push_back(ToUpperWstring(ToHex2(GetMod(firstOperand_) * 64 + GetRM(firstOperand_))));
				if (CalculateExppression(CreateExpression(firstOperand_)) != 0) {
					retValue_.push_back(ToUpperWstring(ToHex2(CalculateExppression(CreateExpression(firstOperand_)))));
				}
				retValue_.push_back(ToUpperWstring(ToHex2(CalculateExppression(CreateExpression(secondOperand_)))));
			}
			else {
				retValue_.push_back(L"81");
				retValue_.push_back(ToUpperWstring(ToHex2(GetMod(firstOperand_) * 64 + GetRM(firstOperand_))));
				if (CalculateExppression(CreateExpression(firstOperand_)) != 0) {
					retValue_.push_back(ToUpperWstring(ToHex2(CalculateExppression(CreateExpression(firstOperand_)))));
				}
				retValue_.push_back(ToUpperWstring(ToHex4(CalculateExppression(CreateExpression(secondOperand_)))));//1000.00sw: mod00.0r/m : data
			}
			 
		}
	}
	return retValue_;
}

int SecondPass::GetRegCode(wstring reg_)
{
	if (reg_ == L"AX" || reg_ == L"AL") {
		return 0;
	}
	if (reg_ == L"CX" || reg_ == L"CL") {
		return 1;
	}
	if (reg_ == L"DX" || reg_ == L"DL") {
		return 2;
	}
	if (reg_ == L"BX" || reg_ == L"BL") {
		return 3;
	}
	if (reg_ == L"AH") {
		return 4;
	}
	if (reg_ == L"CH") {
		return 5;
	}
	if (reg_ == L"DH") {
		return 6;
	}
	if (reg_ == L"BH") {
		return 7;
	}
}

wstring SecondPass::GetPrefixCode(list<Token> operand_){
	for (auto it = operand_.begin(); it != operand_.end(); it++) {
		if (it->value == L"SS") {
			return L"36";
		}
		if (it->value == L"ES") {
			return L"26";
		}
		if (it->value == L"GS") {
			return L"65";
		}
		if (it->value == L"FS") {
			return L"64";
		}
		if (it->value == L"DS") {
			return L"";
		}
	}
};

bool SecondPass::hasPrefix(list<Token> tokens_) {
	for (auto it = tokens_.begin(); it != tokens_.end(); it++) {
		if (classificationMap[it->value] == L"Data Segment"
			|| classificationMap[it->value] == L"Stack Segment"
			|| classificationMap[it->value] == L"Secondary Data Segment"
			|| classificationMap[it->value] == L"Code Segment") {
			return true;
		}
	}
	return false;
};

int SecondPass::GetMod(list<Token> operand_){
	if (getOperandSize(operand_) == 0) {
		return 0;
	}
	if (getOperandSize(operand_) == 1) {
		return 1;
	}
	if (getOperandSize(operand_) == 2) {
		return 2;
	}
};

int SecondPass::GetRM(list<Token> operand_){
	bool hasBX = false;
	bool hasBP = false;
	bool hasSI = false;
	bool hasDI = false;

	for (auto it = operand_.begin(); it != operand_.end(); it++) {
		if (it->value == L"BP") {
			hasBP = true;
		}
		if (it->value == L"BX") {
			hasBX = true;
		}
		if (it->value == L"SI") {
			hasSI = true;
		}
		if (it->value == L"DI") {
			hasDI = true;
		}
	}

	if (hasBX && hasSI) {
		return 0; 
	}
	if (hasBX && hasDI) {
		return 1;
	}
	if (hasBP && hasSI) {
		return 2;
	}
	if (hasBP && hasDI) {
		return 3;
	}
};

int SecondPass::getOperandSize(list<Token> tokens_) {
	list<wstring> expression;
	auto it = tokens_.begin();
	while (it != tokens_.end()
		&& it->value != L"[") {
		it++;
	}
	if (it->value == L"[") {
		it++;
	}
	while (it != tokens_.end()
		&& it->value != L"]") {
		if (isDefinedConstant(it->value)) {
			expression.push_back(constantTable_[it->value].value);
		}
		else if (it->value == L"BP"
			|| it->value == L"BX"
			|| it->value == L"SI"
			|| it->value == L"DI") {
			expression.push_back(L"0H");
		}
		else {
			expression.push_back(it->value);
		}
		it++;
	}
	int value = CalculateExppression(expression);
	if (value == 0) {
		return 0;
	}
	if (value <= 127
		&& value >= -128) {
		return 1; //1 byte
	}
	if (value <= 32767
		&& value >= -32768) {
		return 2;//2 bytes
	}
	return 4;
};

bool SecondPass::isDefinedConstant(wstring varName_) {
	auto it = constantTable_.find(varName_);
	if (it != constantTable_.end()) {
		return true;
	}
	return false;
}
int SecondPass::DistanceBetweenLabels(int cOffset, wstring labelOffset){
	stringstream ss;
	string buf(labelOffset.begin(), labelOffset.end());
	ss << buf;
	int lOffset;
	ss >> hex >>  lOffset;
	return lOffset - cOffset - 2;
};

list<wstring> SecondPass::CreateExpression(list<Token> tokens_) {
	list<wstring> retValue_;
	for (auto it = tokens_.begin(); it != tokens_.end(); it++) {
		if (it->type == L"Personal ident") {
			retValue_.push_back(constantTable_[it->value].value);
		}
		else if (it->value == L"BP"
			|| it->value == L"SI"
			|| it->value == L"DI"
			|| it->value == L"SP"
			|| it->value == L"BX") {
			retValue_.push_back(L"0h");
		}
		else if (it->value == L"+"
			|| it->value == L"-"
			|| it->value == L"*") {
			retValue_.push_back(it->value);
		}
		else if (it->type == L"Const Hex Digit") {
			retValue_.push_back(it->value);
		}
	}
	return retValue_;
};