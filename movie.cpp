#include "movie.h"
#include "util.h"
#include <string>
#include <cmath>

using namespace std;

Movie::Movie(const std::string category, const std::string name, double price, int qty, std::string genre, std::string rating):Product(category, name, price, qty){
	genre_ = genre;
	rating_ = rating;
}

Movie::~Movie(){

}

// <name>
// Genre: <genre> Rating: <rating>
// <price> <quantity> left.
std::string Movie::displayString() const{
	std::string returnThis = name_ + '\n' + "Genre: "  + genre_ +  " Rating: "  + rating_ + '\n' + to_string(price_) + ' ' + to_string(qty_) + " left." + '\n';
	return returnThis;
}

void Movie::dump(std::ostream& os) const{
  os << category_ << "\n" << name_ << "\n" << price_ << "\n" << qty_ << '\n' << genre_ << '\n' << rating_ << endl;
}

// the movie’s genre should be a searchable keyword
std::set<std::string> Movie::keywords() const{
	std::set<std::string> returnThis = parseStringToWords(name_);
	returnThis.insert(genre_);

	std::set<std::string> returnThis9;
	std::set<std::string>::iterator itA;
	for(itA = returnThis.begin(); itA != returnThis.end(); ++itA){
		returnThis9.insert(convToLower(*itA));
	}
	return returnThis9;
}