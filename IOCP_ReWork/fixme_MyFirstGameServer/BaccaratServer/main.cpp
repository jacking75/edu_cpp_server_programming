//#include "stdafx.h"

//#include <windows.h>
//#include <process.h>

#include "Server.h"


LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
HINSTANCE	g_hInst;
HWND		g_hWnd;
CServer		g_Server;


int APIENTRY WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
	char		CTitle[] = "Baccarat Server";
	char		cClass[] = "Server Class";

	g_hInst	=	hInstance;
	MSG			Message;
	WNDCLASS	wc;

	wc.style			= CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc		= ( WNDPROC )WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon( NULL, IDI_APPLICATION );
	wc.hCursor			= LoadCursor( NULL, IDI_APPLICATION );
	wc.hbrBackground	= ( HBRUSH )BLACK_BRUSH;
	wc.lpszClassName	= cClass;
	wc.lpszMenuName		= NULL;

	RegisterClass( &wc );

	g_hWnd = CreateWindow( cClass, CTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
							CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL );
	ShowWindow( g_hWnd, nCmdShow );

	g_Server.Initialize();
	
	while( GetMessage( &Message, 0, 0, 0 ) )
	{
		TranslateMessage( &Message );
		DispatchMessage( &Message );
	}

	return Message.wParam;
}


LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_CREATE:
		{
		}
		break;
	case WM_DESTROY:
		{
			g_Server.FinalCleanup();
			PostQuitMessage( 0 );
		}
		break;
	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
	}
	return 0;
}


