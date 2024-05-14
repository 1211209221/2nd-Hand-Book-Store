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

    Users(string& uname, string& pwd) {
        username = uname;
        password = pwd;
        next = NULL;
    }
};

class UsersLinkedList {
public:
    Users* head;

public:
    UsersLinkedList() {
        head = NULL;
        loadUserFromFile();
    }

    void insert(string& uname, string& pwd) {
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

    bool search(string& uname, string& pwd) {
        Users* current = head;
        while (current != NULL) {
            if (current->username == uname && current->password == pwd) {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    bool checkUsername(string& uname) {
        Users* current = head;
        while (current != NULL) {
            if (current->username == uname) {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    void loadUserFromFile() {
        ifstream file("records/registered/user.txt");
        if (!file.is_open()) {
            cout << "Error: Unable to open the file 'records/registered/user.txt'" << endl;
            return;
        }

        string uname, pwd;
        while (file >> uname >> pwd) {
            insert(uname, pwd);
        }
        file.close();
    }

    void writeUserToFile() {
        ofstream file("records/registered/user.txt");
        if (!file.is_open()) {
            cout << "Error: Unable to open the file 'records/registered/user.txt'" << endl;
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

    void registration(UsersLinkedList& userlist) {
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

        fflush(stdin);
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
            userlist.writeUserToFile();
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

class Verify: public Info{
public:
    string acctype, filename;
    UsersLinkedList linkedList;

    bool login(string acctype){
        retries = 0;
        do{
            cout << "============================================================================="<<endl;
            if(acctype == "user"){
                cout << "\t\t\t         USER LOGIN"<<endl;
            }
            else if(acctype == "admin"){
                cout << "\t\t\t         ADMIN LOGIN"<<endl;
            }
            cout << "============================================================================="<<endl;
            filename = "records/registered/" + acctype + ".txt";
            ifstream file(filename.c_str());
            if (!file.is_open()) {
                cout << "Error: Unable to open the file '" << filename << "'\n";
                exit(1);
            }

            string uname, pwd;
            fflush(stdin);
            cout << "Enter username: ";
            getline(cin, uname);
            cout << "Enter password: ";
            cin >> pwd;
            
            username = uname;
            password = pwd;

            if (linkedList.search(username, password)) {
                cout << "\nLogin successful! Redirecting to "<<acctype<<" menu...\n";
                sleep(1);
                system("cls");
                return true; // Return true if login is successful
            } else {
                cout << "\nThe username or user password you have entered is invalid. "<<2-retries<<" more attempt(s)."<<"\n";
                sleep(2);
                system("cls");
            }
            retries++;
        } while(retries != 3);
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
	
	    string getBookGenre() {
	        return genre;
	    }
	    
	    int getSource() const {
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

		                break;
		            case 2:
		                system("cls");
		                
		                break;
		            case 5:
						system("cls");
						
						break;    
		            default:
		                cout << "\nInvalid choice! Please re-enter...\n";
		                sleep(1);
				    	system("cls");
				    	userMenu();
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
			cout <<left<<setw(5)<<"No."<<left<<setw(35)<<"Book Name"<<left<<setw(12)<<"Price"<<left<<setw(5)<<"Stock"<<endl;
			
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
		    file.close();
		
		    // Display all books from the linked list
		    bookList.displayAll();
		
		    cout << "-----------------------------------------------------------------------------" << endl;
		    cout << "Enter your choice: ";
		    cin >> choice;
		
		    if (choice == 0) {
		        system("cls");
		        userMenu();
		    } else if (choice > 0 && choice <= numEntries) {
		        do{
					rental(choice, bookList);
				}while(choice != 0);
		    } else {
		        cout << "\nInvalid choice! Please re-enter...\n";
		        sleep(1);
		        system("cls");
		        catalog();
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
		    cout << "User Menu > " << (bookList.head->getSource() == 0 ? "Catalog" : "Search Catalog") << " > " << selectedBook->getBookName() << endl;
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
		        if (bookList.head->getSource() == 0) {
		            catalog();
		        } else {
		            // searchCatalog();
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
		        cout << "User Menu > " << (bookList.head->getSource() == 0 ? "Catalog" : "Search Catalog") << " > " << bookList.head->getBookName() << " > Rental" << endl;
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
		
		        if (bookList.head->getBookStock() < quantity) {
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
		        total_fee = quantity * (bookList.head->getBookPrice() + (rentfee * duration));
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

		    cartList.display();
		    cout << "============================================================================="<<endl;
		    cout << "[1] Edit item"<<endl;
		    cout << "[2] Delete item"<<endl;
		    cout << "[3] Check out"<<endl;
		    cout << "-----------------------------------------------------------------------------"<<endl;
		    cout << "Enter your choice: ";
		    cin >> choice;
		    if(numEntries == 0){
		    	if(choice == 0){
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
				
			}
		}

};

int main(){
	Menus client;
	UsersLinkedList userlist;
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
                client.registration(userlist);
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
