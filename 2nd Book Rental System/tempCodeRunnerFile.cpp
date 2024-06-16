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
