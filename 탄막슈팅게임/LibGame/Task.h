#ifndef TASKH
#define TASKH

class CTask;
class CTaskList;
class CTaskIter;

//==============================================================
// �^�X�N
class CTask {
friend CTaskList;
friend CTaskIter;

private:
	CTask *Prev, *Next;

	void* operator new(size_t n) {}
	void operator delete(void* p) {}

public:
	CTask(CTaskList* list);
	virtual ~CTask();
};

//==============================================================
// �^�X�N���X�g
class CTaskList {
friend CTask;
friend CTaskIter;

private:
	char* Buffer;
	CTask *ActiveTask, *FreeTask;
	size_t TaskSize, TaskCount;

	CTask* GetTask(size_t i);

public:
	CTaskList(size_t size, size_t count);
	~CTaskList();

	void* New(size_t size);
	void Delete(void* p);
};

//==============================================================
// �^�X�N�C�e���[�^
class CTaskIter {

private:
	CTaskList* List;
	CTask* Task;

public:
	CTaskIter(CTaskList* list);
	bool HasNext();
	CTask* Next();
	void Remove();
};

#endif

