/* ==========================================================================
  File : ExceptionReport.h
  Vers.: 0.9
  Plat.: Windows 98 or above
  Desc.: ExceptionHandle routine (report)
  Req. : DbgHelp v5.1 or above

  Adapted from MSDN March 2002 - Under the Hood
========================================================================== */



#define _WIN32_WINDOWS		0x0410		// Windows 98
#define _WIN32_WINNT		0x0500		// Windows 2000

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <tchar.h>
#include <dbghelp.h>

#include "ExceptionReport.h"

#define USE_MINIDUMP		0

//CExceptionReport g_CExceptionReport;

typedef struct _DUMP_PARAMETER
{
    HANDLE              Thread;
    PEXCEPTION_POINTERS ExPtrs;
} DUMP_PARAMETER, *PDUMP_PARAMETER;

TCHAR CExceptionReport::m_szLogPrefixName[ MAX_PATH ];
TCHAR CExceptionReport::m_szModuleName[ MAX_PATH ];
LPTOP_LEVEL_EXCEPTION_FILTER CExceptionReport::m_OldFilter;
HANDLE CExceptionReport::m_hReportFile;
HANDLE CExceptionReport::m_hProcess;
BOOL CExceptionReport::m_bHasSymbol;
LPCERUserFunc CExceptionReport::m_lpUserFunc;
int CExceptionReport::bSF = 0;

#pragma comment(linker, "/defaultlib:dbghelp.lib")

//
// Constructor
//
CExceptionReport::CExceptionReport()
{
	m_bHasSymbol = FALSE;
	SetProgramName();
	SetUserFunc( NULL );

	// Install the unhandled exception filter function
    m_OldFilter = SetUnhandledExceptionFilter( UnhandledExceptionFilter );
	m_hProcess = GetCurrentProcess();
	bSF = 0;
}

//
// Destructor
//
CExceptionReport::~CExceptionReport()
{
	SetUnhandledExceptionFilter( m_OldFilter );
}

//
// Set application-specific function
//
void CExceptionReport::SetUserFunc( LPCERUserFunc lpUserFunc )
{
	m_lpUserFunc = lpUserFunc;
}

//
// Set application-name for prefix of log filename
//
void CExceptionReport::SetProgramName( PTSTR pszProgramName )
{
	if( pszProgramName == NULL ) {
		TCHAR szDrive[MAX_PATH], szDir[MAX_PATH], szFilename[MAX_PATH], szExt[MAX_PATH];

		// Figure out what the report file will be named, and store it away
	    GetModuleFileName( 0, m_szLogPrefixName, MAX_PATH );

		PTSTR pszDot = m_szLogPrefixName;
	    // Look for the '.' before the "EXE" extension. Replace '.' to '\0'
		if ((pszDot = _tcsrchr(pszDot, _T('.')))) {
			*pszDot = 0;
		}

		_tsplitpath( m_szLogPrefixName, szDrive, szDir, szFilename, szExt );
		_tcscpy( m_szLogPrefixName, szFilename );
	} else {
		_tcscpy( m_szLogPrefixName, pszProgramName );
	}
}

//
// This is called when unhandled exception occurs
//
LONG WINAPI CExceptionReport::UnhandledExceptionFilter( PEXCEPTION_POINTERS pExceptionInfo )
{
	wchar_t szFileName[MAX_PATH] = { 0, };

	_tcscpy( szFileName, m_szLogPrefixName );

    SYSTEMTIME stSystemTime;
	GetLocalTime( &stSystemTime );

	wsprintf( szFileName + wcslen( szFileName ), L"%04d-%02d-%02d %02d,%02d,%02d.TXT", 
		stSystemTime.wYear, stSystemTime.wMonth, stSystemTime.wDay,
		stSystemTime.wHour, stSystemTime.wMinute, stSystemTime.wSecond );

	if( (m_hReportFile = CreateFile( szFileName, GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, FILE_FLAG_WRITE_THROUGH, 0 )) != INVALID_HANDLE_VALUE ) {
		SetFilePointer( m_hReportFile, 0, 0, FILE_END );
		WriteExceptionReport( pExceptionInfo );
		CloseHandle( m_hReportFile );
		m_hReportFile = NULL;
	}

#if USE_MINIDUMP == 1
	_tcscpy( szFileName, m_szLogPrefixName );
	wsprintf( szFileName + strlen( szFileName ), " %04d-%02d-%02d %02d,%02d,%02d.DMP", 
		stSystemTime.wYear, stSystemTime.wMonth, stSystemTime.wDay,
		stSystemTime.wHour, stSystemTime.wMinute, stSystemTime.wSecond );

	HANDLE hDumpFile;

	if( (hDumpFile = CreateFile( szFileName, GENERIC_WRITE, FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0 )) != INVALID_HANDLE_VALUE ) {
		MINIDUMP_EXCEPTION_INFORMATION ExInfo;

		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = pExceptionInfo;
		ExInfo.ClientPointers = NULL;
		MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &ExInfo, NULL, NULL );
		CloseHandle( hDumpFile );
	}
