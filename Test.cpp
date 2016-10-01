#include <iostream>
#include "LexicalAnalyzer.h"
#include "FirstPass.h"
#include "SecondPass.h"


using namespace std;

int main() {
	LexicalAnalyzer test(L"test1Prog.asm");
	FirstPass tmp1(test.GetLexicalPackege());
	tmp1.MakePass();
	tmp1.CreateFile(L"test1ProgFirstPass.txt");
	SecondPass tmp2(tmp1.GetFirrstPassPackage());
	tmp2.MakePass();
	tmp2.CreateFile(L"test1ProgSecondPass.txt");
	cout << "Done" << endl;
	system("pause");
	return 0;
};