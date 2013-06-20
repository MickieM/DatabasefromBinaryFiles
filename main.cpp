//*************************************************
//*FILE: Maxey_A5                                 *
//*PROG: Carla Maxey							  *
//*PURP: Process database from binary file		  *
//*************************************************


# include <cstdio>
# include <cstring>
# include <cstdlib> 
# include <cctype>


struct dueDate
	{
		int month;
		int day;
		int year;

		//constructor
		dueDate(): month(0), day(0), year(0) {}
	};

struct record
	{
		int custID;
		char name[28];
		char state[3];
		char discCode;
		double balanceDue;
		dueDate date;
	};

struct indexTable
	{
		int ID;
		int pos;
		
		//constructor
		indexTable() : ID(0), pos(0) {}
	};


//PROTOYPES

//menu option functions
void list(const char*, int);
void addRecord(const char* fname, indexTable *index, int& recordCount, int& endPos);
void deleteRecord(const char* fname, indexTable *index, int&);

//validation functions
bool validInt(const char *testInt);
bool validFloat(const char *testFloat);
bool validState(const char *testState);
bool validDate(const char *testDate, record& customer);

//data functions
int getChoice();
int getCustID();
void getName(char *buffer, int bufLen);
void getState(char *buffer, int bufLen);
double getBalance();
char getCode();
void getDate(record& customer); 

int getIndex(indexTable *index, int maxSize);
void getString(const char *prompt, char *buffer, int);
char getChar(const char *prompt);
int getInt(const char *prompt);

//support functions
void bubbleSort(indexTable *index, int);
int binarySearch(const indexTable a[], int low, int high, int target);
void printHeader();
char* strToUpper(char* str);
int getRecCountInDataFile(const char* fname);
void printRecord(const record&);
void printUnderLine();


int main()
{
	const char* fname = "ASSIGNV1.RND";
	
	const int maxSize = 50;
	indexTable index[maxSize]; 
	
	const int bufLen = 80;
	char buf[bufLen];

	//fill index table
	int recordCount = getIndex(index, maxSize);
	
	int endPos = getRecCountInDataFile(fname);

	int choice = 0;
	while((choice = getChoice()) != 5)
	{
		switch (choice)
		{
			case 1:
			{
				system("cls");
				printHeader();
				for(int x = 0; x < recordCount; x++)
					{list(fname, index[x].pos);}
				printUnderLine();
				break;
			}
			case 2:
			{
				int key = getInt("\nPlease enter a custID for search:");
				int sub = binarySearch(index, 0, recordCount-1, key);
				if(sub>=0)
				{
					printHeader();
					list(fname, index[sub].pos);
					printUnderLine();	
					printf("\n\n");
				}
				else
				{
					printf("RECORD %d NOT IN DATABASE...\n\n", key);
				}
				break;
			}
			case 3:
			{
				addRecord(fname, index, recordCount, endPos);
				break;
			}
			case 4:
			{
				int oldCount = recordCount;
				deleteRecord(fname, index, recordCount);
				break;
			}
			
		};
	}//END MAIN WHILE

	getString("\nPress Enter to exit ... ", buf, bufLen);

}
// END MAIN

void list(const char fname[], int pos) // list ONE rec to stdout ... //
{
	FILE* recordFile = fopen(fname,"rb");
	if(recordFile)
	{
		fseek(recordFile, sizeof(record) * pos, SEEK_SET);
		
		record customerTmp;
		fread(&customerTmp, sizeof(record), 1, recordFile);
		fclose(recordFile);
		
		printRecord(customerTmp);
	}
	else
		printf("\nThere was a problem reading file %s\n", fname);		
}//END LIST


/*********************************************************
* Add record to file  and  update recordCount and endPos *
**********************************************************/
void addRecord(const char* fname, indexTable index[], int& recordCount, int& endPos)
{
	
	record customer;
	customer.custID = getCustID();
	int sub = binarySearch(index, 0, recordCount-1, customer.custID);
	
	//check for existing ID
	while(sub >= 0)
	{
		printf("ID already exists.\n");
		customer.custID = getCustID();
		sub = binarySearch(index, 0, recordCount-1, customer.custID);
	}

	const int len = 80;
	char temp[len];
	
	getName(temp, len);
	strcpy(customer.name, temp);
	getState(temp, len);
	strcpy(customer.state, temp);
	customer.discCode = getCode();
	customer.balanceDue = getBalance();
	getDate(customer);
	
	printf("\n");
	printHeader();
	printRecord(customer);
	printUnderLine();
	
	bool wasAdded = false;

	//verify, append to file
	if(tolower(getChar("\nIs this data correct? (y/n) ?")) == 'y') 
	{	
		FILE* recordFile = fopen(fname,"ab");
		if(fname)
		{	
			fwrite(&customer, 1, sizeof(record), recordFile); 
			fclose(recordFile);
			
			index[recordCount].ID = customer.custID;
			
			index[recordCount].pos = endPos;
			
			++endPos;
			++recordCount;
			
			bubbleSort(index, recordCount);
			
			wasAdded = true;
			printf("Record added to database.\n\n");

		}
		else
		{
			printf("\nThere was a problem appending to file %s\n", fname);

		}
	}
	else
	{
		printf("\nRecord addition cancelled.\n");
	}
	
}//END ADDRECORD

