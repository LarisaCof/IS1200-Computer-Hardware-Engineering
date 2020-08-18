#include <stdio.h>

char* text1 = "This is a string.";
char* text2 = "Yet another thing.";

int list1[20];  //.space innebär att vi allokerar 80 bitar till variabeln list1
int list2[20];	// En int är strl 4 bit vilket innebär att det är en int array av storleken 80/4=20
				//alltså 20 element
int count = 0; //motsvarar count .word 0

void copycodes(char* a0, int* a1, int* a2)
{
	while (*a0 != 0) //fortsätt till slutet av strängen, i slutet finns nullbyte
	{
		*a1 = *a0; //store word från t0 till vart a1 pekar

		a0++; //ökar 1 så att nästa index pekar på nästa bokstav
		a1++; //adderar pc med 1 i addressen till a1 dvs den kommer ta nästa char i arrayen
		*a2=*a2+1; //tilldelar en instansvariabel värdet av pekaren
	}

}
void work()
{
	copycodes(text1, list1, &count); //&count = räknarvariabel
	copycodes(text2, list2, &count);
}

void printlist(const int* lst){
  printf("ASCII codes and corresponding characters.\n");
  while(*lst != 0){
    printf("0x%03X '%c' ", *lst, (char)*lst);
    lst++;
  }
  printf("\n");
}

void endian_proof(const char* c){
  printf("\nEndian experiment: 0x%02x,0x%02x,0x%02x,0x%02x\n",
         (int)*c,(int)*(c+1), (int)*(c+2), (int)*(c+3));

}

int main(void){
  work();

  printf("\nlist1: ");
  printlist(list1);
  printf("\nlist2: ");
  printlist(list2);
  printf("\nCount = %d\n", count);

  endian_proof((char*) &count);
}
