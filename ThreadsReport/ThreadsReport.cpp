// ThreadsReport.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <string.h>
#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include <Shlwapi.h>


int _tmain(int argc, char* argv[])
{
	int pid = 0;

	if ( 1 < argc )
	{
		if ( !strcmp("-h", argv[1]) )
		{
			char filename[MAX_PATH+1];
			memset(filename, '\0', MAX_PATH+1);

			_snprintf( filename, MAX_PATH, "%s", argv[0] );

			PathStripPath( filename );

			printf("Usage: %s [ <(optional) Target PID)> ]\n", filename);
			return 0;
		}
		else
		{
			pid = atoi(argv[1]);
		}
	}

	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (h != INVALID_HANDLE_VALUE) 
	{
		SYSTEMTIME systemTime;

		GetSystemTime(&systemTime);

		printf("Thread Report for %04u-%02u-%02u %02u:%02u:%02u.%03u\n", systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);

		THREADENTRY32 te;
		te.dwSize = sizeof(te);

		if (Thread32First(h, &te)) 
		{

			FILETIME creationTime;
			FILETIME exitTime;
			FILETIME kernelTime;
			FILETIME userTime;


			do 
			{
				if (te.dwSize >= FIELD_OFFSET(THREADENTRY32, th32OwnerProcessID) + sizeof(te.th32OwnerProcessID) &&
					(pid == 0 || pid == te.th32OwnerProcessID))
				{
		  			HANDLE threadHandle = OpenThread( THREAD_QUERY_LIMITED_INFORMATION, FALSE, te.th32ThreadID );

					printf("Process ID: %5u  Thread ID: 0x%04x", te.th32OwnerProcessID, te.th32ThreadID);

					if ( threadHandle == NULL )
					{
						DWORD error = GetLastError();
						printf("  could not open thread handle (0x%08X)\n", error);
					}
					else if ( GetThreadTimes( threadHandle, &creationTime, &exitTime, &kernelTime, &userTime ) )
					{
						FileTimeToSystemTime( &creationTime, &systemTime );
						printf( "  Creation time: %04u-%02u-%02u %02u:%02u:%02u.%03u", systemTime.wYear, systemTime.wMonth, systemTime.wDay, systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);

						FileTimeToSystemTime( &userTime, &systemTime );
						printf( "  User CPU time: %02u:%02u:%02u.%03u", systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);

						FileTimeToSystemTime( &kernelTime, &systemTime );
						printf( "  Kernel CPU time: %02u:%02u:%02u.%03u\n", systemTime.wHour, systemTime.wMinute, systemTime.wSecond, systemTime.wMilliseconds);

					}
					else
					{
						DWORD error = GetLastError();
						printf("  could not query for thread times (0x%08X)\n", error);
					}

					if ( threadHandle != NULL )
					{
						CloseHandle( threadHandle );
					}
				}
	   
				te.dwSize = sizeof(te);
	   
			} while (Thread32Next(h, &te));
		}

		CloseHandle(h);
	}
	else
	{
		printf("Could not take System Snapshot (0x%08X)", GetLastError());
	}

	return 0;
}

