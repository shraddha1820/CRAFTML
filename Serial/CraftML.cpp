#include<bits/stdc++.h>
#include<string.h>
#include "LibsvmFileReader.cpp"
#include<math.h>
#include <iostream>
#include <fstream>

#include <chrono> 

using namespace std;
using namespace std::chrono; 

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
		
		unordered_map<int,float> x;
		unordered_map<int,float> y;

		pair<unordered_map<int,float>,unordered_map<int,float>> x_y = fileReader.readNext();

		while(x_y.first.find(-1)==x_y.first.end())
		{
			x = x_y.first;
			y = x_y.second;

			storeForLearning(x, y);
			x_y = fileReader.readNext();
		}
		cout<<"No. of rows "<<keyY.size()<<endl;
		int max_=keyX[1].size();
		for(int i=0;i<keyX.size();i++)
		{
			for(int j=0;j<keyX[i].size();j++)
			{
				if(max_<keyX[i][j])
				{
					max_=keyX[i][j];
				}
			}
		}
		cout<<"Max features "<<max_<<endl;
		max_=keyY[1].size();
		for(int i=0;i<keyY.size();i++)
		{
			for(int j=0;j<keyY[i].size();j++)
			{
				if(max_<keyY[i][j])
				{
					max_=keyY[i][j];
				}
			}
		}
		cout<<"Max labels "<<max_<<endl;

    }
};
struct Args_device
{
	float **valuesX;
	float **valuesY;
	int **keyX;
	int **keyY;
	int *useInst;

	int dimXProj;
	int dimYProj;
	int sparsity;
	int seedX;
	int seedY;
	int minInst;
};
struct cluster
{
	float **clusters;
	int *chosenClus;
	int *indexBelong;
};
struct Node
{
	bool Leaf;
	float *labels;
	float **center_valuesX;
	int **center_indexesX;

	struct Node* children[10];
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

