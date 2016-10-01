#include "FirstPass.h"

FirstPass::FirstPass(){
};

FirstPass::FirstPass(LexicalPackage package) {
	this->package_ = package;
};

FirstPass::~FirstPass() {
}
FirstPassPackage FirstPass::GetFirrstPassPackage()
{
	FirstPassPackage retValue_;
	retValue_.errorList = package_.errorList;
	retValue_.tokenTable = package_.tokenTable;
	retValue_.sentenceTable = package_.sentenceTable;
	retValue_.rows = package_.rows;
	retValue_.result1Pass = result1Pass_;
	retValue_.constantTable = constantTable_;
	retValue_.labelTable = labelTable_;
	retValue_.variableTable = variableTable_;
	retValue_.segmentTable = segmentTable_;
	return retValue_;
};

void FirstPass::MakePass() {
	int i = 0; // iterator for token table
	int itRows = 0;
	bool IFValue = false;
	bool IFBlock = false;
	bool activeBlock = true;
	bool defSegment = false;
	list<Token> firstOperand;
	list<Token> secondOperand;
	list<Token> label;
	list<Token> mnemocode;
	SegmentInfo segmentInfo;
	int offset = 0;
	FirstPassRow row;
	int passedRows = 0;
	Error error;
	for (auto itSentence = package_.sentenceTable.begin(); itSentence != package_.sentenceTable.end(); itSentence++, itRows++) {
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
		//define segment
		if (!defSegment) {
			if (mnemocode.back().value == L"SEGMENT"
				&& label.size() == 1
				&& label.back().type == L"Personal ident"
				&& firstOperand.size() == 0
				&& secondOperand.size() == 0) {
				segmentInfo.name = label.back().value;
				defSegment = true;
				offset = 0;
				row.offset = offset;
				row.row = package_.rows[itRows];
				result1Pass_.push_back(row);
			}
			else {
				if (mnemocode.back().value == L"END") {
					row.offset = 0;
					row.row = package_.rows[itRows];
					result1Pass_.push_back(row);
					return;
				}
				error.numRowError = itRows;
				error.error = L"Error";
				package_.errorList.push_back(error);
			}
		}
		else {
			if (mnemocode.size() != 0) {
				if (mnemocode.back().value == L"IF") {
					if (firstOperand.size() != 0
						&& isCorrcectConstant(firstOperand)) {
						if (CalculateExppression(CreateExpression(firstOperand)) != 0) {
							//If block
							IFBlock = true;
							IFValue = true;
							activeBlock = true;
							i += 2;
							row.offset = offset;
							row.row = package_.rows[itRows];
							result1Pass_.push_back(row);
							mnemocode.clear();
							firstOperand.clear();
							continue;
						}
						else {
							//Else block or ENDIF directive
							IFBlock = true;
							IFValue = false;
							activeBlock = false;
							i += 2;
							passedRows++;
							mnemocode.clear();
							firstOperand.clear();
							continue;
						}
					}
				}
				else if (mnemocode.back().value == L"ELSE") {
					if (IFBlock) {
						if (IFValue) {
							//Pass this block
							activeBlock = false;
							i++;
							row.offset = offset;
							passedRows++;
							mnemocode.clear();
							firstOperand.clear();
							continue;
						}
						else {
							activeBlock = true;
							i++;
							row.offset = offset;
							row.row = package_.rows[itRows];
							result1Pass_.push_back(row);
							mnemocode.clear();
							firstOperand.clear();
							continue;
						}
					}
					else {
						//error
						row.offset = offset;
						row.row = package_.rows[itRows];
						result1Pass_.push_back(row);
						error.numRowError = itRows - passedRows;
						error.error = L"Error";
						package_.errorList.push_back(error);
					}
				}
				else if (mnemocode.back().value == L"ENDIF") {
					IFBlock = false;
					activeBlock = true;
					i++;
					row.offset = offset;
					row.row = package_.rows[itRows];
					result1Pass_.push_back(row);
					mnemocode.clear();
					firstOperand.clear();
					continue;
				}

				if (activeBlock) {
					if (firstOperand.size() == 0) {
						//comands that hasn't operands
						if (mnemocode.back().value == L"STI") {
							row.offset = offset;
							row.row = package_.rows[itRows];
							result1Pass_.push_back(row);
							offset++;//1 byte offset FB
						}
						else if (mnemocode.back().value == L"ENDS") {
							defSegment = false;
							segmentInfo.length = offset;
							//add something
							row.offset = offset;
							row.row = package_.rows[itRows];
							result1Pass_.push_back(row);
							segmentTable_.push_back(segmentInfo);
						}
						else {
							//error
							row.offset = offset;
							row.row = package_.rows[itRows];
							result1Pass_.push_back(row);
							error.numRowError = itRows - passedRows;
							error.error = L"Error";
							package_.errorList.push_back(error);
						}
					}
					else if (firstOperand.size() != 0
						&& secondOperand.size() == 0) {
						//comands that has one operand
						if (mnemocode.back().value == L"DB") {
							if (firstOperand.size() == 1) {
								PersonalIdentInfo tmp;
								tmp.attr = segmentInfo.name;
								tmp.type = L"BYTE";
								tmp.value = ToUpperWstring(ToHex4(offset));
								variableTable_.insert({ label.back().value, tmp });
								row.offset = offset;
								row.row = package_.rows[itRows];
								result1Pass_.push_back(row);
								if (firstOperand.back().type == L"Const Hex Digit") {
									offset++;
								}
								else {
									//stringconstant
									offset += firstOperand.back().length;
								}
							}
						}
						else if (mnemocode.back().value == L"DW") {
							if (firstOperand.size() == 1) {
								if (firstOperand.back().type == L"Const Hex Digit") {
									PersonalIdentInfo tmp;
									tmp.attr = segmentInfo.name;
									tmp.type = L"WORD";
									tmp.value = ToUpperWstring(ToHex4(offset));
									variableTable_.insert({ label.back().value, tmp });
									row.offset = offset;
									row.row = package_.rows[itRows];
									result1Pass_.push_back(row);
									offset += 2;
								}
								else {
									//error
									row.offset = offset;
									row.row = package_.rows[itRows];
									result1Pass_.push_back(row);
									error.numRowError = itRows - passedRows;
									error.error = L"Error";
									package_.errorList.push_back(error);
								}
							}
						}
						else if (mnemocode.back().value == L"DD") {
							if (firstOperand.size() == 1) {
								if (firstOperand.back().type == L"Const Hex Digit") {
									PersonalIdentInfo tmp;
									tmp.attr = segmentInfo.name;
									tmp.type = L"DWORD";
									tmp.value = ToUpperWstring(ToHex4(offset));
									variableTable_.insert({ label.back().value, tmp });
									row.offset = offset;
									row.row = package_.rows[itRows];
									result1Pass_.push_back(row);
									offset += 4;
								}
								else {
									//error
									row.offset = offset;
									row.row = package_.rows[itRows];
									result1Pass_.push_back(row);
									error.numRowError = itRows - passedRows;
									error.error = L"Error";
									package_.errorList.push_back(error);
								}
							}
						}
						else if (mnemocode.back().value == L"EQU") {
							if (firstOperand.size() == 1) {
								if (firstOperand.back().type == L"Const Hex Digit") {
									PersonalIdentInfo tmp;
									tmp.attr = segmentInfo.name;
									tmp.value = firstOperand.back().value;
									if (firstOperand.back().type == L"Const Hex Digit") {
										tmp.type = L"NUMBER";
									}
									else {
										tmp.type = L"STRING";
									}
									constantTable_.insert({ label.back().value, tmp });
									row.offset = offset;
									row.row = L"=" + firstOperand.back().value + L"  " + package_.rows[itRows];
									result1Pass_.push_back(row);
								}
							}
						}
						else if (mnemocode.back().value == L"JNE") {
							row.offset = offset;
							row.row = package_.rows[itRows];
							result1Pass_.push_back(row);
							if (isDefinedfLabel(firstOperand.front().value)) {
								offset += 2;
							}
							else {
								LabelInfo tmpLabelInfo;
								tmpLabelInfo.attr = segmentInfo.name;
								tmpLabelInfo.definedBeforeUsing = false;
								tmpLabelInfo.type = L"L NEAR";
								labelTable_.insert({ firstOperand.front().value, tmpLabelInfo });
								offset += 4;
							}
						}
						else if (mnemocode.back().value == L"INC") {
							OperandInfo firstOperandInfo = GetOperandInfo(firstOperand);
							if (firstOperandInfo.isCorrect
								&& firstOperandInfo.isRegiser) {
								row.offset = offset;
								row.row = package_.rows[itRows];
								result1Pass_.push_back(row);
								if (classificationMap[firstOperand.back().value] == L"Data Register 8 bit") {
									offset += 2;
								}
								else {
									offset++;
								}
							}
							else {
								//error
								row.offset = offset;
								row.row = package_.rows[itRows];
								result1Pass_.push_back(row);
								error.numRowError = itRows - passedRows;
								error.error = L"Error";
								package_.errorList.push_back(error);
							}
						}
						else if (mnemocode.back().value == L"DEC") {
							OperandInfo firstOperandInfo = GetOperandInfo(firstOperand);
							if (firstOperandInfo.isCorrect
								&& firstOperandInfo.isMemory) {
								row.offset = offset;
								row.row = package_.rows[itRows];
								result1Pass_.push_back(row);
								if (firstOperandInfo.hasPrefix){
									offset++;
								}
								offset += 2 + firstOperandInfo.size;
							}
							else {
								//error
								row.offset = offset;
								row.row = package_.rows[itRows];
								result1Pass_.push_back(row);
								error.numRowError = itRows - passedRows;
								error.error = L"Error";
								package_.errorList.push_back(error);
							}
						}
						else {
							//error
							row.offset = offset;
							row.row = package_.rows[itRows];
							result1Pass_.push_back(row);
							error.numRowError = itRows - passedRows;
							error.error = L"Error";
							package_.errorList.push_back(error);
						}
					}
					else {
						//has first and second operand
						if (mnemocode.back().value == L"TEST") {
							OperandInfo firstOperandInfo = GetOperandInfo(firstOperand);
							OperandInfo secondOperandInfo = GetOperandInfo(secondOperand);
							if (firstOperandInfo.isCorrect
								&& firstOperandInfo.isMemory
								&& secondOperandInfo.isCorrect
								&& secondOperandInfo.isRegiser) {
								row.offset = offset;
								row.row = package_.rows[itRows];
								result1Pass_.push_back(row);
								if (firstOperandInfo.hasPrefix) {
									offset++;
								}
								offset += 2 + firstOperandInfo.size;
							}
							else {
								//error
								row.offset = offset;
								row.row = package_.rows[itRows];
								result1Pass_.push_back(row);
								error.numRowError = itRows - passedRows;
								error.error = L"Error";
								package_.errorList.push_back(error);
							}
						}
						else if (mnemocode.back().value == L"OR") {
							OperandInfo firstOperandInfo = GetOperandInfo(firstOperand);
							OperandInfo secondOperandInfo = GetOperandInfo(secondOperand);
							if (firstOperandInfo.isCorrect
								&& firstOperandInfo.isRegiser
								&& secondOperandInfo.isCorrect
									&& secondOperandInfo.isConstant) {
								row.offset = offset;
								row.row = package_.rows[itRows];
								result1Pass_.push_back(row);
								if (firstOperand.back().value == L"AX"
									|| firstOperand.back().value == L"AL") {
									offset++;
								}
								else {
									offset += 2;
								}
								if (classificationMap[firstOperand.back().value] == L"Data Register 16 bit") {
									offset += 2;
								}
								else {
									offset++;
								}
							}
							else {
								//error
								row.offset = offset;
								row.row = package_.rows[itRows];
								result1Pass_.push_back(row);
								error.numRowError = itRows - passedRows;
								error.error = L"Error";
								package_.errorList.push_back(error);
							}
						}
						else if (mnemocode.back().value == L"SBB") {
							OperandInfo firstOperandInfo = GetOperandInfo(firstOperand);
							OperandInfo secondOperandInfo = GetOperandInfo(secondOperand);
							if (firstOperandInfo.isCorrect
								&& firstOperandInfo.isRegiser
								&& secondOperandInfo.isCorrect
								&& secondOperandInfo.isMemory) {
								row.offset = offset;
								row.row = package_.rows[itRows];
								result1Pass_.push_back(row);
								if (secondOperandInfo.hasPrefix) {
									offset++;
								}
								offset += 2;
								offset += secondOperandInfo.size;
							}
							else {
								//error
								row.offset = offset;
								row.row = package_.rows[itRows];
								result1Pass_.push_back(row);
								error.numRowError = itRows - passedRows;
								error.error = L"Error";
								package_.errorList.push_back(error);
							}
						}
						else if (mnemocode.back().value == L"MOVSX") {
							OperandInfo firstOperandInfo = GetOperandInfo(firstOperand);
							OperandInfo secondOperandInfo = GetOperandInfo(secondOperand);
							if (firstOperandInfo.isCorrect
								&& firstOperandInfo.isRegiser
								&& secondOperandInfo.isCorrect
								&& secondOperandInfo.isRegiser) {
								row.offset = offset;
								row.row = package_.rows[itRows];
								result1Pass_.push_back(row);
								offset += 3;
							}
							else {
								//error
								row.offset = offset;
								row.row = package_.rows[itRows];
								result1Pass_.push_back(row);
								error.numRowError = itRows - passedRows;
								error.error = L"Error";
								package_.errorList.push_back(error);
							}
						}
						else if (mnemocode.back().value == L"ADD") {
							OperandInfo firstOperandInfo = GetOperandInfo(firstOperand);
							OperandInfo secondOperandInfo = GetOperandInfo(secondOperand);
							if (firstOperandInfo.isCorrect
								&& firstOperandInfo.isMemory
								&& secondOperandInfo.isCorrect
								&& secondOperandInfo.isConstant) {
								row.offset = offset;
								row.row = package_.rows[itRows];
								result1Pass_.push_back(row);
								if (firstOperandInfo.hasPrefix) {
									offset++;
								}
								offset += 2;
								offset += firstOperandInfo.size + secondOperandInfo.size;
							}
							else {
								//error
								row.offset = offset;
								row.row = package_.rows[itRows];
								result1Pass_.push_back(row);
								error.numRowError = itRows - passedRows;
								error.error = L"Error";
								package_.errorList.push_back(error);
							}
						}
						else {
							//error
							row.offset = offset;
							row.row = package_.rows[itRows];
							result1Pass_.push_back(row);
							error.numRowError = itRows - passedRows;
							error.error = L"Error";
							package_.errorList.push_back(error);
						}
					}
				}
				else {
					passedRows++;
				}
			}
		}
		if (label.size() != 0) {
			//define label
			if (mnemocode.size() == 0
				|| mnemocode.front().value == L"JNE") {
				row.offset = offset;
				row.row = package_.rows[itRows];
				result1Pass_.push_back(row);
				stringstream ss;
				string bufs;
				ss << hex << offset;
				ss >> bufs;
				wstring bufws(bufs.begin(), bufs.end());
				if (isDefinedfLabel(label.front().value)) {
					//set offset
					auto it = labelTable_.find(label.front().value);
					it->second.value = ToUpperWstring(ToHex4(offset));
				}
				else {
					LabelInfo tmpLabelInfo;
					tmpLabelInfo.attr = segmentInfo.name;
					tmpLabelInfo.type = L"L NEAR";
					tmpLabelInfo.definedBeforeUsing = true;
					tmpLabelInfo.value = ToUpperWstring(ToHex4(offset));
					labelTable_.insert({ label.front().value, tmpLabelInfo });
				}
			}
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

bool FirstPass::isCorrectExpression(list<Token> _expression)
{
	bool isOperation = false; //first token must be a number or '('
	int countOpened = 0;
	for (auto it = _expression.begin(); it != _expression.end(); it++) {
		if (it->value == L"(") {
			countOpened++;
		}
		else if (it->value == L")") {
			countOpened--;
		}
		else if (it->value != L"+"
			&& it->value != L"-"
			&& it->value != L"*") {
			if (!isOperation) {
				isOperation = !isOperation;
			}
			else {
				return false;
			}
		}
		else {
			if (isOperation) {
				if (it->type == L"Const Hex Digit") {
					isOperation = !isOperation;
				}
				else {
					return false;
				}
			}
			else {
				return false;
			}
		}
	}
	if (countOpened == 0) {
		return true;
	}
	return false;
};

OperandInfo FirstPass::GetOperandInfo(list<Token> tokens_){
	OperandInfo retValue_;
	if (isRegister(tokens_)) {
		retValue_.isRegiser = true;
	}
	else if (isCorrcectConstant(tokens_)) {
		retValue_.isConstant = true;
		retValue_.size = getOperandSize(tokens_);
	}
	else if (isCorrcectMemory(tokens_)) {
		retValue_.isMemory = true;
		retValue_.hasPrefix = hasPrefix(tokens_);
		retValue_.size = getOperandSize(tokens_);
	}
	else {
		retValue_.isCorrect = false;
	}
	return retValue_;
}

bool FirstPass::isRegister(list<Token> tokens_)
{
	if (tokens_.size() == 1) {
		if (classificationMap[tokens_.front().value] == L"Data Register 8 bit"
			|| classificationMap[tokens_.front().value] == L"Data Register 16 bit") {
			return true;
		}
	}
	return false;
}

bool FirstPass::isCorrcectConstant(list<Token> tokens_)
{
	list<Token> expression;
	Token tmp;
	for (auto it = tokens_.begin(); it != tokens_.end(); it++) {
		if (isDefinedConstant(it->value)) {
			//defined constant
			tmp.type = L"Const Hex Digit";
			tmp.value = constantTable_[it->value].value;
			expression.push_back(tmp);
		}
		else {
			if (it->type == L"Single Symbol"
				|| it->type == L"Const Hex Digit") {
				expression.push_back(*it);
			}
		}
	}

	return isCorrectExpression(expression);
}

bool FirstPass::isCorrcectMemory(list<Token> tokens_){
	bool hasBX = false;
	bool hasBP = false;
	bool hasDI = false;
	bool hasSI = false;
	list<Token> expression;
	Token tmp;
	auto it = tokens_.begin();
	if (it != tokens_.end() 
		&&(it->value == L"BYTE"
		|| it->value == L"WORD"
		|| it->value == L"DWORD")) {
		it++;
		if (it != tokens_.end()
			&& (it->value == L"PTR")) {
			it++;
		}
	}
	if (it != tokens_.end()
		&& hasPrefix(tokens_)) {
		it++;
		it++;
	}
	if (it != tokens_.end()
		/*&& (it->value == L"[")
		&& (tokens_.end()->value == L"]")*/) {
		while (it != tokens_.end()
			&& (it->value != L"]")) {
			if (it->value != L"[") {
				if (it->value == L"BX") {
					if (!hasBX && !hasBP) {
						tmp.type = L"Const Hex Digit";
						tmp.value = L"0h";
						expression.push_back(tmp);
						hasBX = !hasBX;
					}
					else {
						return false;
					}
				}
				else if (it->value == L"BP") {
					if (!hasBX && !hasBP) {
						tmp.type = L"Const Hex Digit";
						tmp.value = L"0h";
						expression.push_back(tmp);
						hasBP = !hasBP;
					}
					else {
						return false;
					}
				}
				else if (it->value == L"SI") {
					if (!hasSI && !hasDI) {
						tmp.type = L"Const Hex Digit";
						tmp.value = L"0h";
						expression.push_back(tmp);
						hasSI = !hasSI;
					}
					else {
						return false;
					}
				}
				else if (it->value == L"DI") {
					if (!hasSI && !hasDI) {
						tmp.type = L"Const Hex Digit";
						tmp.value = L"0h";
						expression.push_back(tmp);
						hasDI = !hasDI;
					}
					else {
						return false;
					}
				}
				else if (it->type == L"Const Hex Digit") {
					expression.push_back(*it);
				}
				else if (it->value == L"+"
					|| it->value == L"-"
					|| it->value == L"*") {
					expression.push_back(*it);
				}
				else if (isDefinedConstant(it->value)) {
					expression.push_back(*it);
				}
				else {
					return false;
				}
			}
			it++;
		}
	}
	else {
		return false;
	}
	if (hasBP || hasBX) {
		return true;
	}
	else {
		return false;
	}
};

bool FirstPass::hasPrefix(list<Token> tokens_){
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

bool FirstPass::isDefinedVariable(wstring varName_){
	auto it = variableTable_.find(varName_);
	if (it != variableTable_.end()) {
		return true;
	}
	return false;
};

bool FirstPass::isDefinedfLabel(wstring varName_)
{
	auto it = labelTable_.find(varName_);
	if (it != labelTable_.end()) {
		return true;
	}
	return false;
};

void FirstPass::CreateFile(wstring fileName_){
	wofstream file(fileName_);
	for (auto it = result1Pass_.begin(); it != result1Pass_.end(); it++) {
		file << Pass::ToHex4(it->offset) << '\t' << it->row << endl;
	}
	file << endl << endl;
	file.fill(L' ');
	file.width(25);
	file << left << L"Name";
	file.width(10);
	file << left << L"Size";
	file << left << L"Length" << endl;

	for (auto it = segmentTable_.begin(); it != segmentTable_.end();  it++) {
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

bool FirstPass::isDefinedConstant(wstring varName_) {
	auto it = constantTable_.find(varName_);
	if (it != constantTable_.end()) {
		return true;
	}
	return false;
};

int FirstPass::getOperandSize(list<Token> tokens_) {
	list<wstring> expression;
	auto it = tokens_.begin();
	if (isCorrcectMemory(tokens_)) {
		while (it != tokens_.end()
			&& it->value != L"[") {
			it++;
		}
	}
	if (it->value == L"[") {
		it++;
	}
	while (it != tokens_.end()
		&& it->value != L"]") {
		if (it->value == L"BP"
			|| it->value == L"BX"
			|| it->value == L"DI"
			|| it->value == L"SI") {
			expression.push_back(L"0h");
		}
		else if (isDefinedConstant(it->value)) {
			expression.push_back(constantTable_[it->value].value);
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


list<wstring> FirstPass::CreateExpression(list<Token> tokens_) {
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