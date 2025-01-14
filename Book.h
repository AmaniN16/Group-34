#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include "Database.h"

using namespace std;

struct Book {
    string isbn;
    string author;
    string year;
    string name;
    bool availability;
    string rating;
    map<string, string> bookRatings;

    Book();
    Book(string _isbn);
    Book(string _isbn, string _name, string _author, string _year);

    void add_review(string username, string review);

    static void addBook(string _isbn, string _name, string _author, string _year);
    static void removeBook(string _isbn);
    static vector<Book> getBookCollection();
    static vector<Book> searchBooks(string query);
};

//Null book constructor
Book::Book() {
    author = "";
    name = "";
    isbn = "-1";
    availability = false;
    year = "";
    rating = "Unrated";
}

//Constructor that takes an isbn and populates the rest of the info from the database
Book::Book(string _isbn) {
    Json::Value data = readData();
    if (data["books"].isMember(_isbn)) {
        author = data["books"][_isbn]["author"].asString();
        isbn = _isbn;
        availability = data["books"][_isbn]["available"].asBool();
        name = data["books"][_isbn]["name"].asString();
        year = data["books"][_isbn]["year"].asString();
        rating = data["books"][_isbn]["rating"].asString();
        for (auto const& rating : data["books"][_isbn]["ratings"].getMemberNames()) {
            bookRatings[rating] = data["books"][_isbn]["ratings"][rating].asString();
        }
    }
    else {
        author = "";
        name = "";
        isbn = "-1";
        availability = false;
        year = "";
        rating = "Unrated";
    }
}

//Constructor that creates a new book and writes it to the database
Book::Book(string _isbn, string _name, string _author, string _year) {
    isbn = _isbn;
    author = _author;
    name = _name;
    availability = true;
    year = _year;
    rating = "Unrated";

    Json::Value data = readData();

    Json::Value book;
    book["author"] = _author;
    book["available"] = true;
    book["name"] = _name;
    book["year"] = _year;
    book["rating"] = "Unrated";
    Json::Value emptymap;
    emptymap["temp"] = "temp";
    emptymap.removeMember("temp");
    book["ratings"] = emptymap;
    data["books"][_isbn] = book;
    writeData(data);
}

//adds a review to a book, updating the object and database
void Book::add_review(string username, string review) {

    Json::Value data = readData();					//Store JSON data and make it accessible

    //Utilizing SMFL, Ask user if they want to leave a review...
    //Click on the star 1 - 5. Each star should have corresponding rating...
    //Grab the rating the User has selected and store that into the database
    //For now we use terminal as an example.

    //Add book rating to the database.... Store into JSON
    data["books"][isbn]["ratings"]["username"] = review;


    //UPDATE RATINGS of the current book------------------------------------------------------------ 
    //Access the current book ratings
    double total = 0;
    for (auto const& user : data["books"][isbn]["ratings"].getMemberNames()) {
        total += stod(data["books"][isbn]["ratings"][user].asString());
    }

    double calculation = total / data["books"][isbn]["ratings"].getMemberNames().size();
    rating = to_string(calculation);                                //Convert into string
    data["books"][isbn]["rating"] = rating;                         //Store into JSON      


    writeData(data);
    this->rating = rating;
    this->bookRatings[username] = review;
}

//adds a book to the database
void Book::addBook(string _isbn, string _name, string _author, string _year) {
    Book(_isbn, _name, _author, _year);
}

//removes a book from the database
void Book::removeBook(string _isbn) {
    Json::Value data = readData();
    data["books"].removeMember(_isbn);
    writeData(data);
}

//returns a vector of book objects representing all books in the database
vector<Book> Book::getBookCollection() {

    Json::Value data = readData();					
    vector<Book> books;

    //Store all books from the "JSON database" into a vector for easier access. 
    for (auto const& id : data["books"].getMemberNames()) {
        if (id != "-1")
            books.push_back(Book(id));
    }
    return books;
}

//returns a vector of book objects representing all books in the database that match the search parameter
vector<Book> Book::searchBooks(string query) {
    vector<Book> results;
    for (Book book : getBookCollection()) {
        if (book.name.find(query) != string::npos) {
            results.push_back(book);
        }
    }

    return results;
}

