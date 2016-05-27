#pragma once
class z80
{
public:
	z80();
	~z80();

	void interpret(char op);
	char * getReg(int r);
	void relPC(char num);
	void assign8reg(int r, char val);
	void assign16reg(int r, short val);
	char get8reg(int r);
	short get16reg(int r);

	void storMem(short loc, char val);
	void storMem(short loc, short val);
};

