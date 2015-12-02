/* A simple XML parser */
#include <stdio.h>
#include <strings.h>
#include <ctype.h>

enum Chartype
{
   EndOfFile,
   ASCII
};

enum Tokentype
{
   Start,
   End,
   Comment,
   Text,
   EndOfFileToken
};

struct character {
  char cargo;
  Chartype type;
};

struct token {
  char cargo[256];
  Tokentype type;
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
    if (t.type == EndOfFileToken) {
      printf("End of file\n");
      return 0;
    }  else {
      //printf("Token : %8s %s\n", t.type, t.cargo);
      switch (t.type)
      {
        case Start:
          printf("Start %s\n", t.cargo);
          break;
          
        case End:
          printf("End %s\n", t.cargo);
          break;
          
        case Comment:
          printf("Comment %s\n", t.cargo);
          break;
          
        case Text:
          printf("Text %s\n", t.cargo);
          break;
          
         default :
           printf("Unknown token %s!\n", t.cargo);
           
       }
    }
    }
  }
}

struct character scanner(FILE *source) 
{
  struct character c;
  if ((c.cargo = fgetc(source)) == EOF) {
    //strncpy(c.type, "EOF", 8);
    c.type = EndOfFile;
    //printf("scanner: EOF\n");
  } else { 
    //strncpy(c.type, "ASCII", 8);
    c.type = ASCII;
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
    if (c.type ==  EndOfFile) {
      //printf("Tokeniser: EOF\n");
      t.type = EndOfFileToken;
      return t;
    } else {
      //printf("Character : %c\n", c.cargo);

      while (c.cargo == ' ' || c.cargo == '\t' || c.cargo =='\n') {
        //printf("Whitespace : %c\n", c.cargo);
        c = scanner(source);    
      }
      //printf("Non w/s character: %c\n", c.cargo);

      if (c.cargo == '<') {
        // An xml node will have a name, and a possibly a list of attr="value", and a trailing /, 
        // but kml uses exlicit node ends and no attributes, so I will take some shortcuts.
        n = 0;
        c = scanner(source); // Check if this is a node start or a node end.
        if (c.cargo == '/') {
          t.type = End;      
        } else if (c.cargo == '!' || c.cargo == '?' ) {
          t.type = Comment;
        } else {
          t.type =Start;
          t.cargo[n] = c.cargo;
          n++;      
        }
        while (c.cargo != '>') {
          c = scanner(source);    /*Keep scanning characters until we hit the end of the token*/
          //printf("Character in token : %c\n", c.cargo);
          t.cargo[n] = c.cargo;
          n++;
        }
        t.cargo[n-1] = '\0';  // Overwrite the > character with the array terminator.
        //printf("End token\n");
        return t;
      } else if (isprint(c.cargo)){
        n = 0;
        t.type = Text;
        //printf("Text\n");
        //t.cargo[0] = c.cargo;
        while (c.cargo !='<') {
          //printf("Character in text : %c\n", c.cargo);
          t.cargo[n] = c.cargo;
          n++;
          c = scanner(source);    /*Keep scanning characters until we hit a non text */
        }
        ungetc(c.cargo, source);  // Push the next character back for processing next time
        t.cargo[n] = '\0';  // Overwrite the > character with the array terminator.        
        return t;
      } else {
        printf("Illegal token start character %c\n", c.cargo);
      }
    }
  }
}

    