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
#include <ctime>
using namespace std;

int main()
{
    int choice;
    string filename = "records/rented/yansan.txt";
    ifstream inputFile(filename.c_str());
    if (!inputFile) {
        cout << "Unable to open file: " << filename << endl;
        return 1; // or some other error handling
    }
    string disName, disDateDue, line;
    int disQuantity, count = 1;

    cout << "============================================================================="<<endl;
    cout << "[0] Back \t\t\tRENTED BOOKS"<<endl;
    cout << "============================================================================="<<endl;
    cout << "User Menu > Rented Books"<<endl;
    cout << "-----------------------------------------------------------------------------"<<endl;
    cout <<left<<setw(5)<<"No."<<left<<setw(37)<<"Book Name"<<left<<setw(9)<<"Qty."<<left<<setw(16)<<"Due Date"<<left<<setw(12)<<"Status"<<endl;

    //inputFile.open(filename.c_str());
    while(inputFile >> disName >> disQuantity >> disDateDue){
        replace(disName.begin(), disName.end(), '%', ' ');
        //declaring variables for date time                        
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
    }
}