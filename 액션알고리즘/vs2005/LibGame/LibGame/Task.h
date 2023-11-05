#ifndef TASKH
#define TASKH

class CTask;
class CTaskList;
class CTaskIter;


//==============================================================
// 태스크
class CTask {
friend CTaskList;
friend CTaskIter;

	// 태스크 리스트에 대한 포인터
	CTaskList* TaskList;

	// 전후 태스크에 대한 포인터
	CTask *Prev, *Next;

	// 태스크 리스트를 지정하지 않은 new, delete를 무효로 한다
	void* operator new(size_t t) {}
	void operator delete(void* p) {}

protected:
	
	// new, delete 처리:
	// 적절한 new, delete 연산자를 서브 클래스에서 정의
	static void* operator_new(size_t t, CTaskList* task_list);
	static void operator_delete(void* p, CTaskList* task_list);

public:
	CTask(CTaskList* task_list);
	virtual ~CTask();
};


//==============================================================
// 태스크 리스트
class CTaskList {
friend CTask;
friend CTaskIter;

	// 액티브 태스크리스트, 프리 태스크리스트
	CTask *ActiveTask, *FreeTask;

	// 태스크 최대 크기, 태스크 최대 수
	int MaxTaskSize, MaxNumTask;
	
	// 프리 태스크 수
	int NumFreeTask;

public:

	// 생성자
	CTaskList(int max_task_size, int max_num_task);
	
	// 태스크 수
	int GetNumFreeTask() { return NumFreeTask; }
	int GetNumActiveTask() { return MaxNumTask-NumFreeTask; }

	// 전체 태스크 지우기
	void DeleteTask();
};


//==============================================================
// 태스크 반복 (이터레이터)
class CTaskIter {
protected:
	CTaskList* TaskList;
	CTask* Task;
public:

	// 생성자
	inline CTaskIter(CTaskList* task_list)
	: TaskList(task_list), Task(task_list->ActiveTask)
	{}

	// 다음 요소가 있을 때 true
	inline bool HasNext() {
		return Task->Next!=TaskList->ActiveTask;
	}

	// 다음 요소를 반환
	inline CTask* Next() {
		return Task=Task->Next;
	}

	// 직전에 반환한 요소를 삭제
	inline void Remove() {
		Task=Task->Prev;
		delete Task->Next;
	}
};


#endif

