// main.cpp : HW9 IAS simulator code
// Author: Cole Blevins

#include "stdafx.h"
#include <string>
#include <regex>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
using namespace std;

//Global variables
int PC = 0, MAR = 0, AC = 0, MQ = 0;
string IR, MBR;

//Memory class------------------------------
class Memory {
private:
	string mem[1000];
public:
	void setMem(int i, string line);
	string getMem(int i);
	Memory();//constructor
};

//Constructor
Memory::Memory(void) {
	for (int i = 0; i < 1000; i++)
		mem[i] = "nop";
}

void Memory::setMem(int i, string line) {
	mem[i] = line;
}

string Memory::getMem(int i) {
	return mem[i];
}


//String to Integer
int GetIntVal(string strConvert) {
	int intReturn;

	intReturn = atoi(strConvert.c_str());

	return(intReturn);
}

//Integer to String
string GetStrVal(int intConvert) {

	ostringstream cstr;
	cstr << intConvert;
	return cstr.str();
}

void printMemory(Memory &mem) {
	cout << "\n Memory" << endl;
	for (int i = 0; i < PC; i++) {
		cout << i << " " << mem.getMem(i) << endl;
	}
}

void Execute(int command, int address, Memory &mem) {
	int i = GetIntVal(mem.getMem(address));
	switch (command) {
	case 1://load MQ
		AC = MQ;
		break;
	case 2://load MQ, M(X)
		MQ = i;
		break;
	case 3://stor M(X)
		mem.setMem(address, GetStrVal(AC));
		break;
	case 4://load M(X)
		AC = i;
		break;
	case 5://load -M(X)
		AC = (-1)*(i);
		break;
	case 6://load |M(X)|
		AC = std::abs(i);
		break;
	case 7://load -|M(X)|
		AC = (-1)*(std::abs(i));
		break;
	case 8://jump M(X)
		PC = address;
		break;
	case 9://jump+ M(X)
		if (AC >= 0)
			PC = address;
		break;
	case 10://add M(X)
		AC += i;
		break;
	case 11://add |M(X)|
		AC += std::abs(i);
		break;
	case 12://sub M(X)
		AC -= i;
		break;
	case 13://sub |M(X)
		AC -= std::abs(i);
		break;
	case 14://mul M(X)
		AC = MQ * i;
		break;
	case 15://div M(X)
		MQ = AC / i;
		AC %= i;
		break;
	case 16://lsh
		AC *= 2;
		break;
	case 17://rsh
		AC /= 2;
		break;
	}
}

void Decode(Memory &mem) {
	int command = 0; match_results<std::string::const_iterator> result;
	bool ok = true;			
				
	if (regex_match(IR, result, regex("load MQ")))
		command = 1;
	else if (regex_match(IR, result, regex("load MQ,M\\((\\d+)\\)")))
		command = 2;
	else if (regex_match(IR, result, regex("stor M\\((\\d+)\\)")))
		command = 3;
	else if (regex_match(IR, result, regex("load M\\((\\d+)\\)")))
		command = 4;
	else if (regex_match(IR, result, regex("load -M\\((\\d+)\\)")))
		command = 5;
	else if (regex_match(IR, result, regex("load \\|M\\((\\d+)\\)\\|")))
		command = 6;
	else if (regex_match(IR, result, regex("load -\\|M\\((\\d+)\\)\\|")))
		command = 7;
	else if (regex_match(IR, result, regex("jump M\\((\\d+)\\)")))
		command = 8;
	else if (regex_match(IR, result, regex("jump\\+ M\\((\\d+)\\)")))
		command = 9;
	else if (regex_match(IR, result, regex("add M\\((\\d+)\\)")))
		command = 10;
	else if (regex_match(IR, result, regex("add \\|M\\((\\d+)\\)\\|")))
		command = 11;
	else if (regex_match(IR, result, regex("sub M\\((\\d+)\\)")))
		command = 12;
	else if (regex_match(IR, result, regex("sub \\|M\\((\\d+)\\)\\|")))
		command = 13;
	else if (regex_match(IR, result, regex("mul M\\((\\d+)\\)")))
		command = 14;
	else if (regex_match(IR, result, regex("div M\\((\\d+)\\)")))
		command = 15;
	else if (regex_match(IR, result, regex("lsh")))
		command = 16;
	else if (regex_match(IR, result, regex("rsh")))
		command = 17;
	else if (regex_match(IR, regex(".(.+)")) || IR.compare("nop") == 0 || IR.compare("begin") == 0 || IR.compare("halt") == 0) {
		ok = false;
	}
	else {
		cout << "Unable to decode" << endl;
	}	

	//Execute
	if (ok) {
		Execute(command, GetIntVal(result[1]), mem);
	}
}

//Read and store the file in memory
void read(string file, Memory &mem) {	
	string line; bool ok = true; match_results<std::string::const_iterator> result;
	ifstream openFile(file);

	if (!openFile.is_open()) {
		cout << "File not found" << endl;
		exit(EXIT_FAILURE);
	}

	cout << "Code" << endl;

	while (getline(openFile, line)) {
		if (!regex_match(line, regex("(\\d+) (.*)"))) {
			cout << "Unable to load file" << endl;
			break;
		}
		else if (regex_match(line, result, regex("(\\d+) (.*)"))) {
			cout << line << endl;
			mem.setMem(GetIntVal(result[1]), result[2]);
			if (result[2].compare("begin") == 0) {
				PC = GetIntVal(result[1]);
				IR = result[2];
			}
		}
	}
	
	openFile.close();
}

// Main----------------------------------------------
int main()
{
	string file, code;
	Memory mem;
	cout << "Please type the name of your file: ";
	getline(cin, file);

	read(file, mem);

	//Fetch Execute cycle
	cout << "\nExecution" << endl;
	while (IR.compare("halt") != 0) {
		MAR = PC;
		MBR = mem.getMem(MAR);
		IR = MBR;
		cout << "PC = " << PC << " IR = " << IR << endl;
		PC++;//increment PC
		Decode(mem);
		cout << "PC = " << PC << " AC = " << AC << " MQ = " << MQ << endl;
		cout << "-------------------------------------------------" << endl;
		//system("pause");
	}

	printMemory(mem);
	
    return 0;
}

