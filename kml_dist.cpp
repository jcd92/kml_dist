/* A simple XML parser */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <vector>
#include <math.h>
#include <stdlib.h>

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

struct character 
{
  char cargo;
  Chartype type;
};

struct token 
{
  char cargo[256];
  Tokentype type;
};

class Node
{
  public:
    Node* addChild(char* name);
    void addText(char* text);
    Node* upLevel();
    Node();
    ~Node();
    void dumpTree(int depth);
    Node* getChild(char* name, int num = 1);
    int getNumChild(char* findName = 0);
    char* getText();
    char* getName();
    
  private:
    char *name;
    char *data;
    Node *parent;
    std::vector<Node*> child;
    int num_child;

};

double ConvertToRadians (double val)
{ 
    double PI = 3.14159265359; 
    return (val*PI) / 180; 
} 


struct character scanner(FILE *source);
struct token tokenizer(FILE *source);
void dumpTree (Node *dumpNode, int depth);
void readFile(Node *root, FILE* xmlFile);

int main (int argc, char *argv[])
{
  Node *root, *currNode;
  FILE *input;
  const double R = 6371; // km 
  const double km_to_miles = 0.621371;
  double total_dist = 0;
  
  if ((input = fopen(argv[1], "r")) == NULL)
    printf("File could not be opened\n");
  else 
  {
    root = new Node;
    readFile(root, input);
    currNode = root->getChild("kml")->getChild("Document");
    int n = currNode->getNumChild("Placemark");
    printf("There are %d routes \n", n);
    for (int i=1; i<=n; i++) 
    {
      printf("%-65s", currNode->getChild("Placemark", i)->getChild("name")->getText());
      int m=currNode->getChild("Placemark", i)->getChild("gx:Track")->getNumChild("gx:coord");
      char * coords = strdup(currNode->getChild("Placemark", i)->getChild("gx:Track")->getChild("gx:coord")->getText());
      char *tokenptr=strtok(coords, " ");
      double prev_X=atof(tokenptr);
      double prev_Y=atof(strtok(NULL, " "));
      double prev_Z=atof(strtok(NULL, " "))/1000;
      double d = 0.0;
      double delta_d = 0.0;
      double delta_h = 0.0;
      //printf("Main: first coords %s\n", coords);
      for (int j=2; j<=m; j++) 
      {
        char * coords = strdup(currNode->getChild("Placemark", i)->getChild("gx:Track")->getChild("gx:coord",j)->getText());
        //printf("Main: coords %s\n", coords);
        char *tokenptr=strtok(coords, " ");
        double X=atof(tokenptr);
        double Y=atof(strtok(NULL, " "));
        double Z=atof(strtok(NULL, " "))/1000;
        delta_d = acos(sin(ConvertToRadians(prev_Y))*sin(ConvertToRadians(Y)) + cos(ConvertToRadians(prev_Y))*cos(ConvertToRadians(Y))* cos(ConvertToRadians(X-prev_X))) * R; 
        delta_h = fabs(Z - prev_Z);
        d += sqrt(delta_d*delta_d + delta_h * delta_h);
        //printf("Dist: %f X: %f Y: %f Z: %f Delta d: %f Delta h: %h\n", d, X, Y, Z, delta_d, delta_h);
        prev_X = X;
        prev_Y = Y;
        prev_Z = Z;
      }
      total_dist += d;  
      printf ("  %.1fm\n", d*km_to_miles);
    }

    printf ("\n\nTotal distance: %.1f\n", total_dist*km_to_miles);

    delete root;
  } 
}

struct character scanner(FILE *source) 
{
  struct character c;
  if ((c.cargo = fgetc(source)) == EOF) 
    c.type = EndOfFile;
  else 
    c.type = ASCII;
 
  return c;
}

struct token tokenizer(FILE *source)
{
  struct token t;
  struct character c;
  int n;
  
