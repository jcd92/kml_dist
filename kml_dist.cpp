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
    //void readFile(FILE* xmlFile);
    node* addChild(char* name);
    void addText(char* text);
    node* upLevel();
    node();
    ~node();
    void dumpTree(int depth);
    node* getChild(char* name, int num = 0);
    int getNumChild(char* findName = 0);
    char* getText();
    char* getName();
    
  private:
    char *name;
    char *data;
    node *parent;
    std::vector<node*> child;
    int num_child;

};

double ConvertToRadians (double val){ 
    double PI = 3.14159265359; 
    return (val*PI) / 180; 
} 


struct character scanner(FILE *source);
struct token tokenizer(FILE *source);
void dumpTree (node *dumpNode, int depth);
void readFile(node *root, FILE* xmlFile);

int main (int argc, char *argv[])
{
  node *root, *currNode;
  FILE *input;
  const double R = 6371; // km 
  const double km_to_miles = 0.621371;
  double total_dist = 0;
  
  if ((input = fopen(argv[1], "r")) == NULL)
    printf("File could not be opened\n");
  else {
    root = new node;
    //root->readFile(input);
    readFile(root, input);
    //printf("End of file\n\n");
    // Now dump out the contents of the tree.
    //currNode = root;  // should be the case anyway.
    //printf("\nStructure of xml tree:\n");
    //dumpTree(root, 0);
    //currNode->dumpTree(0);
    //currNode = root->getChild("kml")->getChild("Document")->getChild("Placemark", 2);
    //currNode->dumpTree(0);
    //printf("Main: %s %d    %s\n", currNode->getName(), currNode->num_child, currNode->data);
    currNode = root->getChild("kml")->getChild("Document");
    int n = currNode->getNumChild("Placemark");
    printf("There are %d routes \n", n);
    for (int i=0; i<n; i++) 
      {
      printf("%-30s", currNode->getChild("Placemark", i)->getChild("name")->getText());
      //xNode=xMainNode.getChildNode("Document").getChildNode("Placemark",i).getChildNode("LineString").getChildNode("coordinates");
      //xNode=xMainNode.getChildNode("Placemark",i).getChildNode("gx:Track");
      int m=currNode->getChild("Placemark", i)->getChild("gx:Track")->getNumChild("gx:coord");
      //printf("There are '%d' points\n", m);
      char * coords = strdup(currNode->getChild("Placemark", i)->getChild("gx:Track")->getChild("gx:coord")->getText());
      char *tokenptr=strtok(coords, " ");
      double prev_X=atof(tokenptr);
      double prev_Y=atof(strtok(NULL, " "));
      double prev_Z=atof(strtok(NULL, " "))/1000;
      double d = 0.0;
      double delta_d = 0.0;
      double delta_h = 0.0;
       for (int j=1; j<m; j++) {
        //printf("gx:coord %s\n", xNode.getChildNode("gx:coord",j).getText());
        char * coords = strdup(currNode->getChild("Placemark", i)->getChild("gx:Track")->getChild("gx:coord",j)->getText());
        char *tokenptr=strtok(coords, " ");
        double X=atof(tokenptr);
        double Y=atof(strtok(NULL, " "));
        double Z=atof(strtok(NULL, " "))/1000;
        //printf ("X: %f Y: %f Z: %f\n", X, Y, Z);
        delta_d = acos(sin(ConvertToRadians(prev_Y))*sin(ConvertToRadians(Y)) + cos(ConvertToRadians(prev_Y))*cos(ConvertToRadians(Y))* cos(ConvertToRadians(X-prev_X))) * R; 
        delta_h = fabs(Z - prev_Z);
        d += sqrt(delta_d*delta_d + delta_h * delta_h);
        //d = acos(sin(ConvertToRadians(lat1))*sin(ConvertToRadians(lat2)) + cos(ConvertToRadians(lat1))*cos(ConvertToRadians(lat2))* cos(ConvertToRadians(lon2-lon1))) * R; 
        prev_X = X;
        prev_Y = Y;
        prev_Z = Z;
      }
      //total_dist += d;  
      total_dist += round(d * 10)/10;  
      //printf ("Distance %.1f    Total %.1f\n", d*km_to_miles, total_dist*km_to_miles);
      printf ("%.1f, Rounded : %f\n", d*km_to_miles, round(d * km_to_miles * 10)/10);
    }
  //free(t);
  printf ("Total distance %.1f\n", total_dist*km_to_miles);

    delete root;
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
        while (c.cargo != '>' && c.cargo != ' ') {
          c = scanner(source);    /*Keep scanning characters until we hit the end of the token*/
          //printf("Character in token : %c\n", c.cargo);
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
        //printf("End token\n");
        return t;
      } else if (isprint(c.cargo)){
        n = 0;
        t.type = Text;
        //printf("Text\n");
        //t.cargo[0] = c.cargo;
        while (c.cargo !='<') {
          //printf("Character in text : %c\n", c.cargo);
          //printf("Scan: read %d characters   %c", n, c.cargo);
          if (n < 254)
          {
            t.cargo[n] = c.cargo;
            n++;
            //printf("Stored!");
          }
          //printf("\n");

          c = scanner(source);    /*Keep scanning characters until we hit a non text */
        }
        //printf("token: %s\n", t.cargo);
        ungetc(c.cargo, source);  // Push the next character back for processing next time
        //printf("token: n= %d", n);
        t.cargo[n] = '\0';  // Overwrite the > character with the array terminator.        
        //printf("token2: %s\n", t.cargo);
        return t;
      } else {
        printf("Illegal token start character %c\n", c.cargo);
      }
    }
  }
}

