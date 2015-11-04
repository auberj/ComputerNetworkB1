// classes example

#include <iostream>
using namespace std;

class Rectangle {
    int width, height;
  public:
    void set_values (int,int);
    int area() {return width*height;}
};

void Rectangle::set_values (int x, int y) {
  width = x;
  height = y;
}

int main () {
  Rectangle rect;
  rect.set_values (3,4);
  cout << "area: " << rect.area();
  return 0;
}

//our classes


//network layer
 class Packet {
	 	char control, checksum; //2 bytes long
	 	char16_t address, destination, length; //1 byte long
	 	char[121] segment; //segment passed from the transport layer
	 public:
 		int create_packet(char, char16_t, char16_t, char16_t, char[121], char);
 };

 int Packet::create_packet(char a, char16_t b, char16_t c, char16_t d, char[121] e, char f){
 	control = a;
 	address = b;
 	destination = c;
 	length = d;
 	segment = e;
 	checksum = f;
 }