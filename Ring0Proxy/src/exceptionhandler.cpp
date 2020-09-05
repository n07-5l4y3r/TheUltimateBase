#include "exceptionhandler.hpp"

int fGlobalExceptionHandler(unsigned int uErrorCode, PEXCEPTION_POINTERS pExceptionPointers)
{
    DbgPrint(" [>] \"" __FUNCTION__ "\"" "\n");

    DbgPrint(" [-] uErrorCode:                                               %u" "\n", uErrorCode);
    DbgPrint(" [-] pExceptionPointers:                                       %p" "\n", pExceptionPointers);

    if (pExceptionPointers)
    {
        DbgPrint(" [-] pExceptionPointers->ExceptionRecord:                      %p" "\n", pExceptionPointers->ExceptionRecord);
        if (pExceptionPointers->ExceptionRecord)
        {
            DbgPrint(" [-] pExceptionPointers->ExceptionRecord->ExceptionAddress:    %p" "\n", pExceptionPointers->ExceptionRecord->ExceptionAddress);
            DbgPrint(" [-] pExceptionPointers->ExceptionRecord->ExceptionCode:       %p" "\n", pExceptionPointers->ExceptionRecord->ExceptionCode);
            DbgPrint(" [-] pExceptionPointers->ExceptionRecord->ExceptionFlags:      %p" "\n", pExceptionPointers->ExceptionRecord->ExceptionFlags);
            DbgPrint(" [-] pExceptionPointers->ExceptionRecord->ExceptionInformation:%p" "\n", pExceptionPointers->ExceptionRecord->ExceptionInformation);
            DbgPrint(" [-] pExceptionPointers->ExceptionRecord->NumberParameters:    %p" "\n", pExceptionPointers->ExceptionRecord->NumberParameters);
            DbgPrint(" [-] pExceptionPointers->ExceptionRecord->ExceptionRecord:     %p" "\n", pExceptionPointers->ExceptionRecord->ExceptionRecord);
        }
    }

    return EXCEPTION_EXECUTE_HANDLER;
}