#endif

//	if( m_OldFilter )
//		return m_OldFilter( pExceptionInfo );
//	else
//		return EXCEPTION_CONTINUE_SEARCH;
	return EXCEPTION_EXECUTE_HANDLER;
}


DWORD WINAPI WriteDumpOnSF(LPVOID param)
{
    PDUMP_PARAMETER dumpParam = reinterpret_cast<PDUMP_PARAMETER>(param);

	CExceptionReport report;
	report.bSF = 1;
	report.UnhandledExceptionFilter(dumpParam->ExPtrs);

	return 0;
}
//
// Write informations to Report file. Called by UnhandledExceptionFilter
//
void CExceptionReport::WriteExceptionReport( PEXCEPTION_POINTERS pExceptionInfo )
{
	if(0 == bSF)
	{
		//_tprintf( _T( "Exception    : %08X (%s)\r\n"), pExceptionRecord->ExceptionCode, GetExceptionString( pExceptionRecord->ExceptionCode ) );
		{
			switch ( pExceptionInfo->ExceptionRecord->ExceptionCode ) 
			{
			case EXCEPTION_ACCESS_VIOLATION: 			_tprintf( _T( "Exception    : EXCEPTION_ACCESS_VIOLATION\r\n") ); 			break;
			case EXCEPTION_BREAKPOINT:			_tprintf( _T( "Exception    : EXCEPTION_BREAKPOINT\r\n") );			break;
			case EXCEPTION_STACK_OVERFLOW:			_tprintf( _T( "Exception    : EXCEPTION_STACK_OVERFLOW\r\n") );			break;
			case EXCEPTION_GUARD_PAGE:			_tprintf( _T( "Exception    : EXCEPTION_GUARD_PAGE\r\n") );			break;
			case EXCEPTION_INVALID_HANDLE:			_tprintf( _T( "Exception    : EXCEPTION_INVALID_HANDLE\r\n") );			break;

			case EXCEPTION_DATATYPE_MISALIGNMENT:			_tprintf( _T( "Exception    : EXCEPTION_DATATYPE_MISALIGNMENT\r\n") );			break;
			case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:			_tprintf( _T( "Exception    : EXCEPTION_ARRAY_BOUNDS_EXCEEDED\r\n") );			break;
			case EXCEPTION_FLT_DENORMAL_OPERAND:			_tprintf( _T( "Exception    : EXCEPTION_FLT_DENORMAL_OPERAND\r\n") );			break;
			case EXCEPTION_FLT_DIVIDE_BY_ZERO:			_tprintf( _T( "Exception    : EXCEPTION_FLT_DIVIDE_BY_ZERO\r\n") );			break;
			case EXCEPTION_FLT_INEXACT_RESULT:			_tprintf( _T( "Exception    : EXCEPTION_FLT_INEXACT_RESULT\r\n") );			break;
			case EXCEPTION_FLT_INVALID_OPERATION:			_tprintf( _T( "Exception    : EXCEPTION_FLT_INVALID_OPERATION\r\n") );			break;
			case EXCEPTION_FLT_OVERFLOW:			_tprintf( _T( "Exception    : EXCEPTION_FLT_OVERFLOW\r\n") );			break;
			case EXCEPTION_FLT_STACK_CHECK:			_tprintf( _T( "Exception    : EXCEPTION_FLT_STACK_CHECK\r\n") );			break;
			case EXCEPTION_FLT_UNDERFLOW:			_tprintf( _T( "Exception    : EXCEPTION_FLT_UNDERFLOW\r\n") );			break;
			case EXCEPTION_INT_DIVIDE_BY_ZERO:			_tprintf( _T( "Exception    : EXCEPTION_INT_DIVIDE_BY_ZERO\r\n") );			break;
			case EXCEPTION_INT_OVERFLOW:			_tprintf( _T( "Exception    : EXCEPTION_INT_OVERFLOW\r\n") );			break;
			case EXCEPTION_PRIV_INSTRUCTION:			_tprintf( _T( "Exception    : EXCEPTION_PRIV_INSTRUCTION\r\n") );			break;
			case EXCEPTION_IN_PAGE_ERROR:			_tprintf( _T( "Exception    : EXCEPTION_IN_PAGE_ERROR\r\n") );			break;
			case EXCEPTION_ILLEGAL_INSTRUCTION:			_tprintf( _T( "Exception    : EXCEPTION_ILLEGAL_INSTRUCTION\r\n") );			break;
			case EXCEPTION_NONCONTINUABLE_EXCEPTION:			_tprintf( _T( "Exception    : EXCEPTION_NONCONTINUABLE_EXCEPTION\r\n") );			break;
			case EXCEPTION_INVALID_DISPOSITION:			_tprintf( _T( "Exception    : EXCEPTION_INVALID_DISPOSITION\r\n") );			break;
			}
		}

	
		if( EXCEPTION_STACK_OVERFLOW == pExceptionInfo->ExceptionRecord->ExceptionCode)
		{
			PDUMP_PARAMETER dumpParam = (PDUMP_PARAMETER)malloc(sizeof(DUMP_PARAMETER));
			dumpParam->Thread = GetCurrentThread();
			dumpParam->ExPtrs = pExceptionInfo;
		
			HANDLE hThread = CreateThread(NULL, 102400, WriteDumpOnSF, dumpParam, 0, NULL);
			WaitForSingleObject(hThread, INFINITE);
			CloseHandle(hThread);
			return;
		}
	}
	
    // Start out with a banner
    _tprintf( _T( "==============================================================================\r\n\r\n" ) );
	WriteBasicInfo( pExceptionInfo->ExceptionRecord );
	WriteRegistersInfo( pExceptionInfo->ContextRecord );

    SymSetOptions( SYMOPT_DEFERRED_LOADS );
    // Initialize DbgHelp
    if( !SymInitialize( GetCurrentProcess(), 0, TRUE ) )
	{
		_tprintf( _T("SymInitialize fail %d error\n", GetLastError()) );
	}
    
    CONTEXT trashableContext = *(pExceptionInfo->ContextRecord);
    WriteStackDetails( &trashableContext, FALSE );
#ifdef PRINT_DETAIL_EXCEPTION
#ifdef _M_IX86  // X86 Only!
	if( m_bHasSymbol ) {
		trashableContext = *(pExceptionInfo->ContextRecord);
		WriteStackDetails( &trashableContext, TRUE );
		_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
	    _tprintf( _T( "    Global Variables\r\n" ) );
		_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
		_tprintf( _T( "\r\n" ) );
	    SymEnumSymbols( GetCurrentProcess(), (DWORD64) GetModuleHandle( m_szModuleName ), 0, EnumerateSymbolsCallback, 0 );
	}
#endif      // X86 Only!
    SymCleanup( GetCurrentProcess() );

	_tprintf( _T("\r\n") );
    _tprintf( _T("\r\n") );

	WriteMemoryDump( pExceptionInfo->ContextRecord );

	if( m_lpUserFunc ) {
		_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
	    _tprintf( _T( "    Application-specific log\r\n" ) );
		_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
		_tprintf( _T( "\r\n" ) );

		m_lpUserFunc( m_hReportFile );
		_tprintf( _T( "\r\n" ) );
		_tprintf( _T( "\r\n" ) );
	}

	_tprintf( _T( "==============================================================================\r\n" ) );
#endif
}

