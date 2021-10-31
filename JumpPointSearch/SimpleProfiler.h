#pragma once
class SimpleProfiler
{
public:
	struct Profile {
		bool use = false;
		char name[100] = {0,};
		LARGE_INTEGER start;
		__int64 sum = 0;
		__int64 max = 0;
		__int64 min = 0x7FFFFFFFFFFFFFFF;
		unsigned int callCnt = 0;
	};

	LARGE_INTEGER freq;

public:

	SimpleProfiler() {
		QueryPerformanceFrequency(&freq);
	}
	
	void profileBegin(const char name[100]) {
		int idx = findIdx(name);
		if (idx == -1) {
			idx = getEmptyIdx();
			if (idx == -1) {
				killProcess();
			}
		}

		Profile* ptrInfo = &info[idx];

		ptrInfo->callCnt += 1;
		ptrInfo->use = true;
		strcpy_s(ptrInfo->name, name);

		QueryPerformanceCounter(&ptrInfo->start);
	}

	void profileEnd(const char name[100]) {
		int idx = findIdx(name);
		if (idx == -1) {
			killProcess();
		}

		LARGE_INTEGER endTime;
		QueryPerformanceCounter(&endTime);

		Profile* ptrInfo = &info[idx];

		__int64 time = endTime.QuadPart - ptrInfo->start.QuadPart;

		if (ptrInfo->max < time) {
			ptrInfo->max = time;
		}

		if (ptrInfo->min > time) {
			ptrInfo->min = time;
		}

		ptrInfo->sum += time;
		ptrInfo->use = false;

	}

	void printToFile() {

		FILE* outFile;
		fopen_s(&outFile, "profiler.txt", "w");

		fprintf_s(outFile, "%20s | %15s | %15s | %15s | %15s \n", "Name", "Average", "Min", "Max", "Call");
		
		for (int infoCnt = 0; infoCnt < 50; infoCnt++) {
			
			Profile* ptrInfo = &info[infoCnt];

			if (ptrInfo->callCnt > 0) {
				ptrInfo->sum = ptrInfo->sum - ptrInfo->max - ptrInfo->min;
				ptrInfo->callCnt -= 2;
				fprintf_s(outFile, "%20s | %12.3lf us | %12.3lf us | %12.3lf us | %15d \n",
					ptrInfo->name,
					ptrInfo->sum / ((double)freq.QuadPart / 1000000) / ptrInfo->callCnt,
					ptrInfo->min / ((double)freq.QuadPart / 1000000),
					ptrInfo->max / ((double)freq.QuadPart / 1000000),
					ptrInfo->callCnt);
			}
		}
		

		fclose(outFile);

	}
	

private:
	Profile info[50];

	int findIdx(const char* name) {

		for (int infoCnt = 0; infoCnt < 50; ++infoCnt) {
			if (strcmp(name, info[infoCnt].name) == 0) {
				return infoCnt;
			}
		}

		return -1;
	}

	int getEmptyIdx() {
		for (int infoCnt = 0; infoCnt < 50; ++infoCnt) {
			if (info[infoCnt].use == 0 && info[infoCnt].callCnt == 0) {
				return infoCnt;
			}
		}

		return -1;
	}

	void killProcess() {
		int* kill = nullptr;
		*kill = 1;
	}


};

