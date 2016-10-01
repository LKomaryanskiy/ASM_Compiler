#include "Pass.h"

int Pass::SimpleOpeartion(wstring _firstOperand, wstring _secondOperand, wstring _operation) {
	int first;
	int second;
	_firstOperand.insert(0, L"0x");
	_firstOperand.erase(_firstOperand.size() - 1, 1);
	_secondOperand.insert(0, L"0x");
	_secondOperand.erase(_secondOperand.size() - 1, 1);
	stringstream ss;
	string fop(_firstOperand.begin(), _firstOperand.end());
	string sop(_secondOperand.begin(), _secondOperand.end());

	ss << hex << fop;
	ss >> first;
	ss.clear();
	ss << hex << sop;
	ss >> second;

	switch (_operation[0]) {
	case '+': {
		return first + second;
	}
	case '-': {
		return first - second;
	}
	case '*': {
		return first * second;
	}
	default:
		return 0;
	}
};

int Pass::CalculateExppression(list<wstring> _expression) {
	stack<wstring> data;
	stack<wstring> operations;
	wstring tmp;
	wstring lastOperation;
	wstring operation;
	wstring firstOperand;
	wstring secondOperand;
	stringstream ss;
	for (auto it = _expression.begin(); it != _expression.end(); it++) {
		tmp = *it;
		if (tmp == L"+"
			|| tmp == L"-"
			|| tmp == L"*"
			|| tmp == L"("
			|| tmp == L")") {
			if (tmp == L"(") {
				operations.push(tmp);
			}
			else if (tmp == L")") {
				operation = operations.top();
				operations.pop();
				firstOperand = data.top();
				data.pop();
				secondOperand = data.top();
				data.pop();
				while (operation != L"(") {
					ss << hex << SimpleOpeartion(firstOperand, secondOperand, operation);
					string toConvert;
					ss >> toConvert;
					toConvert.push_back('h');
					wstring buf(toConvert.begin(), toConvert.end());
					ss.clear();
					data.push(buf);
					operation = operations.top();
					operations.pop();
					if (operation != L"(") {
						firstOperand = data.top();
						data.pop();
						secondOperand = data.top();
						data.pop();
					}
				}
			}
			else {
				if (operations.size() != 0) {
					if ((tmp == L"*"
						&& (lastOperation == L"-"
							|| lastOperation == L"+"))
						|| (lastOperation == L"(")) {
						operations.push(tmp);
					}
					else {
						operation = operations.top();
						operations.pop();
						firstOperand = data.top();
						data.pop();
						secondOperand = data.top();
						data.pop();
						while (operation != L"("
							|| ((operation != L"+"
								|| operation != L"-")
								&& tmp != L"*")) {
							ss << hex << SimpleOpeartion(firstOperand, secondOperand, operation);
							string toConvert;
							ss >> toConvert;
							toConvert.push_back('h');
							wstring buf(toConvert.begin(), toConvert.end());
							ss.clear();
							data.push(buf);
							if (operations.size() != 0) {
								operation = operations.top();
								operations.pop();
								firstOperand = data.top();
								data.pop();
								secondOperand = data.top();
								data.pop();
							}
							else {
								operations.push(tmp);
								break;
							}
						}
					}
				}
				else {
					operations.push(tmp);
				}
			}
			if (operations.size() != 0) {
				tmp = operations.top();
				lastOperation = tmp;
			}
		}
		else {
			data.push(tmp);
		}
	}

	while (operations.size() != 0
		&& data.size() != 0) {
		operation = operations.top();
		operations.pop();
		firstOperand = data.top();
		data.pop();
		secondOperand = data.top();
		data.pop();
		ss << hex << SimpleOpeartion(firstOperand, secondOperand, operation);
		string toConvert;
		ss >> toConvert;
		toConvert.push_back('h');
		wstring buf(toConvert.begin(), toConvert.end());
		ss.clear();
		data.push(buf);
	}

	int result_;
	string buf(data.top().begin(), data.top().end());
	ss << hex << buf;
	ss >> result_;
	return result_;
};

wstring Pass::ToUpperWstring(wstring str_) {
	for (auto it = str_.begin(); it != str_.end(); it++) {
		*it = toupper(*it);
	}
	return str_;
};

wstring Pass::ToHex4(int value) {
	stringstream ss;
	ss << hex << value;
	string bufst;
	ss >> bufst;
	for (int i = bufst.size(); i < 4; i++) {
		bufst = '0' + bufst;
	}
	wstring retValue(bufst.begin(), bufst.end());
	while (retValue.size() > 4) {
		retValue.erase(0, 1);
	}
	retValue = ToUpperWstring(retValue);
	return retValue;
};

wstring Pass::ToHex2(int value) {
	stringstream ss;
	ss << hex << value;
	string bufst;
	ss >> bufst;
	for (int i = bufst.size(); i < 2; i++) {
		bufst = '0' + bufst;
	}
	wstring retValue(bufst.begin(), bufst.end());
	while (retValue.size() > 2) {
		retValue.erase(0, 1);
	}
	retValue = ToUpperWstring(retValue);
	return retValue;
};

