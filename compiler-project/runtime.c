#include <stdio.h>

/* PUT */

void putinteger(int val)
{
	printf("%d\n", val);
	//printf("ayyyyy");
}

// Chars are treated as i8
void putcharacter(int val)
{
	char c = (char)val;
	printf("%c", c);
}

void PUTFLOAT(float* val)
{
	printf("%f", *val);
}

void PUTBOOL(int* val)
{
	printf("%s", *val ? "true" : "false");
}

void PUTSTRING(char** str)
{
	printf("%s", *str);
}

/* GET */

void getinteger(int* val)
{
	scanf("%d", val);
}

// Chars are treated as i8
void getcharacter(int* val)
{
	char a;
	scanf(" %c", &a);
	*val = (int)a;
}

void GETFLOAT(float* val)
{
	scanf("%f", val);
}

void GETBOOL(int* val)
{
	scanf("%d", val);
	// Convert to bool; 0 = false, anything else = true
	*val = *val != 0;
}

void GETSTRING(char** str)
{
	scanf("%s", *str);
}