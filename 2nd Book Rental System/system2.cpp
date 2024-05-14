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

struct FeeManagement{
	float fee;
	float fine;
	float sum_penalty;
	float money_earn_rent;
}FM;

using namespace std;
void changefee(int op);
void checkstatus(int num);
int status(string datedue);
void userMenu();
void adminMenu();
void catalog();
int main();

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


class Book{
	private:
		string id, name, genre, author;
		double price;
		int stock, quantity, source;
		float rentf, overduef, sumf, earnrent;
		
	public:
		//friend
		friend void changefee(int op);		
		
		Book* next;
		
		Book() {
	        next = NULL;
	    }
    
		//destructor for book
		~Book(){
			cout<<"\n**Operation done!**"<<endl;
		}
		void setrentfee(){
			float r,o,s,e;
			
			ifstream check("records/fee.txt");
			if(!check){
				cout <<"Error: Unable to open the file fee.txt"<<endl;
				exit(0);
			}
			else{
				check >> r >> o >> s >> e;
				check.close();
				
				ofstream fee("records/fee.txt",ios::trunc);
				if(!fee){
					cout <<"Error: Unable to open the file fee.txt"<<endl;
					exit(0);
				}
				else{
					//put in file
					fee << rentf <<" "<< o << " "<< s << " "<< e;
					fee.close();
				}
			}				
		}
		float getrentfee(){
			ifstream out("records/fee.txt");
			if(!out){
					cout <<"Error: Unable to open the file fee.txt"<<endl;
					exit(0);
			}
			out >> rentf >> overduef >> sumf >> earnrent;
			
			return rentf;
			out.close();

		}
		void setoverduefee(){
			float r,o,s,e;
			
			ifstream check("records/fee.txt");
			if(!check){
				cout <<"Error: Unable to open the file fee.txt"<<endl;
				exit(0);
			}
			else{
				check >> r >> o >> s >> e;
				check.close();
				
				ofstream fee("records/fee.txt",ios::trunc);
				if(!fee){
					cout <<"Error: Unable to open the file fee.txt"<<endl;
					exit(0);
				}
				else{
					//put in file
					fee << r <<" "<< overduef << " " << s << " " << e;
					fee.close();
				}
			}	
		}
		float getoverduefee(){
			ifstream out("records/fee.txt");
			if(!out){
					cout <<"Error: Unable to open the file fee.txt"<<endl;
					exit(0);
			}
			out >> rentf >> overduef >>sumf >>earnrent;
			out.close();
			
			return overduef;
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
		
		void setdatarental(string n,int q) {
			name = n;
			quantity = q;
			
			replace(name.begin(), name.end(), '%', ' ');
		}
		
		string getBookName(){
	        return name;
	    }
	
	    double getBookPrice(){
	        return price;
	    }
	    
	    int getBookStock(){
	        return stock;
	    }
	    
	    int getBookQuantity(){
	        return quantity;
	    }
	    
	    string getBookAuthor(){
	        return author;
	    }
	    
	    string getBookGenre(){
	        return genre;
	    }
	    
	    int getSource() const {
        	return source;
    	}
    	
		void setSource(int value) {
        source = value;
    	}
	    
		int add(){
			string I, n, a, g;
			float p;
			int s;
			
			//open file to check whether same or not
			ifstream readfile("records/books.txt");
			if(!readfile){
				cout<<"Error: Unable to open the file 'records/books.txt'\n"<<endl;
				exit(0);
			}
			else{
				cout<<"Book ID      : ";
				getline(cin,I);
				while(readfile >> id >> name >> price >> stock >> author >> genre){
					if(I==id){
							cout<<"\n=>The ID already exists. "<<endl;
							return 1;
						}
				}
				cout<<"Book Name    : ";
				getline(cin,n);
				cout<<"Author       : ";
				getline(cin,a);
				cout<<"Genre        : ";
				getline(cin,g);
				cout<<"Base Price   : RM ";
				cin>>p;
				cout<<"Stock        : ";
				cin>>s;
				
				if(n=="0"||a=="0"||g=="0"||p==0){
					cout<<"Please enter the normal input."<<endl;
					return 1;
				}
				//replace space between each string word to %
				replace(n.begin(), n.end(), ' ', '%');
				replace(a.begin(), a.end(), ' ', '%');
				replace(g.begin(), g.end(), ' ', '%');
				
				readfile.clear();
				readfile.seekg(0);
				
				while(readfile >> id >> name >> price >> stock >> author >> genre){
					if(I!=id){
						ofstream outfile("records/books.txt",ios::app);
						if(!outfile){
							cout<<"Error: Unable to open the file 'records/books.txt'\n"<<endl;
							exit(0);
						}
						else{
							outfile <<I <<" "<< n <<" " <<fixed<<setprecision(2)<< p <<" " << s <<" " << a <<" " << g <<endl;
							outfile.close();
							return 0;
						}
					}
				}
			}
			readfile.close();
			
		}
		void update(string code){
			ifstream readb("records/books.txt");
			ofstream temp("records/tempb.txt",ios::trunc);
			
			if(!readb){
				cout<<"Error: Unable to open the file 'records/books.txt'\n"<<endl;
				exit(0);
			}
			else{
				while(readb >> id >> name >> price >> stock >> author >> genre){
					if(code==id){
						replace(name.begin(), name.end(), '%', ' ');
						replace(author.begin(), author.end(), '%', ' ');
						replace(genre.begin(), genre.end(), '%', ' ');
						
						cout<<left<<setw(6)<<id<<setw(35)<<name<<setw(12)
						<<fixed<<setprecision(2)<<price<<setw(8)<<stock
						<<setw(30)<<author<<endl;
						fflush(stdin);
						cout << "-----------------------------------------------------------------------------"<<endl;
						cout << "Book Name: ";
						getline(cin,name);
						cout << "Price    : RM ";
						cin >> price;
						cout << "Stock    : ";
						cin >> stock;
						cin.ignore();
						cout <<"Author   : ";
						getline(cin,author);

						replace(name.begin(), name.end(), ' ', '%');
						replace(author.begin(), author.end(), ' ', '%');
						replace(genre.begin(), genre.end(), ' ', '%');
						//save in temp
						temp << id <<" "<< name <<" " <<fixed<<setprecision(2)<< price <<" " << stock <<" " << author <<" " << genre <<endl;
						
					}
					else{
						temp << id <<" "<< name <<" " <<fixed<<setprecision(2)<< price <<" " << stock <<" " << author <<" " << genre <<endl;
					}
					
				}
				readb.close();
				temp.close();
				
				ifstream source("records/tempb.txt");
				ofstream destination("records/books.txt", ios::trunc); 
				if(!source.is_open() || !destination.is_open()){
					cout << "\nError: Unable to open files tempb.txt or books.txt"<<endl;
					exit(0);
				}
				//copy all contents from source to destination
				destination << source.rdbuf();
				cout<<"\nLoading..."<<endl;
				sleep(2);
				source.close();
				destination.close();
				
			}
			
		}
		void deleteb(string code){
			ifstream readb("records/books.txt");
			ofstream temp("records/tempb.txt",ios::trunc);
			
			if(!readb){
				cout<<"Error: Unable to open the file 'records/books.txt'\n"<<endl;
				exit(0);
			}
			else{
				while(readb >> id >> name >> price >> stock >> author >> genre){
					if(code==id){
						cout<<left<<setw(6)<<id<<setw(35)<<name<<setw(12)
						<<fixed<<setprecision(2)<<price<<setw(8)<<stock
						<<setw(30)<<author<<endl;
						//no need do something					
					}
					else{
						temp << id <<" "<< name <<" " <<fixed<<setprecision(2)<< price <<" " << stock <<" " << author <<" " << genre <<endl;
					}
					
				}
				readb.close();
				temp.close();
				
				ifstream source("records/tempb.txt");
				ofstream destination("records/books.txt", ios::trunc); 
				if(!source.is_open() || !destination.is_open()){
					cout << "\nError: Unable to open files tempb.txt or books.txt"<<endl;
					exit(0);
				}
				//copy all contents from source to destination
				destination << source.rdbuf();
				cout<<"\nLoading..."<<endl;
				sleep(2);
				source.close();
				destination.close();

			}
			
		}
		
		void details(){
		    cout << left<<setw(15)<<"Book Title"<< ": "<< name <<endl;
		    cout << left<<setw(15)<<"Author"<< ": "<< author <<endl;
		    cout << left<<setw(15)<<"Genre"<< ": "<< genre <<endl;
		    cout << left<<setw(15)<<"Base Price"<< ": RM "<< price <<endl;
		    cout << left<<setw(15)<<"Current Stock"<< ": "<< stock <<endl;
		}
		
		void list(){
			cout<<left<<setw(35)<<name<<left<<"RM "<<setw(10)<<fixed<<setprecision(2)<<price<<left<<setw(5)<<stock<<endl;
		}
};

class BookLinkedList {
public:
    Book* head;

public:
    BookLinkedList() {
        head = NULL; // Initialize head to nullptr
    }

