#include "clothing.h"
#include "util.h"
#include <string>

using namespace std;

Clothing::Clothing(const std::string category, const std::string name, double price, int qty, std::string size, std::string brand):Product(category, name, price, qty){
	brand_ = brand;
	size_ = size;
}

Clothing::~Clothing(){

}
// <name>
// Size: <size> Brand: <brand>
// <price> <quantity> left.
std::string Clothing::displayString() const{
	std::string returnThis = name_ + '\n' + "Size: "  + size_ +  " Brand: "  + brand_ + '\n' + to_string(price_) + ' ' + to_string(qty_) + " left." + "\n";
	return returnThis;
}

void Clothing::dump(std::ostream& os) const{
	os << category_ << "\n" << name_ << "\n" << price_ << "\n" << qty_ << "\n" << size_ << "\n" << brand_ << endl;
}

// the words in the brand should be searchable keywords
std::set<std::string> Clothing::keywords() const{
	std::set<std::string> first = parseStringToWords(brand_);
	std::set<std::string> second = parseStringToWords(name_);
	std::set<std::string> returnThis = setUnion(first, second);

	std::set<std::string> returnThis9;
	std::set<std::string>::iterator itA;
	for(itA = returnThis.begin(); itA != returnThis.end(); ++itA){
		returnThis9.insert(convToLower(*itA));
	}
	return returnThis9;
}