//
// Write basic informations(user, computer name, error type)
//
void CExceptionReport::WriteBasicInfo( PEXCEPTION_RECORD pExceptionRecord )
{
	TCHAR szFileName[MAX_PATH] = _T( "" ), szUserName[MAX_PATH] = _T( "" ), szComputerName[MAX_PATH] = _T( "" );
	DWORD dwUserLen = MAX_PATH, dwComputerLen = MAX_PATH;

	SYSTEMTIME stSystemTime;
	GetLocalTime( &stSystemTime );

    GetModuleFileName( (HMODULE) NULL, szFileName, MAX_PATH );
	GetUserName( szUserName, &dwUserLen );
	GetComputerName( szComputerName, &dwComputerLen );

	_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
    _tprintf( _T( "    Basic Information\r\n" ) );
	_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
	_tprintf( _T( "\r\n" ) );

	_tprintf( _T( "Program Name : %s\r\n" ), m_szLogPrefixName );
	_tprintf( _T( "EXE          : %s\r\n" ), szFileName );
	_tprintf( _T( "User         : %s\r\n" ), szUserName );
	_tprintf( _T( "Computer     : %s\r\n" ), szComputerName );
	_tprintf( _T( "ThreadId     : %d\r\n" ), GetCurrentThreadId() );
	_tprintf( _T( "\r\n" ) );

    // Print information about the type of fault and where the fault occured
    DWORD section, offset;
    GetLogicalAddress(  pExceptionRecord->ExceptionAddress, m_szModuleName, sizeof( m_szModuleName ), section, offset );
	_tprintf( _T( "Program      : %s\r\n" ), m_szModuleName );
    _tprintf( _T( "Exception    : %08X (%s)\r\n"), pExceptionRecord->ExceptionCode, GetExceptionString( pExceptionRecord->ExceptionCode ) );
    _tprintf( _T( "Fault Address: %08X %02X:%08X\r\n"), pExceptionRecord->ExceptionAddress, section, offset );
    _tprintf( _T( "\r\n" ) );
	_tprintf( _T( "\r\n" ) );

	// TODO: System Information!
}