    void insert(string& uname, string& pwd) {
        Book* newBook = new Book;
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

    // Add other functions like display, delete, etc. if needed
};

class Cart{
	private:
		string bookName;
		int bookQuantity, bookDays;
		double bookPrice, bookTotalPrice;
		
	public:
		void setdata(string n, double p, int q, int d) {
			bookName = n;
			bookPrice = p;
			bookQuantity = q;
			bookDays = d;
			ifstream fee("records/fee.txt");
			if(!fee){
				cout<<"Error: Unable to open the file fee.txt\n"<<endl;
				exit(0);
			}
			fee >> FM.fee >> FM.fine;
			bookTotalPrice = (p+(d*FM.fee))*q;
			
			replace(bookName.begin(), bookName.end(), '%', ' ');
		}
		string getBookName(){
	        return bookName;
	    }
	
	    double getBookPrice(){
	        return bookPrice;
	    }
	    
	    int getBookQuantity(){
	        return bookQuantity;
	    }
	    
	    int getBookDays(){
	        return bookDays;
	    }
	    
	    double getBookTotalPrice(){
	        return bookTotalPrice;
	    }
	    
		void list(){
			cout <<left<<setw(38)<<bookName<<left<<setw(8)<<bookDays<<left<<setw(8)<<bookQuantity<<left<<"RM "<<setw(10)<<fixed<<setprecision(2)<<bookTotalPrice<<endl;
		}
		
		void listReceipt(ostream& fileOutput = cout){
			fileOutput <<left<<setw(37)<<bookName<<left<<setw(9)<<bookQuantity<<left<<"RM "<<setw(10)<<fixed<<setprecision(2)<<bookTotalPrice<<endl;
			
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
						viewReceipt();
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
			system("cls");
			while (true) 
		    {
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
		        		searchbook();
		        		break;
		        	case 3:
		        		fee();
		        		break;
		        	case 4:
		        		checkrentalrecord();
		        		break;
		            default:
		                cout << "\nInvalid choice! Please re-enter...\n";
		                sleep(1);
				    	adminMenu();
		        }
		    }
		}
		
		void catalog(){
		    string id, name, author, genre, line;
		    double price;
		    int choice = 0, choice2, stock, numEntries, i = 0;
		    
		    cout << "============================================================================="<<endl;
		    cout << "[0] Back \t\t\t  CATALOG"<<endl;
		    cout << "============================================================================="<<endl;
		    cout << "User Menu > Catalog"<<endl;
		    cout << "-----------------------------------------------------------------------------"<<endl;
		

			ifstream countFile("records/books.txt");
			if (countFile.is_open()) {
			    while (getline(countFile, line)) {
			        numEntries++;
			    }
			    countFile.close();
			} else {
			    cout << "Error: Unable to open the file 'records/books.txt'\n";
			    exit(0);
			}
		    countFile.close();
		
		    // Dynamically allocate an array of Book objects
		    Book* book = new Book[numEntries];
		
		    cout <<left<<setw(5)<<"No."<<left<<setw(35)<<"Book Name"<<left<<setw(12)<<"Price"<<left<<setw(5)<<"Stock"<<endl;
		    ifstream file("records/books.txt");
		    //Set Book attributes
		    while(file >> id >> name >> price >> stock >> author >> genre){
		        book[i].setdata(name, price, stock, author, genre);
		        book[i].setSource(0);
		        i++;
		    }
		    //List Books
		    for(int i=0; i<numEntries; i++){
		        cout<<left<<setw(5)<<i+1;
		        book[i].list();
		    }
		
		    cout << "-----------------------------------------------------------------------------"<<endl;
		    cout << "Enter your choice: ";
		    cin >> choice;
		    if(choice == 0){
		    	system("cls");
		        userMenu();
			}
			else if(choice > 0 && choice <= numEntries){
				do{
					rental(choice,book);
				}while(choice != 0);
			}
		    else{
		    	cout << "\nInvalid choice! Please re-enter...\n";
		        sleep(1);
		        system("cls");
		        catalog();
			}
		    // Release the dynamically allocated memory
		    delete[] book;
		
		    file.close();
		}
		
		void rental(int choice, Book* book){
			int quantity, duration;
			int choice2 = 0; 
			system("cls");
			cout << "============================================================================="<<endl;
		    cout << "[0] Back \t\t\t   RENT"<<endl;
		    cout << "============================================================================="<<endl;
		    cout << "User Menu > " << (book[choice - 1].getSource() == 0 ? "Catalog" : "Search Catalog") << " > " << book[choice - 1].getBookName() << endl;
		    cout << "-----------------------------------------------------------------------------"<<endl;
		    book[choice-1].details();
		    cout << "-----------------------------------------------------------------------------"<<endl;
		    cout << "[1] Start renting"<<endl;
		    cout << "-----------------------------------------------------------------------------"<<endl;
		    cout << "Enter your choice: ";
		    cin >> choice2;
		    if (choice2 == 0) {
            system("cls");
            	if (book[choice - 1].getSource() == 0) {
                	catalog();
            } 	else {
                	searchCatalog();
            	}
            }
			else if(choice2 == 1){
				char confirm;
				double total_fee;
				float rentfee = getrentfee();
				system("cls");
				cout << "============================================================================="<<endl;
			    cout << "[0] Back \t\t\t   RENT"<<endl;
			    cout << "============================================================================="<<endl;
			    cout << "User Menu > " << (book[choice - 1].getSource() == 0 ? "Catalog" : "Search Catalog") << " > " << book[choice - 1].getBookName() << " > Rental" << endl;
			    cout << "-----------------------------------------------------------------------------"<<endl;
			    book[choice-1].details();
			    cout << "-----------------------------------------------------------------------------"<<endl;
			    cout << "**DISCLAIMER"<<endl;
			    cout << "  Total fee is calculated based on the base price and additional\n  charges."<<endl;
			    cout << "  Additional charges are RM "<<fixed<<setprecision(2)<<rentfee<<"/day for each book rented."<<endl;
			    cout << "-----------------------------------------------------------------------------"<<endl;
		    	cout <<left<<setw(19)<<"Quantity"<<": ";
		    	cin >>quantity;
		    	
		    	if(book[choice-1].getBookStock() < quantity){
		    		cout<<"\nNot enough stock! Please try again..."<<endl;
		    		sleep(2);
		    		system("cls");
		    		rental(choice, book);
				}
		    	
		    	
		    	cout <<left<<setw(19)<<"Rent duration"<<": ";
		    	cin >>duration;
		    	if(quantity == 0 || duration== 0 ){
		    		cout<<"\nQuantity and Duration cannot be 0! Please re-enter..."<<endl;
		    		sleep(2);
		    		system("cls");
		    		rental(choice, book);
				}
		    	total_fee = quantity*(book[choice-1].getBookPrice() + (rentfee*duration));
		    	cout <<left<<setw(19)<<"Total fee"<<": RM "<<total_fee<<endl;
		    	cout <<left<<setw(19)<<"Add to cart? [Y/N] : ";
		    	cin >>confirm;	
		    	if(confirm == 'Y' || confirm == 'y'){
		    		cout << "\nAdding to cart...\n";
		        	sleep(1);
		        	filename = "records/cart/" + getUsername() + ".txt";
					ofstream outFile(filename.c_str(), ios::app);
				    string rentedBookName = book[choice-1].getBookName();
					encode(rentedBookName);
					outFile << rentedBookName<<" "<< book[choice-1].getBookPrice()<<" "<< quantity<<" "<< duration<<"\n";
				    outFile.close();
				}
				else if(confirm == 'N' || confirm == 'n'){
					cout << "\nCancelling...\n";
					sleep(1);
				}
				else{
					cout << "\nInvalid choice! Please try again...\n";
		        	sleep(1);
				}
				rental(choice,book);
			}
			else{
				cout << "\nInvalid choice! Please re-enter...\n";
		        sleep(1);
		        rental(choice,book);
			}
		}
		
