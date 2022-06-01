module;

#include <time.h>

export module StopWatch;

export class StopWatch {

public:

	void Start() {
		startTime = clock();
	}

	void End(){
		endTime = clock();
	}

	float GetValue() const {
		return (endTime - startTime) / CLOCKS_PER_SEC;
	}

private:

	float startTime = 0;
	float endTime = 0;

};