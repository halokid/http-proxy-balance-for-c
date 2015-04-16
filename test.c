#include <stdio.h>

void main()
{
	
	char *a = "abced";
    char s[5];
    strcpy(s, a);
    // s = a;
	printf("%s\n", a);
	printf("%s\n", s);
}
