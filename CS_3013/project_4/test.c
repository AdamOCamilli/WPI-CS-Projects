#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct pte {
  u_int8_t bit1a:1;
  u_int8_t bit2a:2;
  u_int8_t bit2b:2;
  u_int8_t bit1b:1;
} pte;

unsigned char global[3];

void foo();

int main() {

  foo();
  
  for (int i = 0; i < 3; i++)
    printf("%d %d %d %d\n", ((pte*) &global[i])->bit1a,((pte*) &global[i])->bit2a,((pte*) &global[i])->bit2b,((pte*) &global[i])->bit1b);
  printf("Char: %d\n", global[0]);
  return 0;

}

void foo() {
  pte e1 = {1,3,3,1};
  pte e2 = {0,0,0,0};
  pte e3 = {0,0,0,0};
  pte entries[3] = {e1,e2,e3};

  for (int i = 0; i < 3; i++) 
    memcpy(&global[i], &entries[i], sizeof(pte));
}
