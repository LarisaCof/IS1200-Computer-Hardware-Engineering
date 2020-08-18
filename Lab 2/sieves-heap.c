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
  int *A = (int*) malloc((n-1)*sizeof(int)); //allokerar minne (för int) och returnerar en pekare till det

  for (pos = 0; pos <= (n-2); pos++)
  {
    A[pos] = 1; //sätter allt till true
  }
   int count = 0; //räknare
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
  free(A);
}

void mean_sieves (int n) {
    //tilldelar variabeln count, värdet 0
		int A[n];         // skapar en array A med längden n (n är maximala antalet primtal)

		for (int i = 0; i < n; i++) {
   		A[i] = 1;                  //sätter alla värden till 1 (true)
		}
		for (int i = 2; i < sqrt(n); i++) {
  		if (A[i]== 1) {            // är i == 1 gör loopen

		for (int j = i*i; j < n; j+=i) {
      A[j] = 0;         //sätter de värden i respektive index till 0 , då det talet ej är ett primtal
                        //stryker varannat
    }
  }
}

	int k = 0;	//skapar en räknare för antalet primtal
	int sum = 0;
	double mean = 0;
        for (int i=2; i < n; i++)
        {
          if(A[i] == 1)	//för alla tal som är primtal
          {
            sum += i;	//öka summan med det nya primtalet
            k++;

          }
        }
       mean = (double ) sum/k;	//medelvärdet

      printf("%f", mean);		//skriv ut

}

  /*  for (int i = 2; i < n; i++)
    {
      if (A[i] == 1)
      {
        count++;
        print_number ((count % 6)? i : -i); //om resten blir 0 --> -i  om resten inte är 0 --> i
      }
    }
*/

/*int main () {
	mean_sieves(9);
	print_numbers(10);
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
