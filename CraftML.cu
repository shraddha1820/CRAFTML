#include<bits/stdc++.h>
#include<string.h>
// #include "RecordTextReader.cpp"
#include "LibsvmFileReader.cu"
#include<math.h>
#include <iostream>
#include <fstream>
#include<cuda_runtime.h>
// #include "cuPrintf.cu"
// #include "cuPrintf.cuh"

#include <thrust/device_vector.h>
#include <thrust/copy.h>

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
		// cout<<"No. of rows "<<keyY.size()<<endl;
		
		// for(int i=0;i<5;i++)
		// {
		// 	for(int j=0;j<keyY[i].size();j++)
		// 	{
		// 		cout<<keyY[i][j]<<":"<<valuesY[i][j]<<" ";
		// 	}
		// 	cout<<endl;
		// }
			

		// indicationFinDePasse();
		// hasLearnt = true;
		// fileReader.closeFile();
    }
    

};
struct Args_device
{
	float** valuesX;
	float** valuesY;
	int** indexX;
	int** indexY;
	int* useInst;
	int dimProjX;
	int dimProjY;
	int sparsity;
	int seedX;
	int seedY;
	// int len;
	// int *lenX;
	// int *lenY;
};

__device__ int get_length(int key)
{
	int len = 1;
	while(key>9)
	{
		len++;
		key/=10;
	}
	return len;
}

__device__ uint32_t MurmurHash2 ( const void * key, int len, uint32_t seed )
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