node* node::addChild(char* name)
{
  node *newNode;
  char *n;

  //printf("Node: this node's address is %x\n", this);
  //printf("Node: length of name: %d\n", strlen(name));

  n = new char [strlen(name)+1];
  strcpy(n, name);
  newNode = new node;  // newNode points to the new node
  //children = new std::vector<node*>;
  newNode->parent = this;
  newNode->name = n;
  newNode->data = 0;
  newNode->num_child = 0;
  //printf("Node: %s %d    %s\n", newNode->name, newNode->num_child, newNode->data);
  
  num_child++;
  child.push_back(newNode);
  //printf("Node: child's address is %x : Parent is at %x\n", newNode, newNode->parent);
  
  return newNode;
}

node::node()
{
  parent = 0;
  name = 0;
  data = 0;
  num_child = 0;
}

node::~node()
{
  //printf("Node: Destroying %s\n", name);
  for (std::vector<node*>::iterator j = child.begin(); j != child.end(); ++j)
    delete (*j);
}
  
void node::addText(char* text)
{
  char *t;
  
  //printf("Text %s\n", text);
  t = new char [strlen(text)+1];
  strcpy(t, text);
  data = t;
  //printf("Node: added text %s\n", data);
}

node* node::upLevel()
{
  return parent;
}
        
node* node::getChild(char* name, int num)
{
  //printf("getChild: called with %s %d\n", name, num_child);
  int c = 0;
  
  for (std::vector<node*>::iterator j = child.begin(); j != child.end(); ++j)
  {
    //printf("getChild: found %s \n", (*j)->name);
    
    if (strcmp((*j)->name, name) == 0)
    {
      //printf("getChild: Matched %s %d times\n", (*j)->name, c); 
      if (c == num)
      {
        //printf("getChild: found %s %d times\n", (*j)->name, c);
        return *j;
      }
      c++;
    }  
  }
}

int node::getNumChild(char* findName)
{
  if(findName == 0) 
  {
    return num_child;
  } else {
    int l = 0;
    for (std::vector<node*>::iterator f = child.begin(); f != child.end(); ++f)
    {
      //printf("getNumChild: found %s with loop counter %d\n", (*f)->name, l);
      
      if (strcmp((*f)->name, findName) == 0)
      {
        l++;
        //printf("getNumChild: found %s %d times\n", (*f)->name, l);
      }
    }
    //printf("getNumChild: Result - found %s %d times.  Returning\n", findName, l);
    return l;
  }   
}

void node::dumpTree(int depth)
{
  int i;

  for (i = 0; i < depth; i++)
    printf("  ");
  //printf("%s %d\n", name, num_child);
  printf("%s %d %s\n", name, num_child, data);
  for (std::vector<node*>::iterator j = child.begin(); j != child.end(); ++j)
    (*j)->dumpTree(depth + 1);
}

char* node::getText()
{
  return data;
}

char* node::getName()
{
  return name;
}

void readFile(node *root, FILE* xmlFile)
{
  struct token t;
  node *currNode;
  //printf("Root is at %x\n", root);
  
  currNode = root;
  t = tokenizer(xmlFile);

  while (t.type != EndOfFileToken) 
  {
    //printf("Token : %8s %s\n", t.type, t.cargo);
    switch (t.type)
    {
      case Start:
        //printf("Start %s\n", t.cargo);
        //printf("Length of name: %d\n", strlen(t.cargo));
        
        currNode = currNode->addChild(t.cargo);;
        //printf("Main: new node %s is at %x\n", currNode->getName(), currNode);
        break;
        
      case End:
        //printf("End %s\n", t.cargo);
        //printf("Main: End %s %d    %s\n", currNode->getName(), currNode->getNumChild, currNode->data);
        currNode = currNode->upLevel();
        //printf("Main: parent %s %d    %s\n", currNode->getName(), currNode->getNumChild(), currNode->getText());
        break;
        
      case Comment:
        //printf("Comment %s\n", t.cargo);
        break;
        
      case Text:
        currNode->addText(t.cargo);
        //printf("Main: Text added to %s %d    %s\n", currNode->getName(), currNode->num_child, currNode->getText());
        //printf("Main: Text added to %s %d    %s\n", currNode->getName(), currNode->getNumChild(), currNode->getText());
        break;
        
      default :
        printf("Unknown token %s!\n", t.cargo);
         
    }
    t = tokenizer(xmlFile);
  }
}
