/* A simple XML parser */
#include <stdio.h>

char scanner(FILE *source);

main ()
{
  FILE *input;
  char t;
  if ((input = fopen("test_old.kml", "r")) == NULL)
    printf("File could not be opened\n");
  else {
    if ((t = scanner(input)) == "\0" ) {
      printf("End of file\n");
      return 0;
    }  else {
      printf("Character : %c\n", t);
    }
  }
}

char scanner(FILE *source) 
{
  char c;
  
  return fgetc(source);
}

    