/*********************************
* Delete record *
*********************************/
void deleteRecord(const char* fname, indexTable index[], int& recordCount)
{
	int id = getInt("\nEnter ID to delete:");
	int i = binarySearch(index, 0, recordCount-1, id);
	
	if(i >= 0)
	{
		printHeader();
		list(fname, index[i].pos);
		printUnderLine();	
		printf("\n");
		if(tolower(getChar("Do you want to delete (y/n)?")) == 'y')
		{
			index[i].ID = 9999;
			index[i].pos = 9999;
	
			bubbleSort(index, recordCount);
			
			--recordCount;
			printf("Record deleted.\n\n\n");
		}
		else
			printf("Deletion cancelled.\n");
	}
	else
	{
		const int bufLen = 80;
		char buf[bufLen];
		printf("Record %d not found in database.\n\n", id);
	}
}

/*********************************
* Bubble Sort index array  *
*********************************/
void bubbleSort(indexTable index[], int recordCount) 
{
	
	bool swap = true;
	indexTable tmp;
	while(swap)
	{
		swap = false;
		for(int i = 1; i < recordCount; i++)
		{
			if(index[i-1].ID > index[i].ID)
			{
				tmp = index[i];
				index[i] = index[i-1];
				index[i-1] = tmp;
				swap = true;
			}//ENDIF
		}//END FOR
		--recordCount; //this copy is local
	}//END WHILE
}//END BUBBLESORT

/*********************************
* Binary Search index array  *
*********************************/
int binarySearch(const indexTable index[], int min, int max, int target)
{

	//recursive binary search
	if(max < min)
		return -1;
		
	int middle = (min + max)/2;
	
	if(target < index[middle].ID )
		return binarySearch(index, min, middle-1, target);
	else if(target > index[middle].ID)
		return binarySearch(index, middle+1, max, target);
	
	return middle;
}

/*********************************
* Init index using index file and return size *
*********************************/
int getIndex(indexTable index[], int maxSize)
{
	//flll index
	const char* fname = "ASSIGNV1.IND";
	FILE* indexFile = fopen(fname, "rb");
	if(!indexFile)
	{
		printf("\nFile %s not found\n", fname);
		return -1;
	}
	
	int x = 0;
	//fill empty index elements with 9999
	for( ; x < maxSize; x++ )
	{
		index[x].ID = 9999;
		index[x].pos = 9999;
	}

	x = 0;

	// read in all records to EOF
	for( ; x < maxSize && fread(&index[x], sizeof(indexTable),1, indexFile); x++) ; 
			
	fclose(indexFile);
	
	return x;

}//END GETINDEX

/*********************************
* Gets and returns Menu Choice  *
*********************************/
int getChoice()
{
	printf("1 List the file\n");
	printf("2 Search the file\n");
	printf("3 Add a record\n");
	printf("4 Delete a record\n");
	printf("5 Exit the program\n");
	
	int choice = getInt("Please choose an action:");
	
	while(choice < 1 || choice >5)
	{
		choice = getInt("\nInvalid menu selection. Try again:");
	}

	return choice;
}//END GET CHOICE

/*********************************
* loops and Gets and returns an Integer in range 0..998 *
*********************************/
int getInt(const char prompt[])
{
	const int length = 40;
	int number = 0;
	char testInt[length];
	bool valid = false;

	while(!valid)
	{
		getString(prompt, testInt,length);

		valid = validInt(testInt);

		if(!valid)
			printf("\nNot an integer. Please try again.");
		else
			number = atoi(testInt);
			
		if(number>998)
		{
			printf("\nID must be in range 0 - 998");
			valid = false;
		}
	}
	return number;
}//END getInt

