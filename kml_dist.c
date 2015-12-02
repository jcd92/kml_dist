/* A simple XML parser */
#include <stdio.h>
#include <strings.h>

struct character {
  char cargo;
  char type[8];
};

struct token {
  char cargo[256];
  char type[8];
};

struct character scanner(FILE *source);
struct token tokenizer(FILE *source);

main (int argc, char *argv[])
{
  FILE *input;
  struct token t;
  if ((input = fopen(argv[1], "r")) == NULL)
    printf("File could not be opened\n");
  else {
    while (1) {
    t = tokenizer(input);
    if (strcmp(t.type, "EOF") == 0) {
      printf("End of file\n");
      return 0;
    }  else {
      printf("Token : %s\n", t.cargo);
    }
    }
  }
}

struct character scanner(FILE *source) 
{
  struct character c;
  if ((c.cargo = fgetc(source)) == EOF) {
    strncpy(c.type, "EOF", 8);
    //printf("scanner: EOF\n");
  } else { 
    strncpy(c.type, "ASCII", 8);
//    *c.type = "ASCII";
  }
  return c;
}

struct token tokenizer(FILE *source)
{
  struct token t;
  struct character c;
  int n;
  
  while (1) {
    c = scanner(source);
    //printf("TOK: c.type %s\n", c.type);
    if (strcmp(c.type, "EOF") == 0) {
      //printf("Tokeniser: EOF\n");
      strncpy(t.type, "EOF", 8);
      return t;
    } else {
      //printf("Character : %c\n", c.cargo);
      while (c.cargo == ' ' || c.cargo == '\t' || c.cargo =='\n') {
        //printf("Whitespace : %c\n", c.cargo);
        c = scanner(source);    /* Ignore whitespaces and tabs */
      }
      //printf("Non w/s character: %c\n", c.cargo);
      if (c.cargo == '<') {
        n = 0;
        while (c.cargo != '>') {
          c = scanner(source);    /*Keep scanning characters until we hit the end of the token*/
          //printf("Character in token : %c\n", c.cargo);
          t.cargo[n] = c.cargo;
          n++;
        }
        t.cargo[n-1] = '\0';  // Overwrite the > character with the array terminator.
        //printf("End token\n");
        return t;
      } else {
        printf("Shouldn't be here!\n");
        return t;
      }
    }
  }
}

    