//
// Show the registers
//
void CExceptionReport::WriteRegistersInfo( PCONTEXT pContext )
{
	UNREFERENCED_PARAMETER(pContext);

#ifdef _M_IX86  // X86 Only!
	_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
    _tprintf( _T( "    x86 Registers\r\n" ) );
	_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
	_tprintf( _T( "\r\n" ) );

    _tprintf( _T("EAX=%08X  EBX=%08X  ECX=%08X  EDX=%08X\r\n"), pContext->Eax, pContext->Ebx, pContext->Ecx, pContext->Edx );
	_tprintf( _T("ESI=%08X  EDI=%08X  EBP=%08X\r\n"), pContext->Esi, pContext->Edi, pContext->Ebp );
    _tprintf( _T("DS =%04X      ES=%04X       FS=%04X       GS:%04X\r\n"), pContext->SegDs, pContext->SegEs, pContext->SegFs, pContext->SegGs );
    _tprintf( _T("CS:EIP=%04X:%08X\r\n"), pContext->SegCs, pContext->Eip );
    _tprintf( _T("SS:ESP=%04X:%08X\r\n"), pContext->SegSs, pContext->Esp );
    _tprintf( _T("Flags=%08X\r\n"), pContext->EFlags );
    _tprintf( _T("\r\n") );
    _tprintf( _T("\r\n") );
#endif
}

//
// Show the Memory
//
void CExceptionReport::WriteMemoryDump( PCONTEXT pContext )
{
	_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
    _tprintf( _T( "    Memory Dump\r\n" ) );
	_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
	_tprintf( _T( "\r\n" ) );

	_tprintf( _T( "Code: %d bytes starting at (EIP = %08lX)\r\n" ), 16, pContext->Rip );
	Dump( pContext->Rip, 16, FALSE );
	_tprintf( _T("\r\n") );

	_tprintf( _T( "Stack: %d bytes starting at (ESP = %08lX)\r\n" ), 1024, pContext->Rsp );
	Dump( pContext->Rsp, 1024, TRUE );
	_tprintf( _T("\r\n") );
    _tprintf( _T("\r\n") );
}

//
// Given an exception code, returns a pointer to a static string with a description of the exception                                         
//
#define EXCEPTION( x ) case EXCEPTION_##x: return _T(#x);

LPTSTR CExceptionReport::GetExceptionString( DWORD dwCode )
{
    switch ( dwCode ) {
        EXCEPTION( ACCESS_VIOLATION )
        EXCEPTION( DATATYPE_MISALIGNMENT )
        EXCEPTION( BREAKPOINT )
        EXCEPTION( SINGLE_STEP )
        EXCEPTION( ARRAY_BOUNDS_EXCEEDED )
        EXCEPTION( FLT_DENORMAL_OPERAND )
        EXCEPTION( FLT_DIVIDE_BY_ZERO )
        EXCEPTION( FLT_INEXACT_RESULT )
        EXCEPTION( FLT_INVALID_OPERATION )
        EXCEPTION( FLT_OVERFLOW )
        EXCEPTION( FLT_STACK_CHECK )
        EXCEPTION( FLT_UNDERFLOW )
        EXCEPTION( INT_DIVIDE_BY_ZERO )
        EXCEPTION( INT_OVERFLOW )
        EXCEPTION( PRIV_INSTRUCTION )
        EXCEPTION( IN_PAGE_ERROR )
        EXCEPTION( ILLEGAL_INSTRUCTION )
        EXCEPTION( NONCONTINUABLE_EXCEPTION )
        EXCEPTION( STACK_OVERFLOW )
        EXCEPTION( INVALID_DISPOSITION )
        EXCEPTION( GUARD_PAGE )
        EXCEPTION( INVALID_HANDLE )
    }

    // If not one of the "known" exceptions, try to get the string
    // from NTDLL.DLL's message table.

    static TCHAR szBuffer[512] = { 0 };

    FormatMessage( FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE,
                   GetModuleHandle( _T("NTDLL.DLL") ),
                   dwCode, 0, szBuffer, sizeof( szBuffer ), 0 );

    return szBuffer;
}

