#include "WORK_QUEUE_ITEM.hpp"

VOID ExFreeWorkItem(_In_ PWORK_QUEUE_ITEM item)
{
	DbgPrint(" [>] \"" __FUNCTION__ "\"" "\n");
	ExFreePool(item);
}

namespace nWORK_QUEUE_ITEM
{
	VOID cWorker::WORKER_THREAD_ROUTINE(_In_ PVOID pParam)
	{
		DbgPrint(" [>] \"" __FUNCTION__ "\"" "\n");

		auto instance = (cWorker*)pParam;
		if (!instance)
		{
			DbgPrint(" [-] pParam: %p" "\n", pParam);
			return;
		}

		DbgPrint(" [+] pParam: %p" "\n", pParam);

		instance->pThread(instance->pParam, instance);

		instance->queue();
	}
	void cWorker::queue()
	{
		DbgPrint(" [>] \"" __FUNCTION__ "\"" "\n");
		DbgPrint(" [ ] this->pThread:        %p" "\n", this->pThread);
		DbgPrint(" [ ] this->pWorkQueueItem: %p" "\n", this->pWorkQueueItem);
		DbgPrint(" [ ] this->bRestart:       %u" "\n", this->bRestart);

		if (!this->pThread ||
			!this->pWorkQueueItem ||
			!this->bRestart)
		{
			this->DeInit_cWorker(this);
		}
		else
			ExQueueWorkItem(this->pWorkQueueItem, DelayedWorkQueue);
	}
	void cWorker::finish()
	{
		DbgPrint(" [>] \"" __FUNCTION__ "\"" "\n");

		this->bRestart = false;
	}
	cWorker* cWorker::Init_cWorker(t_pThread pThread, PVOID pParam)
	{
		DbgPrint(" [>] \"" __FUNCTION__ "\"" "\n");

		auto instance = new (NonPagedPool) cWorker();
		if (!instance)
			return nullptr;

		instance->pThread = pThread;
		instance->pParam = pParam;
		instance->pWorkQueueItem = (PWORK_QUEUE_ITEM)ExAllocatePool(NonPagedPool, sizeof(_WORK_QUEUE_ITEM));
		if (!instance->pWorkQueueItem)
		{
			ExFreePool(instance);
			return nullptr;
		}
		instance->bRestart = true;

		ExInitializeWorkItem(instance->pWorkQueueItem, cWorker::WORKER_THREAD_ROUTINE, instance);

		return instance;
	}
	void cWorker::DeInit_cWorker(cWorker* instance)
	{
		DbgPrint(" [>] \"" __FUNCTION__ "\"" "\n");

		ExFreeWorkItem(instance->pWorkQueueItem);

		ExFreePool(instance);
	}
}