#pragma once
#include <map>
#include <string>


using namespace std;

typedef map<wstring, wstring> ClassificationMap;
typedef map<wchar_t, wstring> SingleSymbol;

static ClassificationMap classificationMap = {
	pair<wstring, wstring>(L"AL", L"Data Register 8 bit"),
	pair<wstring, wstring>(L"AH", L"Data Register 8 bit"), 
	pair<wstring, wstring>(L"BL", L"Data Register 8 bit"),
	pair<wstring, wstring>(L"BH", L"Data Register 8 bit"),
	pair<wstring, wstring>(L"CL", L"Data Register 8 bit"),
	pair<wstring, wstring>(L"CH", L"Data Register 8 bit"),
	pair<wstring, wstring>(L"DL", L"Data Register 8 bit"),
	pair<wstring, wstring>(L"DH", L"Data Register 8 bit"),

	pair<wstring, wstring>(L"AX", L"Data Register 16 bit"),
	pair<wstring, wstring>(L"BX", L"Data Register 16 bit"),
	pair<wstring, wstring>(L"CX", L"Data Register 16 bit"),
	pair<wstring, wstring>(L"DX", L"Data Register 16 bit"),

	pair<wstring, wstring>(L"SP", L"Stack Pointer Register 16 bit"),
	pair<wstring, wstring>(L"BP", L"Base Pointer Register 16 bit"),
	pair<wstring, wstring>(L"SI", L"Index Register 16 bit"),
	pair<wstring, wstring>(L"DI", L"Index Register 16 bit"),

	pair<wstring, wstring>(L"CS", L"Code Segment"),
	pair<wstring, wstring>(L"DS", L"Data Segment"),
	pair<wstring, wstring>(L"ES", L"Secondary Data Segment"),
	pair<wstring, wstring>(L"SS", L"Stack Segment"),
	pair<wstring, wstring>(L"GS", L"Secondary Data Segment"),
	pair<wstring, wstring>(L"FS", L"Secondary Data Segment"),

	pair<wstring, wstring>(L"END", L"Directive"),
	pair<wstring, wstring>(L"ENDS", L"Directive"),
	pair<wstring, wstring>(L"SEGMENT", L"Directive"),
	pair<wstring, wstring>(L"EQU", L"Directive"),
	pair<wstring, wstring>(L"IF", L"Directive"),
	pair<wstring, wstring>(L"ELSE", L"Directive"),
	pair<wstring, wstring>(L"ENDIF", L"Directive"),

	pair<wstring, wstring>(L"PTR", L"Identifier definition type"),
	pair<wstring, wstring>(L"BYTE", L"Byte type"),
	pair<wstring, wstring>(L"DWORD", L"Dword type"),
	pair<wstring, wstring>(L"WORD", L"Word type"),
	pair<wstring, wstring>(L"DB", L"Define Byte"),
	pair<wstring, wstring>(L"DW", L"Define Word"),
	pair<wstring, wstring>(L"DD", L"Define Double Word"),

	pair<wstring, wstring>(L"STI", L"Machine Command"),
	pair<wstring, wstring>(L"INC", L"Machine Command"),
	pair<wstring, wstring>(L"DEC", L"Machine Command"),
	pair<wstring, wstring>(L"OR", L"Machine Command"),
	pair<wstring, wstring>(L"SBB", L"Machine Command"),
	pair<wstring, wstring>(L"TEST", L"Machine Command"),
	pair<wstring, wstring>(L"MOVSX", L"Machine Command"),
	pair<wstring, wstring>(L"ADD", L"Machine Command"),
	pair<wstring, wstring>(L"JNE", L"Machine Command"),
};

static SingleSymbol singleSymbol{
	pair<wchar_t, wstring>(L'[', L"Single Symbol"),
	pair<wchar_t, wstring>(L']', L"Single Symbol"),
	pair<wchar_t, wstring>(L',', L"Single Symbol"),
	pair<wchar_t, wstring>(L':', L"Single Symbol"),
	pair<wchar_t, wstring>(L'*', L"Single Symbol"),
	pair<wchar_t, wstring>(L'+', L"Single Symbol"),
	pair<wchar_t, wstring>(L'-', L"Single Symbol")
};

