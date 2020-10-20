#include<bits/stdc++.h>
#include <iostream>
#include <fstream>

using namespace std;

class RecordTextReader
{
    public:

    long nbReadLine=0;
	ifstream reader;
	string separatorRecord =" ";
	
	bool openFile(string filename)
	{
		
		cout<<"\nOpening file: "<<filename<<"\n";
		
		try
		{
			this->reader.open(filename);
			this->nbReadLine=0;
			return true;
		}
		catch(int x)
		{
			cout<<filename<<" can't be opened"<<endl;
		}
		return false;
	}

	vector<string> split(string s,string del)
	{
		stringstream ss(s);
		vector<string> str;
		string temp;
		while (getline(ss, temp, del[0])) { 
			str.push_back(temp); 
		} 
		return str;
	}

	vector<string> readPureRecord()
	{
		try
		{
			string line;
			if ( getline (this->reader,line))
			{
				
				this->nbReadLine++;
				vector<string> record=split(line,separatorRecord);
				return record;
			}
			this->reader.close();    
		}
		catch (int e)
		{
			cerr<<"Error"<<endl;
		}
        vector<string> record(2,"");
		return record;
	}
    
};