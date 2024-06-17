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

float getoverduefee(){
    float rentf, overduef, sumf, earnrent;
    ifstream out("records/fee.txt");
    if(!out){
        cout <<"Error: Unable to open the file fee.txt"<<endl;
        exit(0);
    }
    out >> rentf >> overduef >>sumf >>earnrent;
    out.close();
    
    return overduef;
}

class RentedBook {
	private:
		string bookName;
		int bookQuantity;
		string dueDate;

	public:
		RentedBook* next;

		RentedBook(string name, int quantity, string date)
		{
			bookName = name;
			bookQuantity = quantity;
			dueDate = date;
			next = NULL;
		}
		
		int getBookQuantity() {
            return bookQuantity;
        }

        string getBookName() {
            return bookName;
        }

		friend class RentedLinkedList;
};

class RentedLinkedList {
	private:
		RentedBook* head, * temp;

	public:
		RentedLinkedList()
		{
			head = NULL;
			temp = NULL;
		}

		~RentedLinkedList() {
			while (head != NULL) {
				RentedBook* bookToDelete = head;
				head = head->next;
				delete bookToDelete;
			}
		}

		void addBook(string name, int quantity, string date) {
			if (head == NULL) {
                // Creating the first node
                head = new RentedBook(name, quantity, date);
                temp = head;
            } else {
                // Adding subsequent nodes
                temp->next = new RentedBook(name, quantity, date);
                temp = temp->next;
            }
		}

		vector<pair<int, int> > listBooks(int num) {
            string disDateDue;
            int count = 1;
            int daysDifference = 0;
            vector<pair<int, int> > overdueBooks;

            // Declaring variables for date time
            time_t currentTime = time(0);

            // tm is human readable format, localtime convert time_t to tm
            tm* currentDate = localtime(&currentTime);

            temp = head;
            while (temp != NULL) {
                // Separating due date into day, month, year
                istringstream ss(temp->dueDate);
                string dayStr, monthStr, yearStr;
                getline(ss, dayStr, '/');
                getline(ss, monthStr, '/');
                getline(ss, yearStr);

                // Converting string to int
                int day, month, year;
                istringstream(dayStr) >> day;
                istringstream(monthStr) >> month;
                istringstream(yearStr) >> year;

                // Declaring datatype tm, {0} is to initialize all values to 0
                tm specificDate = {0};

                // Setting the values of specific date
                specificDate.tm_mday = day;
                specificDate.tm_mon = month - 1;
                specificDate.tm_year = year - 1900;
                
                // To calculate the difference between current date and due date
                daysDifference = currentDate->tm_mday - specificDate.tm_mday + (currentDate->tm_mon - specificDate.tm_mon) * 30 + (currentDate->tm_year - specificDate.tm_year) * 365;

                cout << left << setw(5) << count++;
                cout << left << setw(37) << temp->bookName << left << setw(9) << temp->bookQuantity << left << setw(16) << temp->dueDate;
                if (currentDate->tm_year == specificDate.tm_year && currentDate->tm_mon == specificDate.tm_mon && currentDate->tm_mday == specificDate.tm_mday) {
                    cout << "\033[1;32mActive\033[0m" << endl;
                }
                else if (difftime(currentTime, mktime(&specificDate)) > 0) {
                    cout << "\033[1;31mOverdue\033[0m"<< endl;
                    overdueBooks.push_back(make_pair(count-1, daysDifference));
                }
                else {
                    cout << "\033[1;32mActive\033[0m" << endl;
                }
                temp = temp->next;
            }

            return overdueBooks;
        }
        
        RentedBook* getBookByIndex(int index) {
            int count = 0;
            temp = head;
            while (temp != NULL && count < index) {
                temp = temp->next;
                count++;
            }
            return temp;
        }
};

int main()
{
    int choice;
    string filename = "records/rented/vscodetest.txt";            
    ifstream inputFile(filename.c_str());
    string disName, disDateDue, line;
    int disQuantity, numEntries = 0, overdueNo = -1;
    char confirmReturn;
    RentedLinkedList rentedBooks;

    while(getline(inputFile, line)) {
        numEntries++;
    }
    inputFile.close();    

    cout << "============================================================================="<<endl;
    cout << "[0] Back \t\t\tRENTED BOOKS"<<endl;
    cout << "============================================================================="<<endl;
    cout << "User Menu > Rented Books"<<endl;
    cout << "-----------------------------------------------------------------------------"<<endl;
    cout <<left<<setw(5)<<"No."<<left<<setw(37)<<"Book Name"<<left<<setw(9)<<"Qty."<<left<<setw(16)<<"Due Date"<<left<<setw(12)<<"Status"<<endl;
    
    inputFile.open(filename.c_str());
    while(inputFile >> disName >> disQuantity >> disDateDue) {
        replace(disName.begin(), disName.end(), '%', ' ');
        rentedBooks.addBook(disName, disQuantity, disDateDue);
    }

    vector<pair<int, int> > overdueBooks;
    if(numEntries == 0) {
        cout<<"\n\n\t\t\t     No books rented...\n\n"<<endl;
    } else {
        overdueBooks = rentedBooks.listBooks(numEntries);
    }
    
    float fine = getoverduefee();
    cout << "-----------------------------------------------------------------------------"<<endl;
    cout << "**DISCLAIMER"<<endl;
    cout << "  Select a line to return the specific book(s). Late returns are fined"<<endl;
    cout << "  RM "<<fixed<<setprecision(2)<<fine<<"/day for each overdue book."<<endl;
    cout << "-----------------------------------------------------------------------------"<<endl;
    cout << "Enter your choice: ";
    cin >> choice;

    if(numEntries == 0 && choice != 0) {
        cout << "\nNo books rented! Please try again later..." << endl;
        sleep(2);
        system("cls");
        cout << "Recall Function" << endl;
        // rented();
    }
    if(choice == 0) {
        system("cls");
        // userMenu();
        cout << "Return to main" << endl;
    }
    else if(choice > 0 && choice <= numEntries) {
        char confirmReturn;
        int overdue = 0;
        cout << "-----------------------------------------------------------------------------"<<endl;

        for (size_t i = 0; i < overdueBooks.size(); ++i) {
            if (overdueBooks[i].first == choice) {
                cout << "Overdue by " << overdueBooks[i].second << " day(s). Total penalty fee is RM " << fixed << setprecision(2) << overdueBooks[i].second * fine << "." << endl;
                RentedBook* book = rentedBooks.getBookByIndex(choice - 1);
                if(book->getBookQuantity() > 1) {
                    cout << "Pay penalty fee and return " << book->getBookQuantity() << " copies of '" << book->getBookName() << "'?" << endl;
                } else if(book->getBookQuantity() == 1) {
                    cout << "Pay penalty fee and return " << book->getBookQuantity() << " copy of '" << book->getBookName() << "'?" << endl;
                }
                overdue = 1;
            }
        }

        if(overdue == 0){
            RentedBook* book = rentedBooks.getBookByIndex(choice - 1);
            if(book->getBookQuantity() > 1) {
                cout << "Return " << book->getBookQuantity() << " copies of '" << book->getBookName() << "'?" << endl;
            } else if(book->getBookQuantity() == 1) {
                cout << "Return " << book->getBookQuantity() << " copy of '" << book->getBookName() << "'?" << endl;
            }
        }

        cout<<"\nConfirm action [Y/N]: ";
        cin>>confirmReturn;    
    }
    else{
        cout << "\nInvalid choice! Please re-enter...\n";
        sleep(1);
        system("cls");
    }
    inputFile.close();
}
