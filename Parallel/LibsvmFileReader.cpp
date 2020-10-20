#include<bits/stdc++.h>
#include<string.h>
#include "RecordTextReader.cpp"

using namespace std;
class LibsvmFileReader
{
    public:
    bool header = true;
	
	string inputFieldSeparator = " ";
	string inputLabelSeparator = ",";
	string inputKeyValueSeparator = ":";

	RecordTextReader myRecordReader;
	
	string filePath = "";


    void setFile(string filePath) {
		this->filePath = filePath;
		
	}


    void openFile() {
		myRecordReader.openFile(filePath);

		// reading header
		if(header){
			vector<string> a;
			a = myRecordReader.readPureRecord();
		}		

	}
	
	bool isReady(){
		return filePath.length()!=0;
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
	pair<unordered_map<int,float>,unordered_map<int,float>> readNext() {
		vector<string> line, lineY, valX;
		unordered_map<int,float> currentX;
		unordered_map<int,float> currentY;
		line = myRecordReader.readPureRecord();
		if(!line[0].empty() || !line[1].empty()){
			if (line[0].compare("")!=0) {
				lineY = split(line[0],inputLabelSeparator);
				for (int j = 0; j < lineY.size(); j++) {
					currentY[stoi(lineY[j])] = 1.0;
				}
			}
			
			for (int j = 1; j < line.size(); j++) {
				valX = split(line[j],inputKeyValueSeparator);
				currentX[stoi(valX[0])] = stof(valX[1]);
			}
			
		}
		else
		{
			cout<<myRecordReader.nbReadLine<<endl;
			currentX[-1]=1.0;
			currentY[-1]=1.0;
		}
		return make_pair(currentX,currentY);	
	}
};
    
