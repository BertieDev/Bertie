#include "z80.h"


char reg[26] = { }; //AF, BC, DE, HL, AF', BC', DE', HL', IR, IX, IY, SP, PC
//flags are Sign, Zero, unused, Half carry, unused, P/V, N/Add/Subtract, Carry


z80::z80()
{
}


z80::~z80()
{
}

void z80::interpret(char op)
{
	if (op >> 6 == 0x00)
	{
		switch (op & 0x07)
		{
		case 0:
			switch ((op & 0x38) >> 3)
			{
			case 0://nop
				break;
			case 1://EX AF, AF'
				char temp;
				temp = reg[0];
				reg[0] = reg[8];
				reg[8] = temp;
				temp = reg[1];
				reg[1] = reg[9];
				reg[9] = temp;
				break;
			case 2://DJNZ, e
				reg[2]--;
				if (reg[2] != 0)
				{
					//TODO: relPC
				}
				break;
			case 3://JR e
				//TODO: relPC
				break;
			default://JR conditional
				//4 and 6 == 1, 5 and 7 == 0 ((+ 1) % 2)
				//4 and 5 check Zero flag, 6 and 7 check Carry flag (6 6 0 0)

				int shift = 6;
				if (((op & 0x38) >> 3) > 5)
					shift = 0;

				if ((reg[1] >> shift) & 0x01 == (((op & 0x38) >> 3) + 1) % 2)
				{
					//TODO: relPC
				}
				break;
			}
			break;
		case 1:
			int r = ((op >> 4) & 3);
			if ((op >> 3) & 1 == 0)//LD register pairs
			{
				
				//TODO: read two bytes, store in register pair r
				//assign16reg(r, value);
			}
			else//ADD register pair -> HL
			{
				assign16reg(2, get16reg(r));
			}
			break;
		case 2:
			switch ((op >> 3) & 7)
			{
			case 0://LD BC, A
				storMem(get16reg(0), reg[0]);
				break;
			case 1://LD DE, A
				storMem(get16reg(1), reg[0]);
				break;
			case 2:
				//TODO: read two bytes for address, store HL to address
				break;
			case 3:
				//TODO: read two bytes for address, store A to address
				break;
			case 4:
				reg[0] = get16reg(0);
				break;
			case 5:
				reg[0] = get16reg(1);
				break;
			case 6:
				//TODO: storMem(loc, get16reg(2));
				break;
			case 7:
				//TODO: assign16reg(2, readMem(short));
				break;
			}
			break;
		case 3:
			int r = ((op >> 4) & 3);
			if ((op >> 3) & 1 == 0)//INC register pair
			{
				assign16reg(r, get16reg(r) + 1);
			}
			else//DEC register pair
			{
				assign16reg(r, get16reg(r) - 1);
			}
			break;
		case 4://INC reg
			int r = ((op >> 3) & 7);
			assign8reg(r, get8reg(r) + 1);
			break;
		case 5://DEC reg
			int r = ((op >> 3) & 7);
			assign8reg(r, get8reg(r) - 1);
			break;
		case 6://LD reg, char
			int r = ((op >> 3) & 7);
			//TODO: assign8reg(r, nextByte);
			break;
		case 7:
			switch ((op >> 3) & 7)
			{
			case 0://RLCA - rotate left with carry - A
				//store high bit to carry, then shift, then add carry
				reg[1] = reg[1] ^ (reg[0] >> 7);
				assign8reg(7, ((get8reg(7) << 1) + (reg[1] & 1)));
				break;
			case 1://RRCA - rotate right with carry - A
				//store low bit to carry, then shift, then add carry
				reg[1] = reg[1] ^ (reg[0] & 1);
				assign8reg(7, ((get8reg(7) >> 1) + ((reg[1] & 1) << 7)));
				break;
			case 2://RLA - rotate left through carry - A
				char temp = reg[1];
				reg[1] = reg[1] ^ (reg[0] >> 7);
				assign8reg(7, ((get8reg(7) << 1) + (temp & 1)));
				break;
			case 3://RRA - rotate right through carry - A
				char temp = reg[1];
				reg[1] = reg[1] ^ (reg[0] & 1);
				assign8reg(7, ((get8reg(7) >> 1) + ((temp & 1) << 7)));
				break;
			case 4://DAA
				//TODO: IDK???
				break;
			case 5://CPL - complement A
				assign8reg(7, get8reg(7) ^ 0xFF);
				break;
			case 6://SCF - Set Carry Flag
				reg[1] = reg[1] | 1;
				break;
			case 7://CCF - Change (invert) Carry Flag
				reg[1] = reg[1] ^ 1;
				break;
			}
			break;
		}
	}


	if (op >> 6 == 0x01) //LD register -> register
	{
		char * sourcStor, * destStor;
		//lower three bits, check what they are
		sourcStor = getReg(op & 0x07);

		destStor = getReg((op & 0x38) >> 3);

		if (op & 0x07 == 6)//use HL as an address for sourcStor
		{
			if ((op & 0x38) >> 3 == 6)//HALT
			{
				//TODO: set state to HALT. if HALT, execute NOPs.
			}
			else
			{
				//TODO: implement
				//*destStor = memory + ((reg[6] << 8) + reg[7]);
			}
		}
		else if ((op & 0x38) >> 3 == 6)//use HL as an address for destStor
		{
			//TODO: implement
			//memory + ((reg[6] << 8) + reg[7]) = *sourcStor;
		}
		else
			*destStor = *sourcStor;

	}

	if (op >> 6 == 0x02) //register operations TODO: (HL) if r is 6
	{
		switch ((op >> 3) & 7)
		{
		case 0://ADD A, r
			reg[0] += get8reg(op & 7);
			break;
		case 1://ADC A, r
			reg[0] += get8reg(op & 7) + (reg[1] & 1);
			break;
		case 2://SUB A, r
			reg[0] -= get8reg(op & 7);
			break;
		case 3://SBC A, r
			reg[0] -= get8reg(op & 7) + (reg[1] & 1);
			break;
		case 4://AND
			reg[0] &= get8reg(op & 7);
			break;
		case 5://XOR
			reg[0] ^= get8reg(op & 7);
			break;
		case 6://OR
			reg[0] |= get8reg(op & 7);
			break;
		case 7://CP - compare, ugh
			if (reg[0] == get8reg(op & 7))//equal, set Z flag
				reg[1] |= 0x40;
			if (reg[0] < get8reg(op & 7))//lesser, set S flag
				reg[1] |= 0x80;
			break;
		}
	}
	if (op >> 6 == 0x03) //Push, pop, ret, stuff like that
	{
		switch (op & 0x7)
		{
		case 0://RET - conditional returns
			bool cc = false;
			switch ((op >> 3) & 0x7)
			{

			case 0://Z 1
			case 1://Z 0
				if ((reg[1] >> 6) & 1 == 1 - ((op >> 3) & 1))
					cc = true;
				break;
			case 2://C 1
			case 3://C 0
				if ((reg[1] >> 0) & 1 == 1 - ((op >> 3) & 1))
					cc = true;
				break;
			case 4://P 1
			case 5://P 0
				if ((reg[1] >> 2) & 1 == 1 - ((op >> 3) & 1))
					cc = true;
				break;
			case 6://S 1
			case 7://S 0
				if ((reg[1] >> 7) & 1 == 1 - ((op >> 3) & 1))
					cc = true;
				break;
			}

			if (cc)//store Stack Pointer (SP) to program counter (PC)
			{
				//TODO: do this i guess? low first
			}
			break;
		case 1://POP and etc
			if ((op >> 3) & 1 == 0)//POP
			{
				int r = ((op >> 4) & 3);
				if (r == 3)
					r = 4;
				//TODO: word at SP gets stored to register pair
				//assign16reg(r, memRead(SP));
			}
			else
			{
				switch ((op >> 4) & 3)
				{
				case 0://RET - return
					//TODO: store SP to PC
					break;
				case 1://EXX - Exchange register pairs BC, DE, HL with BC', DE', HL'
					char temp;
					//TODO: swapping code
					break;
				case 2://JP HL
					//TODO: set HL to PC
					break;
				case 3://LD SP, HL
					//TODO: store HL to SP
					break;
				}
			}
			break;
		case 2://JP
			bool cc = false;
			switch ((op >> 3) & 0x7)
			{

			case 0://Z 1
			case 1://Z 0
				if ((reg[1] >> 6) & 1 == 1 - ((op >> 3) & 1))
					cc = true;
				break;
			case 2://C 1
			case 3://C 0
				if ((reg[1] >> 0) & 1 == 1 - ((op >> 3) & 1))
					cc = true;
				break;
			case 4://P 1
			case 5://P 0
				if ((reg[1] >> 2) & 1 == 1 - ((op >> 3) & 1))
					cc = true;
				break;
			case 6://S 1
			case 7://S 0
				if ((reg[1] >> 7) & 1 == 1 - ((op >> 3) & 1))
					cc = true;
				break;
			}

			if (cc)
			{
				//TODO: store next two bytes to PC
			}
			break;
			//case 3: x=3, z=3
		}
	}
}

char * z80::getReg(int r)
{
	if (r == 7)
		return reg;
	if (r < 6)
	{
		r += 2;
	}
	return (reg + r);
}

void z80::relPC(char num)
{
	//add two to the PC

	//add num (signed) to PC

}

void z80::assign8reg(int r, char val)//AF, BC, DE, HL, AF', BC', DE', HL', IR, IX, IY, SP, PC
{

}

void z80::assign16reg(int r, short val)//BC, DE, HL, SP, AF
{

}

char z80::get8reg(int r)//AF, BC, DE, HL, AF', BC', DE', HL', IR, IX, IY, SP, PC
{
	return *getReg(r);
}

short z80::get16reg(int r)//BC, DE, HL, SP, AF
{
	return 0;
}

void z80::storMem(short loc, char val)
{

}

void z80::storMem(short loc, short val)
{
	//little-endian
}