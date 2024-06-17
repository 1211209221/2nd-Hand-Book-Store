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

		void listBooks(int num) {
			string disDateDue;
			int count = 1, overdueNo = -1, daysDifference = 0, choice;
			int* overdueNum = new int[num];
		    int* overdueDays = new int[num];

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
			
			//To calculate the difference between current date and due date
			daysDifference = currentDate->tm_mday - specificDate.tm_mday + (currentDate->tm_mon - specificDate.tm_mon) * 30 + (currentDate->tm_year - specificDate.tm_year) * 365;

			temp = head;
			while (temp != NULL) {
				cout << left << setw(5) << count++;
				cout << left << setw(37) << temp->bookName << left << setw(9) << temp->bookQuantity << left << setw(16) << temp->dueDate;
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
				temp = temp->next;
			}
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
			if(numEntries == 0) {
				cout<<"\n\n\t\t\t     No books rented...\n\n"<<endl;
			} else {
				rentedBooks.listBooks(numEntries);
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
                cout << "Recall Function" << endl;
				//rented();
			}
		    if(choice == 0){
		    	system("cls");
		        //userMenu();
                cout << "Return to main" << endl;
			} 
			else if(choice > 0 && choice <= numEntries){
				char confirmReturn;
				int overdue = 0;
				cout << "-----------------------------------------------------------------------------"<<endl;
			    /*for (int i = -1; i < overdueNo; ++i) {
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
		}*/
}
