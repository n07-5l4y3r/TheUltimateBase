#pragma once

#include <inc.hpp>

namespace nWORK_QUEUE_ITEM
{
	class NONPAGESECTION cWorker
	{
	//typedefs
	public:
		typedef void (*t_pThread)(PVOID pParam, cWorker* pcWorker);

	//vars
	private:
		//queue item
		PWORK_QUEUE_ITEM pWorkQueueItem = nullptr;
		//actual thread
		t_pThread pThread = nullptr;
		//thread parameter
		PVOID pParam = nullptr;
		//restart-condition
		bool bRestart = false;

	//private funcs
	private:
		static VOID WORKER_THREAD_ROUTINE(_In_ PVOID pParam);

	//public funcs
	public:
		//restarts the thread if bRestart is set
		void queue();
		//sets bRestart to false which stops the thread loop
		void finish();

	//constructor
	public:
		//typedef void (*t_pThread)(PVOID pParam, cWorker* pcWorker);
		static cWorker* Init_cWorker(t_pThread pThread, PVOID pParam);
	//destructor
	public:
		static void DeInit_cWorker(cWorker* instance);
	};
}