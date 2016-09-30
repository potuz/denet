#include "pugihtml/pugihtml.hpp"
#include <iostream>

int main () {
  pugihtml::html_document doc;
  if (!doc.load_file("/home/heluani/Dropbox/Documents/code/denet/debug-build/hola.html")) return 1;


  pugihtml::html_node panels = doc.child("body").child("form");

  std::cout << "Nombre del child : " << panels.name() << "\n";

} 
