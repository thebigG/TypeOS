#include<stdlib.h>
int main(void)
{
  int* list =  malloc(sizeof(int)* 128);
  int i =0;
  for(i =0 ;i<128;i++)
  {
    list[i] = 12;
  }
  while(1);
  return 0;
}
                        
