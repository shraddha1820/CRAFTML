#include<bits/stdc++.h>
#include<string.h>
#include "RecordTextReader.cu"

using namespace std;
class LibsvmFileReader
{
    public:
    bool header = true;
	
	string inputFieldSeparator = " ";
	string inputLabelSeparator = ",";
	string inputKeyValueSeparator = ":";
	
	// SmallItem currentX;
	// SmallItem currentY;
	
	
	RecordTextReader myRecordReader;
	
	string filePath = "";


    void setFile(string filePath) {
		this->filePath = filePath;
		
	}


    void openFile() {
		// initializing reader
		// myRecordReader = new RecordTextReader();

		myRecordReader.openFile(filePath);

		// reading header
		if(header){
			vector<string> a;
			a = myRecordReader.readPureRecord();
			// cout<<a[0]<<endl<<a[1]<<endl;
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
	pair<unordered_map<string,float>,unordered_map<string,float>> readNext() {
		vector<string> line, lineY, valX;
		// currentX = new SmallItem();
		// currentY = new SmallItem();
		unordered_map<string,float> currentX;
		unordered_map<string,float> currentY;

		line = myRecordReader.readPureRecord();
		if(!line[0].empty() || !line[1].empty()){
			if (line[0].compare("")!=0) {
				lineY = split(line[0],inputLabelSeparator);
				for (int j = 0; j < lineY.size(); j++) {
					currentY[lineY[j]] = 1.0;
				}
			}
			
			for (int j = 1; j < line.size(); j++) {
				valX = split(line[j],inputKeyValueSeparator);
				currentX[valX[0]] = stof(valX[1]);
			}
			
		}
		else
		{
			cout<<myRecordReader.nbReadLine<<endl;
			currentX["-1"]=1.0;
			currentY["-1"]=1.0;
		}
		// pair_ = make_pair(currentX,currentY);
		return make_pair(currentX,currentY);	
	}
	
	
	// public int countLines() {
	//     InputStream is = null;
	// 	try {
	// 		is = new BufferedInputStream(new FileInputStream(filePath));
	// 	} catch (FileNotFoundException e) {
	// 		// TODO Auto-generated catch block
	// 		e.printStackTrace();
	// 	}
	//     try {
	//         byte[] c = new byte[1024];
	//         int count = 0;
	//         int readChars = 0;
	//         boolean empty = true;
	//         try {
	// 			while ((readChars = is.read(c)) != -1) {
	// 			    empty = false;
	// 			    for (int i = 0; i < readChars; ++i) {
	// 			        if (c[i] == '\n') {
	// 			            ++count;
	// 			        }
	// 			    }
	// 			}
	// 		} catch (IOException e) {
	// 			// TODO Auto-generated catch block
	// 			e.printStackTrace();
	// 		}
	//         return (count == 0 && !empty) ? 1 : count;
	//     } finally {
	//         try {
	// 			is.close();
	// 		} catch (IOException e) {
	// 			// TODO Auto-generated catch block
	// 			e.printStackTrace();
	// 		}
	//     }
	// }
	

	// public SmallItem getX() {
	// 	return currentX;
	// }

	// public SmallItem getY() {
	// 	return currentY;
	// }
	
	// public void closeFile(){
	// 	myRecordReader.closeFile();
	// }

};
    
