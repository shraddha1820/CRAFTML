#include<bits/stdc++.h>
#include <iostream>
#include <fstream>

using namespace std;

class RecordTextReader
{
    public:

    long nbReadLine=0;


	ifstream reader;

	string separatorInText = " ";
	string separatorRecord =" ";
	
    bool fistColumnIsID=false;
    bool cleanLineOption=true;
    bool lowerCase=true;
    bool upperCase=false;
    bool recordMode=true;
    bool tryMode=false;
	
	bool secondFieldIsText=true;
	
	
	bool deleteAfterSlash=true;
	

			
	// public string cleanText(string myText) {

	// 	 {
	// 		//line=line.trim();
	// 		if (lowerCase) {
	// 			myText=myText.toLowerCase();   
	// 		}
	// 		if (upperCase) {
	// 			myText=myText.toUpperCase();   
				
	// 		}
	// 		myText=myText.replaceAll("\t", " ");
	// 		myText=myText.replace("(", " ");
	// 		myText=myText.replace(")", " ");
	// 		myText=myText.replace(",", " ");
	// 		myText=myText.replace(";", " ");
	// 		myText=myText.replace(".", " ");    //TODO : v�rifier l'expresseion r�guli�re pour le point
	// 		myText=myText.replaceAll("    ", " ");
	// 		myText=myText.replaceAll("   ", " ");
	// 		myText=myText.replaceAll("  ", " ");
	// 		//line=line.replace(commandTag2, commandTag1);
			
	// 		if (deleteAfterSlash) {
	// 			int p;
	// 			p=myText.indexOf("/");
	// 			if (p>0) {  // pas retir� si en d�but de chaine, au cas o�
	// 				//System.out.println("TEXT:"+myText);
	// 				myText=myText.substring(0, p);
	// 			}
	// 			//System.out.println("TEXT:"+myText);
	// 			//System.exit(1);
	// 		}
			
			
	// 		myText=myText.trim();
	// 		return myText;
	// 	}
	// }
			
			

	// public string getSeparatorRecord() {
	// 	return separatorRecord;
	// }



	// public void setSeparatorRecord(string separatorRecord) {
	// 	this.separatorRecord = separatorRecord;
	// }



	bool openFile(string filename)
	{
		
		cout<<"\n Opening file: "<<filename<<"\n";
		
		
		/*
		System.out.println("ID in 1st column:"+fistColumnIsID);
		if (fistColumnIsID) {
			System.out.println("record separator:["+separatorRecord+"]");
		}
		System.out.println("with text separator:["+separatorInText+"]");
		System.out.println("cleaning :"+cleanLineOption);
		if (cleanLineOption) {
			if (lowerCase) {
			  System.out.println("     to lowercase");
			}
			if (upperCase) {
				System.out.println("    to upperCase");
			}
		}
		
		*/
		
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




	// public void closeFile()
	// {
	// 	if(reader != null) {
	// 		try {
	// 			reader.close();
	// 		} catch (IOException e) {
	// 			e.printStackTrace();
	// 		}
	// 	}
	// }
	
	
	// public string readLine(){
	// 	try
	// 	{
	// 		string line;
	// 		//System.out.println("va lire...");
	// 		if ((line = reader.readLine()) != null)
	// 		{
	// 			return line;
	// 		}
	// 	}
	// 	catch (Exception e)
	// 	{
	// 			e.printStackTrace();
	// 	}
	// 	return null;
	// }
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
			//System.out.println("va lire...");
			if ( getline (this->reader,line))
			{
				
				this->nbReadLine++;
				// if (tryMode) {
				// 	if (nbReadLine>5) {
				// 		Displayer.displayText("TRY MODE: exit");
				// 		reader.close();
				// 		return null;
				// 	}
				// }
				
				//Displayer.displayText("ligne lue:["+line+"]");
				
				// string token = s.substr(0, s.find(delimiter));
				vector<string> record=split(line,separatorRecord);
				// for(auto it: record)
				// {
				// 	cout<<it<<" ";
				// }
				// cout<<endl;
				/*
				 Iterable<string> cutLine = Splitter.on(separatorRecord).split(line);				
				string[] record = Iterables.toArray(cutLine, string.class);
				 */
				
				return record;
			}
			this->reader.close();    
		}
		catch (int e)
		{
				// e.printStackTrace();
		}
        vector<string> record(2,"");
		return record;
	}

	
	// public string[] readLineOrRecord()
	// {
	// 	try
	// 	{
	// 		string line;
	// 		//System.out.println("va lire...");
	// 		if ((line = reader.readLine()) != null)
	// 		{
				
	// 			nbReadLine++;
	// 			if (tryMode) {
	// 				if (nbReadLine>5) {
	// 					Displayer.displayText("TRY MODE: exit");
	// 					reader.close();
	// 					return null;
	// 				}
	// 			}
				
	// 			//Displayer.displayText("ligne lue:["+line+"]");
				
				
	// 			string[] record;
	// 			if (recordMode) {
	// 				if (!fistColumnIsID) {
	// 					record= line.split(separatorInText);
	// 				} else {
	// 					string[] idAndText=line.split(separatorRecord);
	// 					//if (cleanLineOption) {
	// 					//	idAndText[1]=cleanLine(idAndText[1]);
	// 					//	System.out.println("ligne 'clean�e' :["+idAndText[1]+"]");
	// 					//}
	// 					string[] textRecord;
	// 					if (idAndText.length>1) {
	// 						 textRecord=idAndText[1].split(separatorInText);
	// 					} else {
	// 						textRecord=new string[1];
	// 						textRecord[0]="";
	// 					}
	// 					if (cleanLineOption) {
	// 						for (int i=0;i<textRecord.length;i++) {
	// 							textRecord[i]=cleanText(textRecord[i]);
	// 						}
	// 					}
	// 					record=new string[textRecord.length+1];
	// 					record[0]=idAndText[0];
	// 					for (int i=0;i<textRecord.length;i++) {
	// 						record[i+1]=textRecord[i];
	// 					}
	// 				}
					
	// 			} else {
	// 				record=new string[1];
	// 				if (cleanLineOption) {
	// 					line=cleanText(line);
	// 					//Displayer.displayText("ligne 'clean�e' :["+line+"]");
	// 				}
	// 				record[0]=line;
	// 			}
	// 			return record;
	// 		}
	// 		reader.close();    
	// 	}
	// 	catch (Exception e)
	// 	{
	// 			e.printStackTrace();
	// 	}
	// 	return null;
	// }
	
	// static public void printRecord(string[] record,string separatorOut) {
	// 	if (record!=null) {
	// 		for (int i=0; i<record.length;i++) {
	// 			//Displayer.displayText(record[i]);
	// 			//Displayer.displayText(separatorOut);
	// 		}
	// 	}
	// 	//Displayer.displayText("");
	// }
    
};