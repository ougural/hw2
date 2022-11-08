#include <iostream>
#include <fstream>
#include <set>
#include <sstream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include "product.h"
#include "db_parser.h"
#include "product_parser.h"
#include "util.h"
#include "mydatastore.h"
#include "mydatastore.cpp"
#include "datastore.h"

using namespace std;
struct ProdNameSorter {
    bool operator()(Product* p1, Product* p2) {
        return (p1->getName() < p2->getName());
    }
};
void displayProducts(vector<Product*>& hits);

int main(int argc, char* argv[])
{
    if(argc < 2) {
        cerr << "Please specify a database file" << endl;
        return 1;
    }

    /****************
     * Declare your derived DataStore object here replacing
     *  DataStore type to your derived type
     ****************/
    MyDataStore ds;



    // Instantiate the individual section and product parsers we want
    ProductSectionParser* productSectionParser = new ProductSectionParser;
    productSectionParser->addProductParser(new ProductBookParser);
    productSectionParser->addProductParser(new ProductClothingParser);
    productSectionParser->addProductParser(new ProductMovieParser);
    UserSectionParser* userSectionParser = new UserSectionParser;

    // Instantiate the parser
    DBParser parser;
    parser.addSectionParser("products", productSectionParser);
    parser.addSectionParser("users", userSectionParser);

    // Now parse the database to populate the DataStore
    if( parser.parse(argv[1], ds) ) {
        cerr << "Error parsing!" << endl;
        return 1;
    }

    cout << "=====================================" << endl;
    cout << "Menu: " << endl;
    cout << "  AND term term ...                  " << endl;
    cout << "  OR term term ...                   " << endl;
    cout << "  ADD username search_hit_number     " << endl;
    cout << "  VIEWCART username                  " << endl;
    cout << "  BUYCART username                   " << endl;
    cout << "  QUIT new_db_filename               " << endl;
    cout << "====================================" << endl;

    vector<Product*> hits;
    bool done = false;
    while(!done) {
        cout << "\nEnter command: " << endl;
        string line;
        getline(cin,line);
        stringstream ss(line);
        string cmd;
        if((ss >> cmd)) {
            if( cmd == "AND") {
                string term;
                vector<string> terms;
                while(ss >> term) {
                    term = convToLower(term);
                    terms.push_back(term);
                }
                hits = ds.search(terms, 0);
                displayProducts(hits);
            }
            else if ( cmd == "OR" ) {
                string term;
                vector<string> terms;
                while(ss >> term) {
                    term = convToLower(term);
                    terms.push_back(term);
                }
                hits = ds.search(terms, 1);
                displayProducts(hits);
            }
            else if ( cmd == "QUIT") {
                string filename;
                if(ss >> filename) {
                    ofstream ofile(filename.c_str());
                    ds.dump(ofile);
                    ofile.close();
                }
                done = true;
            }
            else if(cmd == "ADD"){
                string username;
                int hitNum; 
                ss >> username;
                ss >> hitNum;
                bool userCartExists = false;
                bool userExists = false;
                set<User*>::iterator itK;
                User* temp;
                for(itK = ds.users_.begin(); itK != ds.users_.end(); ++itK){
                    if((*itK)->getName() == username){
                        userExists = true;
                        temp = *itK; 
                        break;
                    }
                }
                if(!userExists){
                    cout << "Invalid request" << endl;
                }
                else{
                    int index;
                    for(unsigned int i = 0; i < ds.cartUsers.size(); i++){
                        if(ds.cartUsers[i]->getName() == username){
                            userCartExists = true;
                            index = i;
                            break;
                        }
                    }
                    if(!userCartExists){
                        std::vector<Product*> tempVec;
                        ds.cartUsers.push_back(temp);
                        ds.cartProducts.push_back(tempVec);
                        ds.cartProducts[ds.cartProducts.size() - 1].push_back(hits[hitNum - 1]);
                    }
                    else{
                        ds.cartProducts[index].push_back(hits[hitNum - 1]);
                    }
                }
            }
            else if(cmd == "VIEWCART"){
                string username;
                ss >> username;
                int index = -1;
                for(unsigned int i = 0; i < ds.cartUsers.size(); i++){
                    if(ds.cartUsers[i]->getName() == username){
                        index = i;
                        break;
                    }
                }
                if(index == -1){
                    cout << "Invalid username" << endl;
                }
                else{
                    for(unsigned int i = 0; i < ds.cartProducts[index].size(); i++){
                        cout << "item " << i + 1 << endl;
                        cout << ds.cartProducts[index][i]->displayString() << endl;
                    }
                }
            }
            else if ( cmd == "BUYCART"){
                string username;
                ss >> username;
                int index;
                bool userCartExists = false;
                for(unsigned int i = 0; i < ds.cartUsers.size(); i++){
                    if(ds.cartUsers[i]->getName() == username){
                        userCartExists = true;
                        index = i;
                        break;
                    }
                }
                // Buying the cart - You should support the BUYCART username command which 
                // should cause the program to iterate through the items in username's cart 
                // (case insensitive). If the item is in stock AND the user has enough money 
                // it should be removed from the cart, the in stock quantity reduced by 1, and 
                // the product price should be debited from the user’s available credit. If an item 
                // is not in stock or the user does not have enough credit, simply leave it in the 
                // cart and go on to the next product. Note: Your cart implementation must iterate 
                // through the products in the order they were added. If the username provided is 
                // invalid, print Invalid username to the screen and do not process the command.

                if(userCartExists){
                    int counter = 0;
                    for(int i = 0; i < ds.cartProducts[index].size(); i++){
                        double price = ds.cartProducts[index][i]->getPrice();
                        if(ds.cartUsers[index]->getBalance() > price &&  ds.cartProducts[index][i]->getQty() > 0){
                            counter++;
                            ds.cartProducts[index][i]->subtractQty(1);
                            ds.cartUsers[index]->deductAmount(price);
                        }
                    }
                    for(int i = 0; i < ds.cartProducts[index].size()/2; i++){
                        Product* temp = ds.cartProducts[index][ds.cartProducts[index].size() - 1 - i];
                        ds.cartProducts[index][ds.cartProducts[index].size() - 1 - i] = ds.cartProducts[index][i];
                        ds.cartProducts[index][i] = temp;
                    }
                    for (int i = 0; i < counter; i++){
                        ds.cartProducts[index].pop_back();
                    }
                    for(int i = 0; i < ds.cartProducts[index].size()/2; i++){
                        Product* temp = ds.cartProducts[index][ds.cartProducts[index].size() - 1 - i];
                        ds.cartProducts[index][ds.cartProducts[index].size() - 1 - i] = ds.cartProducts[index][i];
                        ds.cartProducts[index][i] = temp;
                    }
                }
                else{
                    cout << "Invalid username" << endl;
                }
            }
            else {
                cout << "Unknown command" << endl;
            }
        }
    }
    return 0;
}

void displayProducts(vector<Product*>& hits)
{
    int resultNo = 1;
    if (hits.begin() == hits.end()) {
    	cout << "No results found!" << endl;
    	return;
    }
    std::sort(hits.begin(), hits.end(), ProdNameSorter());
    for(vector<Product*>::iterator it = hits.begin(); it != hits.end(); ++it) {
        cout << "Hit " << setw(3) << resultNo << endl;
        cout << (*it)->displayString() << endl;
        cout << endl;
        resultNo++;
    }
}
