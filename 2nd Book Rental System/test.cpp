//2nd Books Rental System
//Benjamin Lim Kok Pin 1211209221
//Kerk Ming Da 1211211733
//Tan Yan San 1211108273

#include<iostream>
#include<iomanip>
#include<fstream>
#include<string>
#include <unistd.h>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <vector>
#include <sys/types.h>
#include <dirent.h>
#include <cctype>

using namespace std;

void changefee(int op);
void checkstatus(int num);
int status(string datedue);
void userMenu();
void adminMenu();
void catalog();
int main();
void saveextrastock(int additional,string tempname);
string getGenre(const string& id);
bool isValidID(const string& id);

struct FeeManagement{
	float fee;
	float fine;
	float sum_penalty;
	float money_earn_rent;
}FM;

class Users {
public:
    string username;
    string password;
    Users* next;

    Users(string uname, string pwd) {
        username = uname;
        password = pwd;
        next = NULL;
    }
};

class UsersLinkedList {
private:
    Users* head;

public:
    UsersLinkedList() {
        head = NULL;
    }

    void insert(string uname, string pwd) {
        Users* newUsers = new Users(uname, pwd);
        if (head == NULL) {
            head = newUsers;
        } else {
            Users* current = head;
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = newUsers;
        }
    }

    bool search(string uname, string pwd) {
        Users* current = head;
        while (current != NULL) {
            if (current->username == uname && current->password == pwd) {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    bool checkUsername(string uname) {
        Users* current = head;
        while (current != NULL) {
            if (current->username == uname) {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    void loadUserFromFile(string filename) {
        ifstream file(filename.c_str());
        if (!file.is_open()) {
            cout << "Error: Unable to open the file '" << filename << "'" << endl;
            return;
        }

        string uname, pwd;
        while (file >> uname >> pwd) {
            insert(uname, pwd);
        }
        file.close();
    }

    void writeUserToFile(string filename) {
        ofstream file(filename.c_str());
        if (!file.is_open()) {
            cout << "Error: Unable to open the file '" << filename << "'" << endl;
            return;
        }

        Users* current = head;
        while (current != NULL) {
            file << current->username << " " << current->password << "\n";
            current = current->next;
        }
        file.close();
    }

    ~UsersLinkedList() {
        Users* current = head;
        while (current != NULL) {
            Users* nextUsers = current->next;
            delete current;
            current = nextUsers;
        }
    }
};


class Info {
private:
    string specialCharacters;

protected:
    string username, password;

public:
    int retries;

    Info() : specialCharacters("~!@#$%^&*_-+=`|(){}[]:;\"'<>,.?/") {}

    friend void checkstatus(int num);
    
    void setUsername(string uname) {
        username = uname;
    }

    void setPassword(string pwd) {
        password = pwd;
    }

    void registration() {
    UsersLinkedList userlist;
    char confirm;
    do {
        cout << "=============================================================================" << endl;
        cout << "\t\t\t      USER REGISTRATION" << endl;
        cout << "=============================================================================" << endl;
        cout << "Do you want to create a new account? (y/n): ";
        cin >> confirm;

        if (confirm != 'y' && confirm != 'Y') {
            cout << "Account creation canceled. Returning to the main menu...\n";
            sleep(2);
            system("cls");
            main();
        }

        system("cls");
        cout << "=============================================================================" << endl;
        cout << "\t\t\t      USER REGISTRATION" << endl;
        cout << "=============================================================================" << endl;

        //fflush(stdin);
		cin.ignore();
        cout << "Username Requirements:\n1. Username must contain at least FIVE characters. \n2. Username must start with a LETTER\n" << endl;
        cout << "Enter username: ";
        getline(cin, username);

        if (username.length() < 5 || !isalpha(username[0])) {
            cout << "Invalid username! Please enter a username with at least five characters and starting with a letter... ";
            sleep(2);
            continue;
        }
		cout << "\nPassword Requirements:\n1. Password must contain at least EIGHT characters. \n2. Password must contain at least one UPPERCASE LETTER.\n3. Password must contain at least one LOWERCASE LETTER?\n4. Password must contain at least one DIGIT.\n5. Password must contain at least one SPECIAL CHARACTER within ~!@#$%^&*_-+=`|(){}[]:;\"'<>,.?/.\n" << endl;
        cout << "Enter password: ";
        cin >> password;

        if (password.length() < 8) {
            cout << "Invalid password! Please enter a password with at least eight characters... ";
            sleep(2);
            continue;
        }

        // Check password complexity
        bool hasUppercase = false, hasLowercase = false, hasDigit = false, hasSpecialChar = false;
        for (int i = 0; i < password.length(); i++) {
                if (isupper(password[i])) {
                    hasUppercase = true;
                } else if (islower(password[i])) {
                    hasLowercase = true;
                } else if (isdigit(password[i])) {
                    hasDigit = true;
                } else if (specialCharacters.find(password[i]) != string::npos) {
                    hasSpecialChar = true;
                }
            }
        if (!(hasUppercase && hasLowercase && hasDigit && hasSpecialChar)) {
            cout << "\nInvalid password! Password must contain at least: \nONE UPPERCASE LETTER \nONE LOWERCASE LETTER \nONE DIGIT \nONE SPECIAL CHARACTER ";
            sleep(3);
            continue;
        }

        if (userlist.checkUsername(username)) {
            cout << "\nUsername already taken! Please try again...";
            sleep(2);
            system("cls");
        } else {
            cout << "\nUser registered successfully! Redirecting to menu...\n";
            userlist.insert(username, password);
            userlist.writeUserToFile("records/registered/user.txt");
            sleep(1);
            system("cls");
            main();
            break;
        }
    } while (true);
}


    string getUsername() {
        return username;
    }

    string getPassword() {
        return password;
    }

    void encode(string& text) {
        replace(text.begin(), text.end(), ' ', '%');
    }
};

class Verify: public Info {
public:
    string acctype, filename;
    UsersLinkedList userLinkedList;
    UsersLinkedList adminLinkedList;

    Verify() {
        userLinkedList.loadUserFromFile("records/registered/user.txt");
        adminLinkedList.loadUserFromFile("records/registered/admin.txt");
    }

    bool login(string acctype) {
        retries = 0;
        UsersLinkedList* listToSearch = NULL;

        if (acctype == "user") {
            listToSearch = &userLinkedList;
        } else if (acctype == "admin") {
            listToSearch = &adminLinkedList;
        }
        
        do {
            cout << "=============================================================================" << endl;
            if (acctype == "user") {
                cout << "\t\t\t         USER LOGIN" << endl;
            } else if (acctype == "admin") {
                cout << "\t\t\t         ADMIN LOGIN" << endl;
            }
            cout << "=============================================================================" << endl;

            //fflush(stdin);
			cin.ignore();
            string uname, pwd;
            cout << "Enter username: ";
            getline(cin, uname);
            cout << "Enter password: ";
            cin >> pwd;

            setUsername(uname);
            setPassword(pwd);

            if (listToSearch->search(username, password)) {
                cout << "\nLogin successful! Redirecting to " << acctype << " menu...\n";
                sleep(1);
                system("cls");
                return true;
            } else {
                cout << "\nThe username or password you have entered is invalid. " << 2 - retries << " more attempt(s)." << "\n";
                sleep(2);
                system("cls");
            }
            retries++;
        } while (retries != 3);
        main();
    }
};



class Book {
	private:
	    string id, name, genre, author;
	    double price;
	    int stock, quantity, source;
	    float rentf, overduef, sumf, earnrent;
	
	public:
	    Book* next;
	
	    Book() {
	        next = NULL;
	    }
	
	    void setdata(string n, double p, int s, string a, string g) {
		    name = n;
		    price = p;
		    stock = s;
		    author = a;
		    genre = g;
		
		    replace(name.begin(), name.end(), '%', ' ');
		    replace(author.begin(), author.end(), '%', ' ');
		    replace(genre.begin(), genre.end(), '%', ' ');
		}
		void setid(string i){
			id = i;
		}
		
		void setStock(int st){
			stock =st;
		}
	    
	    string getBookID() {
	    	return id;
		}
	
	    string getBookName() {
	        return name;
	    }
	
	    double getBookPrice() {
	        return price;
	    }
	
	    int getBookStock() {
	        return stock;
	    }
	
	    int getBookQuantity() {
	        return quantity;
	    }
	
	    string getBookAuthor() {
	        return author;
	    }
	    
	    void setSource(int value) {
        source = value;
    	}
	
	    string getBookGenre() {
	        return genre;
	    }
	    
	    int getSource()  {
        	return source;
    	}
	
	    void details() {	        
	        cout<<left<<setw(35)<<name<<left<<"RM "<<setw(10)<<fixed<<setprecision(2)<<price<<left<<setw(5)<<stock<<endl;
	    }
	
	    void list() {
	        cout << left << setw(35) << name << left << "RM " << setw(10) << fixed << setprecision(2) << price << left << setw(5) << stock << endl;
	    }
};

class BookLinkedList {
	public:
	    Book* head;
	
	public:
	    BookLinkedList() {
	        head = NULL; // Initialize head to NULL
	    }
	
	    ~BookLinkedList() {
	        // Destructor to delete all nodes
	        Book* current = head;
	        while (current != NULL) {
	            Book* next = current->next;
	            delete current;
	            current = next;
	        }
	        head = NULL;
	    }
	
	    void insert(string id, string name, double price, int stock, string author, string genre) {
	        Book* newBook = new Book;
	        newBook->setdata(name, price, stock, author, genre);
	        newBook->setid(id);
	        if (head == NULL) {
	            head = newBook;
	        } else {
	            Book* current = head;
	            while (current->next != NULL) {
	                current = current->next;
	            }
	            current->next = newBook;
	        }
	    }
	    
	    void sort() {
		    if (head == NULL || head->next == NULL) {
		        return; // No need to sort if list is empty or has only one element
		    }
		
		    Book* current = head;
		    Book* index = NULL;
		    while (current != NULL) {
		        index = current->next;
		        while (index != NULL) {
		            if (current->getBookName() > index->getBookName()) {
		                string temp_id = current->getBookID();
		                string temp_name = current->getBookName();
		                double temp_price = current->getBookPrice();
		                int temp_stock = current->getBookStock();
		                string temp_author = current->getBookAuthor();
		                string temp_genre = current->getBookGenre();
		
		                current->setdata(index->getBookName(), index->getBookPrice(), index->getBookStock(), index->getBookAuthor(), index->getBookGenre());
		                index->setdata(temp_name, temp_price, temp_stock, temp_author, temp_genre);
		            }
		            index = index->next;
		        }
		        current = current->next;
		    }
		}
		
        void sortByStock() {
        // Find the maximum stock value
        int maxStock = 0;
        int numEntries = 0;
        Book* current = head;
        while (current) {
            if (current->getBookStock() > maxStock) {
                maxStock = current->getBookStock();
            }
            numEntries++;
            current = current->next;
        }

        // Create count array and initialize to 0
        int* count = new int[maxStock + 1]();
        current = head;
        while (current) {
            count[current->getBookStock()]++;
            current = current->next;
        }

        // Change count[i] so that it now contains the position of this stock in the output array
        for (int i = 1; i <= maxStock; i++) {
            count[i] += count[i - 1];
        }

        // Build the sorted array
        Book** output = new Book*[numEntries];
        current = head;
        while (current) {
            output[count[current->getBookStock()] - 1] = current;
            count[current->getBookStock()]--;
            current = current->next;
        }

        // Reconstruct the linked list in sorted order
        head = nullptr;
        for (int i = numEntries - 1; i >= 0; i--) {
            output[i]->next = head;
            head = output[i];
        }

        // Free dynamically allocated arrays
        delete[] count;
        delete[] output;
    }

		//for id overloading function
		Book* find(string id) {
	        Book* current = head;
	        while (current != NULL) {
	            if (current->getBookID() == id) {
	                return current;
	            }
	            current = current->next;
	        }
	        return NULL;
    	}
    	//for name overloading function
    	Book* find(string name, int num) {
	        Book* current = head;
	        while (current != NULL) {
	        	string bookname = current->getBookName();
	        	if(num==0){
	        		replace(bookname.begin(), bookname.end(), '%', ' ');
	        		replace(name.begin(), name.end(), '%', ' ');
	            	transform(name.begin(), name.end(), name.begin(), ::tolower);
	            	transform(bookname.begin(), bookname.end(), bookname.begin(), ::tolower);
				}
	        		            
	            if (name == bookname) {
	                return current;
	            }
	            current = current->next;
	        }
	        return NULL;
    	}

	    void displayAll() {
	    	int count = 1;
	        Book* current = head;
	        while (current != NULL) {
	        	cout<<left<<setw(5)<<count++;
	            current->details();
	            current = current->next;
	        }
	    }
};

class HashTable {
private:
    static const int tableSize = 26;

    struct Node {
        Book* book;
        Node* next;
    };

    Node* hashTable[tableSize];

public:
    HashTable() {
        for (int i = 0; i < tableSize; ++i) {
            hashTable[i] = NULL;
        }
    }

    ~HashTable() {
        for (int i = 0; i < tableSize; ++i) {
            Node* current = hashTable[i];
            while (current != NULL) {
                Node* next = current->next;
                delete current;
                current = next;
            }
            hashTable[i] = NULL;
        }
    }

    void insert(Book* book) {
        string genre = book->getBookGenre();
        char key = genre[0];
        int index = key - 'A';
        if (index < 0 || index >= tableSize) {
            cerr << "Invalid index: " << index << endl;
            return;
        }

        Node* newNode = new Node;
        newNode->book = book;
        newNode->next = NULL;

        if (hashTable[index] == NULL) {
            hashTable[index] = newNode;
        } else {
            Node* current = hashTable[index];
            while (current->next != NULL) {
                current = current->next;
            }
            current->next = newNode;
        }
    }

    Book** searchByGenre(string genre, int& numResults, int maxResults) {
        const int MAX_RESULTS = 100;
        Book** searchResults = new Book*[MAX_RESULTS];
        numResults = 0;

        char key = genre[0];
        int index = key - 'A';
        if (index < 0 || index >= tableSize) {
            cout << "Invalid index: " << index << endl;
            return NULL;
        }

        Node* current = hashTable[index];
        while (current != NULL && numResults < MAX_RESULTS) {
            searchResults[numResults++] = current->book;
            current = current->next;
        }
        return searchResults;
    }
};


class Cart {
private:
    string bookName;
    double bookPrice;
    int bookQuantity;
    int bookDuration;
    Cart* next;

public:
    Cart() : next(NULL) {}

    void setdata(string name, double price, int quantity, int duration) {
        bookName = name;
        bookPrice = price;
        bookQuantity = quantity;
        bookDuration = duration;
    }

    string getBookName() const {
        return bookName;
    }

    double getBookPrice() const {
        return bookPrice;
    }

    int getBookQuantity() const {
        return bookQuantity;
    }

    int getBookDuration() const {
        return bookDuration;
    }

    double getBookTotalPrice() const {
        return bookPrice * bookQuantity;
    }

    Cart* getNext() const {
        return next;
    }

    void setNext(Cart* n) {
        next = n;
    }

    void list() const {
        cout << left << setw(38) << bookName << left << setw(8) << bookDuration << left << setw(8) << bookQuantity << left << "RM " << setw(10) << fixed << setprecision(2) << getBookTotalPrice() << endl;
    }
};


class CartLinkedList {
private:
    Cart* head;

public:
    CartLinkedList() : head(NULL) {}

    ~CartLinkedList() {
        Cart* current = head;
        while (current != NULL) {
            Cart* next = current->getNext();
            delete current;
            current = next;
        }
        head = NULL;
    }

    void insert(string name, double price, int quantity, int duration) {
        Cart* newCart = new Cart;
        replace(name.begin(), name.end(), '%', ' ');
        newCart->setdata(name, price, quantity, duration);
        if (head == NULL) {
            head = newCart;
        } else {
            Cart* current = head;
            while (current->getNext() != NULL) {
                current = current->getNext();
            }
            current->setNext(newCart);
        }
    }

    void display() const {
	    Cart* current = head;
	    int count = 1;
	    int bookFinalQuantity = 0; // Initialize bookFinalQuantity
	    double bookGrandTotal = 0.0; // Initialize bookGrandTotal
	    while (current != NULL) {
	        cout << left << setw(5) << count++;
	        current->list();
	        // Update bookFinalQuantity and bookGrandTotal
	        bookFinalQuantity += current->getBookQuantity();
	        bookGrandTotal += current->getBookTotalPrice();
	        current = current->getNext();
	    }
	    // Display the grand total after displaying all items
	    cout << "-----------------------------------------------------------------------------" << endl;
	    cout << left << setw(51) << "Grand Total: " << left << setw(8) << bookFinalQuantity << left << "RM " << setw(10) << setprecision(2) << bookGrandTotal << endl;
	}
	
	Cart* getHead() const {
        return head;
    }

    Cart* getEntryAtIndex(int index) const {
        if (index < 0) {
            return NULL;
        }

        Cart* current = head;
        int currentIndex = 0;

        while (current != NULL) {
            if (currentIndex == index) {
                return current;
            }
            current = current->getNext();
            currentIndex++;
        }

        return NULL; // Return NULL if index is out of bounds
    }
};

class RentedBook {
	private:
		string bookName;
		int bookQuantity;
		string dueDate;
		RentedBook* next;

	public:
		RentedBook(string name, int quantity, string date) : bookName(name), bookQuantity(quantity), dueDate(date), next(nullptr) {}

		string getBookName() const {
			return bookName;
		}

		int getBookQuantity() const {
			return bookQuantity;
		}

		string getDueDate() const {
			return dueDate;
		}

		RentedBook* getNext() const {
			return next;
		}

		void setNext(RentedBook* n) {
			next = n;
		}

        void list() const {
            cout << left << setw(38) << bookName << left << setw(8) << bookQuantity << left << setw(17) << dueDate << endl;
        }
};

class RentedLinkedList {
	private:
		RentedBook* head;

	public:
		RentedLinkedList() : head(nullptr) {}

		void addBook(string name, int quantity, string date) {
			RentedBook* newBook = new RentedBook(name, quantity, date);
			newBook->setNext(head);
			head = newBook;
		}

		void listBooks() const {
			RentedBook* current = head;
			while (current != nullptr) {
				current->list();
				current = current->getNext();
			}
		}

		// Don't forget to add a destructor to delete the linked list
		~RentedLinkedList() {
			while (head != nullptr) {
				RentedBook* bookToDelete = head;
				head = head->getNext();
				delete bookToDelete;
			}
		}
};

class Menus: public Verify, public Book {
	public:
		//constructor
		Menus(){
			cout << "=================================================================================="<<endl;
    		cout << "\t        ~WELCOME TO THE SUNRISE 2ND BOOK RENTAL SYSTEM~"<<endl;
    		cout << "=================================================================================="<<endl;
    		cout << "\t\t\t\tMAIN MENU"<<endl;
		}
		
		void addbook();
		void searchbook();
		void viewbook();
		void searchbk();
		void searchbg();
		void updatebook(string code);
		void deletebook(string code);
		void fee();
		void checkrentalrecord();
		
		void userMenu(){
		    while (true) 
		    {
		    	cout << "============================================================================="<<endl;
			    cout << "[0] Back \t\t\t USER MENU"<<endl;
			    cout << "============================================================================="<<endl;
			    cout << "User Menu"<<endl;
			    cout << "-----------------------------------------------------------------------------"<<endl;
			    cout << "Welcome "<< getUsername() <<"! What would you like to choose?\n"<<endl;
		        cout << "1. Browse Catalog\n";
		        cout << "2. Search Catalog\n";
		        cout << "3. View Cart\n";
		        cout << "4. View Rented Books\n";
		        cout << "5. View Receipts\n";
		        cout << "-----------------------------------------------------------------------------"<<endl;
		        cout << "Enter your choice: ";
		        int choice;
		        cin >> choice;
		        switch (choice) 
		        {
		        	case 0:
		        		system("cls");
		        		main();
		            case 1:
		            	system("cls");
		                catalog();
		                break;
		            case 3:
		                system("cls");
		                cart();
		                break;
		            case 4:
		            	system("cls");
						rented();
		                break;
		            case 2:
		                system("cls");
		                searchCatalog();
		                break;
		            case 5:
						system("cls");
						//viewReceipt();
						break;    
		            default:
		                cout << "\nInvalid choice! Please re-enter...\n";
		                sleep(1);
				    	system("cls");
				    	userMenu();
		        }
		    }
		}
		
		void adminMenu(){
		    int choice;	
			
			while (true) 
		    {
		    	system("cls");
		    	cout << "============================================================================="<<endl;
			    cout << "[0] Back \t\t\t ADMIN MENU"<<endl;
			    cout << "============================================================================="<<endl;
			    cout << "Admin Menu"<<endl;
			    cout << "-----------------------------------------------------------------------------"<<endl;
			    cout << "Hello "<< getUsername()<<"! What do you want to do?\n"<<endl;
		        cout << "1. Add Book\n";
		        cout << "2. Search Book [Update & Delete]\n";
		        cout << "3. Fee Management\n";
		        cout << "4. View Rental Records\n";
			    cout << "-----------------------------------------------------------------------------"<<endl;
				cout << "Enter your choice: ";
		        cin >> choice;
		        
		        switch (choice) 
		        {
		        	case 0:
		        		system("cls");
		        		main();
		        		break;
		        	case 1:
		        		addbook();
		        		break;
		        	case 2:
		        		//searchbook();
		        		break;
		        	case 3:
		        		//fee();
		        		break;
		        	case 4:
		        		//checkrentalrecord();
		        		break;
		            default:
		                cout << "\nInvalid choice! Please re-enter...\n";
		                sleep(1);
				    	adminMenu();
		        }
		    }
		}
		
		void catalog() {
		    string id, name, author, genre, line;
		    double price;
		    int stock, choice = 0, numEntries = 0;
		
		    cout << "=============================================================================" << endl;
		    cout << "[0] Back \t\t\t  CATALOG" << endl;
		    cout << "=============================================================================" << endl;
		    cout << "User Menu > Catalog" << endl;
		    cout << "-----------------------------------------------------------------------------" << endl;
            cout << left << setw(5) << "No." << left << setw(35) << "Book Name" << left << setw(12) << "Price" << left << setw(5) << "Stock" << endl;

		    ifstream countFile("records/books.txt");
		    if (!countFile.is_open()) {
		        cout << "Error: Unable to open the file 'records/books.txt'\n";
		        exit(0);
		    }
		    while (getline(countFile, line)) {
		        numEntries++;
		    }
		    countFile.close();
		
		    // Create a linked list to store books
		    BookLinkedList bookList;
		
		    ifstream file("records/books.txt");
		    // Set Book attributes and insert into linked list
		    while (file >> id >> name >> price >> stock >> author >> genre) {
		        bookList.insert(id, name, price, stock, author, genre);
			}
			bookList.displayAll();
			
		    bool continueLoop = true;
		    while (continueLoop) {
		
		    cout << "=============================================================================" << endl;
            cout << "[1] Sort by Book Name" << endl;
            cout << "[2] Sort by Stock" << endl;
            cout << "[3] Rent a Book" << endl;
            cout << "Enter your choice: ";
            cin >> choice;
		
		    switch (choice) {
		            case 0:
		                system("cls");
		                userMenu();
		                continueLoop = false;
		                break;
		            case 1:
		                bookList.sort();
		                system("cls");
		                cout << "=============================================================================" << endl;
					    cout << "[0] Back \t\t\t  CATALOG" << endl;
					    cout << "=============================================================================" << endl;
					    cout << "User Menu > Catalog > Sorted by Book Name" << endl;
					    cout << "-----------------------------------------------------------------------------" << endl;
		    			cout << left << setw(5) << "No." << left << setw(35) << "Book Name" << left << setw(12) << "Price" << left << setw(5) << "Stock" << endl;
		                bookList.displayAll();
		                break;
		            case 2:
		                bookList.sortByStock();
		                system("cls");
		                cout << "=============================================================================" << endl;
					    cout << "[0] Back \t\t\t  CATALOG" << endl;
					    cout << "=============================================================================" << endl;
					    cout << "User Menu > Catalog > Sorted by Stock" << endl;
					    cout << "-----------------------------------------------------------------------------" << endl;
		    			cout << left << setw(5) << "No." << left << setw(35) << "Book Name" << left << setw(12) << "Price" << left << setw(5) << "Stock" << endl;
		                bookList.displayAll();
		                break;
		            case 3:
		                cout << "Enter the book number to rent: ";
		                cin >> choice;
		                if (choice > 0 && choice <= numEntries) {
		                    do {
		                        rental(choice, bookList);
		                        cout << "Enter another book number to rent or 0 to go back: ";
		                        cin >> choice;
		                    } while (choice != 0);
		                } else {
		                    cout << "\nInvalid choice! Please re-enter...\n";
		                    sleep(1);
		                    system("cls");
		                }
		                break;
		            default:
		                cout << "\nInvalid choice! Please re-enter...\n";
		                sleep(1);
		                system("cls");
		                break;
		        }
		    }
		}
		
		void searchCatalog() {
		    string keyword;
		    int searchoption;
		    cout << "============================================================================="<<endl;
    		cout << "[0] Back \t\t      SEARCH CATALOG" << endl;
    		cout << "============================================================================="<<endl;
    		cout << "User Menu > Search Catalog" << endl;
    		cout << "-----------------------------------------------------------------------------" << endl;
			cout << "[1] Search by book name" << endl;
			cout << "[2] Search by genre" << endl;
			cout << "-----------------------------------------------------------------------------" << endl;
			cout << "Enter your choice: ";
			cin >> searchoption;
			if(searchoption==0)
			{
				system("cls");
				userMenu();
			}
			else if(searchoption==1)
			{
				system("cls");
				cout << "=============================================================================" << endl;
    			cout << "[0] Back \t\t\tSEARCH CATALOG" << endl;
    			cout << "=============================================================================" << endl;
    			cout << "User Menu > Search Catalog > Search by book name" << endl;
    			cout << "-----------------------------------------------------------------------------"<<endl;
    			cout << "Enter book name to search: ";
    			cin.ignore();
    			getline(cin, keyword);
    			transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);
    			cout << "-----------------------------------------------------------------------------"<<endl;
    		
    			if (keyword == "0") 
        		{
            		system("cls");
            		searchCatalog();  
        		}
		    
		
			    BookLinkedList bookList;
			
			    string id, name, author, genre;
			    double price;
			    int stock;
			
			    ifstream file("records/books.txt");
			    while (file >> id >> name >> price >> stock >> author >> genre) {
			        bookList.insert(id, name, price, stock, author, genre);
			        bookList.sort();
			    }
			    file.close();
			
			    int numBooks = 0;
			    Book* currentBook = bookList.head;
			    while (currentBook != NULL) {
			        numBooks++;
			        currentBook = currentBook->next;
			    }
			    Book** booksArray = new Book*[numBooks];
			    currentBook = bookList.head;
			    int index = 0;
			    while (currentBook != NULL) {
			        booksArray[index++] = currentBook;
			        currentBook = currentBook->next;
			    }
			
			    for (int i = 0; i < numBooks - 1; i++) {
			        for (int j = 0; j < numBooks - i - 1; j++) {
			            if (booksArray[j]->getBookName() > booksArray[j + 1]->getBookName()) {
			                Book* temp = booksArray[j];
			                booksArray[j] = booksArray[j + 1];
			                booksArray[j + 1] = temp;
			            }
			        }
			    }
			
			    int first = 0;
			    int last = numBooks - 1;
			    int mid;
				while (first <= last) {
				    mid = (first + last) / 2;
				    string bookname = booksArray[mid]->getBookName();
				    transform(bookname.begin(), bookname.end(), bookname.begin(), ::tolower);
				    if (bookname == keyword) {
				        cout << "Search Result:\n" << endl;
				        cout<<left<<setw(35)<<"Book Name"<<left<<setw(12)<<"Price"<<left<<setw(5)<<"Stock"<<endl;
				        booksArray[mid]->details();	
				        char choice;
        				cout << "-----------------------------------------------------------------------------"<<endl;
        				cout << "Do you want to rent this book? [Y/N]: ";
        				cin >> choice;
        				if(choice == 'Y' || choice == 'y'){
        					booksArray[mid]->setSource(1);										        
				        	rental(mid + 1, bookList);
				        	delete[] booksArray;
						}
						else if(choice == 'N' || choice == 'n'){
							system("cls");
							searchCatalog();
						}
						else{
							cout << "\nInvalid choice! Please try again...\n";
        					sleep(2);
        					system("cls");
        					searchCatalog();
						}
				    } else if (bookname < keyword) {
				        first = mid + 1;
				    } else {
				        last = mid - 1;
				    }
				}

			    
				int searchagain;
    				cout << "-----------------------------------------------------------------------------" << endl;
        			cout << "\n\n\t\t\t     No results found.\n\n"<<endl;
        			cout << "-----------------------------------------------------------------------------" << endl;
					cout << "[1] Search again"<<endl;
					cout << "-----------------------------------------------------------------------------" << endl;
					cout << "Enter Choice: ";
        			cin >> searchagain;
        		
        			if(searchagain == 1)
        			{
        				system("cls");
        				searchCatalog();
					}
					else if(searchagain == 0)
					{
						system("cls");
						userMenu();
					}
					else
					{	
						cout << "\nInvalid choice! Please try again...\n";
        				sleep(2);
						system("cls");
						searchCatalog();
					}
			
			    delete[] booksArray;
			}
			else if (searchoption == 2)
			{
    			system("cls");
			    cout << "=============================================================================" << endl;
			    cout << "[0] Back \t\t\tSEARCH CATALOG" << endl;
			    cout << "=============================================================================" << endl;
			    cout << "User Menu > Search Catalog > Search by genre" << endl;
			    cout << "-----------------------------------------------------------------------------" << endl;
			    HashTable hashTable;
			    BookLinkedList bookList;
			    string id, name, author, genre, line;
			    double price;
			    int stock;
			    int numGenres = 0;
			    int g = 0;
			    
			    ifstream file("records/books.txt");
			    while (file >> id >> name >> price >> stock >> author >> genre) {
			        bookList.insert(id, name, price, stock, author, genre);
			        g++;
			        bookList.sort();
			    }
			    file.close();
			    
			    Book* currentBook = bookList.head;
				while (currentBook != NULL) {
				    hashTable.insert(currentBook);
				    currentBook = currentBook->next;
				}
			
    			string *genres = new string[g];
    			ifstream genresFile("records/books.txt");
    			while (genresFile >> id >> name >> price >> stock >> author >> genre)
    			{
        			genres[numGenres] = genre;
        			++numGenres;
    			}
    			genresFile.close();

    			// Remove duplicate genres
    			for (int i = 0; i < numGenres - 1; ++i)
    			{
        			for (int j = i + 1; j < numGenres;)
        			{
            			if (genres[i] == genres[j])
            			{
                			for (int k = j; k < numGenres - 1; ++k)
                			{
                    			genres[k] = genres[k + 1];
                			}
                			--numGenres;
            			}
            			else
            			{
                			++j;
            			}
        			}
    			}

    			cout << "Available Genres:\n";
    			for (int i = 0; i < numGenres; i++)
    			{
        			replace(genres[i].begin(), genres[i].end(), '%', ' ');
        			cout << i + 1 << ". " << genres[i] << endl;
        			replace(genres[i].begin(), genres[i].end(), ' ', '%');
    			}
    			cout << "-----------------------------------------------------------------------------"<<endl;
    			cout << "Enter the genre to search: ";
			    cin >> genre;
			    
			    bool validGenre = false;
			    for (int i = 0; i < numGenres; ++i) {
			        if (genre == genres[i]) {
			            validGenre = true;
			            break;
			        }
			    }
			
			    if (!validGenre) {
			        cout << "\nInvalid genre! Please select a genre from the list." << endl;
			        sleep(2);
			        system("cls");
			        searchCatalog();
			    }else if (genre == "0") {
			        system("cls");
			        searchCatalog();
			    } else {
			        int numResults = 0;
            		Book** searchResults = hashTable.searchByGenre(genre, numResults, g);
            		if (searchResults == NULL || numResults == 0) {
            			system("cls");
					    cout << "=============================================================================" << endl;
					    cout << "[0] Back \t\t\tSEARCH CATALOG" << endl;
					    cout << "=============================================================================" << endl;
					    cout << "User Menu > Search Catalog > Search by genre" << endl;
					    cout << "-----------------------------------------------------------------------------" << endl;
		                cout << "-----------------------------------------------------------------------------" << endl;
	        			cout << "\n\n\t\t\t     No results found.\n\n"<<endl;
	        			int searchagain;
	        			cout << "-----------------------------------------------------------------------------" << endl;
						cout << "[1] Search again"<<endl;
						cout << "-----------------------------------------------------------------------------" << endl;
						cout << "Enter Choice: ";
	        			cin >> searchagain;
        		
	        			if(searchagain == 1)
	        			{
	        				system("cls");
	        				searchCatalog();
						}
						else if(searchagain == 0)
						{
							system("cls");
							userMenu();
						}
						else
						{	
							cout << "\nInvalid choice! Please try again...\n";
	        				sleep(2);
							system("cls");
							searchCatalog();
						}
            		} else {
            			system("cls");
					    cout << "=============================================================================" << endl;
					    cout << "[0] Back \t\t\tSEARCH CATALOG" << endl;
					    cout << "=============================================================================" << endl;
					    cout << "User Menu > Search Catalog > Search by genre" << endl;
					    cout << "-----------------------------------------------------------------------------" << endl;
		                cout << "Search Results for Genre '" << genre << "'" << endl;
		                cout << "-----------------------------------------------------------------------------" << endl;
		                cout << left << setw(5) << "No." << left << setw(35) << "Book Name" << left << setw(12) << "Price" << left << setw(8) << "Stock" << endl;
		                for (int i = 0; i < numResults; ++i) {
							cout << left << setw(5) << i + 1;
			                searchResults[i]->details();
                		}
		                // Ask user to choose a book to rent
		                int choice;
		                cout << "-----------------------------------------------------------------------------" << endl;
		                cout << "Enter the number of the book you want to rent (0 to go back): ";
		                cin >> choice;
		                
		                if (choice == 0) {
		                    system("cls");
		                    searchCatalog();
		                } else if (choice > 0 && choice <= numResults) {
		                    Book* selectedBook = bookList.find(searchResults[choice - 1]->getBookID());
			                if (selectedBook != NULL) {
			                	selectedBook->setSource(1);
			                    rental(choice, bookList);
			                } else {
			                    cout << "Selected book not found in the catalog." << endl;
			                }
		                } else {
		                    cout << "Invalid choice!" << endl;
		                }
            		}
			    }    
			}					
		}
				
		void rental(int choice, BookLinkedList& bookList) {
		    int quantity, duration;
		    int choice2 = 0;
		    system("cls");
		    cout << "=============================================================================" << endl;
		    cout << "[0] Back \t\t\t   RENT" << endl;
		    cout << "=============================================================================" << endl;
		
		    // Find the selected book based on the choice
		    Book* selectedBook = bookList.head;
		    int count = 1;
		    while (selectedBook != NULL && count < choice) {
		        selectedBook = selectedBook->next;
		        count++;
		    }
		
		    if (selectedBook == NULL) {
		        cout << "Invalid choice! Please re-enter...\n";
		        sleep(1);
		        system("cls");
		        catalog();
		        return;
		    }
		
		    // Display details of the selected book
		    cout << "User Menu > " << (selectedBook->getSource() == 0 ? "Catalog" : "Search Catalog") << " > " << selectedBook->getBookName() << endl;
		    cout << "-----------------------------------------------------------------------------" << endl;
		    cout << left<<setw(15)<<"Book Title"<< ": "<< selectedBook->getBookName() <<endl;
		    cout << left<<setw(15)<<"Author"<< ": "<< selectedBook->getBookAuthor() <<endl;
		    cout << left<<setw(15)<<"Genre"<< ": "<< selectedBook->getBookGenre() <<endl;
		    cout << left<<setw(15)<<"Base Price"<< ": RM "<< selectedBook->getBookPrice() <<endl;
		    cout << left<<setw(15)<<"Current Stock"<< ": "<< selectedBook->getBookStock() <<endl;
		    cout << "-----------------------------------------------------------------------------" << endl;
		    cout << "[1] Start renting" << endl;
		    cout << "-----------------------------------------------------------------------------" << endl;
		    cout << "Enter your choice: ";
		    cin >> choice2;
		    if (choice2 == 0) {
		        system("cls");
		        if (selectedBook->getSource() == 0) {
		            catalog();
		        } else {
		            searchCatalog();
		        }
		    } else if (choice2 == 1) {
		        char confirm;
		        double total_fee;
//		        float rentfee = getrentfee();
				float rentfee = 0.20;	
		        system("cls");
		        cout << "=============================================================================" << endl;
		        cout << "[0] Back \t\t\t   RENT" << endl;
		        cout << "=============================================================================" << endl;
		        cout << "User Menu > " << (selectedBook->getSource() == 0 ? "Catalog" : "Search Catalog") << " > " << selectedBook->getBookName() << " > Rental" << endl;
		        cout << "-----------------------------------------------------------------------------" << endl;
		        cout << left<<setw(15)<<"Book Title"<< ": "<< selectedBook->getBookName() <<endl;
			    cout << left<<setw(15)<<"Author"<< ": "<< selectedBook->getBookAuthor() <<endl;
			    cout << left<<setw(15)<<"Genre"<< ": "<< selectedBook->getBookGenre() <<endl;
			    cout << left<<setw(15)<<"Base Price"<< ": RM "<< selectedBook->getBookPrice() <<endl;
			    cout << left<<setw(15)<<"Current Stock"<< ": "<< selectedBook->getBookStock() <<endl;
		        cout << "-----------------------------------------------------------------------------" << endl;
		        cout << "**DISCLAIMER" << endl;
		        cout << "  Total fee is calculated based on the base price and additional\n  charges." << endl;
		        cout << "  Additional charges are RM " << fixed << setprecision(2) << rentfee << "/day for each book rented." << endl;
		        cout << "-----------------------------------------------------------------------------" << endl;
		        cout << left << setw(19) << "Quantity" << ": ";
		        cin >> quantity;
		
		        if (selectedBook->getBookStock() < quantity) {
		            cout << "\nNot enough stock! Please try again..." << endl;
		            sleep(2);
		            system("cls");
		            rental(choice, bookList);
		        }
		        
		
		        cout << left << setw(19) << "Rent duration" << ": ";
		        cin >> duration;
		        if (quantity <= 0 || duration <= 0) {
		            cout << "\nQuantity and Duration cannot be 0 or less! Please re-enter..." << endl;
		            sleep(2);
		            system("cls");
		            rental(choice, bookList);
		        }
		        total_fee = quantity * (selectedBook->getBookPrice() + (rentfee * duration));
		        cout << left << setw(19) << "Total fee" << ": RM " << total_fee << endl;
		        cout << left << setw(19) << "Add to cart? [Y/N] : ";
		        cin >> confirm;
		        if(confirm == 'Y' || confirm == 'y'){
		    		cout << "\nAdding to cart...\n";
		        	sleep(1);
		        	filename = "records/cart/" + getUsername() + ".txt";
					ofstream outFile(filename.c_str(), ios::app);
				    string rentedBookName = selectedBook->getBookName();
					encode(rentedBookName);
					outFile << rentedBookName << " " << selectedBook->getBookPrice() << " " << quantity << " " << duration << "\n";
				    outFile.close();
				} else if (confirm == 'N' || confirm == 'n') {
		            cout << "\nCancelling...\n";
		            sleep(1);
		        } else {
		            cout << "\nInvalid choice! Please try again...\n";
		            sleep(1);
		        }
		        rental(choice, bookList);
		    } else {
		        cout << "\nInvalid choice! Please re-enter...\n";
		        sleep(1);
		        rental(choice, bookList);
		    }
		}
		
		void cart(){
		    string bookName, line;
		    int bookQuantity, bookDuration, bookFinalQuantity = 0,numEntries = 0, choice, i = 0;
		    double bookPrice, bookTotalPrice, bookGrandTotal = 0.00;
		    string filename = "records/cart/" + getUsername() + ".txt";
		
		    ifstream countFile(filename.c_str());
		    if (countFile.is_open()) {
		        while (getline(countFile, line)) {
		            numEntries++;
		        }
		        countFile.close();
		    }
		
		    CartLinkedList cartList;
		
		    cout << "============================================================================="<<endl;
		    cout << "[0] Back \t\t\t    CART"<<endl;
		    cout << "============================================================================="<<endl;
		    cout << "User Menu > Cart"<<endl;
		    cout << "-----------------------------------------------------------------------------"<<endl;
		    cout <<left<<setw(5)<<"No."<<left<<setw(37)<<"Book Name"<<left<<setw(8)<<"Days"<<left<<setw(9)<<"Qty."<<left<<setw(12)<<fixed<<setprecision(2)<<"Total"<<endl;
		    ifstream file(filename.c_str());
		    while(file >> bookName >> bookPrice >> bookQuantity >> bookDuration){
		        cartList.insert(bookName, bookPrice, bookQuantity, bookDuration);
		    }
			if(numEntries == 0){
				cout<<"\n\n\t\t\tIt seems your cart is empty...\n\n"<<endl;
			}
		    cartList.display();
		    cout << "============================================================================="<<endl;
		    cout << "[1] Edit item"<<endl;
		    cout << "[2] Delete item"<<endl;
		    cout << "[3] Check out"<<endl;
		    cout << "-----------------------------------------------------------------------------"<<endl;
		    cout << "Enter your choice: ";
		    cin >> choice;
		    if(numEntries == 0){
		    	if(choice < 0){
			    	cout<<"\nInvalid choice! Please re-enter..."<<endl;
			    	sleep(2);
					system("cls");
					cart();
				}
		    	else if(choice == 0){
			    	system("cls");
			        userMenu();
				}	
				else{
					cout<<"\nNo items in cart! Please try again later..."<<endl;
			    	sleep(2);
					system("cls");
					cart();
				}
		    }else{
		    	if(choice == 0){
			    	system("cls");
			        userMenu();
				}
				else if(choice == 1){
					int lineSelect,quantityNew, durationNew;
					string bookName;
					system("cls");
			        cout << "============================================================================="<<endl;
				    cout << "[0] Back \t\t\t    CART"<<endl;
				    cout << "============================================================================="<<endl;
				    cout << "User Menu > Cart > Edit"<<endl;
				    cout << "-----------------------------------------------------------------------------"<<endl;
				    cout <<left<<setw(5)<<"No."<<left<<setw(37)<<"Book Name"<<left<<setw(8)<<"Days"<<left<<setw(9)<<"Qty."<<left<<setw(12)<<"Total"<<endl;
				    while(file >> bookName >> bookPrice >> bookQuantity >> bookDuration){
				    	cartList.insert(bookName, bookPrice, bookQuantity, bookDuration);
				    }
				    cartList.display();
				    cout << "============================================================================="<<endl;
				    cout << "Select line to edit: ";
				    cin >> lineSelect;
				    cout << "-----------------------------------------------------------------------------"<<endl;
					if(lineSelect == 0){
				    	system("cls");
				        cart();
					}
					else if(lineSelect > 0 && lineSelect <= numEntries){
						int stock;
						string ID, bookName2, author, genre;
						double price;
						char updateConfirm;
//						float rentfee = getrentfee();
						float rentfee = 0.20;
						
						Cart* selectedCart = cartList.getEntryAtIndex(lineSelect-1);
						if (selectedCart != NULL) {
					        cout << "Selected '" << selectedCart->getBookName() << "'.\n" << endl;
					    }
						cout << "New Quantity       : ";
						cin >> quantityNew;
						
						ifstream fileStockRead("records/books.txt");
							    
					    while (fileStockRead >> ID >> bookName2 >> price >> stock >> author >> genre) {
					    	replace(bookName2.begin(), bookName2.end(), '%', ' ');	
					        if (bookName2 == selectedCart->getBookName()) {
					            if(stock<quantityNew){
					            	cout<<"\nNot enough stock! Please try again..."<<endl;
						    		sleep(2);
									system("cls");
									cart();
								}
					            break;
					        }
						
						}				
						fileStockRead.close();
						
						cout << "New Duration (Days): ";
				    	cin >> durationNew;
				    	if( quantityNew <= 0 || durationNew <= 0 ){
				    		cout<<"\nQuantity and Duration cannot be 0 or less! Please re-enter..."<<endl;
				    		sleep(2);
							system("cls");
							cart();
						}
						else{
							
						}
						cout << "-----------------------------------------------------------------------------"<<endl;
						cout<<"New total will be RM "<<quantityNew*(selectedCart->getBookPrice() + (rentfee*durationNew))<<"."<<endl;
						
						cout<<"Confirm change? [Y/N]: ";
						cin>>updateConfirm;
						if(updateConfirm == 'Y'||updateConfirm == 'y'){
							cout<<"\nUpdating cart..."<<endl;
						}
						else if(updateConfirm == 'N'||updateConfirm == 'n'){
							cout<<"\nCancelling..."<<endl;
							sleep(1);
							system("cls");
							cart();
						}
						else{
							cout<<"\nInvalid Input! Please try again..."<<endl;
							sleep(2);
							system("cls");
							cart();
						}
				    	
				    	filename = "records/cart/" + getUsername() + ".txt";
				    	ifstream fileInput(filename.c_str());
    					vector<string> lines;
    					string line;
    					
    					while (getline(fileInput, line)) {
        					lines.push_back(line);
    					}
    					
    					fileInput.close();
    					
    					// Convert to string
					    float priceConvert = selectedCart->getBookPrice();
					    stringstream floatStream;
					    floatStream << priceConvert;
					    string bookPrice = floatStream.str();
    					string bookName = selectedCart->getBookName();
    					replace(bookName.begin(), bookName.end(), ' ', '%' );
    					
    					float quantityConvert = quantityNew;
					    stringstream floatStream2;
					    floatStream2 << quantityConvert;
					    string quantityNew = floatStream2.str();
    					
    					float durationConvert = durationNew;
					    stringstream floatStream3;
					    floatStream3 << durationConvert;
					    string durationNew = floatStream3.str();
    					
						lines[lineSelect-1] = bookName + " " + bookPrice + " " + quantityNew + " " + durationNew;
						
				    	ofstream fileOutput(filename.c_str());
					    for (size_t i = 0; i < lines.size(); ++i) {
					        fileOutput << lines[i] << '\n';
					    }

					    fileOutput.close();
	
				    	sleep(1);
						system("cls");
						cart();
					}
				    else{
				    	cout << "\nInvalid choice! Please re-enter...\n";
				        sleep(1);
				        system("cls");
				        cart();
					}
				}
				else if(choice == 2){
					int lineSelect;
			    	system("cls");
			        cout << "============================================================================="<<endl;
				    cout << "[0] Back \t\t\t    CART"<<endl;
				    cout << "============================================================================="<<endl;
				    cout << "User Menu > Cart > Delete"<<endl;
				    cout << "-----------------------------------------------------------------------------"<<endl;
				    cout <<left<<setw(5)<<"No."<<left<<setw(37)<<"Book Name"<<left<<setw(8)<<"Days"<<left<<setw(9)<<"Qty."<<left<<setw(12)<<"Total"<<endl;
				    cartList.display();
				    cout << "============================================================================="<<endl;
				    cout << "Select line to delete: ";
				    cin >> lineSelect;
				    cout << "-----------------------------------------------------------------------------"<<endl;
					if(lineSelect == 0){
				    	system("cls");
				        cart();
					}
					else if(lineSelect > 0 && lineSelect <= numEntries){
						char updateConfirm;
						Cart* selectedCart = cartList.getEntryAtIndex(lineSelect-1);
						
						if (selectedCart != NULL) {
					        cout << "Selected '" << selectedCart->getBookName() << "'.\n" << endl;
					    }
						cout<<"\nConfirm delete? [Y/N]: ";
						cin>>updateConfirm;
						if(updateConfirm == 'Y'||updateConfirm == 'y'){
							cout<<"\nDeleting from cart..."<<endl;
							
						}
						else if(updateConfirm == 'N'||updateConfirm == 'n'){
							cout<<"\nCancelling..."<<endl;
							sleep(1);
							system("cls");
							cart();
						}
						else{
							cout<<"\nInvalid Input! Please try again..."<<endl;
							sleep(2);
							system("cls");
							cart();
						}
				    	
				    	filename = "records/cart/" + getUsername() + ".txt";
				    	ifstream fileInput(filename.c_str());
    					vector<string> lines;
    					string line;
    					
    					while (getline(fileInput, line)) {
        					lines.push_back(line);
    					}
    					
    					fileInput.close();
    											
				    	ofstream fileOutput(filename.c_str());
					     for (size_t i = 0; i < lines.size(); ++i) {
					        if (i != (lineSelect - 1)) {
					            fileOutput << lines[i] << '\n';
					        }
					    }

					
					    fileOutput.close();
				    	
				    	
				    	sleep(1);
						system("cls");
						cart();
					}
					else{
						cout << "\nInvalid choice! Please re-enter...\n";
				        sleep(1);
				        system("cls");
				        cart();
					}
				}
				else if(choice == 3){
					Cart* current = cartList.getHead();
					int index = 0;
				    string bookToSubtract[100];
				    int stockToSubtract[100];
					char checkoutConfirm;
					int bookFinalQuantity2 = 0, count = 0;
					double bookGrandTotal2 = 0;
			        system("cls");
			        cout << "============================================================================="<<endl;
				    cout << "[0] Back  \t\t\t  CHECK OUT"<<endl;
				    cout << "============================================================================="<<endl;
				    cout << "User Menu > Cart > Check Out"<<endl;
				    cout << "-----------------------------------------------------------------------------"<<endl;
				    cout <<left<<setw(5)<<"No."<<left<<setw(37)<<"Book Name"<<left<<setw(8)<<"Days"<<left<<setw(9)<<"Qty."<<left<<setw(12)<<"Total"<<endl;
				    while (current != NULL) {
				        cout << left << setw(5) << ++count;
				        current->list();
				        bookGrandTotal += current->getBookTotalPrice();
				        bookFinalQuantity += current->getBookQuantity();
				        bookGrandTotal2 += current->getBookTotalPrice();
				        bookFinalQuantity2 += current->getBookQuantity();
				        current = current->getNext();
				    }
					cartList.display();
		    		cout << "============================================================================="<<endl;
				    cout << "Confirm check out? [Y/N]: ";
				    cin>>checkoutConfirm;
						if (checkoutConfirm == 'Y' || checkoutConfirm == 'y') {
					        cout << "\nGenerating receipt..." << endl;
					        sleep(2);
					        system("cls");
					
					        int numEntries = 0;
					        Cart* temp = cartList.getHead();
					        while (temp != NULL) {
					            numEntries++;
					            temp = temp->getNext();
					        }
					
					        string bookToSubtract[numEntries];
					        int stockToSubtract[numEntries], lineNum[numEntries];
					
					        string filename = "records/rented/" + getUsername() + ".txt";
					        ofstream fileOutput(filename.c_str(), ios::app);
					        if (!fileOutput) {
					            cout << "Error opening output file!" << endl;
					        }
					
					        Cart* current = cartList.getHead();
					        int index = 0;
					        while (current != NULL) {
					            time_t currentTime = time(0);
					            tm currentDate = *localtime(&currentTime);
					
					            int daysToAdd = current->getBookDuration();
					            string rentedBookName, rentedBookDue;
					            currentDate.tm_mday += daysToAdd;
					            mktime(&currentDate);
					            stringstream ss;
					            ss << setw(2) << setfill('0') << currentDate.tm_mday << "/" << setw(2) << setfill('0') << currentDate.tm_mon + 1 << "/" << setw(4) << setfill('0') << currentDate.tm_year + 1900 << endl;
					            rentedBookDue = ss.str();
					
					            rentedBookName = current->getBookName();
					            encode(rentedBookName);
					            bookToSubtract[index] = rentedBookName;
					            stockToSubtract[index] = current->getBookQuantity();
					
					            fileOutput << rentedBookName << " " << current->getBookQuantity() << " " << rentedBookDue;
					
					            current = current->getNext();
					            index++;
					        }
					
					        int totalsum[numEntries];
					        for (int j = 0; j < numEntries; j++) {
					            totalsum[j] = 0;
					            int duplicatenum = 0;
					
					            for (int k = 0; k < numEntries; k++) {
					                if (bookToSubtract[j] == bookToSubtract[k]) {
					                    if (j != k) {
					                        duplicatenum += stockToSubtract[k];
					                    }
					                }
					            }
					            totalsum[j] += stockToSubtract[j];
					            totalsum[j] += duplicatenum;
					        }
					
					        fileOutput.close();
			    	
					    	filename = "records/cart/" + getUsername() + ".txt";			
					    	ofstream fileClear(filename.c_str(), ios::trunc);
					    	fileClear.close();
					    	
					    	ifstream fileStockRead;
							vector<string> lines;
							string line;
							
							// Read lines from the file
							fileStockRead.open("records/books.txt");
							while (getline(fileStockRead, line)) {
							    lines.push_back(line);
							}
							fileStockRead.close();
							
							// Process entries and update lines
							for (int i = 0; i < numEntries; i++) {
							    string ID, bookName, author, genre;
							    double price;
							    int stock, count = 0;
							
							    // Reopen the file at the beginning of each iteration
							    fileStockRead.open("records/books.txt");
							    
							    while (fileStockRead >> ID >> bookName >> price >> stock >> author >> genre) {
							        count++;
									
							        if (bookName == bookToSubtract[i]) {
							            lineNum[i] = count;
							            
							            int newstock = 0;
							            if(totalsum[i]==0){
							            	newstock = stock-stockToSubtract[i];
										}
										else{
											newstock = stock-totalsum[i];
										}
										int stockConvert  = (newstock), lineChange;
									    stringstream floatStream;
									    floatStream << stockConvert;
									    string stockConverted = floatStream.str();
									    
									    double priceConvert = price;
									    stringstream floatStream2;
									    floatStream2 << priceConvert;
									    string price = floatStream2.str();
									    
							            lines[lineNum[i] - 1] = ID + " " + bookToSubtract[i] + " " + price + " " + stockConverted + " " + author + " " + genre;
							            break;
							        }
							    }
							
							    fileStockRead.close();
							}

				    		//store the grand total fee (money earned) and add the penalty fee in the fee.txt file
						    ifstream feein("records/fee.txt");
						    if(!feein){
						    	cout<<"Error: File fee.txt couldn't found"<<endl;
						    	exit(0);
							}
							else{
								//save in variable
								feein >> FM.fee >> FM.fine >> FM.sum_penalty >> FM.money_earn_rent;
								feein.close();
								//add
								FM.money_earn_rent+=bookGrandTotal2;
								ofstream addearn("records/fee.txt",ios::trunc);
								//store back
								addearn << FM.fee << " " << FM.fine << " " << FM.sum_penalty << " " << FM.money_earn_rent;
								addearn.close();
								
							}					

							ofstream fileStockEdit("records/books.txt");
							for (size_t i = 0; i < lines.size(); ++i) {
						        fileStockEdit<< lines[i]<<"\n";
						    }
					    	fileStockEdit.close();
					    	
//							receipt(c,numEntries);
						}
						else if(checkoutConfirm == 'N'||checkoutConfirm == 'n'){
							cout<<"\nCancelling..."<<endl;
							sleep(1);
							system("cls");
							cart();
						}
						else{
							cout<<"\nInvalid Input! Please try again..."<<endl;
							sleep(2);
							system("cls");
							cart();
						}
				    cout << "-----------------------------------------------------------------------------"<<endl;
				}
				else{
			    	cout << "\nInvalid choice! Please re-enter...\n";
			        sleep(1);
			        system("cls");
			        cart();
				}
			}
		}
		
		void rented(){
			int choice;
			filename = "records/rented/" + getUsername() + ".txt";			
			ifstream inputFile(filename.c_str());
			string disName, disDateDue, line;
			int disQuantity, numEntries = 0, count = 1, overdueNo = -1;

			cout << "============================================================================="<<endl;
		    cout << "[0] Back \t\t\tRENTED BOOKS"<<endl;
		    cout << "============================================================================="<<endl;
		    cout << "User Menu > Rented Books"<<endl;
		    cout << "-----------------------------------------------------------------------------"<<endl;
		    cout <<left<<setw(5)<<"No."<<left<<setw(37)<<"Book Name"<<left<<setw(9)<<"Qty."<<left<<setw(16)<<"Due Date"<<left<<setw(12)<<"Status"<<endl;
		    
			/*need to remove if success
			while(getline(inputFile, line)) {
		        numEntries++;
		    }
		    inputFile.close();
		    
		    Book r[numEntries];			//no need coz link list infinite size
		    int* overdueNum = new int[numEntries];
		    int* overdueDays = new int[numEntries];
		    
		    inputFile.open(filename.c_str());*/
		    while(inputFile >> disName >> disQuantity >> disDateDue) {
		    	replace(disName.begin(), disName.end(), '%', ' ');
				//declaring variables for date time
		    	time_t currentTime = time(0);
				//tm is human readable format, localtime convert time_t to tm
    			tm* currentDate = localtime(&currentTime);
		    	//declaring due date to tm format, {0} is to initialize all values to 0
		    	tm dueDate = {};
				//variable use to separate duedate into day, month, year
			    istringstream ss(disDateDue);
			    string dayStr, monthStr, yearStr;
			    getline(ss, dayStr, '/');
			    getline(ss, monthStr, '/');
			    getline(ss, yearStr);
				//to convert string to int
			    int day, month, year;
			    istringstream(dayStr) >> day;
			    istringstream(monthStr) >> month;
			    istringstream(yearStr) >> year;
		    	//declaring datatype tm, {0} is to initialize all values to 0
		    	tm specificDate = {0};
				//setting the values of specific date
			    specificDate.tm_mday = day;
			    specificDate.tm_mon = month - 1;
			    specificDate.tm_year = year - 1900;
				rentedBooks.addBook(disName, disQuantity, disDateDue);
				// Now, you can list the rented books
				rentedBooks.listBooks();
		    	cout<<left<<setw(5)<<count++;
				cout<<left<<setw(37)<<disName<<" "<<left<<setw(6)<<disQuantity<<" "<<left<<setw(17)<<disDateDue;
				
				int daysDifference = currentDate->tm_mday - specificDate.tm_mday + (currentDate->tm_mon - specificDate.tm_mon) * 30 + (currentDate->tm_year - specificDate.tm_year) * 365;
				if (currentDate->tm_year == specificDate.tm_year && currentDate->tm_mon == specificDate.tm_mon && currentDate->tm_mday == specificDate.tm_mday) {
					cout << "\033[1;32mActive\033[0m" << endl;
				}
				else if (difftime(currentTime, mktime(&specificDate)) > 0) {
				    cout << "\033[1;31mOverdue\033[0m"<< endl;
				    overdueNum[overdueNo++] = count-1;
				    overdueDays[overdueNo] = daysDifference;
				}
				else{
				    cout << "\033[1;32mActive\033[0m" << endl;
				}
				//r[count-2].setdatarental(disName,disQuantity);
				
			
			}
			
			
			if(numEntries == 0){
				cout<<"\n\n\t\t\t     No books rented...\n\n"<<endl;
			}
			float fine = 0;//getoverduefee();
			cout << "-----------------------------------------------------------------------------"<<endl;
		    cout << "**DISCLAIMER"<<endl;
			cout << "  Select a line to return the specific book(s). Late returns are fined"<<endl;
			cout << "  RM "<<fixed<<setprecision(2)<<fine<<"/day for each overdue book."<<endl;
			cout << "-----------------------------------------------------------------------------"<<endl;
		    cout << "Enter your choice: ";
		    cin>>choice;
		    if(numEntries == 0 && choice != 0){
				cout<<"\nNo books rented! Please try again later..."<<endl;
		    	sleep(2);
				system("cls");
				rented();
			}
		    if(choice == 0){
		    	system("cls");
		        userMenu();
			} 
			else if(choice > 0 && choice <= numEntries){
				char confirmReturn;
				int overdue = 0;
				cout << "-----------------------------------------------------------------------------"<<endl;
			    for (int i = -1; i < overdueNo; ++i) {
			        if (overdueNum[i] == choice) {
			            cout<<"Overdue by "<<overdueDays[i+1]<<" day(s). Total pentaly fee is RM "<<fixed<<setprecision(2)<<overdueDays[i+1]*fine*r[choice-1].getBookQuantity()<<"."<<endl;
			            if(r[choice-1].getBookQuantity()>1){
				        	cout << "Pay penalty fee and return " << r[choice-1].getBookQuantity() << " copies of '" << r[choice-1].getBookName() << "'?" << endl;
						}
						else if(r[choice-1].getBookQuantity()==1){
							cout<<"Pay penalty fee and return "<<r[choice-1].getBookQuantity()<<" copy of '"<<r[choice-1].getBookName()<<"'?"<<endl;
						}
						overdue = 1;
			        }
		    	}
		    	if(overdue == 0){
		    		if(r[choice-1].getBookQuantity()>1){
			        	cout << "Return " << r[choice-1].getBookQuantity() << " copies of '" << r[choice-1].getBookName() << "'?" << endl;
					}
					else if(r[choice-1].getBookQuantity()==1){
						cout<<"Return "<<r[choice-1].getBookQuantity()<<" copy of '"<<r[choice-1].getBookName()<<"'?"<<endl;
					}
				}

				cout<<"\nConfirm action [Y/N]: ";
			    cin>>confirmReturn;
			    
				if(confirmReturn == 'Y' || confirmReturn == 'y'){
					ifstream fileStockRead;
					vector<string> lines;
					string line;
					
					// Read lines from the file
					fileStockRead.open("records/books.txt");
					while (getline(fileStockRead, line)) {
					    lines.push_back(line);
					}
					fileStockRead.close();
					
					// Process entries and update lines
				    string ID, bookName, author, genre, price;
				    int stock, newStock, count = 0;
				
				    // Reopen the file at the beginning of each iteration
				    fileStockRead.open("records/books.txt");
				    
				    while (fileStockRead >> ID >> bookName >> price >> stock >> author >> genre) {
				        count++;
				        replace(bookName.begin(), bookName.end(), '%', ' ');
				        
				        if (bookName == r[choice-1].getBookName()) {
				            int stockConvert = (stock+r[choice-1].getBookQuantity()), lineChange;
						    stringstream floatStream;
						    floatStream << stockConvert;
						    string newStock = floatStream.str();
						    
						    replace(bookName.begin(), bookName.end(), ' ', '%');
						    
				            lines[count-1] = ID + " " + bookName + " " + price + " " + newStock + " " + author + " " + genre;
				            break;
				        }
				    }
				
				    fileStockRead.close();								
	
					ofstream fileStockEdit("records/books.txt");
					for (size_t i = 0; i < lines.size(); ++i) {
				        fileStockEdit<< lines[i]<<"\n";
				    }
			    	fileStockEdit.close();
			    	
			    	filename = "records/rented/" + getUsername() + ".txt";
			    	ifstream fileInput(filename.c_str());
					vector<string> lines2;
					string line2;
					
					while (getline(fileInput, line2)) {
    					lines2.push_back(line2);
					}
					
					fileInput.close();
											
			    	ofstream fileOutput(filename.c_str());
				     for (size_t i = 0; i < lines2.size(); ++i) {
				        if (i != (choice - 1)) {
				            fileOutput << lines2[i] << '\n';
				        }
				    }

				    fileOutput.close();
				    
				    for (int i = -1; i < overdueNo; ++i) {
				    	if (overdueNum[i] == choice) {
				    		//use a temp variable to store the penalty fee
						    float temp_penaltyfee = overdueDays[i+1]*fine*r[choice-1].getBookQuantity();
						    
						    //add the penalty fee in the fee.txt file
						    ifstream feein("records/fee.txt");
						    if(!feein){
						    	cout<<"Error: File fee.txt couldn't found"<<endl;
						    	exit(0);
							}
							else{
								//save in variable
								feein >> FM.fee >> FM.fine >> FM.sum_penalty >>FM.money_earn_rent;
								feein.close();
								//add
								FM.sum_penalty+=temp_penaltyfee;
								ofstream addfee("records/fee.txt",ios::trunc);
								//store back
								addfee << FM.fee << " " << FM.fine << " " << FM.sum_penalty << " "<< FM.money_earn_rent;
								addfee.close();
								
							}
						
						}
					}
				    cout<<"\nReturning book(s)..."<<endl;
				    sleep(1);
					system("cls");
					rented();
				}
				else if(confirmReturn == 'N' || confirmReturn == 'n'){
					cout<<"\nCancelling..."<<endl;
				    sleep(1);
					system("cls");
					rented();
				}
				else{
					cout<<"\nInvalid choice! Please re-enter..."<<endl;
				    sleep(1);
					system("cls");
					rented();
				}
			}
			else{
				cout << "\nInvalid choice! Please re-enter...\n";
		        sleep(1);
		        system("cls");
		        rented();
			}
		    inputFile.close();
		    delete[] overdueNum;
		    delete[] overdueDays;
		}
};

//functions outside the Menus class are admin part (YS)
void Menus::addbook(){
	char choice;
	string yesno;
	int error;
	
	do{
		system("cls");
		fflush(stdin);
		cout << "=============================================================================================="<<endl;
		cout << "[0] Back \t\t\t\t  ADD BOOK"<<endl;
		cout << "=============================================================================================="<<endl;
		cout << "Admin Menu > Add Book "<<endl;
		cout << "----------------------------------------------------------------------------------------------"<<endl;
		cout << "The first letter of ID that corresponds to the genre:"<<endl;
		cout << left << setw(15) <<"\tM-Mystery" << setw(15) <<"N-Fiction"<< setw(15) <<"X-Non-fiction"<<setw(15) <<"C-Classics"<< setw(15) <<"A-Action"<<endl;
		cout << left << setw(15) <<"\tT-Thriller"<< setw(15) <<"R-Children"<< setw(15) <<"H-Horror"<< setw(15) <<"D-Dystopian"<< setw(15) <<"I-Historical"<<endl;
		cout << left << setw(15) <<"\tF-Fantasy" << setw(15) <<"L-Romance"<< setw(15) <<"P-Philosophy"<< setw(15) <<"E-Detective"<<endl;
		cout << "----------------------------------------------------------------------------------------------"<<endl;
		cout << "Please fill the information of new book."<<endl<<endl;
		fflush(stdin);
		
		//declare dma booklinkedlist obj
		BookLinkedList bl;
	    
		string id, name, author, genre, I, n, a, g;
		float price, p;
		int stock, s;
			
		//open file to check whether same or not
		ifstream readfile("records/books.txt");
		if(!readfile){
			cout<<"Error: Unable to open the file 'records/books.txt'\n"<<endl;
			exit(0);
		}
		else{
			cout << "Book ID (0 to go back): ";
			getline(cin,I);
			if(I=="0"){
				cout<<"Back to admin menu..."<<endl;
				sleep(1);
				adminMenu();
			}
			else if(!isValidID(I)){
				cout<<"~ Incorrect format!"<<endl;
				cout << "\nInvalid data! Please re-enter...\n";
				sleep(2);
				addbook();
			}
			
			
			while (readfile >> id >> name >> price >> stock >> author >> genre) {
		        bl.insert(id, name, price, stock, author, genre);
		        bl.sort();
			}
			readfile.close();
			    
			int numBooks = 0;
			Book* currentBook = bl.head;
		    while (currentBook != NULL) {
		        numBooks++;
		        currentBook = currentBook->next;
		    }
		    Book** booksArray = new Book*[numBooks];
		    currentBook = bl.head;
		    int index = 0;
		    while (currentBook != NULL) {
		        booksArray[index++] = currentBook;
		        currentBook = currentBook->next;
		    }
			
			for (int i = 0; i < numBooks - 1; i++) {
		        for (int j = 0; j < numBooks - i - 1; j++) {
		            if (booksArray[j]->getBookID() > booksArray[j + 1]->getBookID()) {
		                Book* temp = booksArray[j];
		                booksArray[j] = booksArray[j + 1];
		                booksArray[j + 1] = temp;
		            }
		        }
		    }
			string tempid = I;
			int first = 0;
		    int last = numBooks - 1;
			int mid;
			bool idExists = false;
			while (first <= last && !idExists) {
			    mid = (first + last) / 2;
			    string bookid = booksArray[mid]->getBookID();
			    
			    if(tempid==bookid){
			    	idExists = true;
			    
				}
				else if (bookid < tempid) {
				    first = mid + 1;
				} else {
				    last = mid - 1;
				}
			}
			if (idExists) {
			    cout << "\n=>The ID already exists. " << endl;
			    sleep(2);
			    addbook();
			}
			
			
			//move to beginning of file
//			readfile.clear();
//			readfile.seekg(0);
			fflush(stdin);
			
			cout << "Name    : ";
			getline(cin,n);
			if(n == "0"){
				cout<<"\n~ Incorrect format!"<<endl;
				cout << "\nInvalid data! Please re-enter...\n";
				sleep(2);
				addbook();
			} 
			else{
				//check whether has same book
				string tempname;
				tempname = n;
				transform(tempname.begin(), tempname.end(), tempname.begin(), ::tolower);
				for (int i = 0; i < numBooks - 1; i++) {
			        for (int j = 0; j < numBooks - i - 1; j++) {
			            if (booksArray[j]->getBookName() > booksArray[j + 1]->getBookName()) {
			                Book* temp = booksArray[j];
			                booksArray[j] = booksArray[j + 1];
			                booksArray[j + 1] = temp;
			            }
			        }
			    }
				
				int f = 0;
			    int l = numBooks - 1;
				int m;
				while (f <= l) {
				    m = (f + l) / 2;
				    string bookname = booksArray[m]->getBookName();
				    replace(bookname.begin(), bookname.end(), '%', ' ');
				    transform(bookname.begin(), bookname.end(), bookname.begin(), ::tolower);
				    if(tempname==bookname){
						cout<<"\nThere is a book same name with it"<<endl<<"Do you want to add stock instead of adding new book? [Y/N]: ";
						cin >> yesno;
						fflush(stdin);
						if(yesno =="Y"||yesno == "y"){
							int addstock;
							cout<<"\nEnter additional stock: ";
							cin >> addstock;
							
							saveextrastock(addstock, tempname);
							cout << "----------------------------------------------------------------------------------------------"<<endl;
							
							cout<<"Back to admin menu... "<<endl;
							sleep(1);
							adminMenu();
						}
						else if(yesno == "N" || yesno == "n"){
							cout<<endl;
							break;
						}
						else{
							cout <<"\nInvalid choice. Please re-enter..." << endl;
							sleep(1);
							system("cls");
							addbook();
						}
					} else if (bookname < tempname) {
				        f = m + 1;
				    } else {
				        l = m - 1;
				    }
				}
			}
			delete[] booksArray;
			
			cout << "Price   : RM ";
			cin >> p;
			if(p <= 0){
				cout<<"\n~ Price CANNOT be written as NEGATIVE / ZERO value!"<<endl;
				cout << "\nInvalid data! Please re-enter...\n";
				sleep(2);
				addbook();
			} 
			fflush(stdin);
			cout << "Stock   : ";
			cin >> s;
			if(s <= 0){
				cout<<"\n~ Stock CANNOT be written as NEGATIVE / ZERO value!"<<endl;
				cout << "\nInvalid data! Please re-enter...\n";
				sleep(2);
				addbook();
			}
			fflush(stdin);
			cout << "Author  : ";
			getline(cin,a);
			if(a == "0"){
				cout<<"~ Incorrect format!"<<endl;
				cout << "\nInvalid data! Please re-enter...\n";
				sleep(2);
				addbook();
			}
			fflush(stdin);
			
			genre = getGenre(id);
		}
			
			//add
			ofstream outfile("records/books.txt",ios::app);
			if(!outfile){
				cout<<"Error: Unable to open the file 'records/books.txt'\n"<<endl;
				exit(0);
			}
			else{
				replace(I.begin(), I.end(), ' ', '%');
				replace(a.begin(), a.end(), ' ', '%');
				replace(g.begin(), g.end(), ' ', '%');
				
				outfile <<I <<" "<< n <<" " <<fixed<<setprecision(2)<< p <<" " << s <<" " << a <<" " << g <<endl;
				outfile.close();
			}
			
		
		cout << ".............................................................................."<<endl;
		cout << "Do you want to add another book? [y/n]: ";
		cin >> choice;
	}while(choice=='y'||choice=='Y');
	cout<<"Back to admin menu..."<<endl;
	sleep(1);
	//back to admin menu
	adminMenu();
}

void saveextrastock(int additional, string tempname){
	bool found = false;
	fstream putinfile("records/books.txt", ios::in | ios::out);
	if (!putinfile) {
        cout << "Error: Unable to open the file 'books.txt'\n"<< endl;
        exit(0);
	}
	else{
		
		BookLinkedList bl;
	    
		string id, name, author, genre;
		float price;
		int stock;
		
		while (putinfile >> id >> name >> price >> stock >> author >> genre) {
            bl.insert(id, name, price, stock, author, genre);
        }
        
        putinfile.clear();
        putinfile.seekp(0, ios::beg);
		
                
        Book* book = bl.find(tempname, 0);
        if (book != NULL) {
            int currentstock = book->getBookStock();
            currentstock += additional;
            book->setStock(currentstock);
            found = true;
        }
        
        // Write the updated linked list back to the file
        Book* currentBook = bl.head;
        putinfile.seekp(0, ios::beg);
        while (currentBook != NULL) {
        	
        	string bookName = currentBook->getBookName();
            replace(bookName.begin(), bookName.end(), ' ', '%');
            string bookAuthor = currentBook->getBookAuthor();
            replace(bookAuthor.begin(), bookAuthor.end(), ' ', '%');
            string bookGenre = currentBook->getBookGenre();
            replace(bookGenre.begin(), bookGenre.end(), ' ', '%');
            
            putinfile << currentBook->getBookID() << " " << bookName << " "
                      << fixed << setprecision(2) << currentBook->getBookPrice() << " "
                      << currentBook->getBookStock() << " " << bookAuthor << " "
                      << bookGenre << endl;
            currentBook = currentBook->next;
        }

		if (found) {
		    cout << "Additional stock updated successfully!" << endl;
	    } else {
		    cout << "Book not found!" << endl;
	    }
    }

    putinfile.close();
 
    
}
bool isValidID(const string& id) {
	if (id.empty()) {
       return false; // An empty ID is not valid
    }
    return isalpha(id[0]);
}
//to know genre from id
string getGenre(const string& id) {
	if (id.empty()) {
		return "Invalid ID";
	}
		
	char firstChar = toupper(id[0]); // Convert to uppercase to handle both cases
		
	switch (firstChar) {
		case 'M': return "Mystery";break;
		case 'N': return "Fiction";break;
		case 'X': return "Non-fiction";break;
		case 'P': return "Philosophy";break;
		case 'C': return "Classics";break;
		case 'A': return "Action";break;
		case 'T': return "Thriller";break;
		case 'R': return "Children";break;
		case 'H': return "Horror";break;
		case 'D': return "Dystopian";break;
		case 'E': return "Detective";break;
		case 'I': return "Historical";break;
		case 'F': return "Fantasy";break;
		case 'L': return "Romance";break;
		default: return "Unknown"; 
	}
}

int main(){
	Menus client;
    int choice=0;
	
    while(true){
    
    	cout << "----------------------------------------------------------------------------------"<<endl;
        cout << "1. USER REGISTRATION\n";
        cout << "2. USER LOGIN\n";
        cout << "3. ADMIN LOGIN\n";
        cout << "4. EXIT\n";
    	cout << "----------------------------------------------------------------------------------"<<endl;
        cout << "Enter your choice: ";
        cin >> choice;
        
		switch (choice) 
        {
            case 1:
                system("cls");
                client.registration();
                break;
            case 2:
                system("cls");
                if (client.login("user")) 
                {
                    client.userMenu(); // Call the user menu after successful user login
                }
                break;
            case 3:
                system("cls");
                if (client.login("admin")) 
                {
                    client.adminMenu();
                }
                break;
            case 4:
                exit(0);
                return 0;
                break;
            default:
                cout << "\nInvalid choice! Please re-enter...\n";
                sleep(1);
                system("cls");
                main();
        }
    }
    
	return 0;
}