__device__ int getIndex(int key,int seed,int sizeMax){
	int len = get_length(key)+3;

	char* hashkey=new char[len];

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
__device__ float getSign(int key,int seed){
	const int len = get_length(key)+3;

	char* hashkey=new char[len];

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
struct Rand_proj
{
	int *a[1000];
	// int *b[1000];
};
__global__ void buildTree(struct Args_device *d_args,struct Rand_proj *out)
{
	// *var=d_args->lenX[1000];
	for(int j = 0; j<d_args->useInst[0];j++){
		xKey = keyX.get(useInst.get(j));
		xValues = valuesX.get(useInst.get(j));

		if(xKey!=null) {

			projectedX = new float[dimXProj];

			for(int i = 0;i<xKey.length;i++){
				currentIndex = getIndex(xKey[i], seedIndexX, dimXProj);
				currentSign = getSign(xKey[i], seedSignX);
				projectedX[currentIndex] = projectedX[currentIndex] + currentSign * xValues[i];
			}

			float norm = 0;
			int nonZeros = 0;

			for(int i = 0;i<projectedX.length;i++){
				if(projectedX[i] != 0f) {
					norm += projectedX[i]*projectedX[i];
					nonZeros++;
				}
			}
		}
	}

}
int main(int argc,char** argv)
{
	string trainFile=argv[1];

    CraftML model ;

    LibsvmFileReader readerTrain;
					
	readerTrain.setFile(trainFile);

	model.trainAlgoOnFileStandard(readerTrain);

	struct Args_device *args;
	struct Args_device *d_args;
	struct Args_device *id1_args;
	struct Args_device *id2_args;

	args = (struct Args_device *)malloc(sizeof(struct Args_device));
	id1_args = (struct Args_device *)malloc(sizeof(struct Args_device));
	id2_args = (struct Args_device *)malloc(sizeof(struct Args_device));
	cudaMalloc((void **)&d_args,sizeof(struct Args_device));


	args->indexX = (int **)malloc(model.keyX.size()*sizeof(int *));
	cudaMalloc((void **)&(id1_args->indexX),model.keyX.size()*sizeof(int *));
	id2_args->indexX = (int **)malloc(model.keyX.size()*sizeof(int *));

	
	args->valuesX = (float **)malloc(model.keyX.size()*sizeof(float *));
	cudaMalloc((void **)&(id1_args->valuesX),model.keyX.size()*sizeof(float *));
	id2_args->valuesX = (float **)malloc(model.keyX.size()*sizeof(float *));
	

	for(int i=0;i<model.keyX.size();i++)
	{
		args->indexX[i]=(int *)malloc((model.keyX[i].size()+1)*sizeof(int));
		cudaMalloc((void **)&(id2_args->indexX[i]),(model.keyX[i].size()+1)*sizeof(int));

		args->valuesX[i]=(float *)malloc((model.keyX[i].size()+1)*sizeof(float));
		cudaMalloc((void **)&(id2_args->valuesX[i]),(model.keyX[i].size()+1)*sizeof(float));

		args->indexX[i][0]=model.keyX[i].size();
		args->valuesX[i][0]=model.keyX[i].size();

		for(int j=0;j<model.keyX[i].size();j++)
		{
			args->indexX[i][j+1] = model.keyX[i][j];
			args->valuesX[i][j+1] = model.valuesX[i][j];
		}
		cudaMemcpy(id2_args->indexX[i],args->indexX[i],model.keyX[i].size()+1,cudaMemcpyHostToDevice);
		cudaMemcpy(id2_args->valuesX[i],args->valuesX[i],model.keyX[i].size()+1,cudaMemcpyHostToDevice);
	}
	cudaMemcpy(id1_args->indexX,id2_args->indexX,model.keyX.size(),cudaMemcpyHostToDevice);
	cudaMemcpy(id1_args->valuesX,id2_args->valuesX,model.keyX.size(),cudaMemcpyHostToDevice);

	args->indexY = (int **)malloc(model.keyY.size()*sizeof(int *));
	cudaMalloc((void **)&(id1_args->indexY),model.keyY.size()*sizeof(int *));
	id2_args->indexY = (int **)malloc(model.keyY.size()*sizeof(int *));

	args->valuesY = (float **)malloc(model.keyY.size()*sizeof(float *));
	cudaMalloc((void **)&(id1_args->valuesY),model.keyY.size()*sizeof(float *));
	id2_args->valuesY = (float **)malloc(model.keyY.size()*sizeof(float *));

	for(int i=0;i<model.keyY.size();i++)
	{
		args->indexY[i]=(int *)malloc((model.keyY[i].size()+1)*sizeof(int));
		cudaMalloc((void **)&(id2_args->indexY[i]),(model.keyY[i].size()+1)*sizeof(int));

		args->valuesY[i]=(float *)malloc((model.keyY[i].size()+1)*sizeof(float));
		cudaMalloc((void **)&(id2_args->valuesY[i]),(model.keyY[i].size()+1)*sizeof(float));

		args->indexY[i][0]=model.keyY[i].size();
		args->valuesY[i][0]=model.keyY[i].size();

		for(int j=0;j<model.keyY[i].size();j++)
		{
			args->indexY[i][j+1] = model.keyY[i][j];
			args->valuesY[i][j+1] = model.valuesY[i][j];
		}
		cudaMemcpy(id2_args->indexY[i],args->indexY[i],model.keyY[i].size()+1,cudaMemcpyHostToDevice);
		cudaMemcpy(id2_args->valuesY[i],args->valuesY[i],model.keyY[i].size()+1,cudaMemcpyHostToDevice);

	}
	cudaMemcpy(id1_args->indexY,id2_args->indexY,model.keyY.size(),cudaMemcpyHostToDevice);
	cudaMemcpy(id1_args->valuesY,id2_args->valuesY,model.keyY.size(),cudaMemcpyHostToDevice);

	args->useInst = (int *)malloc((model.keyY.size()+1)*sizeof(int));
	args->useInst[0]=keyX.size();
	for(int i=0;i<model.keyX.size();i++)
		args->useInst[i+1] = i;
	cudaMalloc((void **)&(id1_args->useInst),(model.keyY.size()+1)*sizeof(int));
	cudaMemcpy(id1_args->useInst,args->useInst,model.keyX.size()+1,cudaMemcpyHostToDevice);
	
	// args->lenX = (int *)malloc(model.keyY.size()*sizeof(int));
	// for(int i=0;i<model.keyX.size();i++)
	// 	args->lenX[i] = model.keyX[i].size();
	// cudaMalloc((void **)&(id1_args->lenX),model.keyY.size()*sizeof(int));
	// cudaMemcpy(id1_args->lenX,args->lenX,model.keyX.size(),cudaMemcpyHostToDevice);
	
	// args->lenY = (int *)malloc(model.keyY.size()*sizeof(int));
	// for(int i=0;i<model.keyX.size();i++)
	// 	args->lenY[i] = model.keyY[i].size();
	// cudaMalloc((void **)&(id1_args->lenY),model.keyY.size()*sizeof(int));
	// cudaMemcpy(id1_args->lenY,args->lenY,model.keyX.size(),cudaMemcpyHostToDevice);
	
	id1_args->dimProjX=1000;
	id1_args->dimProjY=1000;
	id1_args->sparsity=100;
	id1_args->seedX=rand()%100;
	id1_args->seedY=rand()%100;
	// id1_args->len = model.keyX.size();

	cudaMemcpy(d_args,id1_args,sizeof(struct Args_device),cudaMemcpyHostToDevice);

	struct Rand_proj *out;
	cudaMalloc((void **)&out,sizeof(struct Rand_proj));
	buildTree<<<1,1>>>(d_args,out);
	
	// struct Rand_proj *from_cuda=(struct Rand_proj*)malloc(sizeof(struct Rand_proj));

	// cudaMemcpy(from_cuda,out,sizeof(struct Rand_proj),cudaMemcpyDeviceToHost);
	
	// for()
	// cout<<*spar<<endl;

	// cout<<model.keyX[1000].size()<<endl;
	// cout<<getIndex(1,78,1000)<<endl;
	
	// cout<<getSign(1,78)<<endl;
	
    return 0;
}