/*********************************
* Gets and returns a String  *
*********************************/
void getString(const char prompt[], char buffer[], int max)
{
	printf("\n%s ",prompt);
	fgets(buffer,max,stdin);
	char* p = strchr( buffer, '\n');
	if( p != NULL ) 
		*p = 0; 
	else while(fgetc(stdin) != '\n');
}//END GETSTRING


/*********************************
* Print Table Header            *
*********************************/
void printHeader()
{
	printf("CustID  ");
	printf("Name               ");
	printf("State  ");
	printf("Disc Code       ");
	printf("Balance         ");
	printf("Due Date\n");
	printUnderLine();
	printf("\n");
}

/*********************************
* Gets CustID  *
*********************************/
int getCustID()
{
	int custID = 0;

	custID = getInt("Enter the customer ID, must be < 999:"); 

	while(custID > 998)
	{
		custID = getInt("Invalid customer ID, please try again:"); 
	}

	return custID;
}

/*********************************
* Convert string to all caps    *
*********************************/
char* strToUpper( char* str )
{
	char* p = str;
	while(*p != 0)
	{*p = toupper(*p); ++p;}

	return str;
}

/*********************************
* Gets name  *
*********************************/
void getName(char buffer[], int bufLen)
{
	getString("Please enter the customer name:", buffer, bufLen);

	while(buffer[0] == '\0' || strlen(buffer) > 27)
	{
		getString("Enter a customer name using 1 - 27 char's:", buffer, bufLen);
	}
	strToUpper( buffer );

}//END GETNAME

/*********************************
* Gets state  *
*********************************/
void getState(char buffer[], int bufLen)
{
	bool valid = false;

	while(!valid)
	{
		getString("Please enter the state as a 2 char abbreviation:",
					buffer, bufLen);
		buffer[0] = toupper(buffer[0]);
		buffer[1] = toupper(buffer[1]);
		valid = validState(buffer);

		if(!valid) 
		{
			if(strlen(buffer))
				printf( "\n'%s' is not a valid 2 char code for a State.\n", 
						buffer );
			else
				printf( "\nYou must enter a state.\n" );
		}		
	}
}//END GETSTATE

/*********************************
* Validate state  *
*********************************/

bool validState(const char testState[])
{
	const char* fname = "STATES.DAT";
	const int maxStates = 50;
	
	static char stateTable[maxStates][3]; 
	static bool loaded = false;
	
	if(!loaded)
	{
		const int length = 80;
		FILE *stateFile;
		char buffer[length];

		stateFile = fopen(fname, "r");

		if(!stateFile)
		{
			printf("File %s failed to open!", fname);
			exit(1);
		}
		
		//skip every other record, read to EOF
		for(int x = 0; x < maxStates && fgets(buffer, length, stateFile); x++)
		{
			// read every 2nd line into buffer ...
			fgets(buffer, length, stateFile);
			strncpy(stateTable[x], buffer, 2);
			stateTable[x][2] = '\0';
		}

		fclose(stateFile);
			
		loaded = true;
	}//END LOAD IF


	bool validState = false;
	
	for(int x = 0; x < maxStates; x++)
	{
		if(strcmp(stateTable[x], testState) == 0)
		{
			validState = true;
			break;
		}
	}
	
	return validState;

}//END VALIDSTATE

/***********************************************
* Gets and returns discount code *
************************************************/
char getCode()
{
	bool valid = false;
	char code = ' ';

	while(!valid)
	{
		code = toupper(getChar("Please enter the discount code: "));

		if(code == 'A' || code == 'B' || code == 'C' || code == '\0')
			valid = true;
		else
			printf( "\nValid codes here A, B, C or just press 'Enter' ..." );
	}

	return code;
}

/*********************************
* Gets and returns a Char  *
*********************************/
char getChar(const char prompt[])
{
	const int length = 40;
	char buffer[length];
	char code;
	getString(prompt, buffer,length);
	code = buffer[0];

	return code;
}//END GETCHAR

/***********************************************
* Gets and returns balance *
************************************************/
double getBalance()
{
	const int length = 40;
	char testBal[length];
	bool floatTest = false;
	double balance = 0.00;

	while(!floatTest)
	{
		getString("Please enter the balance due:", testBal, length);
		floatTest = validFloat(testBal);
		if(!floatTest)
			printf("\nEnter a number with 2 decimal places.");
	}
	
	balance = atof(testBal);

	return balance;

}//END GETBALANCE