		void cart(){
			string bookName, line;
			int bookQuantity, bookDuration, bookFinalQuantity = 0,numEntries = 0, choice, i = 0;
			double bookPrice, bookTotalPrice, bookGrandTotal = 0.00;
			filename = "records/cart/" + getUsername() + ".txt";
			
			ifstream countFile(filename.c_str());
			if (countFile.is_open()) {
			    while (getline(countFile, line)) {
			        numEntries++;
			    }
			    countFile.close();
			}
		    countFile.close();
		    
		    Cart* c = new Cart[numEntries];
		    
			cout << "============================================================================="<<endl;
		    cout << "[0] Back \t\t\t    CART"<<endl;
		    cout << "============================================================================="<<endl;
		    cout << "User Menu > Cart"<<endl;
		    cout << "-----------------------------------------------------------------------------"<<endl;
		    cout <<left<<setw(5)<<"No."<<left<<setw(37)<<"Book Name"<<left<<setw(8)<<"Days"<<left<<setw(9)<<"Qty."<<left<<setw(12)<<fixed<<setprecision(2)<<"Total"<<endl;
			ifstream file(filename.c_str());
			while(file >> bookName >> bookPrice >> bookQuantity >> bookDuration){
				c[i].setdata(bookName, bookPrice, bookQuantity, bookDuration);
		        i++;
			}
			
			if(numEntries == 0){
				cout<<"\n\n\t\t\tIt seems your cart is empty...\n\n"<<endl;
			}
			
			for(int i=0; i<numEntries; i++){
		        cout<<left<<setw(5)<<i+1;
		        c[i].list();
		        bookGrandTotal += c[i].getBookTotalPrice();
		        bookFinalQuantity += c[i].getBookQuantity();
		    }
		    cout << "-----------------------------------------------------------------------------"<<endl;
		    cout << left<<setw(51)<<"Grand Total: "<<left<<setw(8)<<bookFinalQuantity<<left<<"RM "<<setw(10)<<setprecision(2)<<bookGrandTotal<<endl;
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
		    }
		    else{
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
				    for(int i=0; i<numEntries; i++){
				        cout<<left<<setw(5)<<i+1;
				        c[i].list();
				        bookGrandTotal += c[i].getBookTotalPrice();
				        bookFinalQuantity += c[i].getBookQuantity();
				    }
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
						float rentfee = getrentfee();
						
						cout << "Selected '" << c[lineSelect - 1].getBookName() << "'.\n" << endl;
						cout << "New Quantity       : ";
						cin >> quantityNew;
				
						ifstream fileStockRead("records/books.txt");
							    
					    while (fileStockRead >> ID >> bookName2 >> price >> stock >> author >> genre) {
					    	replace(bookName2.begin(), bookName2.end(), '%', ' ');	
					        if (bookName2 == c[lineSelect - 1].getBookName()) {
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
				    	if( quantityNew == 0 || durationNew == 0 ){
				    		cout<<"\nQuantity and Duration cannot be 0! Please re-enter..."<<endl;
				    		sleep(2);
							system("cls");
							cart();
						}
						else{
							
						}
						cout << "-----------------------------------------------------------------------------"<<endl;
						cout<<"New total will be RM "<<quantityNew*(c[lineSelect-1].getBookPrice() + (rentfee*durationNew))<<"."<<endl;
						
						
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
					    float priceConvert = c[lineSelect-1].getBookPrice();
					    stringstream floatStream;
					    floatStream << priceConvert;
					    string bookPrice = floatStream.str();
    					string bookName = c[lineSelect-1].getBookName();
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
				    for(int i=0; i<numEntries; i++){
				        cout<<left<<setw(5)<<i+1;
				        c[i].list();
				        bookGrandTotal += c[i].getBookTotalPrice();
				        bookFinalQuantity += c[i].getBookQuantity();
				    }
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
						cout << "Selected '" << c[lineSelect - 1].getBookName() << "'." << endl;
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
					char checkoutConfirm;
					int bookFinalQuantity2 = 0;
					double bookGrandTotal2 = 0;
			        system("cls");
			        cout << "============================================================================="<<endl;
				    cout << "[0] Back  \t\t\t  CHECK OUT"<<endl;
				    cout << "============================================================================="<<endl;
				    cout << "User Menu > Cart > Check Out"<<endl;
				    cout << "-----------------------------------------------------------------------------"<<endl;
				    cout <<left<<setw(5)<<"No."<<left<<setw(37)<<"Book Name"<<left<<setw(8)<<"Days"<<left<<setw(9)<<"Qty."<<left<<setw(12)<<"Total"<<endl;
				    for(int i=0; i<numEntries; i++){
				        cout<<left<<setw(5)<<i+1;
				        c[i].list();
				        bookGrandTotal += c[i].getBookTotalPrice();
				        bookFinalQuantity += c[i].getBookQuantity();
				        bookGrandTotal2 += c[i].getBookTotalPrice();
				        bookFinalQuantity2 += c[i].getBookQuantity();
				    }
				    cout << "-----------------------------------------------------------------------------"<<endl;
				    cout << left<<setw(51)<<"Grand Total: "<<left<<setw(8)<<bookFinalQuantity2<<left<<"RM "<<setw(10)<<setprecision(2)<<bookGrandTotal2<<endl;
		    		cout << "============================================================================="<<endl;
				    cout << "Confirm check out? [Y/N]: ";
				    cin>>checkoutConfirm;
						if(checkoutConfirm == 'Y'||checkoutConfirm == 'y'){
							cout<<"\nGenerating receipt..."<<endl;
							sleep(2);
							system("cls");
							
							string bookToSubtract[numEntries];
							int stockToSubtract[numEntries], lineNum[numEntries]; 
							
							filename = "records/rented/" + getUsername() + ".txt";			
					    	ofstream fileOutput(filename.c_str(), ios::app);
					    	
					    	for(int i=0; i<numEntries; i++){
					    		time_t currentTime = time(0);
		    					tm currentDate = *localtime(&currentTime);
		    					
		    					int daysToAdd = c[i].getBookDays();
					    		string rentedBookName, rentedBookDue;
					    		currentDate.tm_mday += daysToAdd;
					    		mktime(&currentDate);
					    		stringstream ss;
							    ss << setw(2) << setfill('0') << currentDate.tm_mday << "/" << setw(2) << setfill('0') << currentDate.tm_mon + 1 << "/" << setw(4) << setfill('0') << currentDate.tm_year + 1900<<endl;
								rentedBookDue = ss.str();
					    		
					    		rentedBookName = c[i].getBookName();
								encode(rentedBookName);
								bookToSubtract[i] = rentedBookName;
								stockToSubtract[i] = c[i].getBookQuantity();
								
						        fileOutput<<rentedBookName<<" "<<c[i].getBookQuantity()<<" "<< rentedBookDue;
						    }
							
							int totalsum[numEntries];
							
						    for(int j=0; j<numEntries; j++){
								totalsum[j]=0;
								int duplicatenum = 0;
								
								for(int k=0; k<numEntries; k++){
									if(bookToSubtract[j] == bookToSubtract[k])
									{
										if(j!=k){
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
					    	
							receipt(c,numEntries);
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
		    delete[] c;
		}
		
		void receipt(Cart* c, int numEntries){
			double bookGrandTotal;
			int bookFinalQuantity;
			string choice;
			
			time_t currentTime = time(0);
		    tm currentDate = *localtime(&currentTime);
		
			cout << "============================================================================="<<endl;
		    cout << "         \t\t\t  RECEIPT"<<endl;
		    cout << "=============================================================================\n\n"<<endl;
		    cout<<"\t-------------------------------------------------------------"<<endl;
            cout<<"\t\t      ~SUNRISE 2ND BOOK RENTAL SYSTEM~"<<endl;
            cout<<"\t\t        NO. 44, JALAN DESA MELUR 4/1,"<<endl;
            cout<<"\t\t           TAMAN BANDAR CONNAUGHT,"<<endl;
            cout<<"\t\t                56000 CHERAS,"<<endl;
            cout<<"\t\t           KUALA LUMPUR, MALAYSIA"<<endl;
			cout<<"\t=============================================================\n\t";
            cout<<setfill('0') << setw(2) << currentDate.tm_mday << "/"<<setfill('0') << right << setw(2) << (currentDate.tm_mon + 1) << "/" << setfill(' ') << left << setw(47) << (currentDate.tm_year + 1900);
            cout<<setfill('0') << setw(2) << currentDate.tm_hour << ":"<<setfill('0') << right << setw(2) << currentDate.tm_min;
            if (currentDate.tm_hour < 12) {
				cout << " AM";
		    } else {
		        cout << " PM";
		    }		    
		    
		    DIR *dp;
			int rental_ID = -2;
			struct dirent *ep;     
			dp = opendir ("records/receipts/");
			
			if (dp != NULL)
			{
			while (ep = readdir (dp))
			rental_ID++;
			
			(void) closedir (dp);
			}
			else
			perror ("Couldn't open the directory");
			
		    rental_ID++;

		    stringstream idStream;
		    idStream <<"R" << setw(3) << setfill('0') << rental_ID;
		    string rental_string = idStream.str();

		    string filename = "records/receipts/" + rental_string + ".txt";
		    ofstream ReceiptWrite(filename.c_str());
		
		    ReceiptWrite << getUsername()<<endl;
		    ReceiptWrite << rental_string<<endl;
		    ReceiptWrite << setfill('0') << setw(2) << currentDate.tm_mday << "/"<<setfill('0') << right << setw(2) << (currentDate.tm_mon + 1) << "/" << setfill(' ') << left << setw(4) << (currentDate.tm_year + 1900)<<endl;
		    ReceiptWrite << setfill('0') << setw(2) << currentDate.tm_hour << ":"<<setfill('0') << right << setw(2) << currentDate.tm_min;
			if (currentDate.tm_hour < 12) {
					ReceiptWrite << "%AM"<<endl;
			    } else {
			        ReceiptWrite << "%PM"<<endl;
		    }	
			ReceiptWrite << fixed << setprecision(2) << getrentfee() <<endl;
		    for(int i=0; i<numEntries; i++){
		    	string ReceiptBookName = c[i].getBookName();
		    	replace(ReceiptBookName.begin(), ReceiptBookName.end(), ' ', '%');
				ReceiptWrite << ReceiptBookName << " " << c[i].getBookQuantity() << " " << c[i].getBookPrice() << " "<< c[i].getBookDays() << endl;		    }
		    
			ReceiptWrite.close();
		    
			cout<<"\n\t-------------------------------------------------------------"<<endl;
			cout<<"\tRENTAL ID: "<<rental_string<<endl;
			cout<<"\n\t"<<setfill(' ')<<left<<setw(5)<<"No."<<left<<setw(37)<<"Book Name"<<left<<setw(9)<<"Qty."<<left<<setw(16)<<"Price\n"<<endl;
			for(int i=0; i<numEntries; i++){
		        cout<<"\t"<<left<<setw(5)<<i+1;
				c[i].listReceipt();
				bookGrandTotal += c[i].getBookTotalPrice();
				bookFinalQuantity += c[i].getBookQuantity();
		    }
			cout<<"\t-------------------------------------------------------------"<<endl;
			cout <<"\t"<< left<<setw(42)<<"Grand Total: "<<left<<setw(9)<<bookFinalQuantity<<left<<"RM "<<setw(10)<<fixed<<setprecision(2)<<bookGrandTotal<<endl;
			cout<<"\t============================================================="<<endl;
			cout<<"\n\t\t           THANK YOU FOR RENTING!\n"<<endl;
			cout<<"\t=============================================================\n\n"<<endl;
			cout<< "-----------------------------------------------------------------------------"<<endl;
			cout<< "Press any key to return: ";
			cin>>choice;
			system("cls");
			cart();
		}
		
		void viewReceipt() {
		    int choice;
		    // Displaying the receipt menu
		    cout << "=============================================================================" << endl;
		    cout << "[0] Back \t\t\tVIEW RECEIPTS" << endl;
		    cout << "=============================================================================" << endl;
		    cout << "User Menu > View Receipts" << endl;
		    cout << "-----------------------------------------------------------------------------" << endl;
		    cout << left << setw(5) << "No." << left << setw(20) << "Rental ID" << left << setw(9) << "Qty." << left << setw(19) << "Price(RM)" << left << setw(12) << "Date" << endl;
			
		    int rental_ID, numEntries = 1, numReceipts = 0;
		    vector<string> foundReceipts;
		    ifstream ReceiptRead;
		
		    do {
		        ReceiptRead.close();
		        string receiptUsername, receiptID, receiptDate, receiptTime, receiptFee, R;
		        rental_ID++;
		
		        stringstream idStream;
		        idStream << "R" << setw(3) << setfill('0') << rental_ID;
		        string ReceiptName = idStream.str();
		
		        string filename = "records/receipts/" + ReceiptName + ".txt";
		        ReceiptRead.open(filename.c_str());
		
		        if (ReceiptRead.is_open()) {
		            // Read username, rental ID, date, and time
		            ReceiptRead >> receiptUsername >> receiptID >> receiptDate >> receiptTime >> receiptFee;
		
		            if (receiptUsername == getUsername()) {
		                int qty;
		                double price;
		                // Count variables for display
		                int totalQty = 0, days = 0;
		                double totalPrice = 0.0;
		                
//					    istringstream iss(ReceiptDays);
//					    int DaysConverted;
//					    iss >> DaysConverted;

					    istringstream iss(receiptFee);
					    float FeeConverted;
					    iss >> FeeConverted;
		
		                // Read and display book information
		                while (ReceiptRead >> R >> qty >> price >> days) {
		                    // Update counts
		                    totalQty += qty;
		                    totalPrice += (qty * (price + (FeeConverted * days)));
		                }
		                cout << left << setw(5) << numEntries << left << setw(20) << receiptID << right << setw(3) << totalQty << right << setw(9) << "RM " << left << setw(11) << setfill(' ') << fixed << setprecision(2) << totalPrice << "  " << left << setw(30) << setfill(' ') << receiptDate << endl;
		
		                numEntries++;
		                numReceipts = numEntries;
		                foundReceipts.push_back(filename);
		            }
		        } else {
		            // Unable to open the file, break out of the loop
		            break;
		        }
		    } while (true);
		
		    ReceiptRead.close();
		    
		    if(numReceipts == 0){
				cout<<"\n\n\t\t\t     No receipts...\n\n"<<endl;
			}
			// Displaying the available receipts
		    cout << "-----------------------------------------------------------------------------" << endl;
		    cout << "Enter your choice: ";
		    cin >> choice;
		    if (choice == 0) {
		        system("cls");
		        userMenu();
		    } else if (choice > 0 && choice <= numEntries) {
		        // Implement logic to display the selected receipt with the corresponding rental ID
		        if (choice <= foundReceipts.size()) {
		            system("cls");
		            string selectedReceipt = foundReceipts[choice - 1];
		            ifstream SelectedReceipt(selectedReceipt.c_str());
		
		            if (SelectedReceipt.is_open()) {
		                // Variables to store information from the selected receipt
		                string selectedUsername, selectedID, selectedDate, selectedTime, selectedFee, selectedDays, R;
		                int selectedQty;
		                double selectedPrice;
		                int totalqty = 0;
		                double totalprice = 0.0;
		
		                // Read and store information
		                SelectedReceipt >> selectedUsername >> selectedID >> selectedDate >> selectedTime >> selectedFee;
		                istringstream iss(selectedFee);
    					float FeeConverted;
    					iss >> FeeConverted;
		                cout << "============================================================================="<<endl;
		                cout << "         \t\t\t  RECEIPT"<<endl;
		                cout << "=============================================================================\n\n"<<endl;
		                cout<<"\t-------------------------------------------------------------"<<endl;
		                cout<<"\t\t      ~SUNRISE 2ND BOOK RENTAL SYSTEM~"<<endl;
		                cout<<"\t\t        NO. 44, JALAN DESA MELUR 4/1,"<<endl;
		                cout<<"\t\t           TAMAN BANDAR CONNAUGHT,"<<endl;
		                cout<<"\t\t                56000 CHERAS,"<<endl;
		                cout<<"\t\t           KUALA LUMPUR, MALAYSIA"<<endl;
		                cout<<"\t=============================================================\n\t";
		                cout<< setw(2) << selectedDate;
		                replace(selectedTime.begin(), selectedTime.end(), '%', ' ');
		                cout<< right <<setw(51) << selectedTime;
		                cout<<"\n\t-------------------------------------------------------------"<<endl;
		                cout<<"\tRENTAL ID: "<< selectedID <<endl;
		                cout<<"\n\t"<<setfill(' ')<<left<<setw(5)<<"No."<<left<<setw(37)<<"Book Name"<<left<<setw(9)<<"Qty."<<left<<setw(16)<<"Price\n"<<endl;
		
		                // Display book information
		                int numBooks = 1;  // Count the number of books in the receipt
		                while (SelectedReceipt >> R >> selectedQty >> selectedPrice >> selectedDays) {
		                    replace(R.begin(), R.end(), '%', ' ');
		                    stringstream ss(selectedDays);
						    int DaysConverted;
						    ss >> DaysConverted;
					    
		                    cout << "\t" <<left<<setw(5)<< numBooks <<left<<setw(37)<< R << right << setw(3) << selectedQty << right << setw(9) << "RM " << selectedQty*(selectedPrice+(FeeConverted*DaysConverted)) << endl;
		                    numBooks++;
		                    totalqty += selectedQty;
		                    totalprice += (selectedQty*(selectedPrice+(FeeConverted*DaysConverted)));
		                }
		
		                cout<<"\t-------------------------------------------------------------"<<endl;
		                cout <<"\t"<< left<<setw(42)<<"Grand Total: "<<right<<setw(3)<<totalqty<<right<<setw(9)<<"RM "<<fixed<<setprecision(2)<<totalprice<<endl;
		                cout<<"\t============================================================="<<endl;
		                cout<<"\n\t\t           THANK YOU FOR RENTING!\n"<<endl;
		                cout<<"\t=============================================================\n\n"<<endl;
		                cout<< "-----------------------------------------------------------------------------"<<endl;
		                cout<< "Press any key to return: ";
		                cin>>choice;
		                system("cls");
		                viewReceipt();
		
		                SelectedReceipt.close();
		            } else {
		                cout << "Unable to open the selected receipt file." << endl;
		            }
		        } else {
		            cout << "\nInvalid choice! Please re-enter...\n";
		            sleep(1);
		            system("cls");
		            viewReceipt();
		        }
		    }
		    else {
	            cout << "\nInvalid choice! Please re-enter...\n";
	            sleep(1);
	            system("cls");
	            viewReceipt();
	        }
		}

		void rented(){
			int choice;
			string disName, disDateDue, line;
			int disQuantity, numEntries = 0, count = 1, overdueNo = -1;
			filename = "records/rented/" + getUsername() + ".txt";			
			ifstream inputFile(filename.c_str());
			
			cout << "============================================================================="<<endl;
		    cout << "[0] Back \t\t\tRENTED BOOKS"<<endl;
		    cout << "============================================================================="<<endl;
		    cout << "User Menu > Rented Books"<<endl;
		    cout << "-----------------------------------------------------------------------------"<<endl;
		    cout <<left<<setw(5)<<"No."<<left<<setw(37)<<"Book Name"<<left<<setw(9)<<"Qty."<<left<<setw(16)<<"Due Date"<<left<<setw(12)<<"Status"<<endl;
		    while(getline(inputFile, line)) {
		        numEntries++;
		    }
		    inputFile.close();
		    
		    Book r[numEntries];
		    int* overdueNum = new int[numEntries];
		    int* overdueDays = new int[numEntries];
		    
		    inputFile.open(filename.c_str());
		    while(inputFile >> disName >> disQuantity >> disDateDue){
		    	replace(disName.begin(), disName.end(), '%', ' ');
									    
		    	time_t currentTime = time(0);
    			tm* currentDate = localtime(&currentTime);
		    	
		    	tm dueDate = {};
			    istringstream ss(disDateDue);
			    
			    string dayStr, monthStr, yearStr;
			    getline(ss, dayStr, '/');
			    getline(ss, monthStr, '/');
			    getline(ss, yearStr);

			    int day, month, year;
			    istringstream(dayStr) >> day;
			    istringstream(monthStr) >> month;
			    istringstream(yearStr) >> year;
		    	
		    	tm specificDate = {0};
			    specificDate.tm_mday = day;
			    specificDate.tm_mon = month - 1;
			    specificDate.tm_year = year - 1900;
        	
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
				r[count-2].setdatarental(disName,disQuantity);
				
			
			}
			
			
			if(numEntries == 0){
				cout<<"\n\n\t\t\t     No books rented...\n\n"<<endl;
			}
			float fine = getoverduefee();
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
		
		void searchCatalog() 
		{
			string id, name, author, genre, line;
    		double price;
    		int stock, count, searchoption;
    		string keyword;
    		// Displaying the search catalog menu
    		cout << "============================================================================="<<endl;
    		cout << "[0] Back \t\t      SEARCH CATALOG" << endl;
    		cout << "============================================================================="<<endl;
    		cout << "User Menu > Search Catalog" << endl;
    		cout << "-----------------------------------------------------------------------------" << endl;
			cout << "[1] Search by keyword" << endl;
			cout << "[2] Search by genre" << endl;
			cout << "-----------------------------------------------------------------------------" << endl;
			cout << "Enter your choice: ";
			cin >> searchoption;
			// Handle user choices
			if(searchoption==0)
			{
				system("cls");
				userMenu();
			}
			else if(searchoption==1)
			{
				// Keyword search
				system("cls");
				cout << "=============================================================================" << endl;
    			cout << "[0] Back \t\t\tSEARCH CATALOG" << endl;
    			cout << "=============================================================================" << endl;
    			cout << "User Menu > Search Catalog > Search by keyword" << endl;
    			cout << "-----------------------------------------------------------------------------"<<endl;
    			cout << "Enter keyword: ";
    			cin.ignore();
    			getline(cin, keyword);
    			encode(keyword);
    			cout << "-----------------------------------------------------------------------------"<<endl;
    		
    			if (keyword == "0") 
        		{
            		system("cls");
            		searchCatalog();  
        		}

    			ifstream countFile("records/books.txt");
    			if (countFile.is_open()) 
				{
			    	while (getline(countFile, line)) 
					{
			        	count++;
			    	}
			    countFile.close();
				} 
				else 
				{
				    cout << "Error: Unable to open the file 'records/books.txt'\n";
				    exit(0);
				}
		    	countFile.close();
				string decodedkeyword = keyword;
        		replace(decodedkeyword.begin(), decodedkeyword.end(), '%', ' ');
    			cout << "Showing results for '"<< decodedkeyword <<"'\n"<<endl;
    			cout << left << setw(5) << "No." << left << setw(35) << "Book Name" << left << setw(12) << "Price" << left << setw(8) << "Stock" << endl;
			
				Book* searchResults = new Book[count];
				int currentResult = 0;
				ifstream file("records/books.txt");
    			while (file >> id >> name >> price >> stock >> author >> genre) 
				{
					string bkname = name;
					transform(bkname.begin(), bkname.end(), bkname.begin(), ::tolower);
					transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);
        			// Check if the keyword matches any part of the book information
        			if (bkname.find(keyword) != string::npos)
					{
						replace(name.begin(), name.end(), '%', ' ');
            			replace(author.begin(), author.end(), '%', ' ');
            			replace(genre.begin(), genre.end(), '%', ' ');
            			searchResults[currentResult].setdata(name, price, stock, author, genre);
            			searchResults[currentResult].setSource(1);
            			cout << left << setw(5) << currentResult + 1 << left << setw(35) << name <<"RM "<< left << setw(10) << fixed << setprecision(2) << price << left << setw(8) << stock << endl;
            			currentResult++;
        			}
    			}
    			file.close();

				if (currentResult == 0) 
    			{
    				// Check if the user wants to search again or go back
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
    			}
    			else 
    			{
        			// Prompt user to choose a book for rental
        			int choice;
        			cout << "-----------------------------------------------------------------------------"<<endl;
        			cout << "Enter your choice: ";
        			cin >> choice;
				
					if (choice != 0 && (choice <= 0 || choice > currentResult)) 
        			{
        				cout << "\nInvalid choice! Please try again...\n";
        				sleep(2);
        				system("cls");
        				searchCatalog();
        		
					}
        			else if (choice == 0) 
        			{
            			// User chose to go back
            			system("cls");
            			searchCatalog();
        			}
        			else if (choice > 0 && choice <= currentResult) 
        			{
            			// User chose a valid book for rental, call the rental function
            			rental(choice, searchResults);
        			} 
    			}
    			// Release the dynamically allocated memory
    			delete[] searchResults;
    		}
    		else if (searchoption == 2)
			{
    			system("cls");
    			cout << "=============================================================================" << endl;
    			cout << "[0] Back \t\t\tSEARCH CATALOG" << endl;
    			cout << "=============================================================================" << endl;
    			cout << "User Menu > Search Catalog > Search by genres" << endl;
    			cout << "-----------------------------------------------------------------------------" << endl;
	
    			int numGenres = 0;
			 	ifstream gfile("records/books.txt");
    			int g = 0;
    			if (gfile.is_open())
    			{
        			// Count the total number of books in the file
        			while (getline(gfile, line))
        			{
            			g++;
        			}	
        		gfile.close();
    			}
    			else
    			{
        			cout << "Error: Unable to open the file 'records/books.txt'\n";
        			exit(0);
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
        			cout << "[" << i + 1 << "] " << genres[i] << endl;
        			replace(genres[i].begin(), genres[i].end(), ' ', '%');
    			}

    			int genrechoice;
				cout << "-----------------------------------------------------------------------------"<<endl;
        		cout << "Enter your choice: ";
        		cin >> genrechoice;
    			do
    			{
        			if (genrechoice == 0)
        			{
            			system("cls");
            			searchCatalog();
        			}
        			else if (genrechoice < 1 || genrechoice > numGenres)
        			{
            			cout << "Invalid choice. Please re-enter your choice." << endl;
            			cin >> genrechoice;
        			}
    			}while (genrechoice < 1 || genrechoice > numGenres);

    			ifstream countFile("records/books.txt");
    			int count = 0;
    			if (countFile.is_open())
    			{
        			// Count the total number of books in the file
        			while (getline(countFile, line))
        			{
            			count++;
        			}
        			countFile.close();
    			}
    			else
    			{
        			cout << "Error: Unable to open the file 'records/books.txt'\n";
        			exit(0);
    			}

    			string selectedGenre = genres[genrechoice - 1];
    			system("cls");
    			cout << "=============================================================================" << endl;
    			cout << "[0] Back \t\t\tSEARCH CATALOG" << endl;
    			cout << "=============================================================================" << endl;
    			cout << "User Menu > Search Catalog > Search by genre" << endl;
    			cout << "-----------------------------------------------------------------------------"<<endl;

    			// Now, find and display books with the selected genre
    			replace(selectedGenre.begin(), selectedGenre.end(), '%', ' ');
    			cout << "Search Results for Genre '" << selectedGenre << "'" << endl;
				replace(selectedGenre.begin(), selectedGenre.end(), ' ', '%');
				cout << "-----------------------------------------------------------------------------"<<endl;
				cout << left << setw(5) << "No." << left << setw(35) << "Book Name" << left << setw(12) << "Price" << left << setw(8) << "Stock" << endl;
    			Book* searchResults = new Book[count];
    			int currentResult = 0;

    			ifstream booksFile("records/books.txt");
    			while (booksFile >> id >> name >> price >> stock >> author >> genre)
    			{
        			// Check if the genre matches the selected genre
        			if (genre == selectedGenre)
        			{
            			replace(name.begin(), name.end(), '%', ' ');
            			replace(author.begin(), author.end(), '%', ' ');
            			replace(genre.begin(), genre.end(), '%', ' ');
            			searchResults[currentResult].setdata(name, price, stock, author, genre);
            			searchResults[currentResult].setSource(1);
            			cout << left << setw(5) << currentResult + 1 << left << setw(35) << name << left << setw(12) << price << left << setw(8) << stock << endl;
            			currentResult++;
        			}
    			}
    			booksFile.close();

    			if (currentResult == 0)
    			{
        			cout << "\nNo results found for the selected genre.\n";
    			}
    			else
    			{
        			// Prompt user to choose a book for rental
        			int choice;
        			cout << "-----------------------------------------------------------------------------"<<endl;
        			cout << "Enter your choice: ";
        			cin >> choice;

        			while (choice != 0 && (choice <= 0 || choice > currentResult))
        			{
            			// Invalid choice, inform the user and ask for re-entry
            			cout << "\nInvalid choice! Reenter your choice (0 to go back): ";
            			cin >> choice;
        			}

        			if (choice == 0)
        			{
            			// User chose to go back
            			system("cls");
            			searchCatalog();
        			}
        			else if (choice > 0 && choice <= currentResult)
        			{
            			// User chose a valid book for rental, call the rental function
            			rental(choice, searchResults);
        			}
    			}

    			// Release the dynamically allocated memory
    			delete[] searchResults;
    			delete[] genres;
			}
		}
};
//functions outside the Menus class are admin part (YS)
void Menus::addbook(){
	char choice;
	string back;
	int error;
	
	do{
		system("cls");
		fflush(stdin);
		cout << "============================================================================="<<endl;
		cout << "[0] Back \t\t\t  ADD BOOK"<<endl;
		cout << "============================================================================="<<endl;
		cout << "Admin Menu > Add Book "<<endl;
		cout << "-----------------------------------------------------------------------------"<<endl;
		cout << "Back to admin menu please press 0, or else will continue --> ";
		cin >> back;
		cout << endl;
		
		if(back=="0"){
			//back to admin menu
			adminMenu();
		}
		fflush(stdin);
		//create a dma for book obj
		Book *B = new Book;
		error = B->add();
		
		if(error==1){
			cout<<"\nBack to Add Book Menu..."<<endl;
			sleep(3);
			addbook();
		}
		else if(error==0){
			//after add delete
			delete B;
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
void Menus::searchbook(){
	int ch;
	
		system("cls");
		fflush(stdin);
		cout << "============================================================================================"<<endl;
		cout << "[0] Back \t\t\t          SEARCH BOOK"<<endl;
		cout << "============================================================================================"<<endl;
		cout << "Admin Menu > Search Book "<<endl;
		cout << "--------------------------------------------------------------------------------------------"<<endl;
		cout << "Please search the book you want before operating update or delete."<<endl<<endl;
		cout << "[1] View all books"<<endl;
		cout << "[2] Search by keyword"<<endl;
		cout << "[3] Search by genre"<<endl;
		cout << "--------------------------------------------------------------------------------------------"<<endl;
		cout << "Enter your choice: ";
		cin >> ch;

		if (ch==0){
			cout << "Back to Admin Menu..."<<endl;
			sleep(1);
			adminMenu();
		}
		else if(ch==1){
			viewbook();
		}
		else if(ch==2){
			searchbk();
		}
		else if(ch==3){
			searchbg();
		}
		else{
			cout<<"Invalid choice. Please enter again."<<endl;
			sleep(1);
			searchbook();
		}	

}
void Menus::viewbook(){
	string code, name, author, genre,c;
	char back;
	float price;
	int stock, totalnum=0, choice, foundid=0, option;
	
	system("cls");
		fflush(stdin);
		cout << "============================================================================================"<<endl;
		cout << "[0] Back \t\t\t          SEARCH BOOK"<<endl;
		cout << "============================================================================================"<<endl;
		cout << "Admin Menu > Search Book > View Book"<<endl;
		cout << "--------------------------------------------------------------------------------------------"<<endl;
		
		//open file
		ifstream inbook("records/books.txt");
		if(!inbook){
			cout<<"\nFile books.txt couldn't found!"<<endl;
			exit(0);
		}
		else{
			while(inbook >> code >> name >> price >> stock >> author >> genre){
				totalnum++; 
			}
			cout<<"=> There are total of " <<totalnum<<" books."<<endl;
			cout << "============================================================================================"<<endl;
			
			//back to beginning of file 
			inbook.clear();
			inbook.seekg(0);
		
		    cout <<left<<setw(6)<<"ID"<<left<<setw(35)<<"Book Name"<<setw(10)<<"Price(RM)"<<setw(7)<<"Stock"<<setw(22)<<"Author"<<setw(15)<<"Genre"<<endl;
		    ifstream file("records/books.txt");
		    //Set Book attributes
		    while(file >> code >> name >> price >> stock >> author >> genre){
		        replace(name.begin(), name.end(), '%', ' ');
				replace(author.begin(), author.end(), '%', ' ');
				replace(genre.begin(), genre.end(), '%', ' ');
				cout <<left<<setw(6)<< code <<setw(35)<< name <<setw(10)<<fixed<<setprecision(2)<<price<<setw(7)<<stock<<setw(22)<<author<<setw(15)<<genre<<endl;
		    }
		    inbook.close();	
		    
		    cout << "============================================================================================"<<endl;
			cout << "[1] Search by keyword"<<endl;
			cout << "[2] Search by genre"<<endl;
			cout << "[3] Update or Delete"<<endl;
			cout << "--------------------------------------------------------------------------------------------"<<endl;
			cout << "Enter your choice: ";
			cin >> choice;
								
			ifstream scanid("records/books.txt");
			if(!scanid){
				cout<<"\nFile books.txt couldn't found!"<<endl;
				exit(0);
			}
			switch(choice){
				case 0:
					cout << "\nBack to Search Book Menu..."<<endl;
					sleep(1);
					searchbook();
					break;
				case 1:
					searchbk();
					break;
				case 2:
					searchbg();
					break;
				case 3:
					fflush(stdin);
					cout << "Book ID: ";
					getline(cin,c);
				
					while(scanid >> code >> name >> price >> stock >> author >> genre){
						if(c==code){
							foundid++;
						}
					}
					scanid.close();
					if(foundid==0){
						cout<<"Invalid code. Please try again."<<endl;
						sleep(2);
						viewbook();
					}
					else{
						cout<<"\n**Book Found**"<<endl;
						cout << "............................................................................................"<<endl;
						cout<<"[1] Update Book"<<endl;
						cout<<"[2] Delete Book"<<endl;
						cout<<"Enter your choice: ";
						cin>>option;
					
						switch(option){
							case 0:
								cout << "\nBack to Search Book Menu..."<<endl;
								sleep(1);
								searchbook();
								break;
							case 1:
								updatebook(c);
								break;
							case 2:
								deletebook(c);
								break;
							default:
								cout<<"\n-_- Wrong input!..."<<endl;
								sleep(1);
								viewbook();
						}
					}
					break;
					
				default:
					cout << "\nInvalid choice." <<endl;
					sleep(1);
					viewbook();
			}
			
		}
		
}
void Menus::searchbk(){
	string keyword, code, name, genre, author,c;
	float price;
	int stock, option;
	char yesno;
	
	system("cls");
		fflush(stdin);
		cout << "============================================================================================"<<endl;
		cout << "[0] Back \t\t\t         SEARCH BOOK"<<endl;
		cout << "============================================================================================"<<endl;
		cout << "Admin Menu > Search Book > Search by keyword"<<endl;
		cout << "--------------------------------------------------------------------------------------------"<<endl;
		cout << "Enter keyword: ";
    	getline(cin, keyword);
    	cout << "--------------------------------------------------------------------------------------------"<<endl;
    		
    	if (keyword == "0") 
        {
            cout << "Back to Search Book Menu..."<<endl;
            sleep(1);
            searchbook();
        }
				
    	cout << "Showing results for '"<<keyword<<"'\n"<<endl;
		cout <<left<<setw(6)<<"ID"<<left<<setw(35)<<"Book Name"<<setw(10)<<"Price(RM)"<<setw(7)<<"Stock"<<setw(22)<<"Author"<<setw(15)<<"Genre"<<endl;
			
		int currentResult = 0;
		
		ifstream file("records/books.txt");
    	while (file >> code >> name >> price >> stock >> author >> genre) 
		{
        	// Check if the keyword matches any part of the book information
        	if (name.find(keyword) != string::npos)
			{
				replace(name.begin(), name.end(), '%', ' ');
            	replace(author.begin(), author.end(), '%', ' ');
            	replace(genre.begin(), genre.end(), '%', ' ');
            	cout << left <<setw(6)<<code<< setw(35) << name << setw(10) << fixed << setprecision(2) << price << left << setw(7) << stock <<setw(22)<<author
				<<setw(15)<<genre<< endl;
            	currentResult++;
        	}
    	}
    	file.close();
    	cout << "============================================================================================"<<endl;


		if (currentResult == 0) 
    	{
    		int searchagain;
    		cout << "--------------------------------------------------------------------------------------------" << endl;
        	cout << "\n\n\t\t\t          No results found.\n\n"<<endl;
        	cout << "--------------------------------------------------------------------------------------------" << endl;
			cout << "[1] Search again"<<endl;
			cout << "--------------------------------------------------------------------------------------------" << endl;
			cout << "Enter Choice: ";
        	cin >> searchagain;
        		
        	if(searchagain == 1)
        	{
        		searchbk();
			}
			else if(searchagain == 0)
			{
				cout << "Back to Search Book Menu..."<<endl;
            	sleep(1);
            	searchbook();
			}
			else
			{	
				cout << "\nInvalid choice! Please try again...\n";
        		sleep(2);
				searchbk();
			}
    	}
    	
    	cout << "Do you want to search again? [y/n]: ";
    	cin >> yesno;
    	
    	if(yesno=='y'){
    		searchbk();
		}
		else{
			fflush(stdin);
			cout << "Enter book ID you want to update or delete? [0 to go back]: ";
			getline(cin,c);
			if(c=="0"){
				searchbook();
			}
			
			ifstream check("records/books.txt");
			if(!check){
				cout<<"\nFile books.txt couldn't found!"<<endl;
				exit(0);
			}
			int i=0;
			while(check >> code >> name >> price >> stock >> author >> genre){
				if(c==code){
					i=1;
					cout<<"**Book found!**"<<endl;
					break;
				}	
			}
			if(i!=1){
				cout << "Invalid input. "<<endl;
				sleep(1);
				searchbk();
			}
			
			cout << "............................................................................................"<<endl;
			cout<<"[1] Update Book"<<endl;
			cout<<"[2] Delete Book"<<endl;
			cout<<"Enter your choice: ";
			cin>>option;
		
			switch(option){
				case 0:
					cout << "\nBack to Search Book Menu..."<<endl;
					sleep(1);
					searchbook();
					break;
				case 1:
					updatebook(c);
					break;
				case 2:
					deletebook(c);
					break;
				default:
					cout<<"\n-_- Wrong input!..."<<endl;
					sleep(1);
					searchbk();
			}
		}
		
}
void Menus::searchbg(){
	char firstletter,yesno;
	string code, name, genre, author,c;
	float price;
	int stock, count=0, option;

	system("cls");
		fflush(stdin);
		cout << "============================================================================================"<<endl;
		cout << "[0] Back \t\t\t         SEARCH BOOK"<<endl;
		cout << "============================================================================================"<<endl;
		cout << "Admin Menu > Search Book > Search by genre"<<endl;
		cout << "--------------------------------------------------------------------------------------------"<<endl;
		cout << "TIPS:	M-Mystery	N-Fiction	P-Philosophy	C-Classics	"<<endl;
		cout << "	R-Children	F-Fantasy	H-Horror	D-Dystopian	"<<endl;
		cout << "	T-Thriller"<<endl;
		cout << "--------------------------------------------------------------------------------------------"<<endl;
		cout<<"Which genre do you want to search? [E.g. M for Mystery]: ";
		cin>>firstletter;
		cout<<endl;
		if(firstletter=='0'){
			cout << "Back to Search Book Menu..."<<endl;
			sleep(2);
			searchbook();
		}
		cout <<left<<setw(6)<<"ID"<<left<<setw(35)<<"Book Name"<<setw(10)<<"Price(RM)"<<setw(7)<<"Stock"<<setw(22)<<"Author"<<setw(15)<<"Genre"<<endl;
		
		ifstream inbook("records/books.txt");
		while(inbook >> code >> name >> price >> stock >> author >> genre){
			if(firstletter==code[0]){
				replace(name.begin(), name.end(), '%', ' ');
				replace(author.begin(), author.end(), '%', ' ');
				replace(genre.begin(), genre.end(), '%', ' ');					
				cout << left <<setw(6)<<code<< setw(35) << name << setw(10) << fixed << setprecision(2) << price << left << setw(7) << stock <<setw(22)<<author
				<<setw(15)<<genre<< endl;				
				count++;
			}
		}
		if(count == 0){
				cout << "\n\t\t\t**Incorrect Input...**"<<endl;
				sleep(2);
				searchbg();
		}
		cout << "============================================================================================"<<endl;
		cout << "Do you want to search again? [y/n]: ";
    	cin >> yesno;
    	
    	if(yesno=='y'|| yesno=='Y'){
    		searchbg();
		}
		else if(yesno=='n'|| yesno=='N'){
			fflush(stdin);
			cout << "Enter book ID you want to update or delete? [0 to go back]: ";
			getline(cin,c);
			if(c=="0"){
				searchbook();
			}
			ifstream check("records/books.txt");
			if(!check){
				cout<<"\nFile books.txt couldn't found!"<<endl;
				exit(0);
			}
			int i=0;
			while(check >> code >> name >> price >> stock >> author >> genre){
				if(c==code){
					i=1;
					cout<<"**Book found!**"<<endl;
					break;
				}	
			}
			if(i!=1){
				cout << "Invalid input. "<<endl;
				sleep(1);
				searchbg();
			}
			
			cout << "............................................................................................"<<endl;
			cout<<"[1] Update Book"<<endl;
			cout<<"[2] Delete Book"<<endl;
			cout<<"Enter your choice: ";
			cin>>option;
		
			switch(option){
				case 0:
					cout << "\nBack to Search Book Menu..."<<endl;
					sleep(1);
					searchbook();
					break;
				case 1:
					updatebook(c);
					break;
				case 2:
					deletebook(c);
					break;
				default:
					cout<<"\n-_- Wrong input!..."<<endl;
					sleep(1);
					searchbg();
			}
		}
		else{
			cout<<"Invalid input."<<endl;
			sleep(1);
			searchbg();
		}
	
		
}
void Menus::updatebook(string code){
	int choice;
	
	system("cls");
	cout << "============================================================================="<<endl;
	cout << "[0] Back \t\t\t UPDATE BOOK"<<endl;
	cout << "============================================================================="<<endl;
	cout << "Admin Menu > Search Book > Update Book"<<endl;
	cout << "-----------------------------------------------------------------------------"<<endl;
	sleep(1);
	cout<<left<<setw(6)<<"ID"<<setw(35)<<"BOOK"<<setw(12)<<"PRICE (RM)"<<setw(8)<<"STOCK"<<setw(30)<<"AUTHOR"<<endl;

	//create book obj dma
	Book* UB = new Book;
	UB->update(code);
	
	delete UB;
	

	cout << "\n............................................................................."<<endl;
	cout<<"[0] Search menu"<<endl;
	cout<<"[1] Admin menu"<<endl;
	cout<<"Enter your choice: ";
	cin >> choice;
	
	if(choice==0){
		cout<<"\nBack to Search menu..."<<endl;
		sleep(1);
		searchbook();
	}
	else if(choice==1){
		cout<<"\nBack to Admin menu..."<<endl;
		sleep(1);
		adminMenu();
	}
	else{
		cout<<"\nInvalid choice -_- => Back to search menu"<<endl;
		sleep(1);
		searchbook();
	}

}	
void Menus::deletebook(string code){
	int choice;
	
	system("cls");
	cout << "============================================================================="<<endl;
	cout << "[0] Back \t\t\t DELETE BOOK"<<endl;
	cout << "============================================================================="<<endl;
	cout << "Admin Menu > Search Book > Delete Book"<<endl;
	cout << "-----------------------------------------------------------------------------"<<endl;
	sleep(1);
	cout<<left<<setw(6)<<"ID"<<setw(35)<<"BOOK"<<setw(12)<<"PRICE (RM)"<<setw(8)<<"STOCK"<<setw(30)<<"AUTHOR"<<endl;
	
	//create book obj dma
	Book* DB = new Book;
	DB->deleteb(code);
	
	delete DB;
	
	cout << "\n............................................................................."<<endl;
	cout<<"[0] Search menu"<<endl;
	cout<<"[1] Admin menu"<<endl;
	cout<<"Enter your choice: ";
	cin >> choice;
	
	if(choice==0){
		cout<<"\nBack to Search Menu..."<<endl;
		sleep(1);
		searchbook();
	}
	else if(choice==1){
		cout<<"\nBack to Admin Menu..."<<endl;
		sleep(1);
		adminMenu();
	}
	else{
		cout<<"\nInvalid choice -_- => Back to search menu"<<endl;
		sleep(1);
		searchbook();
	}

}
void Menus::fee(){
	int choice;
	char yesno,option;
	
	do{
		system("cls");
		fflush(stdin);
		cout << "============================================================================="<<endl;
		cout << "[0] Back \t\t\t  FEE MANAGEMENT"<<endl;
		cout << "============================================================================="<<endl;
		cout << "Admin Menu > Fee Management "<<endl;
		cout << "-----------------------------------------------------------------------------"<<endl;
		cout << "[1] Rental Fee"<<endl;
		cout << "[2] Penalty Fee"<<endl;
		cout << "[3] View Incured Penalty Fee and Money Earned"<<endl;
		cout << "-----------------------------------------------------------------------------"<<endl;
		cout << "Enter your choice: ";
		cin>>choice;
		cout<<endl;
		ifstream checkfee("records/fee.txt");
		if(!checkfee){
			cout <<"Error: Unable to open the file fee.txt"<<endl;
			exit(0);
		}
		checkfee >> FM.fee >> FM.fine >> FM.sum_penalty >> FM.money_earn_rent;
			switch(choice){
				case 0:
					adminMenu();
				case 1:
					cout<<"The current rental fee is RM " <<fixed<<setprecision(2)<< FM.fee <<"."<<endl;
					cout<<"Do you want to change? [y/n]: ";
					cin>>yesno;
					if(yesno=='y'||yesno=='Y'){
						changefee(1);
					}
					else{
						fee();
					}
					break;
				case 2:
					cout<<"The current overdue fee is RM " <<fixed<<setprecision(2)<< FM.fine <<"."<<endl;
					cout<<"Do you want to change? [y/n]: ";
					cin>>yesno;
					if(yesno=='y'||yesno=='Y'){
						changefee(2);
					}
					else{
						fee();
					}
					break;
				case 3:
					cout<<"The Total Incurred Penalty Fees is RM "<<fixed<<setprecision(2)<< FM.sum_penalty <<"."<<endl;
					cout<<"The Total Money Earned From Renting is RM "<<fixed<<setprecision(2)<< FM.money_earn_rent<<"."<<endl;
					cout<<"\n=>Total Money Earned (including Penalty Fees) is RM "<<fixed<<setprecision(2)<< FM.sum_penalty + FM.money_earn_rent<<"."<<endl;
					break;
				default:
					cout<<"Invalid choice. Please enter the correct input."<<endl;
					sleep(2);
					fee();
			}
		checkfee.close();
		cout << "-----------------------------------------------------------------------------"<<endl;
		cout << "Do you want to stay in this page? [y/n]: ";
		cin >> option;
		
		if(option=='n'||option=='N'){
			cout<< "Back to Admin Menu..."<<endl;
			sleep(2);
			adminMenu();
		}
		else if(option!='n'&&option!='N'&&option!='y'&&option!='Y'){
			cout<< "Wrong input."<<endl;
			sleep(2);
			fee();
		}
		
	}while(option=='y'||option=='Y');
	
}
//this is a friend func of class Book
void changefee(int op){
	Book *B = new Book;

	cout << "-----------------------------------------------------------------------------"<<endl;
	if(op ==1){
		cout<<"New Rental Fee each book per day: RM ";
		cin >> FM.fee;
		B->rentf=FM.fee;
		B->setrentfee();
		delete B;
	}
	else if(op==2){
		cout<<"New Overdue Penalty Cost: RM ";
		cin >> FM.fine;
		B->overduef=FM.fine;
		B->setoverduefee();
		delete B;
	}

}
void Menus::checkrentalrecord(){
	int choice;
	char yesno;
			
		system("cls");
		cout << "======================================================================================="<<endl;
		cout << "[0] Back \t\t\tVIEW RENTAL RECORDS"<<endl;
		cout << "======================================================================================="<<endl;
		cout << "Admin Menu > View Rental Records"<<endl;
		cout << "---------------------------------------------------------------------------------------"<<endl;
		cout << "[1] View all rental records"<<endl;
		cout << "[2] View Overdue records"<<endl;
		cout << "[3] View Active records"<<endl;
		cout << "---------------------------------------------------------------------------------------"<<endl;
		cout << "Enter your choice: ";
		cin >> choice;
		if(choice==0){
			cout<<"Back to Admin Menu..."<<endl;
			sleep(2);
			adminMenu();
		}
		else if(choice==1){
			checkstatus(1);
		}
		else if(choice==2){
			checkstatus(2);
		}
		else if(choice==3){
			checkstatus(3);
		}
		else{
			cout<<"Invalid input. Please try again."<<endl;
			sleep(2);
			checkrentalrecord();
		}
		fflush(stdin);
		cout << "Do you want to stay in this page? [y/n]: ";
		cin >> yesno;
		if(yesno=='y'||yesno=='Y'){
			checkrentalrecord();
		}
		else if(yesno=='n'||yesno=='N'){
			cout<<"Back to Admin Menu..."<<endl;
			sleep(1);
			adminMenu();
		}
		else{
			cout<<"Wrong input."<<endl;
			sleep(1);
			checkrentalrecord();
		}
		
}
//another friend function
void checkstatus(int num){
	int i=0,j=0,s, overdue=0, active=0,n=0;
	string filename, bn, dd;
	int qty, choice;
	
	ifstream read("records/registered/user.txt");
	Info *I = new Info;
	if(!read){
		cout <<"Error: Unable to open the file user.txt"<<endl;
		exit(0);
	}
	else{
		while(read >> I-> username >> I->password){
			i++;
		}
		//back to beginning of file 
		read.clear();
		read.seekg(0);
		string *u = new string [i];
		
		while(read >> I-> username >> I->password){
			u[j] = I->username;
			j++;
		}

		read.close();
		
		j=0;
		cout << "---------------------------------------------------------------------------------------"<<endl;
		cout<<left<<setw(5)<<"No."<<setw(20)<<"User"<<setw(35)<<"Book Name"<<setw(5)<<"Qty"<<setw(13)<<"Date Due"<<setw(8)<<"Status"<<endl;
		while(j<i){
			filename = "records/rented/" + u[j] + ".txt";
			ifstream out(filename.c_str());
						
			if(!out){
				j++;
				continue;
			}
			if(num==1){
				cout<<endl;
				while(out >> bn >> qty >> dd){
					replace(bn.begin(), bn.end(), '%', ' ');
					s=status(dd);
						
					cout<<left<<setw(5)<<n+1<<setw(20)<<u[j]<<setw(35)<<bn<<setw(5)<<qty<<setw(13)<<dd;
					if(s==0){
						cout <<setw(8)<< "\033[1;32mActive\033[0m" <<endl;
						active++;
					}
					else if(s==1){
						cout <<setw(8)<< "\033[1;31mOverdue\033[0m"<<endl;
						overdue++;
					}
					n++;
				}
				
				if(n==0){
					cout<<"\n\n\t\t   No books currently being rented...\n\n"<<endl;
				}
			}

			else if(num==2){
				cout<<endl;
				while(out >> bn >> qty >> dd){
					replace(bn.begin(), bn.end(), '%', ' ');
					s=status(dd);
						
					if(s==1){
						cout<<left<<setw(5)<<n+1<<setw(20)<<u[j]<<setw(35)<<bn<<setw(5)<<qty<<setw(13)<<dd;
						cout <<setw(8)<< "\033[1;31mOverdue\033[0m"<<endl;
						overdue++;
						if(overdue!=0){
							n++;
						}
					}
					
				}
				
			}

			else if(num==3){
				cout<<endl;
				while(out >> bn >> qty >> dd){
					replace(bn.begin(), bn.end(), '%', ' ');
					s=status(dd);
					
					if(s==0){
						cout<<left<<setw(5)<<n+1<<setw(20)<<u[j]<<setw(35)<<bn<<setw(5)<<qty<<setw(13)<<dd;
						cout <<setw(8)<< "\033[1;32mActive\033[0m" <<endl;
						active++;
						if(active!=0){
							n++;
						}
					}
					
				}
				
			}
			
			out.close();

			j++;	
		}
		if(overdue==0&&num==2){
			cout<<"\n\n\t\t\tThere is no overdued book...\n\n"<<endl;
		}
		else if(active==0&&num==3){
			cout<<"\n\n\t\t      There is no books with status active...\n\n"<<endl;
		}

		cout << "======================================================================================="<<endl;

		
		delete []u;
		delete I;
		
		
	}//end of else
	
}
//normal function to calculate the duedate to get status
int status(string datedue){
	time_t currentTime = time(0);
    tm* currentDate = localtime(&currentTime);
		    	
	tm dueDate = {};
	istringstream ss(datedue);
			    
	string dayStr, monthStr, yearStr;
	getline(ss, dayStr, '/');
	getline(ss, monthStr, '/');
	getline(ss, yearStr);

	int day, month, year;
	istringstream(dayStr) >> day;
	istringstream(monthStr) >> month;
	istringstream(yearStr) >> year;
		    	
	tm specificDate = {0};
	specificDate.tm_mday = day;
	specificDate.tm_mon = month - 1;
	specificDate.tm_year = year - 1900;
	
	int daysDifference = currentDate->tm_mday - specificDate.tm_mday + (currentDate->tm_mon - specificDate.tm_mon) * 30 + (currentDate->tm_year - specificDate.tm_year) * 365;
	if (currentDate->tm_year == specificDate.tm_year && currentDate->tm_mon == specificDate.tm_mon && currentDate->tm_mday == specificDate.tm_mday) {
		return 0;
	}
	else if (difftime(currentTime, mktime(&specificDate)) > 0) {
		return 1;
	}
	else{
		return 0;
	}
	
}
//YS

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
                    client.adminMenu(); // Call the admin menu after successful admin login
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