	char* hashkey=new char[len];
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
int getSign(int key,int seed){
	const int len = get_length(key)+3;

	char* hashkey=new char[len];
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

	return (int)abs((int)MurmurHash2(hashkey, len, seed) % 2)*2 - 1;
}

float cosineDistance(float* valuesA, int* indexesA, float* b)
{
	float dot = 0;
	for(int i = 1; i <= valuesA[0];i++)
	{
		dot = dot + valuesA[i]*b[indexesA[i]];
	}
	return  (1-dot);
}
int getClusterIndex(float value,float* valuesArray,int indexDebut,int indexFin){
		
	if(indexDebut == indexFin-1){
		return indexDebut;
	}else{
		int indexCheck = (indexDebut+indexFin)/2;
		if(value>valuesArray[indexCheck]){
			return getClusterIndex(value, valuesArray, indexCheck, indexFin);
		}else{
			return getClusterIndex(value, valuesArray, indexDebut, indexCheck);
		}
	}
}
void initializecluster(struct cluster *cl,float **dataValues,int **dataIndexes,int nbInst)
{
	for(int i=0;i<10;i++)
	{
		for(int j=0;j<2000;j++)
		{
			cl->clusters[i][j]=0;
		}
	}

	int index = rand()%nbInst;
	
	cl->chosenClus[0] = index;
	
	float *currentValues = new float[(int)dataValues[index][0]+1];
	memcpy(currentValues,dataValues[index],sizeof(float)*((int)dataValues[index][0]+1));

	int *currentIndexes = new int[dataIndexes[index][0]+1];
	memcpy(currentIndexes,dataIndexes[index],sizeof(int)*(dataIndexes[index][0]+1));
	
	for(int i = 1;i<=currentIndexes[0];i++){
		cl->clusters[0][currentIndexes[i]] = currentValues[i];
	}
	delete currentValues;
	delete currentIndexes;

	float* probaCum = new float[nbInst+1];
	probaCum[0]=0;
	
	float currentDist = 0;
	
	float currentRandom;
	
	for(int i = 1; i < 10;i++)
	{
		for(int j = 0;j<nbInst;j++)
		{
			float min_=FLT_MAX;
			for(int k=0;k<i;k++)
			{
				float dist=cosineDistance(dataValues[j],dataIndexes[j],cl->clusters[k]);
				if(dist<min_)
					min_=dist;
			}
			currentDist = min_;
			probaCum[j+1] = probaCum[j] + currentDist*currentDist;
		}
		currentRandom =((float)rand()/RAND_MAX)*probaCum[nbInst];
		
		
		index = getClusterIndex(currentRandom,probaCum,0,nbInst);
		cl->chosenClus[i] = index;
		
		float *currentValues = new float[(int)dataValues[index][0]+1];
		memcpy(currentValues,dataValues[index],sizeof(float)*((int)dataValues[index][0]+1));

		int *currentIndexes = new int[dataIndexes[index][0]+1];
		memcpy(currentIndexes,dataIndexes[index],sizeof(int)*(dataIndexes[index][0]+1));
	
		for(int k = 1;k<=currentIndexes[0];k++){
			cl->clusters[i][currentIndexes[k]] = currentValues[k];
		}
		delete currentValues;
		delete currentIndexes;
		
		
	}

}
void updatecluster(struct cluster *cl,float **dataValues,int **dataIndexes,int nbInst,int nbPass)
{
	cl->indexBelong[0] = nbInst;
	float newClusters[10][2000];
	int indexClusterCurrent;
	
	for(int pass = 0; pass < nbPass; pass++)
	{		
		int *nbInstClust =new int[10]();
		for(int i=0;i<10;i++)
		{
			for(int j=0;j<2000;j++)
			{
				newClusters[i][j]=0;
			}
		}
		
		for(int i = 0;i<nbInst;i++)
		{
			float min_=FLT_MAX;
			indexClusterCurrent = 0;
			for(int k=0;k<10;k++)
			{
				float dist=cosineDistance(dataValues[i],dataIndexes[i],cl->clusters[k]);
				if(dist<min_)
				{	
					min_=dist;
					indexClusterCurrent = k;
				}
			}

			cl->indexBelong[i+1] = indexClusterCurrent;
			nbInstClust[indexClusterCurrent]++;
			
			float *currentValues = new float[(int)dataValues[i][0]+1];
			memcpy(currentValues,dataValues[i],sizeof(float)*((int)dataValues[i][0]+1));

			int *currentIndexes = new int[dataIndexes[i][0]+1];
			memcpy(currentIndexes,dataIndexes[i],sizeof(int)*(dataIndexes[i][0]+1));
				
			for(int k = 1;k<=currentIndexes[0];k++)
			{
				newClusters[indexClusterCurrent][currentIndexes[k]] = newClusters[indexClusterCurrent][currentIndexes[k]] + currentValues[k];
			}
			delete currentValues;
			delete currentIndexes;
							
		}
		
		for(int i = 0;i<10;i++)
		{
			for(int j = 0;j<2000;j++)
			{
				cl->clusters[i][j] = newClusters[i][j]/nbInstClust[i];
			}	
		}
		delete nbInstClust;
		
	}
}

void computeClusters(struct cluster *cl,float **val,int **ind,int iter,int length)
{
	initializecluster(cl,val,ind,length);
	updatecluster(cl,val,ind,length,iter);
}

void computeSeparateInstances(int **keyX,float **valuesX,int *useInst,int **keyY,float **valuesY,int seedY,int dimYProj,int dimXProj,int sparsity,int **childrenInstancesIndex,float **childrenInstancesValues)
{
	int maxreservoir = 10000;
	int reservoirSize = min(useInst[0],maxreservoir);

	int *reservoirIndex = new int[reservoirSize+1];
	//reservoir sampling
	if(useInst[0] < maxreservoir){
		reservoirIndex[0]=useInst[0];
		for(int i = 1;i<=useInst[0];i++){
			reservoirIndex[i] = useInst[i];
		}
	}
	else
	{
		reservoirIndex[0]=maxreservoir;

		for(int i = 1;i<=maxreservoir;i++){
			reservoirIndex[i] = useInst[i];
		}
		for(int i = maxreservoir+1;i<=useInst[0];i++){
			int index = rand()%useInst[0];
			if(index <= maxreservoir && index>0)
			{
				reservoirIndex[index] = useInst[i];
			}
		}
	}
	int** indexesY = new int*[reservoirIndex[0]];
	float** newValuesY = new float*[reservoirIndex[0]];
	
	int currentIndex;
	int currentSign;
	

	for(int j = 1; j<=reservoirIndex[0];j++)
	{
		int* yKey;
		float* yValues;
		float* projectedY;
		yKey = new int[keyY[reservoirIndex[j]][0]+1];
		yValues = new float[(int)valuesY[reservoirIndex[j]][0]+1];

		memcpy(yKey, keyY[reservoirIndex[j]], sizeof(int)*(keyY[reservoirIndex[j]][0]+1));
		memcpy(yValues, valuesY[reservoirIndex[j]], sizeof(float)*(keyY[reservoirIndex[j]][0]+1));
		
		projectedY = new float[dimYProj]();

		if(yKey[0]!=0) 
		{
			for(int i = 1;i<=yKey[0];i++)
			{
				currentIndex = getIndex(yKey[i], seedY, dimYProj);
				currentSign = getSign(yKey[i], seedY);
				projectedY[currentIndex] = projectedY[currentIndex] + currentSign * yValues[i];
			}
		}

		float norm = 0;
		int nonZeros = 0;
		for(int i = 0;i<dimYProj;i++)
		{
			if(abs(projectedY[i]) > 0.0001) 
			{
					norm += projectedY[i]*projectedY[i];
					nonZeros++;
			}
		}
		norm = (float) sqrt(norm);

		if(norm < 0.0001) 
		{
			indexesY[j-1] = new int[2];
			indexesY[j-1][0]=1;
			indexesY[j-1][1]=rand()%dimYProj;

			newValuesY[j-1] = new float[2];
			newValuesY[j-1][0]=1;
			newValuesY[j-1][1]=1;

		}
		else 
		{
			int* currIndexes = new int[nonZeros+1];
			float* currValues = new float[nonZeros+1];
			currIndexes[0]=nonZeros;
			currValues[0]=nonZeros;
			int k = 1;
			for(int i = 0;i<dimYProj;i++){
				if(abs(projectedY[i]) > 0.0001) {
					currIndexes[k] = i;
					currValues[k] = projectedY[i]/norm;
					k++;
				}
			}
			indexesY[j-1] = new int[nonZeros+1];
			newValuesY[j-1] = new float[nonZeros+1];

			memcpy(indexesY[j-1], currIndexes, sizeof(int)*(nonZeros+1));
			memcpy(newValuesY[j-1], currValues, sizeof(float)*(nonZeros+1));
				
			delete currIndexes;
			delete currValues;
		}

		delete projectedY;
		delete yKey;
		delete yValues;
	}

	struct cluster *cl=new struct cluster[1]; 
	cl->clusters=new float*[10];
	for(int i=0;i<10;i++)
	{
		cl->clusters[i]=new float[dimYProj];
	}
	cl->chosenClus=new int[10];
	cl->indexBelong=new int[reservoirIndex[0]+1];

	computeClusters(cl,newValuesY,indexesY, 5,reservoirIndex[0]);
	
	int indexCurrentClust;
	
	float currentX[dimXProj];

	float childrenInstancesCurrent [10][dimXProj];
	
	for(int i=0;i<10;i++)
	{
		for(int j=0;j<dimXProj;j++)
		{
			childrenInstancesCurrent[i][j]=0;
		}
	}
	
	int *nbInst=new int[10]();

	for(int i=1;i<=reservoirSize;i++)
	{
		indexCurrentClust = cl->indexBelong[i];

		for(int j=0;j<dimXProj;j++)
			currentX[j]=0;
		
		

		for(int j=1;j<=keyX[reservoirIndex[i]][0];j++)
		{
			currentX[keyX[reservoirIndex[i]][j]]= valuesX[reservoirIndex[i]][j];
		}
		
		nbInst[indexCurrentClust]++;

		for(int j = 0;j<dimXProj;j++)
		{
			childrenInstancesCurrent[indexCurrentClust][j] = childrenInstancesCurrent[indexCurrentClust][j] + currentX[j];
		}
	}
	for(int i=0;i<10;i++)
	{
		delete cl->clusters[i];
	}
	delete cl->clusters;
	delete cl->chosenClus;
	delete cl->indexBelong;
	delete cl;

	for(int k=0;k<10;k++)
	{
		if(nbInst[k]!=0)
		{	
			float *fullInstance =new float[dimXProj];
			memcpy(fullInstance,childrenInstancesCurrent[k],sizeof(float)*dimXProj);

			int* bestIndexes = new int[sparsity];
			float* bestValues = new float[sparsity];

			int indexMin=0;
			float valueMin;

			for(int i = 0;i<sparsity;i++) 
			{
				bestValues[i] = abs(fullInstance[i]);
				bestIndexes[i] = i;
				if(bestValues[i]<bestValues[indexMin])
				{
					indexMin=i;
				}
			}

			valueMin = bestValues[indexMin];

			for(int i = sparsity;i<dimXProj;i++) 
			{
				if(abs(fullInstance[i]) > valueMin) 
				{
					bestValues[indexMin] = abs(fullInstance[i]);
					bestIndexes[indexMin] = i;
					for(int i=0;i<sparsity;i++)
					{
						if(bestValues[i]<bestValues[indexMin])
						{
							indexMin=i;
						}
					}
					valueMin = bestValues[indexMin];
				}
			}

			for(int i = 0;i<sparsity;i++) {
				 bestValues[i] = fullInstance[bestIndexes[i]];
			}

			float normvec = 0.0;
			int nonzeros = 0;

			for(int i = 0;i<sparsity;i++) {
				if(bestValues[i] > 0.000001){
					nonzeros++;
					normvec = normvec + bestValues[i]*bestValues[i];
				}
			}

			normvec = (float)sqrt(normvec);

			int* bestIndexesSparse = new int[nonzeros+1];
			float* bestValuesSparse = new float[nonzeros+1];

			bestIndexesSparse[0]=nonzeros;
			bestValuesSparse[0]=nonzeros;

			int j = 1;
			for(int i = 0;i<sparsity;i++) {
				if(bestValues[i] > 0.000001){
					bestIndexesSparse[j] = bestIndexes[i];
					bestValuesSparse[j] = bestValues[i]/normvec;
					j++;
				}
			}

			childrenInstancesIndex[k]=new int[nonzeros+1];
			memcpy(childrenInstancesIndex[k],bestIndexesSparse,sizeof(int)*(nonzeros+1));

			childrenInstancesValues[k]=new float[nonzeros+1];
			memcpy(childrenInstancesValues[k],bestValuesSparse,sizeof(float)*(nonzeros+1));
		}
		else
		{
			childrenInstancesIndex[k]=new int[1];
			childrenInstancesIndex[k][0]=0;

			childrenInstancesValues[k]=new float[1];
			childrenInstancesValues[k][0]=0;
		}
	}
}

bool sameVector(float *x1,float *x2)
{
	if(x1[0]!=x2[0])
		return false;

	for(int i=1;i<=x1[0];i++)
	{
		if(x1[i]!=x2[i])
			return false;
	}
	return true;
}
bool checkSame(float** valX,float** valY,int *useInst)
{
	bool check=true;
	for(int i=2;i<=useInst[0];i++)
	{
		if(!sameVector( valX[useInst[1]], valX[useInst[i]] ))
			check=false;
	}
	if(check)
		return check;
	
	check=true;
	for(int i=2;i<=useInst[0];i++)
	{
		if(!sameVector( valY[useInst[1]], valY[useInst[i]] ))
			check=false;
	}

	return check;
}

float sparseDotProduct(float *x,int *ind,float *y)
{
	float out=0;
	for(int i=1;i<=ind[0];i++)
	{
		out+=x[ind[i]]*y[i];
	}
	return out;
}
void projectVector(struct Args_device *d_args, int** indexesX, float **newValuesX)
{
	int currentIndex;
	int currentSign;
	
	for(int j = 1; j<=d_args->useInst[0];j++)
	{
		int* xKey;
		float* xValues;
		float* projectedX;
		xKey = new int[d_args->keyX[d_args->useInst[j]][0]+1];
		xValues = new float[(int)d_args->valuesX[d_args->useInst[j]][0]+1];

		memcpy(xKey, d_args->keyX[d_args->useInst[j]], sizeof(int)*(d_args->keyX[d_args->useInst[j]][0]+1));
		memcpy(xValues, d_args->valuesX[d_args->useInst[j]], sizeof(float)*(int)(d_args->valuesX[d_args->useInst[j]][0]+1));
		
		if(xKey[0]!=0) 
		{
			projectedX = new float[d_args->dimXProj]();
			for(int i = 1;i<=xKey[0];i++)
			{
				currentIndex = getIndex(xKey[i], d_args->seedX, d_args->dimXProj);
				currentSign = getSign(xKey[i], d_args->seedX);
				projectedX[currentIndex] = projectedX[currentIndex] + currentSign * xValues[i];
			}
			
			float norm = 0;
			int nonZeros = 0;
			for(int i = 0;i<d_args->dimXProj;i++)
			{
				if(abs(projectedX[i]) > 0.0001) 
				{
					norm += projectedX[i]*projectedX[i];
					nonZeros++;
				}
			}
			if(nonZeros == 0) 
			{
				indexesX[j-1] = new int[1]();
				newValuesX[j-1] = new float[1]();
			}
			else 
			{
				norm = (float) sqrt(norm);
				int* currIndexes = new int[nonZeros+1];
				float* currValues = new float[nonZeros+1];
				currIndexes[0]=nonZeros;
				currValues[0]=nonZeros;
				int k = 1;
				for(int i = 0;i<d_args->dimXProj;i++){
					if(abs(projectedX[i]) > 0.0001) {
						currIndexes[k] = i;
						currValues[k] = projectedX[i]/norm;
						k++;
					}
				}
				indexesX[j-1] = new int[nonZeros+1];
				newValuesX[j-1] = new float[nonZeros+1];

				memcpy(indexesX[j-1], currIndexes, sizeof(int)*(nonZeros+1));
				memcpy(newValuesX[j-1], currValues, sizeof(float)*(nonZeros+1));
				
				delete currIndexes;
				delete currValues;
			}
		}
		else 
		{
			indexesX[j-1] = new int[1]();
			newValuesX[j-1] = new float[1]();
		}

		delete projectedX;
		delete xKey;
		delete xValues;
	}
}

void buildTree(struct Args_device *d_args,struct Node *root)
{
	int** indexesX = new int*[d_args->useInst[0]];
	float** newValuesX = new float*[d_args->useInst[0]];
	
	projectVector(d_args,indexesX,newValuesX);

	float **Y_val=new float*[d_args->useInst[0]];
	for(int i=0;i<d_args->useInst[0];i++)
	{
		Y_val[i]=new float[30];
		memcpy(Y_val[i], d_args->valuesY[i], sizeof(float)*30);
	}
	

	int **Y_ind = new int*[d_args->useInst[0]];
	for(int i=0;i<d_args->useInst[0];i++)
	{
		Y_ind[i]=new int[30];
		memcpy(Y_ind[i], d_args->keyY[i], sizeof(int)*30);
	}

	// Actual Tree
	int *queue_useInst[1000];
	struct Node *queue_Node[1000];
	int front=0,rear=0;

	

	int **childrenInstancesIndex = new int*[10];
	float **childrenInstancesValues = new float*[10];

	computeSeparateInstances(indexesX,newValuesX,d_args->useInst,Y_ind,Y_val,d_args->seedY,d_args->dimYProj,d_args->dimXProj,d_args->sparsity,childrenInstancesIndex,childrenInstancesValues);
	
	
	root->Leaf=true;
	root->center_indexesX=new int*[10];
	root->center_valuesX=new float*[10];

	for(int i=0;i<10;i++)
	{
		root->center_valuesX[i]=new float[(int)childrenInstancesValues[i][0]+1];
		memcpy(root->center_valuesX[i],childrenInstancesValues[i],sizeof(float)*((int)childrenInstancesValues[i][0]+1));
	}
	for(int i=0;i<10;i++)
	{
		root->center_indexesX[i]=new int[(int)childrenInstancesValues[i][0]+1];
		memcpy(root->center_indexesX[i],childrenInstancesIndex[i],sizeof(int)*((int)childrenInstancesValues[i][0]+1));
	}
	

	queue_Node[rear]=root;
	queue_useInst[rear]=new int[d_args->useInst[0]+1];
	
	

	memcpy(queue_useInst[rear],d_args->useInst,sizeof(int)*(d_args->useInst[0]+1));
	rear++;

	while(front!=rear && rear<1000 &&front<1000)
	{
		if(queue_useInst[front][0]<d_args->minInst || checkSame(newValuesX,Y_val,queue_useInst[front]))
		{
			struct Node *node=queue_Node[front];
			int *useNodes=new int[queue_useInst[front][0]+1];
			memcpy(useNodes,queue_useInst[front],sizeof(int)*(queue_useInst[front][0]+1));
			front++;

			float *denseY = new float[4000]();
			for(int i=1;i<=useNodes[0];i++)
			{
				for(int j=1;j<=Y_ind[useNodes[i]][0];j++)
				{
					denseY[Y_ind[useNodes[i]][j]]+=Y_val[useNodes[i]][j]/useNodes[0];
				}
			}
			node->labels=new float[4000];
			memcpy(node->labels,denseY,4000*sizeof(float));
			delete useNodes;
			delete denseY;
			
		}
		else
		{
			struct Node *node=queue_Node[front];
			int *useNodes=new int[queue_useInst[front][0]+1];
			memcpy(useNodes,queue_useInst[front],sizeof(int)*(queue_useInst[front][0]+1));
			
			front++;
			node->Leaf=false;
			int noChild=0;
			
			for(int i=0;i<10;i++)
			{
				if(node->center_indexesX[i][0]!=0)
				{
					noChild++;
				}
				
			}
			
			int *childvalues=new int[useNodes[0]];
			int *childindexes=new int[useNodes[0]];
			int *noChildCluster=new int[noChild]();
			
			for(int i=1;i<=useNodes[0];i++)
			{
				float *denseX=new float[d_args->dimXProj]();
				for(int j=1;j<=indexesX[useNodes[i]][0];j++)
				{
					denseX[indexesX[useNodes[i]][j]]=newValuesX[useNodes[i]][j];
				}
				
				float currentMin = 1-sparseDotProduct(denseX,node->center_indexesX[0],node->center_valuesX[0]);
				int currentMinIndex = 0;
			
				for(int k = 1;k<noChild;k++)
				{
					float newDistance = 1-sparseDotProduct(denseX,node->center_indexesX[k],node->center_valuesX[k]);
					if(newDistance < currentMin)
					{
						currentMin = newDistance;
						currentMinIndex = k;
					}
				}
				
				childindexes[i-1]=useNodes[i];
				childvalues[i-1]=currentMinIndex;
				noChildCluster[currentMinIndex]++;
				
				delete denseX;
			}

			for(int i=0;i<noChild;i++)
			{
				if(noChildCluster[i]>0)
				{
					int *childuseInst=new int[noChildCluster[i]+1];
					childuseInst[0]=noChildCluster[i];
					int k=1;
					for(int j=0;j<useNodes[0];j++)
					{
						if(childvalues[j]==i)
						{
							childuseInst[k]=childindexes[j];
							k++;
						}
					}
					//Changes
					node->children[i]=new struct Node[1];
					node->children[i]->Leaf=true;
					node->children[i]->center_indexesX=new int*[10];
					node->children[i]->center_valuesX=new float*[10];

					int **childchildrenInstancesIndex = new int*[10];
					float **childchildrenInstancesValues = new float*[10];
					
					computeSeparateInstances(indexesX,newValuesX,childuseInst,Y_ind,Y_val,d_args->seedY,d_args->dimYProj,d_args->dimXProj,d_args->sparsity,childchildrenInstancesIndex,childchildrenInstancesValues);
					
					for(int l=0;l<10;l++)
					{
						node->children[i]->center_valuesX[l]=new float[(int)childchildrenInstancesValues[l][0]+1];
						memcpy(node->children[i]->center_valuesX[l],childchildrenInstancesValues[l],sizeof(float)*((int)childchildrenInstancesValues[l][0]+1));
					}
					for(int l=0;l<10;l++)
					{
						node->children[i]->center_indexesX[l]=new int[(int)childchildrenInstancesValues[l][0]+1];
						memcpy(node->children[i]->center_indexesX[l],childchildrenInstancesIndex[l],sizeof(int)*((int)childchildrenInstancesValues[l][0]+1));
					}
	
					queue_Node[rear]=node->children[i];
					queue_useInst[rear]=new int[childuseInst[0]+1];
					memcpy(queue_useInst[rear],childuseInst,sizeof(int)*(childuseInst[0]+1));
					rear++;

					delete childuseInst;
					delete childchildrenInstancesIndex;
					delete childchildrenInstancesValues;
					
					if(rear>1000)
						break;
				}
				else
				{
					node->children[i]=NULL;
				}
			}
			delete childvalues;
			delete childindexes;
			delete noChildCluster;
			delete useNodes;
		}
	}

}

void predictRow(struct Node *root,float *denseX,float *label)
{
	
	if(root->Leaf==true)
	{
		for(int i=0;i<4000;i++)
		{
			label[i] = root->labels[i];	
		}
	}
	else
	{
		
		float *currentMin = new float[1];
		currentMin[0]= FLT_MAX;

		int *currentMinIndex=new int[1]; 
		currentMinIndex[0] = -1;
		
		for(int i=0;i<10;i++)
		{
			float *newDistance = new float[1];

			if(root->children[i]==NULL)
			{
				continue;
			}

			newDistance[0] = 1-sparseDotProduct(denseX,root->center_indexesX[i],root->center_valuesX[i]);
			if(newDistance[0] < currentMin[0])
			{
				currentMin[0] = newDistance[0];
				currentMinIndex[0] = i;
			}

			delete newDistance;
		}
		delete currentMin;
		predictRow(root->children[currentMinIndex[0]],denseX,label);
		delete currentMinIndex;

	}
}
void predict(struct Node *root,int **projindexTest,float **projvalueTest,float **ypred,int nbInstance,int dimXProj)
{

	for(int i=0;i<nbInstance;i++)
	{
	
		float *label=new float[4000]();
		float *denseX=new float[dimXProj]();

		for(int j=1;j<=projindexTest[i][0];j++)
		{
			denseX[projindexTest[i][j]]=projvalueTest[i][j];
		}
		
		predictRow(root,denseX,label);
		
		for(int j=0;j<4000;j++)
		{
			ypred[i][j]=ypred[i][j] + label[j];
		}
		

		delete label;
		delete denseX;
	}
}
float precisionAtK(int **indexes,struct Args_device *args,int k)
{
	float prec=0;
	for(int i=0;i<args->useInst[0];i++)
	{
		for(int j=1;j<=args->keyY[i][0];j++)
		{
			for(int l=0;l<k;l++)
			{
				if(indexes[i][l]==args->keyY[i][j])
					prec = prec+(float)1/k;
			}
		}
	}
	return prec/args->useInst[0];
}
int main(int argc,char** argv)
{
	string trainFile=argv[1];
	string testFile=argv[2];

    CraftML model ;

    LibsvmFileReader readerTrain;
					
	readerTrain.setFile(trainFile);

	model.trainAlgoOnFileStandard(readerTrain);

	struct Args_device *args;
	struct Args_device *T_args;

	
	args = new struct Args_device[1];
	
	args->keyX=new int*[model.keyX.size()];
	args->valuesX=new float*[model.keyX.size()];

	for(int i=0;i<model.keyX.size();i++)
	{
		args->keyX[i]=new int[model.keyX[i].size()+1];
		args->valuesX[i]=new float[model.keyX[i].size()+1];

		args->keyX[i][0]=model.keyX[i].size();
		args->valuesX[i][0]=model.keyX[i].size();

		for(int j=0;j<model.keyX[i].size();j++)
		{
			args->keyX[i][j+1] = model.keyX[i][j];
			args->valuesX[i][j+1] = model.valuesX[i][j];
		}
	}
	args->keyY=new int*[model.keyY.size()];
	args->valuesY=new float*[model.keyY.size()];

	for(int i=0;i<model.keyY.size();i++)
	{
		args->keyY[i]=new int[model.keyY[i].size()+1];
		args->valuesY[i]=new float[model.keyY[i].size()+1];

		args->keyY[i][0]=model.keyY[i].size();
		args->valuesY[i][0]=model.keyY[i].size();

		for(int j=0;j<model.keyY[i].size();j++)
		{
			args->keyY[i][j+1] = model.keyY[i][j];
			args->valuesY[i][j+1] = model.valuesY[i][j];
		}
	}
	args->useInst=new int[model.keyX.size()+1];

	args->useInst[0]=model.keyX.size();
	for(int i=0;i<model.keyX.size();i++)
		args->useInst[i+1] = i;
	
	args->dimXProj=2500;
	args->dimYProj=2000;
	args->sparsity=100;
	args->minInst=100;

	int noTrees=20;
	struct Node **root=new struct Node*[noTrees];
	int seedX[noTrees];
	int seedY[noTrees];

	auto start = high_resolution_clock::now(); 

	for(int i=0;i<noTrees;i++)
	{
		args->seedX=rand()%1000;
		args->seedY=rand()%1000;

		seedX[i]=args->seedX;
		seedY[i]=args->seedY;

		root[i]=new struct Node[1];
		buildTree(args,root[i]);
		cout<<"Tree "<<i+1<<" Finished"<<endl;
	}
	auto stop = high_resolution_clock::now(); 
	
    auto duration = duration_cast<microseconds>(stop - start); 
  
    cout << "Time taken by function: "<< duration.count() << " microseconds" << endl;
	
	for(int i=0;i<model.keyY.size();i++)
	{
		delete args->keyY[i];
		delete args->valuesY[i];
	}
	delete args->keyY;
	delete args->valuesY;
	for(int i=0;i<model.keyX.size();i++)
	{
		delete args->keyX[i];
		delete args->valuesX[i];
	}
	delete args->keyX;
	delete args->valuesX;
	delete args->useInst;
	delete args;


	readerTrain.setFile(testFile);
	CraftML modelTest;

	modelTest.trainAlgoOnFileStandard(readerTrain);
	T_args = new struct Args_device[1];
	
	T_args->keyX=new int*[modelTest.keyX.size()];
	T_args->valuesX=new float*[modelTest.keyX.size()];
	
	for(int i=0;i<modelTest.keyX.size();i++)
	{

		T_args->keyX[i]=new int[modelTest.keyX[i].size()+1];
		T_args->valuesX[i]=new float[modelTest.keyX[i].size()+1];

		T_args->keyX[i][0]=modelTest.keyX[i].size();
		T_args->valuesX[i][0]=modelTest.keyX[i].size();

		for(int j=0;j<modelTest.keyX[i].size();j++)
		{
			T_args->keyX[i][j+1] = modelTest.keyX[i][j];
			T_args->valuesX[i][j+1] = modelTest.valuesX[i][j];
		}
	}
	T_args->keyY=new int*[modelTest.keyY.size()];
	T_args->valuesY=new float*[modelTest.keyY.size()];

	for(int i=0;i<modelTest.keyY.size();i++)
	{

		T_args->keyY[i]=new int[modelTest.keyY[i].size()+1];
		T_args->valuesY[i]=new float[modelTest.keyY[i].size()+1];

		T_args->keyY[i][0]=modelTest.keyY[i].size();
		T_args->valuesY[i][0]=modelTest.keyY[i].size();

		for(int j=0;j<modelTest.keyY[i].size();j++)
		{
			T_args->keyY[i][j+1] = modelTest.keyY[i][j];
			T_args->valuesY[i][j+1] = modelTest.valuesY[i][j];
		}
	}
	T_args->useInst=new int[modelTest.keyX.size()+1];

	T_args->useInst[0]=modelTest.keyX.size();
	for(int i=0;i<modelTest.keyX.size();i++)
	T_args->useInst[i+1] = i;

	T_args->dimXProj=2500;
	T_args->dimYProj=2000;
	T_args->sparsity=100;
	T_args->minInst=100;
		
	float **ypred =new float*[T_args->useInst[0]];
	for(int i=0;i<T_args->useInst[0];i++)
	{
		ypred[i]=new float[4000]();
	}

	for(int i=0;i<noTrees;i++)
	{
		int **projindexTest = new int*[T_args->useInst[0]];
		float **projvalueTest = new float*[T_args->useInst[0]];
		cout<<"Predicting for "<<i+1<<" Tree"<<endl;

		T_args->seedX=seedX[i];
		T_args->seedY=seedY[i];

		projectVector(T_args,projindexTest,projvalueTest);
		
		predict(root[i],projindexTest,projvalueTest,ypred,T_args->useInst[0],T_args->dimXProj);
		for(int j=0;j<T_args->useInst[0];j++)
		{
			delete projindexTest[j];
			delete projvalueTest[j];	
		}

		delete projindexTest;
		delete projvalueTest;
	}
	int **bestindexes=new int*[T_args->useInst[0]];
	for(int i=0;i<T_args->useInst[0];i++)
	{
		bestindexes[i]=new int[5];
	}
	float **bestvalues=new float*[T_args->useInst[0]];
	for(int i=0;i<T_args->useInst[0];i++)
	{
		bestvalues[i]=new float[5];
	}

	for(int i=0;i<T_args->useInst[0];i++)
	{
		int minIndex=0;
		float minVal=ypred[i][0];
		for(int j=0;j<5;j++)
		{
			if(minVal>ypred[i][j])
			{
				minIndex=j;
				minVal=ypred[i][j];
			}
			bestindexes[i][j] = j;
			bestvalues[i][j] = ypred[i][j];
		}
		for(int j=5;j<4000;j++)
		{
			if(ypred[i][j]>minVal)
			{
				bestindexes[i][minIndex] = j;
				bestvalues[i][minIndex]=ypred[i][j];
				minVal=ypred[i][j];
				for(int k=0;k<5;k++)
				{
					if(minVal>bestvalues[i][k])
					{
						minVal=bestvalues[i][k];
						minIndex=k;
					}
				}
			}
		}
		
	}

	for(int i=0;i<T_args->useInst[0];i++)
	{
		for(int j=0;j<4;j++)
		{
			for(int k=j+1;k<5;k++)
			{
				if(bestvalues[i][k]>bestvalues[i][j])
				{
					float temp1;int temp2;
					temp1=bestvalues[i][j];
					temp2=bestindexes[i][j];

					bestvalues[i][j] = bestvalues[i][k];
					bestindexes[i][j] = bestindexes[i][k]; 

					bestvalues[i][k]=temp1;
					bestindexes[i][k]=temp2;
				}
			}
			
		}
	}
	cout<<"Pat1 = "<<precisionAtK(bestindexes,T_args,1)<<endl;
	cout<<"Pat3 = "<<precisionAtK(bestindexes,T_args,3)<<endl;
	cout<<"Pat5 = "<<precisionAtK(bestindexes,T_args,5)<<endl;

	return 0;
}