/*********************************
* Test For Valid Float *
*********************************/
bool validFloat(const char testFloat[])
{
	int testLength = strlen(testFloat);
	int decimal = 0;
	
	for (int x = 0; x < testLength; x++)
	{	
		if (testFloat[x] == '.')
			decimal++;
		else if(!isdigit(testFloat[x]) && testFloat[x] != '.')
		{
			return false;
		}

	}//END FOR

	if( decimal != 1 )
		return false;
		
	if(testFloat[testLength - 3] != '.')
		return false;
	
	return true;
}//END VALID

/***********************************************
* Gets date as string*
************************************************/
void getDate(record& customer)
{
	const int length = 40;
	char testDate[length];
	bool valid = false;
	
	getString("Please enter the date as mm/dd/yyyy: ", testDate, length); 
	
	
	valid = validDate(testDate, customer);
	
	while(!valid)
	{
		getString("Please enter a VALID date as mm/dd/yyyy: ", 
					testDate, length); 
		valid = validDate(testDate, customer);
	}
}//END GETDATE

/***********************************************
* Check valid date*
************************************************/
bool validDate(const char testDate[], record& customer)
{
	//check length
	if(strlen(testDate) != 10) 
		return false;
		
	// check for '/'
	if(!(testDate[2] == '/' && testDate[5] == '/'))
		return false;
	
	// check for 1 or 0 in month
	if(!(testDate[0] == '1' || (testDate[0]) == '0') )
	{
		printf("Please enter the month as 01, 02, 03, etc.");
		return false;
	}
	
	//check digits
	for(int i = 0; i < 10; ++i )
		if(i != 2 && i != 5 && !isdigit(testDate[i]) )
			return false;

	// split string into parts
	int month = 0;
	int day = 0;
	int year = 0;
	
	char copy[12];
	
	char tMonth[3];
	char tDay[3];
	char tYear[5];

	strcpy(copy, testDate); 

	//extract ints for struct
	char* token;

	//GET MONTH
	token = strtok(copy,"/");
	strcpy(tMonth, token);
	month = atoi(tMonth);

	//GET DAY
	token = strtok(NULL, "/");
	strcpy(tDay, token);
	day = atoi(tDay);

	//GET YEAR
	token = strtok(NULL, "/");
	strcpy(tYear, token);
	year = atoi(tYear);

	//test month
	if(month < 1 || month > 12)
	{
		printf("\nValid months range 1 - 12");
		return false;
	}
	
	//test year
	if(year < 1600 || year > 2013)
	{
		printf("\nValid years range 1600 - 2013");
		return false;
	}
		
	//get days for month and year
	int numDays = 0;
	
	if(month == 4 || month == 9 || month == 6 || month == 11)
			numDays = 30;
	else if (month == 2) //leap year
	{
		if(year % 400 == 0)
			numDays = 29;
		else if(year % 100 == 0)
			numDays = 28;
		else if(year % 4 == 0)
			numDays = 29;
		else
			numDays = 28;
	}//end leap year
	else
		numDays = 31;
	
	if( day < 1 || day > numDays )
	{
		printf("\nFor month %d and year %d, valid day range is 1 - %d\n", 
				month, year, numDays);
		return false;
	} //end day test

	customer.date.month = month;
	customer.date.day = day;
	customer.date.year = year;

	return true;
}


/*********************************
* Test For Valid Int  *
*********************************/
bool validInt(const char testInt[])
{
	int testLength = strlen(testInt);
	
	if( !testLength )
		return false;
	
	for (int x = 0; x < testLength; x++)
	{
		if(!isdigit(testInt[x]))
			return false;
	}

	return true;
}


/**************************************
* Get number of records in binary file*
***************************************/
int getRecCountInDataFile(const char* fname)
{
	FILE* fp = fopen(fname, "rb");
	int numRecs = 0;
	record rec;
	if(fp)
	{
		while( fread(&rec, 1, sizeof(record), fp) ) 
			++numRecs;
		fclose(fp);
	}
	return numRecs;
}


/**************************************
*Print 1 record -					  *
***************************************/
void printRecord( const record& customer )
{
	printf("%-8i",customer.custID);
	printf("%-20s",customer.name);
	printf("%-10s",customer.state);
	printf("%-9c",customer.discCode);
	printf("%10.2f",customer.balanceDue);
	printf("%9.02i/%02i/%i\n", customer.date.month,
			customer.date.day, customer.date.year);
}


/*********************************************
*Print underline to format reports all pretty*
*********************************************/
void printUnderLine()
{
	printf("__________________________________________________________________________\n");
}
