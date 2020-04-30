#include<bits/stdc++.h>
#include<string.h>
// #include "RecordTextReader.cpp"
#include "LibsvmFileReader.cu"

#include <iostream>
#include <fstream>
using namespace std;

class CraftML
{
	public:
		
	vector<vector<float>> valuesX;
	vector<vector<string>> keyX;
	vector<vector<float>> valuesY;
	vector<vector<string>> keyY;
	
	void storeForLearning(unordered_map<string,float> x,unordered_map<string,float> y)
	{
		vector<float> valueX;
		vector<float> valueY;

		vector<string> indexX;
		vector<string> indexY;


		for (auto row : x) 
      	{
			indexX.push_back(row.first);
			valueX.push_back(row.second); 
		}

		for (auto row : y) 
      	{
			indexY.push_back(row.first);
			valueY.push_back(row.second); 
		}

		if(indexX.size()>0 && indexY.size()>0)
		{
			keyX.push_back(indexX);
			keyY.push_back(indexY);

			valuesX.push_back(valueX);
			valuesY.push_back(valueY);

		} 

	}
    void trainAlgoOnFileStandard(LibsvmFileReader &fileReader)
    {
        fileReader.openFile();
		
		// reinitialize();
		// SmallItem x;
		// SmallItem y;
		unordered_map<string,float> x;
		unordered_map<string,float> y;

		pair<unordered_map<string,float>,unordered_map<string,float>> x_y = fileReader.readNext();
		// boolean continueRead = fileReader.readNext();
		
		while(x_y.first.find("-1")==x_y.first.end())
		{
		// 	if(interrompre()) {
		// 		return;
		// 	}
			x = x_y.first;
			y = x_y.second;

			storeForLearning(x, y);
			x_y = fileReader.readNext();
		}
		cout<<keyX.size()<<endl;
		// for(int i=0;i<keyX.size();i++)
		// {
		// 	for(int j=0;j<keyX[i].size();j++)
		// 	{
		// 		cout<<keyX[i][j]<<" "<<valuesX[i][j]<<"; ";
		// 	}
		// 	cout<<endl;
		// }
		// indicationFinDePasse();
		// hasLearnt = true;
		// fileReader.closeFile();
    }
    

};



int main(int argc,char** argv)
{
	string trainFile=argv[1];

    CraftML model ;

    LibsvmFileReader readerTrain;
					
	readerTrain.setFile(trainFile);
					
	model.trainAlgoOnFileStandard(readerTrain);
    
    return 0;
}