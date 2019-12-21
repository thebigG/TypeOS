#define POEM "p"
#define SHORT_NOVEL "shn"
#define SMALL_NOVEL "smn"
#define MEDIUM_NOVEL "mn"
#define LARGE_NOVEL "ln"
#define SUPER_NOVEL "sp"
#define NO_CONTEXT "nc"
#include "scriptfs_util_userspace.h"
#include <string.h>
#include <stdio.h>
int main(int argc, char** argv)
{
  printf("working\n");
  //  enum scriptfs_context new_context;
  if(argc<2)
{
  printf("Please pass in the context\n");
  return -1;
}
  printf("value of argc: %d\n", argc);
  char context_string[5];
  printf("working2\n");
  strcpy(context_string,argv[1]);
  int syscall_value = -1;
  if(strcmp(context_string, POEM) == 0 )
{
  syscall_value =   syscall(335,scriptfs_context_POEM); //assign context
}
  else if(strcmp(context_string, SHORT_NOVEL) == 0)
{
  syscall_value = syscall(335,scriptfs_context_SHORT_NOVEL); //asign context
}
  else if(strcmp(context_string, SMALL_NOVEL) == 0)
{
  syscall_value = syscall(335, scriptfs_context_SMALL_NOVEL);
}
  else if(strcmp(context_string, MEDIUM_NOVEL) == 0)
{
  syscall_value =  syscall(335, scriptfs_context_MEDIUM_NOVEL); //assign context 
}
  else if (strcmp(context_string, LARGE_NOVEL) == 0)
{
  syscall_value =  syscall(335, scriptfs_context_LARGE_NOVEL);  
}
  else if(strcmp(context_string, SUPER_NOVEL) == 0 )
{
  syscall_value =  syscall(335, scriptfs_context_SUPER_NOVEL);
}
  else if (strcmp(context_string, NO_CONTEXT) == 0)
{
  syscall_value =  syscall(335, scriptfs_context_NO_CONTEXT);
}
  if(syscall_value == -1)
{
  printf("context syscall failed\n");
}
  return 0;

}
