#include <stdio.h>

void main(){
	unsigned short myTR;

	__asm{
		_emit 0x8B;
		_emit 0x0D;
		_emit 0x00;
		_emit 0x00;
		_emit 0x00;
		_emit 0x00;
	};

	return;
}