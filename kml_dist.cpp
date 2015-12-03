/* A simple XML parser */
#include <stdio.h>
#include <strings.h>
#include <ctype.h>
#include <vector>

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

class node
{
  public:
  char *name;
  char *data;
  int num_child;
  node *parent;
  std::vector<node*> child;
  node* addChild(char* name);
  void addText(char* text);
  node* upLevel();
  node();
  void dumpTree(int depth);
  node* getChild(char* name, int num);
};

node* node::addChild(char* name)
{
  node *newNode;
  char *n;

  printf("Node: this node's address is %x\n", this);
  //printf("Node: length of name: %d\n", strlen(name));

  n = new char [strlen(name)+1];
  strcpy(n, name);
  newNode = new node;  // newNode points to the new node
  //children = new std::vector<node*>;
  newNode->parent = this;
  newNode->name = n;
  newNode->data = 0;
  newNode->num_child = 0;
  printf("Node: %s %d    %s\n", newNode->name, newNode->num_child, newNode->data);
  
  num_child++;
  child.push_back(newNode);
  printf("Node: child's address is %x : Parent is at %x\n", newNode, newNode->parent);
  
  return newNode;
}

node::node()
{
  parent = 0;
  name = 0;
  data = 0;
  num_child = 0;
}
  
void node::addText(char* text)
{
  char *t;
  
  printf("Text %s\n", text);
  t = new char [strlen(text)+1];
  strcpy(t, text);
  data = t;
}

node* node::upLevel()
{
  return parent;
}
        
node* node::getChild(char* name, int num)
{
  printf("getChild: called with %s %d\n", name, num_child);
  
  for (std::vector<node*>::iterator j = child.begin(); j != child.end(); ++j)
  {
    printf("getChild: found %s\n", (*j)->name);
    
    if (strcmp((*j)->name, name) == 0)
    {
      printf("getChild: Matched\n");
      return *j;
    }
      
  }
}


struct character scanner(FILE *source);
struct token tokenizer(FILE *source);
void dumpTree (node *dumpNode, int depth);

int main (int argc, char *argv[])
{
  FILE *input;
  struct token t;
  node *root;
  node *currNode;
  node *newNode;
  root = new node;
  printf("Root is at %x\n", root);
  root->name = 0;
  root->num_child = 0;
  char *n;
  
  if ((input = fopen(argv[1], "r")) == NULL)
    printf("File could not be opened\n");
  else {
    currNode = root;
    t = tokenizer(input);

    while (t.type != EndOfFileToken) {
      //printf("Token : %8s %s\n", t.type, t.cargo);
      switch (t.type)
      {
        case Start:
          printf("Start %s\n", t.cargo);
          //printf("Length of name: %d\n", strlen(t.cargo));
          
          currNode = currNode->addChild(t.cargo);;
          printf("Main: new node %s is at %x\n", currNode->name, currNode);
          break;
          
        case End:
          printf("End %s\n", t.cargo);
          printf("Main: End %s %d    %s\n", currNode->name, currNode->num_child, currNode->data);
          currNode = currNode->upLevel();
          printf("Main: parent %s %d    %s\n", currNode->name, currNode->num_child, currNode->data);
          break;
          
        case Comment:
          printf("Comment %s\n", t.cargo);
          break;
          
        case Text:
          currNode->addText(t.cargo);
          printf("Main: Text added to %s %d    %s\n", currNode->name, currNode->num_child, currNode->data);

          break;
          
         default :
           printf("Unknown token %s!\n", t.cargo);
           
      }
      t = tokenizer(input);
    }

  printf("End of file\n\n");
  // Now dump out the contents of the tree.
  //currNode = root;  // should be the case anyway.
  printf("\n\nStructure of xml tree:\n");
  //dumpTree(root, 0);
  currNode->dumpTree(0);
  currNode = root->getChild("kml", 1)->getChild("Document", 1);
  printf("Main: %s %d    %s\n", currNode->name, currNode->num_child, currNode->data);

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


void node::dumpTree(int depth)
{
  int i;

  for (i = 0; i < depth; i++)
    printf("  ");
  printf("%s %d\n", name, num_child);
  for (std::vector<node*>::iterator j = child.begin(); j != child.end(); ++j)
    (*j)->dumpTree(depth + 1);
}