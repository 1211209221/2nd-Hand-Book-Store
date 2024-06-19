receipt(&cartList);
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

		void receipt(CartLinkedList* cartList){
			double bookGrandTotal=0;
			int bookFinalQuantity=0;
			string choice;

			// Get the head of the passed CartLinkedList
			Cart* current = cartList->getHead();
			
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
		    //Cart* current = c;
			while(current != nullptr){
				string ReceiptBookName = current->getBookName();
				replace(ReceiptBookName.begin(), ReceiptBookName.end(), ' ', '%');
				ReceiptWrite << ReceiptBookName << " " << current->getBookQuantity() << " " << current->getBookPrice() << " "<< current->getBookDays() << endl;
				current = current->getNext();
			}

			ReceiptWrite.close();
		    
			cout<<"\n\t-------------------------------------------------------------"<<endl;
			cout<<"\tRENTAL ID: "<<rental_string<<endl;
			cout<<"\n\t"<<setfill(' ')<<left<<setw(5)<<"No."<<left<<setw(37)<<"Book Name"<<left<<setw(9)<<"Qty."<<left<<setw(16)<<"Price\n"<<endl;
			// Use the passed CartLinkedList when printing the receipt
			current = cartList->getHead();
			int i = 1;
			while(current != nullptr){
				cout<<"\t"<<left<<setw(5)<<i;
				current->listReceipt();
				bookGrandTotal += current->getBookTotalPrice();
				bookFinalQuantity += current->getBookQuantity();
				current = current->getNext();
				i++;
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