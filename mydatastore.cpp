#include "mydatastore.h"
#include "util.h"
#include <string>
#include <vector>
#include <set>
#include "book.h"
#include "movie.h"
#include "clothing.h"
#include "product.h"
#include "user.h"
#include "datastore.h"

MyDataStore::MyDataStore(){

}

MyDataStore::~MyDataStore(){
	std::set<Product*>::iterator itP;
	for(itP = products_.begin(); itP != products_.end(); itP++){
		delete *itP;
	}

	std::set<User*>::iterator itU;
	for(itU = users_.begin(); itU != users_.end(); itU++){
		delete *itU;
	}
}

/**
	* Adds a product to the data store
	*/
void MyDataStore::addProduct(Product* p){
	products_.insert(p);
}

/**
	* Adds a user to the data store
	*/
void MyDataStore::addUser(User* u){
	users_.insert(u);
}

/**
	* Performs a search of products whose keywords match the given "terms"
	*  type 0 = AND search (intersection of results for each term) while
	*  type 1 = OR search (union of results for each term)
	*/
std::vector<Product*> MyDataStore::search(std::vector<std::string>& terms, int type){
	std::vector<Product*> returnThis;

	std::set<std::string> termsSet;
	for(unsigned int i = 0; i < terms.size(); i++){
		termsSet.insert(terms[i]);
	}
	if(type == 0){
		std::set<Product*>::iterator itP;
		// std::cout << terms[0] << std::endl;
		for(itP = products_.begin(); itP != products_.end(); ++itP){
			std::set<std::string> keyw = (*itP)->keywords();
			std::set<std::string> inter = setIntersection(termsSet, keyw);
			if(inter == termsSet){
				returnThis.push_back(*itP);
			}
		}
	}
	else{
		std::set<Product*>::iterator it;
		for(it = products_.begin(); it != products_.end(); ++it){
			std::set<std::string> keyw = (*it)->keywords();
			std::set<std::string> inter = setIntersection(termsSet, keyw);
			if(inter.size() > 0){
				returnThis.push_back(*it);
			}
		}
	}
	return returnThis;
}

/**
	* Reproduce the database file from the current Products and User values
	*/
void MyDataStore::dump(std::ostream& ofile){
	ofile << "<products>" << std::endl;
	std::set<Product*>::iterator itP;
	for(itP = products_.begin(); itP != products_.end(); ++itP){
    (*itP)->dump(ofile);
	}
	ofile << "</products>" << std::endl;
	ofile << "<users>" << std::endl;
	std::set<User*>::iterator itU;
	for (itU = users_.begin(); itU != users_.end(); ++itU){
    (*itU)->dump(ofile);
	}
	ofile << "</users>" << std::endl;
}
