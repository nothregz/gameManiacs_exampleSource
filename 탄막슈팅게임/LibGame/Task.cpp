#include "Task.h"
#include <stdio.h>
#include <assert.h>

//=============================================================
// タスクリスト
CTask* CTaskList::GetTask(size_t i) {
	return (CTask*)(Buffer+TaskSize*i);
}

CTaskList::CTaskList(size_t size, size_t count)
:	TaskSize(size), TaskCount(count)
{
	Buffer=new char[TaskSize*(TaskCount+2)];

	ActiveTask=GetTask(0);
	ActiveTask->Prev=ActiveTask->Next=ActiveTask;

	FreeTask=GetTask(1);
	for (size_t i=1; i<TaskCount+2; i++) {
		GetTask(i)->Prev=NULL;
		GetTask(i)->Next=GetTask(i+1);
	}
	GetTask(TaskCount+1)->Next=FreeTask;	
}

CTaskList::~CTaskList() {
	delete [] Buffer;
}

void* CTaskList::New(size_t size) {
	assert(size<=TaskSize);
	if (FreeTask->Next==FreeTask) return NULL;

	CTask* task=FreeTask->Next;
	FreeTask->Next=task->Next;
	return task;
}

void CTaskList::Delete(void* p) {
	CTask* task=(CTask*)p;
	assert(task->Prev!=NULL);
	
	task->Prev=NULL;
	task->Next=FreeTask->Next;
	FreeTask->Next=task;
}

//=============================================================
// タスク
CTask::CTask(CTaskList* list) {
	Prev=list->ActiveTask->Prev;
	Next=list->ActiveTask;
	Prev->Next=Next->Prev=this;
}

CTask::~CTask() {
	Prev->Next=Next;
	Next->Prev=Prev;
}

//==============================================================
// タスクイテレータ
CTaskIter::CTaskIter(CTaskList* list)
:	List(list), Task(list->ActiveTask)
{}

bool CTaskIter::HasNext() {
	return Task->Next!=List->ActiveTask;
}

CTask* CTaskIter::Next() {
	return Task=Task->Next;
}

void CTaskIter::Remove() {
	Task=Task->Prev;
	delete Task->Next;
}
