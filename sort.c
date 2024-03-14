#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int extraMemoryAllocated;

void *Alloc(size_t sz)
{
	extraMemoryAllocated += sz;
	size_t* ret = malloc(sizeof(size_t) + sz);
	*ret = sz;
	printf("Extra memory allocated, size: %ld\n", sz);
	return &ret[1];
}

void DeAlloc(void* ptr)
{
	size_t* pSz = (size_t*)ptr - 1;
	extraMemoryAllocated -= *pSz;
	printf("Extra memory deallocated, size: %ld\n", *pSz);
	free((size_t*)ptr - 1);
}

size_t Size(void* ptr)
{
	return ((size_t*)ptr)[-1];
}

// implement merge sort
// extraMemoryAllocated counts bytes of extra memory allocated
void mergeSort(int pData[], int l, int r)
{
	//This code was made with the assumption that l is always 0... given, it SHOULD work if l is a non-zero value.
	//For reference, this code expects l to be the starting index, r to be the final index, 
	//and mid to be the final index of the first half.

	//I've been able to figure out that mergeSort IS working... 
	//I think, but even the fixed version of the printing algorithm is wrong. If only I could read the array in the debugger.
	//Or maybe the printing algorithm is completely fine and I'm going crazy?

	//Don't do anything if you were given an array of size 1.
	if (l == r) {
		return;
	}

	//First, split the array in half.
	int mid = (l + r)/2;
	int* lowHalf = Alloc(sizeof(int) * (mid + 1 - l));
	int* highHalf = Alloc(sizeof(int) * (r - mid));

	for (int c = 0; c < mid + 1 - l; c++) {
		lowHalf[c] = pData[l + c];
	}

	for (int c = 0; c < r - mid; c++) {
		highHalf[c] = pData[mid + 1 + c];
	}

	//Then, call mergeSort on each half.
	mergeSort(lowHalf, 0, mid - l);
	mergeSort(highHalf, 0, r - mid - 1);

	//Finally, stitch the two halves together.
	int currentPDataIndex = l;
	int lowHalfIndex = 0;
	int highHalfIndex = 0;

	while (lowHalfIndex < (mid + 1) && highHalfIndex < (r - mid)) {
		if (lowHalf[lowHalfIndex] <= highHalf[highHalfIndex]) {
			pData[currentPDataIndex] = lowHalf[lowHalfIndex];
			lowHalfIndex++;
		} else {
			pData[currentPDataIndex] = highHalf[highHalfIndex];
			highHalfIndex++;
		}
		currentPDataIndex++;
	}

	//One of the arrays is empty. We don't know which one. Since whiles are used anyway, an if statement is useless here.
	while (lowHalfIndex < (mid + 1)) {
		pData[currentPDataIndex] = lowHalf[lowHalfIndex];
		lowHalfIndex++;
		currentPDataIndex++;
	}

	while (highHalfIndex < (r - mid)) {
		pData[currentPDataIndex] = highHalf[highHalfIndex];
		highHalfIndex++;
		currentPDataIndex++;
	}

	//Deallocate the intermediary arrays.
	DeAlloc(lowHalf);
	DeAlloc(highHalf);
}

// parses input file to an integer array
int parseData(char *inputFileName, int **ppData)
{
	FILE* inFile = fopen(inputFileName,"r");
	int dataSz = 0;
	int i, n, *data;
	*ppData = NULL;
	
	if (inFile)
	{
		fscanf(inFile,"%d\n",&dataSz);
		*ppData = (int *)malloc(sizeof(int) * dataSz);
		// Implement parse data block
		if (*ppData == NULL)
		{
			printf("Cannot allocate memory\n");
			exit(-1);
		}
		for (i=0;i<dataSz;++i)
		{
			fscanf(inFile, "%d ",&n);
			data = *ppData + i;
			*data = n;
		}

		fclose(inFile);
	}
	
	return dataSz;
}

// prints first and last 100 items in the data array
void printArray(int pData[], int dataSz)
{
    int i, sz = (dataSz > 100 ? dataSz - 100 : 0);
    int firstHundred = (dataSz < 100 ? dataSz : 100);
    printf("\tData:\n\t");
    for (i=0;i<firstHundred;++i)
    {
        printf("%d ",pData[i]);
    }
    printf("\n\t");
    
    for (i=sz;i<dataSz;++i)
    {
        printf("%d ",pData[i]);
    }
    printf("\n\n");
}

int main(void)
{
	clock_t start, end;
	int i;
    double cpu_time_used;
	char* fileNames[] = { "input1.txt", "input2.txt", "input3.txt", "input4.txt" };
	
	for (i=0;i<4;++i)
	{
		int *pDataSrc, *pDataCopy;
		int dataSz = parseData(fileNames[i], &pDataSrc);
		
		if (dataSz <= 0)
			continue;
		
		pDataCopy = (int *)malloc(sizeof(int)*dataSz);
	
		printf("---------------------------\n");
		printf("Dataset Size : %d\n",dataSz);
		printf("---------------------------\n");
		
		printf("Merge Sort:\n");
		memcpy(pDataCopy, pDataSrc, dataSz*sizeof(int));
		extraMemoryAllocated = 0;
		start = clock();
		mergeSort(pDataCopy, 0, dataSz - 1);
		end = clock();
		cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
		printf("\truntime\t\t\t: %.1lf\n",cpu_time_used);
		printf("\textra memory allocated\t: %d\n",extraMemoryAllocated);
		printArray(pDataCopy, dataSz);
		
		free(pDataCopy);
		free(pDataSrc);
	}
	
}