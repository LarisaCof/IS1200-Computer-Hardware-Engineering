#include <stdio.h>
#include <stdlib.h>

#define COLUMNS 6

int is_prime(int n) {

  int i = 2;
  for (i = 2; i < n; i++) {
    if (n % i == 0) { //om n % i = 0 betyder det att talet inte är ett primtal
      return 0; //skicka 0 då
    }
  }
  return 1; //om det är ett primtal skickas en 1a.
}

//gets one parameter representing the prime number that should be printed
//
int count = 0;
void print_numbers (int prim)
{
	/*if ( prim > 0)
	{
		printf("%10i", prim); //skriv ut primtalet
	}
	else
	{
		printf("%10i\n", -prim); //om vi får ett negativt argument hoppar vi till en ny rad och skriver -(-prim) = prim.
	}*/
  count += 1;
  if (count % 6 == 0) {
  printf("%10i\n", prim);
}
  else {
  printf("%10i", prim);
}

}

//should print all prime numbers from 2 to n by calling function print_numbers and is prime
void print_primes (int n)
{
	if (n<2) {
		printf("Finns inga primtal mindre än: %10i", n);
	}
    else
		{
			int a = 2;
			for(a=2; a<n; a++){
				if (is_prime(a) == 1 )
				{
				  print_numbers(a);
				}
			}
		}

}

// 'argc' contains the number of program arguments, and
// 'argv' is an array of char pointers, where each
// char pointer points to a null-terminated string.
 int main(int argc, char *argv[]){
  if(argc == 2)
    print_primes(atoi(argv[1]));
  else
    printf("Please state an interger number.\n");
  return 0;
}
