all:
	nvcc CraftML.cu LibsvmFileReader.cu  RecordTextReader.cu -o craftml