#undef EXCEPTION

//                                                                             
// Given a linear address, locates the module, section, and offset containing that address.                                                               
// Note: the szModule paramater buffer is an output buffer of length specified by the len parameter (in characters!)                                       
//                                                                             
BOOL CExceptionReport::GetLogicalAddress( PVOID addr, PTSTR szModule, DWORD len, DWORD& section, DWORD& offset )
{
    MEMORY_BASIC_INFORMATION mbi;

    if( !VirtualQuery( addr, &mbi, sizeof( mbi ) ) )
        return FALSE;

    PVOID hMod = mbi.AllocationBase;

    if( !GetModuleFileName( (HMODULE) hMod, szModule, len ) )
        return FALSE;

    // Point to the DOS header in memory
    PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER) hMod;

    // From the DOS header, find the NT (PE) header
    PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)(((DWORD64) hMod) + pDosHdr->e_lfanew);

    PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION( pNtHdr );

    DWORD64 rva = (DWORD64)addr - (DWORD64)hMod; // RVA is offset from module load address

    // Iterate through the section table, looking for the one that encompasses
    // the linear address.
    for( unsigned i = 0; i < pNtHdr->FileHeader.NumberOfSections; i++, pSection++ ) {
        DWORD sectionStart = pSection->VirtualAddress;
        DWORD sectionEnd = sectionStart + max(pSection->SizeOfRawData, pSection->Misc.VirtualSize);

        // Is the address in this section???
        if ( (rva >= sectionStart) && (rva <= sectionEnd) ) {
            // Yes, address is in the section.  Calculate section and offset,
            // and store in the "section" & "offset" params, which were
            // passed by reference.
            section = i+1;
            offset = (DWORD) (rva - sectionStart);
            return TRUE;
        }
    }

    return FALSE;   // Should never get here!
}

//
// Walks the stack, and writes the results to the report file 
//
void CExceptionReport::WriteStackDetails( PCONTEXT pContext, BOOL bWriteVariables )  // TRUE if local/params should be output
{
	_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
	_tprintf( _T( "    Call Stack (%s)\r\n" ), bWriteVariables ? "Detail" : "Short" );
	_tprintf( _T( "------------------------------------------------------------------------------\r\n" ) );
	_tprintf( _T( "\r\n" ) );
    _tprintf( _T("Address   Frame     Function                        SourceFile\r\n") );
    DWORD dwMachineType = 0;
    // Could use SymSetOptions here to add the SYMOPT_DEFERRED_LOADS flag
    STACKFRAME sf;

   memset( &sf, 0, sizeof(sf) );

#ifdef _M_IX86
    // Initialize the STACKFRAME structure for the first call.  This is only
    // necessary for Intel CPUs, and isn't mentioned in the documentation.
    sf.AddrPC.Offset       = pContext->Eip;
    sf.AddrPC.Mode         = AddrModeFlat;
    sf.AddrStack.Offset    = pContext->Esp;
    sf.AddrStack.Mode      = AddrModeFlat;
    sf.AddrFrame.Offset    = pContext->Ebp;
    sf.AddrFrame.Mode      = AddrModeFlat;

    dwMachineType = IMAGE_FILE_MACHINE_I386;
#endif
    while( 1 ) {
        // Get the next stack frame
        if( !StackWalk( dwMachineType, m_hProcess, GetCurrentThread(), &sf, pContext, 0, SymFunctionTableAccess, SymGetModuleBase, 0 ) )
            break;
        if( sf.AddrFrame.Offset == 0 )  // Basic sanity check to make sure
            break;                      // the frame is OK.  Bail if not.
        _tprintf( _T("%08X  %08X  "), sf.AddrPC.Offset, sf.AddrFrame.Offset );

        // Get the name of the function for this stack frame entry
        BYTE symbolBuffer[ sizeof(SYMBOL_INFO) + 1024 ];
        PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)symbolBuffer;
        pSymbol->SizeOfStruct = sizeof(symbolBuffer);
        pSymbol->MaxNameLen = 1024;
                        
        DWORD64 symDisplacement = 0;    // Displacement of the input address,
                                        // relative to the start of the symbol
        if( SymFromAddr( m_hProcess,sf.AddrPC.Offset,&symDisplacement,pSymbol ) ) {
            TCHAR szModule[MAX_PATH] = _T("");

			wsprintf( szModule, _T("%-s+%I64X"), pSymbol->Name, symDisplacement );
			_tprintf( _T( "%-30s" ), szModule );
        } else {
            // No symbol found.  Print out the logical address instead.
            TCHAR szModule[MAX_PATH] = _T("");
            DWORD section = 0, offset = 0;

            GetLogicalAddress( (PVOID) (DWORD64) sf.AddrPC.Offset, szModule, sizeof( szModule ), section, offset );
            _tprintf( _T("%04X:%08X                   %s"), section, offset, szModule );
        }

        // Get the source line for this stack frame entry
        IMAGEHLP_LINE lineInfo = { sizeof(IMAGEHLP_LINE) };
        DWORD dwLineDisplacement;
        if ( SymGetLineFromAddr( m_hProcess, sf.AddrPC.Offset, &dwLineDisplacement, &lineInfo ) ) {
            _tprintf(_T("  %s line %u"),lineInfo.FileName,lineInfo.LineNumber); 
			if( m_bHasSymbol == FALSE )
				m_bHasSymbol = TRUE;
        }
        _tprintf( _T("\r\n") );

        // Write out the variables, if desired
        if( bWriteVariables ) {
            // Use SymSetContext to get just the locals/params for this frame
            IMAGEHLP_STACK_FRAME imagehlpStackFrame;

            imagehlpStackFrame.InstructionOffset = sf.AddrPC.Offset;
            SymSetContext( m_hProcess, &imagehlpStackFrame, 0 );

            // Enumerate the locals/parameters
            SymEnumSymbols( m_hProcess, 0, 0, EnumerateSymbolsCallback, &sf );

            _tprintf( _T("\r\n") );
        }
    }
	_tprintf( _T( "\r\n" ) );
	_tprintf( _T( "\r\n" ) );
}

