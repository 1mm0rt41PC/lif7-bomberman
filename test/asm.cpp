#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <winbase.h>
#include <winnt.h>

int main();
static inline void *get_caller(void) {
	unsigned long *ebp;

	/* WARNING: This is working only with frame pointers */
	asm ("movl %%ebp, %0" : "=r" (ebp) : );
	ebp = (unsigned long*)*ebp;
	ebp = (unsigned long*)*(ebp+1);
	return ebp;
}


typedef VOID (WINAPI *proc_RtlCaptureContext)( CONTEXT* ContextRecord );
typedef USHORT (*proc_RtlCaptureStackBackTrace)( ULONG, ULONG, PVOID*, PULONG );
typedef PVOID/*CallersCaller*/ (NTAPI *proc_RtlGetCallersAddress)(PVOID*/*__out CallersAddress*/, PVOID*/*__out CallersCaller*/);

		// RtlWalkFrameChain
		// ULONG RtlWalkFrameChain(OUT PVOID *Callers, IN ULONG Count, IN ULONG Flags);
		// http://doxygen.reactos.org/d4/d85/rtl_2amd64_2unwind_8c_a09c41fd53a3c8845d1c6edb32f131036.html
		// PVOID Callers[4];
		// Number = RtlWalkFrameChain(Callers, 4, 0);
		//.
		// RtlGetFrame
		// RtlPopFrame
		// RtlPushFrame
		// RtlQueryProcessBackTraceInformation
		// RtlQueryProcessDebugInformation
		// RtlQueryProcessHeapInformation
		// RtlWalkHeap
char f4()
{
	HMODULE hDll = ::LoadLibrary("kernel32.dll");
	if( !hDll ){
		fprintf(stderr, "LoadLibrary(kernel32.dll) error %lu", GetLastError());
		exit(1);
	}
	// RtlCaptureContext
//	RtlCaptureContext = (proc_RtlCaptureContext)::GetProcAddress( hDll, "RtlCaptureContext" );
//	if( !RtlCaptureContext ){
//		fprintf(stderr, "GetProcAddress(RtlCaptureContext) error %lu", GetLastError());
//		exit(1);
//	}
	// RtlCaptureStackBackTrace
	proc_RtlCaptureStackBackTrace RtlCaptureStackBackTrace = (proc_RtlCaptureStackBackTrace)::GetProcAddress( hDll, "RtlCaptureStackBackTrace" );
	if( !RtlCaptureStackBackTrace ){
		fprintf(stderr, "GetProcAddress(RtlCaptureStackBackTrace) error %lu", GetLastError());
		exit(1);
	}

	// FONCTIONNEL MAIS RENVOIE TOUJOURS le point d'appel et pas l'adresse de la fonction
	// --> RtlGetCallersAddress
	proc_RtlGetCallersAddress RtlGetCallersAddress = (proc_RtlGetCallersAddress)::GetProcAddress( ::LoadLibrary("ntdll.dll"), "RtlGetCallersAddress" );
	if( !RtlGetCallersAddress ){
		fprintf(stderr, "GetProcAddress(RtlGetCallersAddress) error %lu", GetLastError());
		exit(1);
	}
	PVOID CallersAddress = 0;
	PVOID CallersCaller = 0;
	RtlGetCallersAddress( &CallersAddress, &CallersCaller );
	fprintf(stderr, "%p, %p\n", CallersAddress, CallersCaller);


	// --> RtlCaptureStackBackTrace
	// From http://msdn.microsoft.com/en-us/library/bb204633(VS.85).aspx,
	// the sum of FramesToSkip and FramesToCapture must be less than 63,
	// so set it to 62.
	void* BackTrace[60] = {0};
	ULONG BackTraceHash = 0;
	int frames = RtlCaptureStackBackTrace(0, 60, BackTrace, &BackTraceHash);
	printf("No of frames found %d\n", frames);
	for( int i=0; i<frames; i++ )
	   fprintf(stderr, "*** %d called from %p %lu\n", i, BackTrace[i], BackTraceHash);//%016I64LX

//	CONTEXT Context;
//	RtlCaptureContext(&Context);
//	char *eNextBP  = (char *)Context.Ebp;
//	for(ULONG Frame = 0; eNextBP ; Frame++)
//	{
//		char *pBP = eNextBP;
//		eNextBP = *(char **)pBP; // Next BP in Stack
//		fprintf(stderr, "*** %2d called from %016LX  (pBP at %016LX)\n", Frame,
//			(ULONG64)*(char **)(pBP + 8), (ULONG64)pBP);

//	}
	if( hDll )
		FreeLibrary(hDll);

//	char* eNextBP=0;
//	asm("mov %%eBp, %0" : "=r" (eNextBP));//mov [ia], eax
//	fprintf(stderr, "%p %p f4=%p diff=%d\n", eNextBP, *((char **)(eNextBP+sizeof(char*))), f4, *((char **)(eNextBP+sizeof(char*)))-(char*)f4);
	printf("fin\n");
}

char f3()
{
//	char* eNextBP=0;
//	asm("mov %%eBp, %0" : "=r" (eNextBP));//mov [ia], eax
//	fprintf(stderr, "%p %p f3=%p diff=%d\n", eNextBP, *((char **)(eNextBP+sizeof(char*))), f3, *((char **)(eNextBP+sizeof(char*)))-(char*)f3);
	f4();
}

char f2()
{
//	char* eNextBP=0;
//	asm("mov %%eBp, %0" : "=r" (eNextBP));//mov [ia], eax
//	fprintf(stderr, "%p %p f2=%p diff=%d\n", eNextBP, *((char **)(eNextBP+sizeof(char*))), f2, *((char **)(eNextBP+sizeof(char*)))-(char*)f2);
	f3();
}

char f1()
{
//	void *caller = get_caller();
//	fprintf(stderr, "%p %p \n", main, caller);

//	char* eNextBP=0;
//	asm("mov %%eBp, %0" : "=r" (eNextBP));//mov [ia], eax
//	fprintf(stderr, "%p %p f1=%p diff=%d\n", eNextBP, *((char **)(eNextBP+sizeof(char*))), f1, *((char **)(eNextBP+sizeof(char*)))-(char*)f1);
	f2();
}



int main()// -fdump-rtl-expand, -fdump-final-insns
{
	void *caller = get_caller();
//	char* eNextBP=0;
//	asm("mov %%eBp, %0" : "=r" (eNextBP));//mov [ia], eax
//	fprintf(stderr, "%p %p main=%p diff=%d\n", eNextBP, *((char **)(eNextBP+sizeof(char*))), main, *((char **)(eNextBP+sizeof(char*)))-(char*)main);
	f1();
	return 0;
}
