#include<stdlib.h>
#include<time.h>
int main()
{

  int* mem = malloc(77);
  while(1)
  {
    srand(time(NULL));
    mem[0] = rand();
    //
  }

}
