#include "config.h"

#if defined(NO_SEGFAULT) && !defined(BACKTRACE_LOG_h)
#define BACKTRACE_LOG_h

/*!
* @file BackTrace_Log.h
* @brief Ce fichier Permet d'enregister la pile d'appels dans le fichier <em>CRASH_FILENAME</em> ou
* dans le flux d'erreur (<em>stderr</em>) s'il est impossible de créer le fichier.<br />
* En plus d'enregister les crash, il est possible, via le define <em>FULL_LOG_ACTIVITY</em>
* de collecter TOUTE l'activité du programme, permettant ainsi d'avoir des statistiques:
* Les fonctions les plus appelées.
*
* <b>FULL_LOG_ACTIVITY</b><br />
* - Enregistre TOUTE l'activité du programme, permettant ainsi d'avoir des statistiques:
* Les fonctions les plus appelées. Ce define rend <em>USE_WINDOWS_API</em> et <em>USE_LINUX_API</em>
* inopérant.<br />
* <b>Note</b>: Retourne seulement les pointeurs de fonctions. Utilisez <em>nm</em> ou <em>addr2line</em>
* pour obtenir le nom des fonctions ( et si compilé en mode debug (-ggdb) : fichier et ligne )<br />
* <b>! ATTENTION ! Il faut compiler chaque fichiers .o avec CFLAGS += -finstrument-functions</b><br />
* Pour exclure des fonctions : <em>-finstrument-functions-exclude-function-list=sym,sym,...</em><br />
* ex: sym=int main( int argc, char* arvg[] )<br />
* <b>NE PAS METTRE main dans la liste des fonctions a exclure !</b>
*
* <b>USE_INTERNAL_COUNTER</b><br />
* - Force l'utilisation d'un compteur interne au programme.<br />
* <b>Note</b>: Retourne seulement les pointeurs de fonctions. Utilisez nm ou addr2line
* pour obtenir le nom de la fonction ( et si compilé en mode debug (-ggdb) : fichier et ligne )<br />
* <b>! ATTENTION ! Il faut compiler chaque fichiers .o avec CFLAGS += -finstrument-functions</b><br />
* Pour exclure des fonctions : -finstrument-functions-exclude-function-list=sym,sym,...<br />
* ex: sym=int main( int argc, char* arvg[] )<br />
* <b>NE PAS METTRE main dans la liste des fonctions a exclure !</b>
* Si <em>USE_INTERNAL_COUNTER</em> n'est pas def alors utilisation des methode interne
* a chaque OS pour récup EPB ( backtrace(), MiniDumpWriteDump(), ... )<br />
* Modifications faite par le compilateur: ( <em><b>TOTALEMENT INVISIBLE !</b></em> )
* @code
* #include "BackTrace_Log.h"
* void toto()
* {
*	__cyg_profile_func_enter( toto,  main );
*
*	//...
*
*	__cyg_profile_func_exit( toto,  main );
* }
* #include "BackTrace_Log.h"
* // Utilisation du module
* int main( int argc, char* arvg[] )// <- Le main DOIT avoir ce prototype !
* {
* 	getBackTraceIfCrash();// <- A METTRE EN 1er !
*	//...
*	toto();
*	//...
*	return 0;
* }
* @endcode
*
* <b>USE_WINDOWS_API</b><br />
* - Si vous compilez sous une platforme Windows, vous pouvez utiliser ce define, pour
* permettre l'utilisation des api Windows ( SetUnhandledExceptionFilter() ) plustôt que
* la fonction <em>signal</em>.  Si <em>USE_LINUX_API</em> est def et que la platforme
* n'est pas Windows alors define ignoré.
*
* <b>USE_LINUX_API</b><br />
* - Si vous compilez sous une platforme Linux, vous pouvez utiliser ce define, pour
* permettre l'utilisation des api Linux ( sigaction() ) plustôt que la fonction
* <em>signal</em>. Si <em>USE_LINUX_API</em> est def et que la platforme n'est pas Linux
* alors define ignoré.
*
* <b>CRASH_FILENAME</b><br />
* - Nom du fichier où enregistrer les données
*
* <b>CRASH_FILE_OPTION</b><br />
* - Mode d'ouverture du fichier. ( fopen(<em>CRASH_FILENAME</em>, <em>CRASH_FILE_OPTION</em>) )
*
* <b>MAX_DEPHT</b> <em>(defaut: 50)</em><br />
* - Profondeur maximal d'enregistrement. ( Paramètre non existant avec <em>FULL_LOG_ACTIVITY</em> )<br />
* NOTE: Ce paramètre est utilisé si def <em>USE_INTERNAL_COUNTER</em> ou def <em>USE_LINUX_API</em>
* ( avec la fonction backtrace() )
*
* <b>Utilisation</b>
* @code
* #include "BackTrace_Log.h"
* // Utilisation du module
* int main( int argc, char* arvg[] )// <- Le main DOIT avoir ce prototype !
* {
* 	getBackTraceIfCrash();// <- A METTRE EN 1er !
*	...
*	return 0;
* }
* @endcode
*
* @note <b>Le registre EBP (Frame Base Pointer)</b><br />
* Ce registre 32 bits contient un déplacement correspondant à une position dans la pile. Ce registre sert à pointer sur une donnée dans la pile. La partie basse 16 bits de ce registre peut être utilisée comme le registre BP.<br />
* <br />
* <b>Le registre EIP (Instruction Pointer)</b><br />
* Le registre EIP contient l'offset de la prochaîne instruction à exécuter. Il est modifié automatique à chaque exécution et peut être manipulé par des instruction du type jmp, call, ret, etc. On ne peut pas accéder directement à ce registre.
*/

