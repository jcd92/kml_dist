/* A simple XML parser */
#include <stdio.h>

struct character {
  char cargo;
  char *type;
};

struct token {
  char *cargo;
  char *type;
};

struct character scanner(FILE *source);
struct token tokenizer(FILE *source);

main ()
{
  FILE *input;
  struct token t;
  if ((input = fopen("test_old.kml", "r")) == NULL)
    printf("File could not be opened\n");
  else {
    while (1) {
    t = tokenizer(input);
    if (t.type == "EOF" ) {
      printf("End of file\n");
      return 0;
    }  else {
//      printf("Character : %c\n", t);
    }
    }
  }
}

struct character scanner(FILE *source) 
{
  struct character c;
  if ((c.cargo = fgetc(source)) == EOF) {
    c.type = "EOF";
  } else { 
    c.type = "ASCII";
  }
  return c;
}

struct token tokenizer(FILE *source)
{
  struct token t;
  struct character c;
  
  while (1) {
    c = scanner(source);
    if (c.type == "EOF" ) {
      t.type = "EOF";
      return t;
    } else {
      printf("Character : %c\n", c.cargo);
    }
  }
}

    