//
// The function invoked by SymEnumSymbols
//
BOOL CALLBACK CExceptionReport::EnumerateSymbolsCallback( PSYMBOL_INFO  pSymInfo, ULONG SymbolSize, PVOID UserContext )
{
	UNREFERENCED_PARAMETER(SymbolSize);
	
	char szBuffer[2048];

    __try {
        if( FormatSymbolValue( pSymInfo, (STACKFRAME*)UserContext, szBuffer, sizeof(szBuffer) ) )
            _tprintf( _T("\t%s\r\n"), szBuffer );
    }
    __except( 1 ) {
        _tprintf( _T("punting on symbol %s\r\n"), pSymInfo->Name );
    }
    return TRUE;
}

//
// Given a SYMBOL_INFO representing a particular variable, displays its contents.
// If it's a user defined type, display the members and their values.
//
BOOL CExceptionReport::FormatSymbolValue( PSYMBOL_INFO pSym, STACKFRAME * sf, char * pszBuffer, unsigned cbBuffer )
{
	UNREFERENCED_PARAMETER(cbBuffer);

    char * pszCurrBuffer = pszBuffer;

    // Indicate if the variable is a local or parameter
    if( pSym->Flags & IMAGEHLP_SYMBOL_INFO_PARAMETER )
        pszCurrBuffer += sprintf( pszCurrBuffer, "Parameter " );
    else if( pSym->Flags & IMAGEHLP_SYMBOL_INFO_LOCAL )
        pszCurrBuffer += sprintf( pszCurrBuffer, "Local " );

    // If it's a function, don't do anything.
    if ( pSym->Tag == 5 )   // SymTagFunction from CVCONST.H from the DIA SDK
        return FALSE;

    // Emit the variable name
    pszCurrBuffer += sprintf( pszCurrBuffer, "\'%s\'", pSym->Name );

    DWORD_PTR pVariable = 0;    // Will point to the variable's data in memory

    if( pSym->Flags & IMAGEHLP_SYMBOL_INFO_REGRELATIVE ) {
        // if ( pSym->Register == 8 )   // EBP is the value 8 (in DBGHELP 5.1)
        {                               //  This may change!!!
            pVariable = sf->AddrFrame.Offset;
            pVariable += (DWORD_PTR)pSym->Address;
        }
        // else
        //  return FALSE;
    }
    else if ( pSym->Flags & IMAGEHLP_SYMBOL_INFO_REGISTER ) {
        return FALSE;   // Don't try to report register variable
    } else {
        pVariable = (DWORD_PTR)pSym->Address;   // It must be a global variable
    }

    // Determine if the variable is a user defined type (UDT).  IF so, bHandled
    // will return TRUE.
    BOOL bHandled;
    pszCurrBuffer = DumpTypeIndex(pszCurrBuffer,pSym->ModBase, pSym->TypeIndex, 0, pVariable, bHandled );
    if( !bHandled ) {
        // The symbol wasn't a UDT, so do basic, stupid formatting of the
        // variable.  Based on the size, we're assuming it's a char, WORD, or
        // DWORD.
        BasicType basicType = GetBasicType( pSym->TypeIndex, pSym->ModBase );
        pszCurrBuffer = FormatOutputValue(pszCurrBuffer, basicType, pSym->Size, (PVOID) pVariable ); 
    }
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// If it's a user defined type (UDT), recurse through its members until we're
// at fundamental types.  When he hit fundamental types, return
// bHandled = FALSE, so that FormatSymbolValue() will format them.
//////////////////////////////////////////////////////////////////////////////
char *CExceptionReport::DumpTypeIndex( char *pszCurrBuffer, DWORD64 modBase, DWORD dwTypeIndex, unsigned nestingLevel, DWORD_PTR offset, BOOL & bHandled )
{
    bHandled = FALSE;

    // Get the name of the symbol.  This will either be a Type name (if a UDT), or the structure member name.
    WCHAR * pwszTypeName;
    if( SymGetTypeInfo( m_hProcess, modBase, dwTypeIndex, TI_GET_SYMNAME, &pwszTypeName ) ) {
        pszCurrBuffer += sprintf( pszCurrBuffer, " %ls", pwszTypeName );
        LocalFree( pwszTypeName );
    }

    // Determine how many children this type has.
    DWORD dwChildrenCount = 0;
    SymGetTypeInfo( m_hProcess, modBase, dwTypeIndex, TI_GET_CHILDRENCOUNT, &dwChildrenCount );

    if( !dwChildrenCount )     // If no children, we're done
        return pszCurrBuffer;

    // Prepare to get an array of "TypeIds", representing each of the children.
    // SymGetTypeInfo(TI_FINDCHILDREN) expects more memory than just a
    // TI_FINDCHILDREN_PARAMS struct has.  Use derivation to accomplish this.
    struct FINDCHILDREN : TI_FINDCHILDREN_PARAMS {
        ULONG   MoreChildIds[1024];
        FINDCHILDREN(){Count = sizeof(MoreChildIds) / sizeof(MoreChildIds[0]);}
    } children;

    children.Count = dwChildrenCount;
    children.Start= 0;
    // Get the array of TypeIds, one for each child type
    if( !SymGetTypeInfo( m_hProcess, modBase, dwTypeIndex, TI_FINDCHILDREN, &children ) ) {
        return pszCurrBuffer;
    }

    // Append a line feed
    pszCurrBuffer += sprintf( pszCurrBuffer, "\r\n" );

    // Iterate through each of the children
    for( unsigned i = 0; i < dwChildrenCount; i++ ) {
        // Add appropriate indentation level (since this routine is recursive)
        for( unsigned j = 0; j <= nestingLevel+1; j++ )
            pszCurrBuffer += sprintf( pszCurrBuffer, "\t" );

        // Recurse for each of the child types
        BOOL bHandled2;
        pszCurrBuffer = DumpTypeIndex( pszCurrBuffer, modBase, children.ChildId[i], nestingLevel+1, offset, bHandled2 );

        // If the child wasn't a UDT, format it appropriately
        if( !bHandled2 ) {
            // Get the offset of the child member, relative to its parent
            DWORD dwMemberOffset;
            SymGetTypeInfo( m_hProcess, modBase, children.ChildId[i], TI_GET_OFFSET, &dwMemberOffset );

            // Get the real "TypeId" of the child.  We need this for the SymGetTypeInfo( TI_GET_TYPEID ) call below.
            DWORD typeId;
            SymGetTypeInfo( m_hProcess, modBase, children.ChildId[i], TI_GET_TYPEID, &typeId );

            // Get the size of the child member
            ULONG64 length;
            SymGetTypeInfo(m_hProcess, modBase, typeId, TI_GET_LENGTH,&length);

            // Calculate the address of the member
            DWORD_PTR dwFinalOffset = offset + dwMemberOffset;

            BasicType basicType = GetBasicType(children.ChildId[i], modBase );

            pszCurrBuffer = FormatOutputValue( pszCurrBuffer, basicType, length, (PVOID)dwFinalOffset ); 

            pszCurrBuffer += sprintf( pszCurrBuffer, "\r\n" );
        }
    }

    bHandled = TRUE;
    return pszCurrBuffer;
}

char *CExceptionReport::FormatOutputValue( char *pszCurrBuffer, BasicType basicType, DWORD64 length, PVOID pAddress )
{
    // Format appropriately (assuming it's a 1, 2, or 4 bytes (!!!)
    if( length == 1 )
        pszCurrBuffer += sprintf( pszCurrBuffer, " = %X", *(PBYTE)pAddress );
    else if ( length == 2 )
        pszCurrBuffer += sprintf( pszCurrBuffer, " = %X", *(PWORD)pAddress );
    else if ( length == 4 ) {
        if ( basicType == btFloat ) {
            pszCurrBuffer += sprintf(pszCurrBuffer," = %f", *(PFLOAT)pAddress);
        } else if ( basicType == btChar ) {
            if( !IsBadStringPtr( *(LPCWSTR*)pAddress, 32) ) {
                pszCurrBuffer += sprintf( pszCurrBuffer, " = \"%.31s\"", *(PDWORD)pAddress );
			} else
                pszCurrBuffer += sprintf( pszCurrBuffer, " = %X", *(PDWORD)pAddress );
        } else
            pszCurrBuffer += sprintf(pszCurrBuffer," = %X", *(PDWORD)pAddress);
    } else if ( length == 8 ) {
        if( basicType == btFloat ) {
            pszCurrBuffer += sprintf( pszCurrBuffer, " = %lf", *(double *)pAddress );
        } else
            pszCurrBuffer += sprintf( pszCurrBuffer, " = %I64X", *(DWORD64*)pAddress );
    }
    return pszCurrBuffer;
}

BasicType CExceptionReport::GetBasicType( DWORD typeIndex, DWORD64 modBase )
{
    BasicType basicType;

    if( SymGetTypeInfo( m_hProcess, modBase, typeIndex, TI_GET_BASETYPE, &basicType ) ) {
        return basicType;
    }
    // Get the real "TypeId" of the child.  We need this for the
    // SymGetTypeInfo( TI_GET_TYPEID ) call below.
    DWORD typeId;
    if (SymGetTypeInfo(m_hProcess,modBase, typeIndex, TI_GET_TYPEID, &typeId)) {
        if ( SymGetTypeInfo( m_hProcess, modBase, typeId, TI_GET_BASETYPE, &basicType ) ) {
            return basicType;
        }
    }
    return btNoType;
}

#define BYTES_PER_LINE		16

//
// Memory Dump function
//
void CExceptionReport::Dump( DWORD64 dw64Offset, DWORD dwSize, BOOL bAlign )
{
	DWORD dwLoc, dwILoc, dwX;
	LPBYTE pOut = (LPBYTE) dw64Offset;
	
	if( bAlign == TRUE )
		pOut = (LPBYTE) ((dw64Offset >> 4) << 4);
	for( dwLoc = 0; dwLoc < dwSize; dwLoc += 16, pOut += BYTES_PER_LINE ) {
		LPBYTE pLine = pOut;

		_tprintf( _T( "%08lX: " ), (DWORD64) pOut );
		for( dwX = 0, dwILoc = dwLoc; dwX < BYTES_PER_LINE; dwX++ ) {
			if( dwX == (BYTES_PER_LINE / 2) )
				_tprintf( _T( " " ) );
			if( dwILoc++ > dwSize ) {
				_tprintf( _T( "?? " ) );
			} else {
				_tprintf( _T( "%02X " ), *(pLine++) );
			}
		}
		pLine = pOut;
		_tprintf( L" " );
		for( dwX = 0, dwILoc = dwLoc; dwX < BYTES_PER_LINE; dwX++ ) {
			if( dwILoc++ > dwSize ) {
				_tprintf( _T( " " ) );
			} else {
				_tprintf( _T( "%c" ), isprint( *pLine ) ? *pLine : '.');
				pLine++;
			}
		}
		_tprintf( L"\r\n" );
	}
}

//
// Helper function that writes to the report file, and allows the user to use printf style formating                                                     
//
int __cdecl CExceptionReport::_tprintf( const TCHAR * format, ... )
{
    TCHAR szBuff[1024];
    int retValue;
    DWORD cbWritten;
    va_list argptr;

    va_start( argptr, format );
    retValue = vswprintf( szBuff, format, argptr );
    va_end( argptr );
    WriteFile( m_hReportFile, szBuff, retValue * sizeof( TCHAR ), &cbWritten, 0 );
    return retValue;
}