//#define FULL_LOG_ACTIVITY
#define USE_WINDOWS_API
#define USE_LINUX_API
//#define USE_INTERNAL_COUNTER

#define CRASH_FILENAME "backtrace.log"
#define CRASH_FILE_OPTION "w"
#define MAX_DEPHT 50

//------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>

#if defined(USE_WINDOWS_API) && defined(OS_WINDOWS)
	#undef USE_LINUX_API
	#include <windows.h>
	#include <string.h>
	static LPTOP_LEVEL_EXCEPTION_FILTER prevExceptionFilter = NULL;// Pour Récup l'ancien filtre d'exception
	//#define USE_INTERNAL_COUNTER//<--------------------------------------------- A SUPPRIMER

	/***************************************************************************
	* Based on Dbghelp.h
	*/
	typedef enum
	{
		AddrMode1616,
		AddrMode1632,
		AddrModeReal,
		AddrModeFlat
	} ADDRESS_MODE;

	typedef struct _tagADDRESS
	{
		DWORD        Offset;
		WORD         Segment;
		ADDRESS_MODE Mode;
	} ADDRESS, *LPADDRESS;

	typedef struct _KDHELP
	{
		DWORD       Thread;
		DWORD       ThCallbackStack;
		DWORD       NextCallback;
		DWORD       FramePointer;
		DWORD       KiCallUserMode;
		DWORD       KeUserCallbackDispatcher;
		DWORD       SystemRangeStart;
	} KDHELP, *PKDHELP;

	typedef struct _tagSTACKFRAME {
		ADDRESS     AddrPC;
		ADDRESS     AddrReturn;
		ADDRESS     AddrFrame;
		ADDRESS     AddrStack;
		PVOID       FuncTableEntry;
		DWORD       Params[4];
		BOOL        Far;
		BOOL        Virtual;
		DWORD       Reserved[3];
		KDHELP      KdHelp;
		ADDRESS     AddrBStore;
	} STACKFRAME, *LPSTACKFRAME;

#elif defined(USE_LINUX_API) && defined(OS_LINUX)
	#undef USE_WINDOWS_API
	#ifndef _GNU_SOURCE
		#define _GNU_SOURCE
	#endif
	#ifndef __USE_GNU
		#define __USE_GNU
	#endif
	#include <execinfo.h>
	#include <signal.h>
	#include <string.h>// Pour strsignal
	#include <ucontext.h>
	#include <cxxabi.h>
	// This structure mirrors the one found in /usr/include/asm/ucontext.h
	typedef struct _sig_ucontext {
		unsigned long		uc_flags;
		struct ucontext*	uc_link;
		stack_t				uc_stack;
		struct sigcontext	uc_mcontext;
		sigset_t			uc_sigmask;
	} sig_ucontext_t;

#else
	#undef USE_WINDOWS_API
	#undef USE_LINUX_API
	#ifndef USE_INTERNAL_COUNTER
		#define USE_INTERNAL_COUNTER
	#endif
	#include <signal.h>
#endif


// On détermine ici si on récupère le kill signal et comment on le traite
#ifndef FULL_LOG_ACTIVITY
	#if defined(USE_WINDOWS_API)
		#define getBackTraceIfCrash() prevExceptionFilter = SetUnhandledExceptionFilter( Win32FaultHandler )
	#elif defined(USE_LINUX_API)
		#define getBackTraceIfCrash() intercept_signals()
		#undef intercept_signals// On veux la fonction PAS UN MACRO
	#else
		#define getBackTraceIfCrash() signal(SIGSEGV, handler)// install our handler
	#endif
#else
	#define getBackTraceIfCrash()
#endif


#define SaveStackCall() \
	FILE* fp_backtrace = fopen(CRASH_FILENAME, CRASH_FILE_OPTION); \
	if( !fp_backtrace ){ \
		fp_backtrace = stderr; \
		fprintf(stderr, "[file: "__FILE__", line: %u] Impossible d'ouvrir le fichier <" CRASH_FILENAME ">\n", __LINE__); \
	} \
	\
	fprintf(fp_backtrace, "[S] SPECIFIC LOG OF CRASH SYSTEM : Error <%s>\n", FaultTx); \
	\
	for( int i=0; i<=G_depth; i++ ) \
		fprintf(fp_backtrace, "[%d] %p\n", i, G_functionList[i]);/* %08x */ \
	\
	fseek(fp_backtrace, -2, SEEK_CUR); \
	fprintf(fp_backtrace, " <- Crash here !\n"); \
	\
	if( fp_backtrace != stderr ) \
		fclose(fp_backtrace)


