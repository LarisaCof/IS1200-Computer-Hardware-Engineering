#include <stdio.h>

int is_prime(int n) {

  int i = 2;
  for (i = 2; i < n; i++) {
    if (n % i == 0) { //om n % i = 0 betyder det att talet inte är ett primtal
      return 0; //skicka 0 då
    }
  }
  return 1; //om det är ett primtal skickas en 1a.
}

int main(void) {
  printf("%d\n", is_prime(11));  // 11 is a prime.      Should print 1.
  printf("%d\n", is_prime(383)); // 383 is a prime.     Should print 1.
  printf("%d\n", is_prime(987)); // 987 is not a prime. Should print 0.
}
