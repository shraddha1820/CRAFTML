#include<bits/stdc++.h>
#include<string.h>
// #include "RecordTextReader.cpp"
#include "LibsvmFileReader.cu"
#include<math.h>
#include <iostream>
#include <fstream>
using namespace std;

uint32_t MurmurHash2 ( const void * key, int len, uint32_t seed );
class CraftML
{
	public:
		
	vector<vector<float>> valuesX;
	vector<vector<int>> keyX;
	vector<vector<float>> valuesY;
	vector<vector<int>> keyY;
	
	void storeForLearning(unordered_map<int,float> x,unordered_map<int,float> y)
	{
		vector<float> valueX;
		vector<float> valueY;

		vector<int> indexX;
		vector<int> indexY;


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
		unordered_map<int,float> x;
		unordered_map<int,float> y;

		pair<unordered_map<int,float>,unordered_map<int,float>> x_y = fileReader.readNext();
		// boolean continueRead = fileReader.readNext();
		
		while(x_y.first.find(-1)==x_y.first.end())
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

int get_length(int key)
{
	int len = 1;
	while(key>9)
	{
		len++;
		key/=10;
	}
	return len;
}

uint32_t MurmurHash2 ( const void * key, int len, uint32_t seed )
{
  /* 'm' and 'r' are mixing constants generated offline.
     They're not really 'magic', they just happen to work well.  */

  const uint32_t m = 0x5bd1e995;
  const int r = 24;

  /* Initialize the hash to a 'random' value */

  uint32_t h = seed ^ len;

  /* Mix 4 bytes at a time into the hash */

  const unsigned char * data = (const unsigned char *)key;

  while(len >= 4)
  {
    uint32_t k = *(uint32_t*)data;

    k *= m;
    k ^= k >> r;
    k *= m;

    h *= m;
    h ^= k;

    data += 4;
    len -= 4;
  }

  /* Handle the last few bytes of the input array  */

  switch(len)
  {
  case 3: h ^= data[2] << 16;
  case 2: h ^= data[1] << 8;
  case 1: h ^= data[0];
      h *= m;
  };

  /* Do a few final mixes of the hash to ensure the last few
  // bytes are well-incorporated.  */

  h ^= h >> 13;
  h *= m;
  h ^= h >> 15;

  return h;
} 

int getIndex(int key,int seed,int sizeMax){
	int len = get_length(key)+3;

	char hashkey[len];

	//key="azv"

	hashkey[0]='a';
	hashkey[1]='z';
	hashkey[2]='v';

	int i=1;
	while(key>0)
	{
		hashkey[len-i]=(char)(key%10+48);
		i++;
		key/=10;
	}

	return abs((int)MurmurHash2(hashkey,len,seed) % sizeMax);
}
float getSign(int key,int seed){
	int len = get_length(key)+3;

	char hashkey[len];

	//key="azv"

	hashkey[0]='a';
	hashkey[1]='z';
	hashkey[2]='v';

	int i=1;
	while(key>0)
	{
		hashkey[len-i]=(char)(key%10+48);
		i++;
		key/=10;
	}


	return (float)abs((int)MurmurHash2(hashkey, len, seed) % 2)*2 - 1;
}


int main(int argc,char** argv)
{
	string trainFile=argv[1];

    CraftML model ;

    LibsvmFileReader readerTrain;
					
	readerTrain.setFile(trainFile);

	// model.trainAlgoOnFileStandard(readerTrain);

	cout<<getIndex(1,78,1000)<<endl;
	
	cout<<getSign(1,78)<<endl;

    
    return 0;
}