#include "Task.h"
#include <stdio.h>
#include <assert.h>


//=============================================================
// 태스크 리스트 초기화
CTaskList::CTaskList(int max_task_size, int max_num_task)
:	MaxTaskSize(max_task_size), MaxNumTask(max_num_task), 
	NumFreeTask(max_num_task)
{
	// 태스크 초기화용 매크로
	#define TASK(INDEX) ((CTask*)(buf+max_task_size*(INDEX)))

	// 태스크용 메모리 확보
	char* buf=new char[max_task_size*(max_num_task+2)];

	// 액티브 태스크 리스트의 초기화
	ActiveTask=TASK(0);
	ActiveTask->Prev=ActiveTask->Next=ActiveTask;

	// 프리 태스크 리스트의 초기화
	FreeTask=TASK(1);
	for (int i=1; i<max_num_task+1; i++) {
		TASK(i)->Next=TASK(i+1);
	}
	TASK(max_num_task+1)->Next=FreeTask;
}


//=============================================================
// 태스크 생성
// 프리 태스크 리스트에서 메모리 블록의 선두에 CTask를 배치한다.
// 서브 클래스의 생성자를 호출한 후에는,
// 메모리 블록의 선두에 CTask가 배치된다고는 할 수 없다 (다중상속 등)
// 여기서, 생성 처리를 new와 생성자로 분할한다.
// 삭제 처리에서도 마찬가지로 소멸자와 delete로 분할한다.

// new 처리
void* CTask::operator_new(size_t t, CTaskList* task_list) {

	// 클래스 크기가 최대 크기를 초과하면 에러
	assert(t<=(size_t)task_list->MaxTaskSize);

	// 프리 태스크가 없을 때는 NULL 반환
	if (task_list->NumFreeTask<=0) return NULL;

	// 프리 태스크를 1개 검색
	CTask* task=task_list->FreeTask->Next;
	task_list->FreeTask->Next=task->Next;
	task_list->NumFreeTask--;
	
	// 생성자로 돌려준다
	return task;
}

// 생성자
CTask::CTask(CTaskList* task_list)
:	TaskList(task_list)
{

	// 태스크를 액티브 태스크 리스트에 삽입
	Prev=task_list->ActiveTask->Prev;
	Next=task_list->ActiveTask;
	Prev->Next=Next->Prev=this;
}


//=============================================================
// 태스크 삭제

// 소멸자
CTask::~CTask() {

	// 태스크를 액티브 태스크 리스트에서 제거
	Prev->Next=Next;
	Next->Prev=Prev;
}

// delete 처리
void CTask::operator_delete(void* p, CTaskList* task_list) {

	// 태스크
	CTask* task=(CTask*)p;

	// 태스크를 프리 태스크 리스트에 삽입
	task->Next=task_list->FreeTask->Next;
	task_list->FreeTask->Next=task;
	task_list->NumFreeTask++;

	// 프리 태스크 ㅅ ㅜ가 최대 태스크 수를 초과하면 에러
	// （delete의 중복 호출 등)
	assert(task_list->NumFreeTask<=task_list->MaxNumTask);
}


//=============================================================
// 모든 태스크 지우기
void CTaskList::DeleteTask() {
	for (CTaskIter i(this); i.HasNext(); i.Next(), i.Remove()) ;
}