extern "C"
{
#if defined(USE_INTERNAL_COUNTER) || defined(FULL_LOG_ACTIVITY)
	void __cyg_profile_func_enter (void *, void *) __attribute__((no_instrument_function));
	void __cyg_profile_func_exit (void *, void *) __attribute__((no_instrument_function));
#endif
#if !defined(main)// Pour éviter les multiples redéfinition de main
	int main( int argc, char* arvg[] );
#endif

#if defined(FULL_LOG_ACTIVITY)
	FILE* G_fp_backtrace = 0;//!< Pointeur de fichier utilisé pour enregistrer les données
	#undef USE_INTERNAL_COUNTER
#elif defined(USE_INTERNAL_COUNTER)
	void* G_functionList[MAX_DEPHT] = {0};//!< Contient la pile
	bool G_CRASH_LOGGED = false;//!< Indique si le crash a été répertorié. Utilisé avec la fonction printStackTrace()
#endif
	int G_depth = -1;//!< Profondeur actuel

#ifndef FULL_LOG_ACTIVITY// !FULL_LOG_ACTIVITY
	#if !defined(USE_WINDOWS_API) && !defined(USE_LINUX_API)
	/***********************************************************************//*!
	* @fn void handler( int sig )
	* @brief Génère un rapport d'erreur. Cette fonction est appelée via signal()
	* en cas de crash.
	* @param[in] sig	Identifiant du signal reçut.
	* @note sig ne peux pas être plus grand que 9999 ( voir le default du switch ).
	*/
	void handler( int sig )
	{
		const char* FaultTx = 0;
		switch( sig )
		{
			case SIGABRT:
				FaultTx = "Abnormal termination";
				break;
			case SIGFPE:
				FaultTx = "Floating-point error";
				break;
			case SIGILL:
				FaultTx = "Illegal instruction";
				break;
			case SIGINT:
				FaultTx = "CTRL+C signal";
				break;
			case SIGSEGV:
				FaultTx = "Illegal storage access / Segmentation violation";
				break;
			case SIGTERM:
				FaultTx = "Termination request";
				break;
			default:
				static char tmp[] = "UNKNOWN 0000";
				FaultTx = tmp;
				for( int i=0; i<4 && sig; i++ ){
					*((char*)FaultTx+11-i) = '0'+sig%10;
					sig /= 10;
				}
				break;
		}

		SaveStackCall();

		#if defined(USE_INTERNAL_COUNTER)
			G_CRASH_LOGGED = true;
		#endif
		exit(__LINE__);
	}
	#endif// !defined(USE_WINDOWS_API) && !defined(USE_LINUX_API)


	#ifdef USE_WINDOWS_API

		#if !defined(USE_INTERNAL_COUNTER)
		// The GetModuleBase function retrieves the base address of the module that contains the specified address.
		static DWORD GetModuleBase(DWORD dwAddress)
		{
			MEMORY_BASIC_INFORMATION Buffer;

			return VirtualQuery((LPCVOID) dwAddress, &Buffer, sizeof(Buffer)) ? (DWORD) Buffer.AllocationBase : 0;
		}
		#endif

	/***********************************************************************//*!
	* @fn static LONG WINAPI Win32FaultHandler( EXCEPTION_POINTERS* ExInfo )
	* @brief Génère un rapport d'erreur. Cette fonction est appelée via
	* SetUnhandledExceptionFilter() en cas de crash.
	* @note VERSION spécial Windows
	* @todo Coder la partie !defined(USE_INTERNAL_COUNTER)
	*/
	static LONG WINAPI Win32FaultHandler( EXCEPTION_POINTERS* ExInfo )
	{
		const char* FaultTx = 0;
		EXCEPTION_RECORD* ExRecord = ExInfo->ExceptionRecord;

		switch( ExRecord->ExceptionCode )
		{
			case EXCEPTION_ACCESS_VIOLATION:
				FaultTx = "ACCESS VIOLATION";
				break;
			case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
				FaultTx = "ARRAY BOUND EXCEEDED";
				break;
			case EXCEPTION_BREAKPOINT:
				FaultTx = "BREAKPOINT";
				break;
			case EXCEPTION_DATATYPE_MISALIGNMENT:
				FaultTx = "DATATYPE MISALIGNMENT";
				break;
			case EXCEPTION_FLT_DENORMAL_OPERAND:
				FaultTx = "FLOAT DENORMAL OPERAND";
				break;
			case EXCEPTION_FLT_DIVIDE_BY_ZERO:
				FaultTx = "FLT DIVIDE BY ZERO";
				break;
			case EXCEPTION_FLT_INEXACT_RESULT:
				FaultTx = "Float Inexact Result";
				break;
			case EXCEPTION_FLT_INVALID_OPERATION:
				FaultTx = "Float Invalid Operation";
				break;
			case EXCEPTION_FLT_OVERFLOW:
				FaultTx = "Float Overflow";
				break;
			case EXCEPTION_FLT_STACK_CHECK:
				FaultTx = "Float Stack Check";
				break;
			case EXCEPTION_FLT_UNDERFLOW:
				FaultTx = "Float Underflow";
				break;
			case EXCEPTION_GUARD_PAGE:
				FaultTx = "Guard Page";
				break;
			case EXCEPTION_ILLEGAL_INSTRUCTION:
				FaultTx = "Illegal Instruction";
				break;
			case EXCEPTION_IN_PAGE_ERROR:
				FaultTx = "In Page Error";
				break;
			case EXCEPTION_INT_DIVIDE_BY_ZERO:
				FaultTx = "Integer Divide By Zero";
				break;
			case EXCEPTION_INT_OVERFLOW:
				FaultTx = "Integer Overflow";
				break;
			case EXCEPTION_INVALID_DISPOSITION:
				FaultTx = "Invalid Disposition";
				break;
			case EXCEPTION_INVALID_HANDLE:
				FaultTx = "Invalid Handle";
				break;
			case EXCEPTION_NONCONTINUABLE_EXCEPTION:
				FaultTx = "Noncontinuable Exception";
				break;
			case EXCEPTION_PRIV_INSTRUCTION:
				FaultTx = "Privileged Instruction";
				break;
			case EXCEPTION_SINGLE_STEP:
				FaultTx = "Single Step";
				break;
			case EXCEPTION_STACK_OVERFLOW:
				FaultTx = "Stack Overflow";
				break;
			case DBG_CONTROL_C:
				FaultTx = "Control+C";
				break;
			case DBG_CONTROL_BREAK:
				FaultTx = "Control+Break";
				break;
			case DBG_TERMINATE_THREAD:
				FaultTx = "Terminate Thread";
				break;
			case DBG_TERMINATE_PROCESS:
				FaultTx = "Terminate Process";
				break;
			case RPC_S_UNKNOWN_IF:
				FaultTx = "Unknown Interface";
				break;
			case RPC_S_SERVER_UNAVAILABLE:
				FaultTx = "Server Unavailable";
				break;
			default:
				static char tmp[50] = {0};
				sprintf(tmp, "UNKNOWN (%X)", (unsigned int)ExRecord->ExceptionCode);
				FaultTx = tmp;
				break;
		}

		#if defined(USE_INTERNAL_COUNTER)
			SaveStackCall();
			G_CRASH_LOGGED = true;
		#else
			FILE* fp_backtrace = fopen(CRASH_FILENAME, CRASH_FILE_OPTION);
			if( !fp_backtrace ){
				fp_backtrace = stderr;
				fprintf(stderr, "[file: "__FILE__", line: %u] Impossible d'ouvrir le fichier <" CRASH_FILENAME ">\n", __LINE__);
			}

			fprintf(fp_backtrace, "[SW] SPECIFIC LOG OF CRASH SYSTEM : Error <%s> at location %p", FaultTx, ExRecord->ExceptionAddress);
			// If the exception was an access violation, print out some additional information, to the error log and the debugger.
			if( ExRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION && ExRecord->NumberParameters >= 2 )
				fprintf(fp_backtrace, " %s location %p", ExRecord->ExceptionInformation[0] ? "Writing to" : "Reading from", (void*)ExRecord->ExceptionInformation[1]);

			{// Affichage de la date
				SYSTEMTIME SystemTime;
				GetLocalTime(&SystemTime);
				fprintf(fp_backtrace, ".\nDate: %02i/%02i/%02i at %02i:%02i:%02i\n", SystemTime.wDay, SystemTime.wMonth, SystemTime.wYear, SystemTime.wHour, SystemTime.wMinute, SystemTime.wSecond);
			}

			CONTEXT* ExContext = ExInfo->ContextRecord;
			HMODULE hModule = NULL;
			TCHAR szModule[MAX_PATH] = {0};

			#ifdef _M_IX86	// Intel Only!
				// Show the registers
				fprintf(fp_backtrace, "Registers:\n");
				if( ExContext->ContextFlags & CONTEXT_INTEGER )
					fprintf(fp_backtrace,
						"eax=%08lx ebx=%08lx ecx=%08lx edx=%08lx esi=%08lx edi=%08lx\n",
						ExContext->Eax,
						ExContext->Ebx,
						ExContext->Ecx,
						ExContext->Edx,
						ExContext->Esi,
						ExContext->Edi
					);
				if( ExContext->ContextFlags & CONTEXT_CONTROL )
					fprintf(fp_backtrace,
						"eip=%08lx esp=%08lx ebp=%08lx iopl=%1lx %s %s %s %s %s %s %s %s %s %s\n",
						ExContext->Eip,
						ExContext->Esp,
						ExContext->Ebp,
						(ExContext->EFlags >> 12) & 3,	//  IOPL level value
						ExContext->EFlags & 0x00100000 ? "vip" : "   ",	//  VIP (virtual interrupt pending)
						ExContext->EFlags & 0x00080000 ? "vif" : "   ",	//  VIF (virtual interrupt flag)
						ExContext->EFlags & 0x00000800 ? "ov" : "nv",	//  VIF (virtual interrupt flag)
						ExContext->EFlags & 0x00000400 ? "dn" : "up",	//  OF (overflow flag)
						ExContext->EFlags & 0x00000200 ? "ei" : "di",	//  IF (interrupt enable flag)
						ExContext->EFlags & 0x00000080 ? "ng" : "pl",	//  SF (sign flag)
						ExContext->EFlags & 0x00000040 ? "zr" : "nz",	//  ZF (zero flag)
						ExContext->EFlags & 0x00000010 ? "ac" : "na",	//  AF (aux carry flag)
						ExContext->EFlags & 0x00000004 ? "po" : "pe",	//  PF (parity flag)
						ExContext->EFlags & 0x00000001 ? "cy" : "nc"	//  CF (carry flag)
					);
				if( ExContext->ContextFlags & CONTEXT_SEGMENTS )
				{
					fprintf(fp_backtrace,
						"cs=%04lx  ss=%04lx  ds=%04lx  es=%04lx  fs=%04lx  gs=%04lx",
						ExContext->SegCs,
						ExContext->SegSs,
						ExContext->SegDs,
						ExContext->SegEs,
						ExContext->SegFs,
						ExContext->SegGs
					);
					if(ExContext->ContextFlags & CONTEXT_CONTROL)
						fprintf(fp_backtrace,
							"             efl=%08lx",
							ExContext->EFlags
						);
				}else
					if(ExContext->ContextFlags & CONTEXT_CONTROL)
						fprintf(fp_backtrace,
							"                                                                       efl=%08lx",
							ExContext->EFlags
						);
				fprintf(fp_backtrace, "\n\n");
			#endif// _M_IX86 - Intel Only!

			STACKFRAME StackFrame;

			memset( &StackFrame, 0, sizeof(StackFrame) );

			// Initialize the STACKFRAME structure for the first call.  This is only
			// necessary for Intel CPUs, and isn't mentioned in the documentation.
			StackFrame.AddrPC.Offset = ExContext->Eip;
			StackFrame.AddrPC.Mode = AddrModeFlat;
			StackFrame.AddrStack.Offset = ExContext->Esp;
			StackFrame.AddrStack.Mode = AddrModeFlat;
			StackFrame.AddrFrame.Offset = ExContext->Ebp;
			StackFrame.AddrFrame.Mode = AddrModeFlat;

			fprintf(fp_backtrace, "Call stack:\n");

			HANDLE hProcess = GetCurrentProcess();
			bool Crash_here_MSG = 1;
			while(1){
				if( !StackFrame.Reserved[0] ){
					StackFrame.Reserved[0] = 1;
					StackFrame.AddrReturn.Mode = AddrModeFlat;
				}else{
					StackFrame.AddrPC.Offset = StackFrame.AddrReturn.Offset;
					//AddrStack = AddrFrame + 2*sizeof(DWORD);
					if( !ReadProcessMemory(hProcess, (LPCVOID) StackFrame.AddrFrame.Offset, &StackFrame.AddrFrame.Offset, sizeof(DWORD), NULL) )
						break;
					if( !ReadProcessMemory(hProcess, (LPCVOID) (StackFrame.AddrFrame.Offset + sizeof(DWORD)), &StackFrame.AddrReturn.Offset, sizeof(DWORD), NULL) )
						break;
				}

				ReadProcessMemory(hProcess, (LPCVOID) (StackFrame.AddrFrame.Offset + 2*sizeof(DWORD)), StackFrame.Params, sizeof(StackFrame.Params), NULL);

				if( !StackFrame.AddrFrame.Offset )
					break;

				fprintf(fp_backtrace, "%p", (void*)(StackFrame.AddrPC.Offset-0x10));

				if((hModule = (HMODULE) GetModuleBase(StackFrame.AddrPC.Offset)) && GetModuleFileName(hModule, szModule, sizeof(szModule)))
					fprintf(fp_backtrace, "  %s:ModuleBase %p stack: %p", szModule, (void*)hModule, (void*)StackFrame.AddrPC.Offset);
				else
					fprintf(fp_backtrace, "  Module unknown, stack: %p", (void*)StackFrame.AddrPC.Offset);

				fprintf(fp_backtrace, ", Param %010X %010X %010X %010X\n", (unsigned int)StackFrame.Params[0], (unsigned int)StackFrame.Params[1], (unsigned int)StackFrame.Params[2], (unsigned int)StackFrame.Params[3]);

				if( Crash_here_MSG ){
					Crash_here_MSG = 0;
					fseek(fp_backtrace, -2, SEEK_CUR);
					fprintf(fp_backtrace, " <- Crash here !\n");
				}
			}

			if( fp_backtrace != stderr )
				fclose(fp_backtrace);
		#endif// !defined(USE_INTERNAL_COUNTER)

		/*if(want to continue)
		{
			ExInfo->ContextRecord->Eip++;
			return EXCEPTION_CONTINUE_EXECUTION;
		}
		*/

		if( prevExceptionFilter )// S'il y avait déjà un gestionnaire de crash -> On lui passe les info que l'on a
			return prevExceptionFilter(ExInfo);
		return EXCEPTION_EXECUTE_HANDLER;
	}
	#endif// defined(USE_WINDOWS_API)


	/***************************************************************************
	* BackTrace avec sigaction ( LINUX )
	*/
	#ifdef USE_LINUX_API // defined(USE_LINUX_API)
	/***********************************************************************//*!
	* @fn void crit_err_hdlr( int si_signo, siginfo_t* info, void* ucontext )
	* @brief Génère un rapport d'erreur. Cette fonction est appelée via sigaction()
	* en cas de crash.
	* @param[in] si_signo		Numéro de signal
	* @param[in,out] info		Pointeur sur les info du crash ( cf struct siginfo_t )
	* @param[in,out] ucontext	Pointeur sur les info du crash ( cf struct sig_ucontext_t )
	* @note VERSION spécial Linux
	* @warning NE PAS OUBLIER DE COMPILER AVEC CFLAGS += -Wl,--export-all-symbols et -rdynamic<br />
	* (-finstrument-functions n'est pas du tout utilisé ICI)
	*/
	void crit_err_hdlr( int si_signo, siginfo_t* info, void* ucontext )
	{
		/*
		siginfo_t {
			int     si_signo;		// Numéro de signal
			int     si_errno;		// Numéro d'erreur
			int     si_code;		// Code du signal
			pid_t   si_pid;			// PID de l'émetteur
			uid_t   si_uid;			// UID réel de l'émetteur
			int     si_status;		// Valeur de sortie
			clock_t si_utime;		// Temps utilisateur écoulé
			clock_t si_stime;		// Temps système écoulé
			sigval_t si_value;		// Valeur de signal
			int     si_int;			// Signal Posix.1b
			void *  si_ptr;			// Signal Posix.1b
			void *  si_addr;		// Emplacement d'erreur
			int     si_band;		// Band event
			int     si_fd;			// Descripteur de fichier
		}
		*/

		#if defined(USE_INTERNAL_COUNTER)
			(void)info;
			(void)ucontext;
			const char* FaultTx = strsignal(si_signo);
			SaveStackCall();
		#else
			/***********************************************************************
			* Ouverture du fichier
			*/
			FILE* fp_backtrace = fopen(CRASH_FILENAME, CRASH_FILE_OPTION); \
			if( !fp_backtrace ){
				fp_backtrace = stderr;
				fprintf(stderr, "[file: "__FILE__", line: %u] Impossible d'ouvrir le fichier <" CRASH_FILENAME ">\n", __LINE__);
			}

			/***********************************************************************
			* On Affiche la pile
			*/
			sig_ucontext_t* uc = (sig_ucontext_t*)ucontext;

			void* caller_address = (void *)uc->uc_mcontext.eip; // x86 specific

			fprintf(fp_backtrace, "[SU] Got signal %d (%s), faulty address is %p, from %p\n", si_signo, strsignal(si_signo), info->si_addr, caller_address);
			fflush(fp_backtrace);

			void* trace[MAX_DEPHT];
			int size = backtrace(trace, MAX_DEPHT);

			// Overwrite sigaction with caller's address
			trace[1] = caller_address;

			char** messages = backtrace_symbols(trace, size);
			char* mangled_name = 0, *offset_begin = 0, *offset_end = 0;

			// i=1 => Skip first stack frame (points here)
			for( int i=size-1; i>1 && messages != NULL; --i )// Lecture de la pile de bas en haut
			{
				mangled_name = 0;
				offset_begin = 0;
				offset_end = 0;

				// Find parantheses and +address offset surrounding mangled name
				for( char* p = messages[i]; *p; p++ )
				{
					if (*p == '(')
						mangled_name = p;
					else if (*p == '+')
						offset_begin = p;
					else if (*p == ')')
					{
						offset_end = p;
						break;
					}
				}

				// If the line could be processed, attempt to demangle the symbol
				if( mangled_name && offset_begin && offset_end && mangled_name < offset_begin )
				{
					*mangled_name++ = '\0';
					*offset_begin++ = '\0';
					*offset_end++ = '\0';

					#ifdef _CXXABI_H// defined(_CXXABI_H)
						int status;
						char* real_name = abi::__cxa_demangle(mangled_name, 0, 0, &status);

						// if demangling is successful, output the demangled function name
						if (status == 0)
							fprintf(fp_backtrace, "[%d] %p <%s> : %s+%s%s\n", size-i-1, trace[i], messages[i], real_name, offset_begin, offset_end);
						// otherwise, output the mangled function name
						else
							fprintf(fp_backtrace, "[%d] %p <%s>: %s+%s%s\n", size-i-1, trace[i], messages[i], mangled_name, offset_begin, offset_end);

						free(real_name);
					#else// if defined(_CXXABI_H) else !defined(_CXXABI_H)
						fprintf(fp_backtrace, "[%d] %p <%s> : %s+%s%s\n", size-i-1, trace[i], messages[i], mangled_name, offset_begin, offset_end);
					#endif// !defined(_CXXABI_H)
				}
				// otherwise, print the whole line
				else
					fprintf(fp_backtrace, "[%d] %p <%s>\n", size-i-1, trace[i], messages[i]);
			}

			free(messages);

			fseek(fp_backtrace, -1, SEEK_CUR);
			fprintf(fp_backtrace, " <- Crash here !\n");

			if( fp_backtrace != stderr )
				fclose(fp_backtrace);
		#endif

		#if defined(USE_INTERNAL_COUNTER)
			G_CRASH_LOGGED = true;
		#endif
		exit(EXIT_FAILURE);
	}


	/***********************************************************************//*!
	* @fn void intercept_signals()
	* @brief Installe le système de capture des signaux ( sigaction )
	* @note VERSION spécial Linux
	* @warning NE PAS OUBLIER DE COMPILER AVEC<br />
	* CFLAGS += -Wl,--export-all-symbols (WINDOWS), -rdynamic (LINUX)
	*/
	void intercept_signals()
	{
		/*
		struct sigaction {
			void     (* sa_handler)   (int);// sa_handler indique l'action affectée au signal signum, et peut être SIG_DFL pour l'action par défaut, SIG_IGN pour ignorer le signal, ou un pointeur sur une fonction de gestion de signaux.
			void     (* sa_sigaction) (int, siginfo_t *, void *);
			sigset_t    sa_mask;			// sa_mask fournit un masque de signaux à bloquer pendant l'exécution du gestionnaire. De plus le signal ayant appelé le gestionnaire est bloqué à moins que les attributs SA_NODEFER ou SA_NOMASK soient précisés.
			int         sa_flags;			// sa_flags spécifie un ensemble d'attributs qui modifient le comportement du gestionnaire de signaux. Il est formé par un OU binaire ( | ) entre les options suivantes :
			void     (* sa_restorer)  (void);// Obsolète ( DO NOT USE ! )
		}
		*/
		struct sigaction sigact;
		sigact.sa_sigaction = crit_err_hdlr;
		sigact.sa_flags = SA_RESTART | SA_SIGINFO;

		if( sigaction(SIGSEGV, &sigact, (struct sigaction *)NULL) != 0 )
		{
			fprintf(stderr, "[file " __FILE__ ", line %d]: Error setting signal handler for %d (%s)\n", __LINE__, SIGSEGV, strsignal(SIGSEGV));
			exit(EXIT_FAILURE);
		}
		if( sigaction(SIGILL, &sigact, (struct sigaction *)NULL) != 0 )
		{
			fprintf(stderr, "[file " __FILE__ ", line %d]: Error setting signal handler for %d (%s)\n", __LINE__, SIGILL, strsignal(SIGILL));
			exit(EXIT_FAILURE);
		}
		if( sigaction(SIGFPE, &sigact, (struct sigaction *)NULL) != 0 )
		{
			fprintf(stderr, "[file " __FILE__ ", line %d]: Error setting signal handler for %d (%s)\n", __LINE__, SIGFPE, strsignal(SIGFPE));
			exit(EXIT_FAILURE);
		}
	}
	#endif// defined(USE_LINUX_API)
#endif// !FULL_LOG_ACTIVITY


	#if defined(USE_INTERNAL_COUNTER) || defined(FULL_LOG_ACTIVITY)
	/***********************************************************************//*!
	* @fn void __cyg_profile_func_enter( void* func,  void* caller )
	* @brief Fonction qui reçoit les ENTREES de fonctions
	* @warning A COMPILER AVEC -finstrument-functions pour chaque fichiers .o ( CFLAGS )
	*/
	void __cyg_profile_func_enter( void* func,  void* caller )
	{
		(void)caller;
		#ifndef FULL_LOG_ACTIVITY// !FULL_LOG_ACTIVITY
			#if defined(USE_WINDOWS_API)
				if( func == Win32FaultHandler )
			#elif defined(USE_LINUX_API)
				if( func == crit_err_hdlr )
			#else// !defined(USE_WINDOWS_API) && !defined(USE_LINUX_API)
				if( func == handler || func == signal )
			#endif
					return ;
			G_depth++;
			if( G_depth < MAX_DEPHT )
				G_functionList[G_depth] = func;
			else{
				fprintf(stderr, "Attention ! Plus de EBP > 50 !\n");
				fprintf(stderr, "Entrée dans la fonction <%p>\n", func);
			}
//			void* eNextBP=0;
//			asm("mov %%eBp, %0" : "=r" (eNextBP));//mov [ia], eax
//			fprintf(stderr, "%p %p %p %p\n", eNextBP, *(void **)(eNextBP-sizeof(void*)*2), __cyg_profile_func_enter, func);
		#else// FULL_LOG_ACTIVITY
			if( func == main )
			{
				if( !(G_fp_backtrace = fopen(CRASH_FILENAME, CRASH_FILE_OPTION)) )
				//	stdError("Erreur lors de l'ouverture du fichier");
					fprintf(stderr, "Erreur lors de l'ouverture du fichier\n");

				fprintf(G_fp_backtrace, "[F] FULL LOG OF CRASH SYSTEM\n");
			}

			if( !G_fp_backtrace ){
				G_fp_backtrace = stderr;
				fprintf(stderr, "ATTENTION ! Erreur lors de la création du fichier <backtrace.log> !\n");
			}

			G_depth++;
			for( int n=0; n<G_depth; n++ )
				fprintf(G_fp_backtrace, " ");
			fprintf(G_fp_backtrace, "-> %p\n", func);// %08x
			fflush(G_fp_backtrace);
		#endif
	}


	/***********************************************************************//*!
	* @fn void __cyg_profile_func_exit( void *func, void *caller )
	* @brief Fonction qui reçoit les SORTIES de fonctions
	* @warning A COMPILER AVEC -finstrument-functions pour chaque fichiers .o ( CFLAGS )<br />
	* Pour exclure des fonctions : -finstrument-functions-exclude-function-list=sym,sym,...<br />
	* ex: sym=int main( int argc, char* arvg[] )
	* @code
	* // Utilisation du module
	* int main( int argc, char* arvg[] )
	* {
	* 	getBackTraceIfCrash();// <- A METTRE EN 1er !
	*	...
	*	return 0;
	* }
	* @endcode
	*/
	void __cyg_profile_func_exit( void *func, void *caller )
	{
		(void)caller;
		#ifndef FULL_LOG_ACTIVITY// !FULL_LOG_ACTIVITY
			#if defined(USE_WINDOWS_API)
				if( func == Win32FaultHandler )
			#elif defined(USE_LINUX_API)
				if( func == crit_err_hdlr )
			#else// !defined(USE_WINDOWS_API) && !defined(USE_LINUX_API)
				if( func == handler || func == signal )
			#endif
					return ;

			if( G_functionList[G_depth] == func ){
				G_functionList[G_depth] = 0;
				G_depth--;
			}else{
				fprintf(stderr, "Sortie de la fonction <%p>\n", func);
			}
		#else // FULL_LOG_ACTIVITY
			if( !G_fp_backtrace ){
				G_fp_backtrace = stderr;
				fprintf(stderr, "ATTENTION ! Erreur lors de la création du fichier <backtrace.log> !\n");
			}

			for( int n=0; n<G_depth; n++ )
				fprintf(G_fp_backtrace, " ");
			fprintf(G_fp_backtrace, "<- %p\n", func);
			fflush(G_fp_backtrace);
			G_depth--;

			if( func == main )
			{
				fclose(G_fp_backtrace);
				G_fp_backtrace = 0;
			}
		#endif// FULL_LOG_ACTIVITY
	}
	#endif// defined(USE_INTERNAL_COUNTER) || defined(FULL_LOG_ACTIVITY)


	/***********************************************************************//*!
	* @fn static void printStackTrace()
	* @brief Fonction qui enregistre la pile dans le fichier CRASH_FILENAME
	* @warning A COMPILER AVEC -finstrument-functions pour chaque fichiers .o ( CFLAGS )
	*/
	#if defined(USE_INTERNAL_COUNTER)
	static void printStackTrace()
	{
		const char FaultTx[] = "Just a Print";

		if( G_CRASH_LOGGED == true )
			return ;

		#define tmp_CRASH_FILE_OPTION CRASH_FILE_OPTION
		#undef CRASH_FILE_OPTION
		#define CRASH_FILE_OPTION "a"
		SaveStackCall();
		#undef CRASH_FILE_OPTION
		#define CRASH_FILE_OPTION tmp_CRASH_FILE_OPTION
		#undef tmp_CRASH_FILE_OPTION
	}
	#else// if defined(USE_INTERNAL_COUNTER) else !defined(USE_INTERNAL_COUNTER)
	inline static void printStackTrace()
	{
		fprintf(stderr, "Impossible d'afficher la pile !\n");
	}
	#endif// !defined(USE_INTERNAL_COUNTER)
}
#undef SaveStackCall
#endif// BACKTRACE_LOG_h
