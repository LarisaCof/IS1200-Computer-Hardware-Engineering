#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define COLUMNS 6


//gets one parameter representing the prime number that should be printed
//

void print_numbers (int prim)
{
	if ( prim > 0)
	{
		printf("%10i", prim); //skriv ut primtalet
	}
	else
	{
		printf("%10i\n", -prim); //om vi får ett negativt argument hoppar vi till en ny rad och skriver -(-prim) = prim.
	}
}

//should print all prime numbers from 2 to n by calling function print_numbers and is prime
void print_sieves(int n)
{
  int pos = 0;
  int A[n-2];

  for (pos = 0; pos <= (n-2); pos++) {
    A[pos] = 1; //sätter allt till true
  }
   int count = 0;
   int i = 2;
   int j = 1;
  for (i = 2; i <=n; i++) {
    if (A[i-2] == 1)
	{
      for (j = 1; j*i <=n; ++j) {
        A[i*j-2] = 0;
      }
	  count++;
      int rad = count % 6;
      print_numbers( (rad)? i : -i);
    }

  }
}


/*int main () {
}*/

// 'argc' contains the number of program arguments, and
// 'argv' is an array of char pointers, where each
// char pointer points to a null-terminated string.
int main(int argc, char *argv[]){
  if(argc == 2)
    print_sieves(atoi(argv[1]));
  else
    printf("Please state an interger number.\n");
  return 0;
}