  while (1) 
  {
    c = scanner(source);
    if (c.type ==  EndOfFile) 
    {
      t.type = EndOfFileToken;
      return t;
    } else 
    {
      while (c.cargo == ' ' || c.cargo == '\t' || c.cargo =='\n') 
      {
        c = scanner(source);    
      }

      if (c.cargo == '<') 
      {
        n = 0;
        c = scanner(source); // Check if this is a node start or a node end.
        if (c.cargo == '/') 
        {
          t.type = End;      
        } else if (c.cargo == '!' || c.cargo == '?' ) 
        {
          t.type = Comment;
        } else 
        {
          t.type =Start;
          t.cargo[n] = c.cargo;
          n++;      
        }
        while (c.cargo != '>' && c.cargo != ' ') 
        {
          c = scanner(source);    /*Keep scanning characters until we hit the end of the token*/
          t.cargo[n] = c.cargo;
          n++;
        }
        if (c.cargo == ' ') 
        {  // Found some more text, likely parameters
          while (c.cargo != '>')
          {
            c = scanner(source);    /*Keep scanning characters until we hit the end of the token*/
          }
        }      
        t.cargo[n-1] = '\0';  // Overwrite the > character with the array terminator.
        return t;
      } else if (isprint(c.cargo))
      {
        n = 0;
        t.type = Text;
        while (c.cargo !='<') {
          if (n < 254)
          {
            t.cargo[n] = c.cargo;
            n++;
          }

          c = scanner(source);    /*Keep scanning characters until we hit a non text */
        }
        ungetc(c.cargo, source);  // Push the next character back for processing next time
        t.cargo[n] = '\0';  // Overwrite the > character with the array terminator.        
        return t;
      } else 
        printf("Illegal token start character %c\n", c.cargo);
    }
  }
}

void readFile(Node *root, FILE* xmlFile)
{
  struct token t;
  Node *currNode;
  
  currNode = root;
  t = tokenizer(xmlFile);

  while (t.type != EndOfFileToken) 
  {
    switch (t.type)
    {
      case Start:
        currNode = currNode->addChild(t.cargo);;
        break;
        
      case End:
        currNode = currNode->upLevel();
        break;
        
      case Comment:
        break;
        
      case Text:
        currNode->addText(t.cargo);
        break;
        
      default :
        printf("Unknown token %s!\n", t.cargo);
         
    }
    t = tokenizer(xmlFile);
  }
}

Node* Node::addChild(char* name)
{
  Node *newNode;
  char *n;

  n = new char [strlen(name)+1];
  strcpy(n, name);
  newNode = new Node;  // newNode points to the new node
  newNode->parent = this;
  newNode->name = n;
  newNode->data = 0;
  newNode->num_child = 0;
  num_child++;
  child.push_back(newNode);
  
  return newNode;
}

Node::Node()
{
  parent = 0;
  name = 0;
  data = 0;
  num_child = 0;
}

Node::~Node()
{
  for (std::vector<Node*>::iterator j = child.begin(); j != child.end(); ++j)
    delete (*j);
}
  
void Node::addText(char* text)
{
  char *t;
  
  t = new char [strlen(text)+1];
  strcpy(t, text);
  data = t;
}

Node* Node::upLevel()
{
  return parent;
}
        
Node* Node::getChild(char* name, int num)
{
  int c = 1;
  
  for (std::vector<Node*>::iterator j = child.begin(); j != child.end(); ++j)
  {
    if (strcmp((*j)->name, name) == 0)
    {
      if (c == num)
        return *j;
      c++;
    }  
  }
}

int Node::getNumChild(char* findName)
{
  if(findName == 0) 
  {
    return num_child;
  } else {
    int l = 0;
    for (std::vector<Node*>::iterator f = child.begin(); f != child.end(); ++f)
    {
      if (strcmp((*f)->name, findName) == 0)
        l++;
    }
    return l;
  }   
}

void Node::dumpTree(int depth)
{
  int i;

  for (i = 0; i < depth; i++)
    printf("  ");
  printf("%s %d %s\n", name, num_child, data);
  for (std::vector<Node*>::iterator j = child.begin(); j != child.end(); ++j)
    (*j)->dumpTree(depth + 1);
}

char* Node::getText()
{
  return data;
}

char* Node::getName()
{
  return name;
}

