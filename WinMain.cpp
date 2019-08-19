#include <windows.h>
#include <commctrl.h>
#include <time.h>
#include "resource.h"
#include <atlstr.h>

//void test();
//
//#pragma comment(lib,"MyHook.lib")

//---------------��ȡ��Ļ��Ϣ------------------------------------------------
const int nScreenCX = ::GetSystemMetrics(SM_CXSCREEN);             //  ��Ļ��С
const int nScreenCY = ::GetSystemMetrics(SM_CYSCREEN);

const int nDesktopIconCX = ::GetSystemMetrics(SM_CXICONSPACING);   //  ͼ���С
const int nDesktopIconCY = ::GetSystemMetrics(SM_CYICONSPACING);
	
const int low = nScreenCX/nDesktopIconCX;                        //  ���и���
const int column = nScreenCY/nDesktopIconCY;

int lowStep = (nScreenCX-low*nDesktopIconCX)/low;             //  ���м��
int columnStep = (nScreenCY-column*nDesktopIconCY)/column;
//---------------------------------------------------------------


//------------------------��ȡ��Ļͼ��-----------------------------------------------------
HWND  hwndParent = ::FindWindow("Progman", "Program Manager" );    //  ���Ҵ���
HWND  hwndSHELLDLL_DefView = ::FindWindowEx( hwndParent, NULL, "SHELLDLL_DefView", NULL ); //  �����Ӵ���
HWND  hwndSysListView32 = ::FindWindowEx( hwndSHELLDLL_DefView, NULL, "SysListView32", "FolderView" );  
int Nm = ListView_GetItemCount( hwndSysListView32 );            //��ȡ����������ĸ���
//-----------------------------------------------------------------------------




HINSTANCE hIns;
LRESULT CALLBACK MyProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);


HWND CreateGameWindow()   //  ��������
{
	WNDCLASSEX wndclass;
	wndclass.cbClsExtra = 0;//���ӵ���Ϣ ����Ҫ�����ڴ���
	wndclass.cbSize = sizeof(wndclass);
	wndclass.cbWndExtra = 0;//���е����� ����Ҫ��
	wndclass.hbrBackground = (HBRUSH)COLOR_WINDOW; //����ɫ
	wndclass.hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW));  //���
	wndclass.hIcon = ::LoadIcon(hIns,MAKEINTRESOURCE(IDI_ICON1));   //ͼ��
	wndclass.hIconSm = NULL; //����   ͼ��
	wndclass.hInstance = hIns;// Ӧ�ó���ʵ�����
	wndclass.lpfnWndProc = MyProc;
	wndclass.lpszClassName = "classname";//�����������
	wndclass.lpszMenuName = NULL;   //�˵�
	wndclass.style = CS_HREDRAW|CS_VREDRAW; //ˢ��

	RegisterClassEx(&wndclass);//ע�ᴰ��
	//��������   �������������⣬��ʽ�����Ͻ�����(X,Y)�����ڵĳ��Ϳ����״��ڣ��˵���Ӧ�ó���ʵ����������ĵ�
	HWND hWnd = CreateWindow("classname","�ҵĴ���",WS_OVERLAPPEDWINDOW,
		100,0,850+16,700+38,NULL,NULL,hIns,NULL);
	if (hWnd == NULL)     //    �жϴ����Ƿ񴴽��ɹ�
	{
		MessageBox(NULL,"���ڴ���ʧ�ܣ�","ERROR",MB_OK);
		return 0;
	}

	return hWnd;
}






//----------------------------��------------------------------------------------

struct Bean
{
	int nDesktopIconID;
	int x;
	int y;
};

struct Snake
{
	int nDesktopIconID;  //  ����ͼ���ID
	int x;
	int y;
	Snake* pNext;
	Snake* pPre;
};
Snake* g_pSnakeHead = NULL;
Snake* g_pSnakeEnd = NULL;

Bean g_pBean;
int FX = VK_RIGHT;

void CreateBean()
{
	if(g_pSnakeEnd != NULL)
	{
		g_pBean.nDesktopIconID = g_pSnakeEnd->nDesktopIconID+1;   //  ������ͼ���װ�� ����
		while(1)   
		{
			g_pBean.x = rand()%low * nDesktopIconCX;
			g_pBean.y = rand()%column * nDesktopIconCY;	
			//----  �ж��Ƿ����������
			Snake* temp = g_pSnakeHead;
			int nPosx = g_pBean.x+nDesktopIconCX/2;
			int nPosy = g_pBean.y+nDesktopIconCY/2;
			while(temp)
			{
				if(nPosx >= temp->x && nPosx <= temp->x+nDesktopIconCX 
					&& nPosy >= temp->y && nPosy <= temp->y+nDesktopIconCY )
				{
					break;
				}
				temp = temp->pNext;
			}
			//----  ����ɹ�
			if(temp == NULL)
				return;
		}
	}
}

void ShowBean()
{
	//  ��hwndSysListView32��������ͼ���λ����Ϣ              g_pBean.nDesktopIconID���õ�ͼ��ID      MAKELPARAM(g_pBean.x,g_pBean.y)���õ�λ��
	::SendMessage( hwndSysListView32,   LVM_SETITEMPOSITION, g_pBean.nDesktopIconID,  MAKELPARAM(g_pBean.x,g_pBean.y));
	//  ����ͼ��
	ListView_RedrawItems(hwndSysListView32, 0, ListView_GetItemCount(hwndSysListView32) - 1);
	//  ���´���
	::UpdateWindow(hwndSysListView32); 
}

void HideDesktopIcon()
{
	// ��������ͼ��
	for(int i=3;i<Nm;i++)
		::SendMessage( hwndSysListView32,   LVM_SETITEMPOSITION, i,   MAKELPARAM(-nDesktopIconCX,-nDesktopIconCY));

	ListView_RedrawItems(hwndSysListView32, 0, ListView_GetItemCount(hwndSysListView32) - 1);
	::UpdateWindow(hwndSysListView32); 
}


void CreateSnake()  //  ������
{
	for(int i=0;i<3;i++)
	{
		Snake* temp = new Snake;
		temp->pNext = NULL;
		temp->pPre = NULL;
		temp->nDesktopIconID = i;    //  װ��ͼ��
		temp->x = i*nDesktopIconCX;
		temp->y = 0;
		if(!g_pSnakeHead)
		{
			g_pSnakeHead = temp;
			g_pSnakeEnd = temp;
		}
		else
		{
			g_pSnakeEnd->pNext = temp;
			temp->pPre = g_pSnakeEnd;
			g_pSnakeEnd = temp;
		}
	}
}
void ShakeShow()  //  ��ʾ��
{
	Snake* temp = g_pSnakeHead;
	while(temp)
	{
		::SendMessage( hwndSysListView32,   LVM_SETITEMPOSITION, temp->nDesktopIconID,   MAKELPARAM(temp->x,temp->y));
		temp = temp->pNext;
	}
	ListView_RedrawItems(hwndSysListView32, 0, ListView_GetItemCount(hwndSysListView32) - 1);
	::UpdateWindow(hwndSysListView32); 
}
void SnakeMove()
{
	//  ���нڵ�Ǩ��
	Snake* temp = g_pSnakeHead;
	int k=0;
	while(temp->pNext)
	{
		temp->x = temp->pNext->x;
		temp->y = temp->pNext->y;
		temp = temp->pNext;
	}
	//  �޸����һ���ڵ�
	switch(FX)
	{
	case VK_LEFT:
		if(temp->x <= 0)
			temp->x = nScreenCX-nDesktopIconCX;
		else
			temp->x -= nDesktopIconCX;
		break;
	case VK_RIGHT:
		if(temp->x >= nScreenCX-nDesktopIconCX)
			temp->x = 0;
		else
			temp->x += nDesktopIconCX;
		break;
	case VK_UP:
		if(temp->y <= 0)
			temp->y = nScreenCY-nDesktopIconCY;
		else
			temp->y -= nDesktopIconCY;
		break;
	case VK_DOWN:
		if(temp->y >= nScreenCY-nDesktopIconCY)
			temp->y = 0;
		else
			temp->y += nDesktopIconCY;
		break;
	}
}


bool IsEatBean()
{
	int nPosx = g_pSnakeEnd->x+nDesktopIconCX/2;
	int nPosy = g_pSnakeEnd->y+nDesktopIconCY/2;

	if(nPosx >= g_pBean.x && nPosx <= g_pBean.x+nDesktopIconCX 
		&& nPosy >= g_pBean.y && nPosy <= g_pBean.y+nDesktopIconCY )
	{
		return true;
	}

	return false;
}

void GrowUp()
{
	Snake* temp = new Snake;
	temp->nDesktopIconID = g_pBean.nDesktopIconID;
	temp->x = g_pBean.x;
	temp->y = g_pBean.y;
	temp->pNext = NULL;
	temp->pPre = NULL;

	g_pSnakeEnd->pNext = temp;
	temp->pPre = g_pSnakeEnd;
	g_pSnakeEnd = temp;
}
bool GameOver()
{
	if(g_pSnakeEnd->nDesktopIconID == Nm-1)
		return true;
	return false;
}
//----------------------------------------------------------------------------



//-------------------------------��ȡ����ͼ���λ��---------------------------------------
Snake* m_pRevertHead = 0;
Snake* m_pRevertEnd = 0;
void InitIconPos()
{
	//--------------��ȡ����ͼ��λ��װ��������-------------------------------------------------------------------
	DWORD dwProcessId;
	//ͨ�����洰�ھ����ȡ�˴������ڽ��̵�PID,��ʵ����explorer���� 
	GetWindowThreadProcessId(hwndSysListView32, &dwProcessId);
	//��ָ��PID����,ȡ�ý��̾�� 
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	//��ָ��������������һ��POINI�ṹ��С�Ŀռ�.
	LPVOID lpvPt = VirtualAllocEx(hProcess,NULL,sizeof(POINT),MEM_COMMIT,PAGE_READWRITE);
		
	POINT pt;

	for(int i=0;i<Nm;i++)
	{
		Snake* temp = new Snake;
		temp->nDesktopIconID = i;
		temp->pNext = 0;
		temp->pPre = 0;
		
		//��ȡ��һ��ͼ�������,����lpvPt
		ListView_GetItemPosition(hwndSysListView32,i,lpvPt);
		//lpvPt���Ǳ����������,����ʹ��,���Ծ�Ҫ����ReadProcessMemory��ָ�����̸������� 
		ReadProcessMemory(hProcess,lpvPt,&pt,sizeof(POINT),NULL); 

	   temp->x = pt.x;
	   temp->y = pt.y;

	  //---------------------------------------------------------------------------------

		if(!m_pRevertHead)
		{
			m_pRevertHead =temp;
			m_pRevertEnd = temp;
		}
		else
		{
			m_pRevertEnd->pNext = temp;
			temp->pPre = m_pRevertEnd;
			m_pRevertEnd = temp;
		}
	}
	// �ͷ�����Ŀռ�
	VirtualFreeEx(hProcess, lpvPt, 0, MEM_RELEASE);
	//�رվ��
	CloseHandle(hProcess);
}
void RevertIconPos()  //  ��ͼ��Żص�ԭ����λ��
{
	Snake* temp = m_pRevertHead;
	while(temp)
	{
		::SendMessage( hwndSysListView32,LVM_SETITEMPOSITION,temp->nDesktopIconID,MAKELPARAM(temp->x,temp->y));
		temp = temp->pNext;
	}
	ListView_RedrawItems(hwndSysListView32, 0, ListView_GetItemCount(hwndSysListView32) - 1);
	::UpdateWindow(hwndSysListView32); 
}
//-------------------------------��ȡ����ͼ���λ��---------------------------------------






//----------------------��Ϸ��ʼ---------------------------------------------
int g_nRandomDesktopIconTime = 0;
int g_nGameStartTime = 3;

void RandomDesktopIcon()
{
	for(int i=0;i<Nm;i++)
	{
		int x = rand()%low;
		int y = rand()%column;

		::SendMessage( hwndSysListView32,   LVM_SETITEMPOSITION, i,   MAKELPARAM(x*nDesktopIconCX,y*nDesktopIconCY));
	}
	ListView_RedrawItems(hwndSysListView32, 0, ListView_GetItemCount(hwndSysListView32) - 1);
	::UpdateWindow(hwndSysListView32); 
}

int g_nTimeArr3[3][5] = {
	{1,0,1,0,1},
	{1,0,1,0,1},
	{1,1,1,1,1}
};
int g_nTimeArr2[3][5] = {
	{1,0,1,1,1},
	{1,0,1,0,1},
	{1,1,1,0,1}
};
int g_nTimeArr1[3][5] = {
	{0,1,0,0,1},
	{1,1,1,1,1},
	{0,0,0,0,1}
};
int g_nTimeArr0[3][5] = {
	{1,1,1,1,1},
	{1,0,0,0,1},
	{1,1,1,1,1}
};
int g_nWin[12][4] = {
	{1,1,0,0},
	{0,0,1,1},
	{1,1,0,0},
	{0,0,1,1},
	{1,1,0,0},
	{0,0,0,0},
	{1,1,1,1},
	{0,0,0,0},
	{1,1,1,1},
	{0,1,0,0},
	{0,0,1,0},
	{1,1,1,1},
};

void GameStartTime(int (*g_nTimeArr)[5])
{
	int k=0;
	for(int i=0;i<3;i++)
	{
		for(int j=0;j<5;j++)
		{
			if(g_nTimeArr[i][j] == 1)
			{
				int x = (low-3)/2*nDesktopIconCX + i*nDesktopIconCX;
				int y = (column-5)/2*nDesktopIconCY + j*nDesktopIconCY;
				::SendMessage( hwndSysListView32,LVM_SETITEMPOSITION,k,MAKELPARAM(x,y));
				k++;
			}
		}
	}

	for(;k<Nm;k++)
		::SendMessage( hwndSysListView32,LVM_SETITEMPOSITION,k,MAKELPARAM(-nDesktopIconCX,-nDesktopIconCY));
	
	ListView_RedrawItems(hwndSysListView32, 0, ListView_GetItemCount(hwndSysListView32) - 1);
	::UpdateWindow(hwndSysListView32); 

	g_nGameStartTime--;
}

void Win()
{
	int k=0;
	for(int i=0;i<12;i++)
	{
		for(int j=0;j<4;j++)
		{
			if(g_nWin[i][j] == 1)
			{
				int x = (low-12)/2*nDesktopIconCX + i*nDesktopIconCX;
				int y = (column-4)/2*nDesktopIconCY + j*nDesktopIconCY;
				::SendMessage( hwndSysListView32,LVM_SETITEMPOSITION,k,MAKELPARAM(x,y));
				k++;
			}
		}
	}

	for(int i=0;k<Nm;k++,i++)
	{
		::SendMessage( hwndSysListView32,LVM_SETITEMPOSITION,k,MAKELPARAM(i*nDesktopIconCX,0));
	}
	
	ListView_RedrawItems(hwndSysListView32, 0, ListView_GetItemCount(hwndSysListView32) - 1);
	::UpdateWindow(hwndSysListView32); 
}

//----------------------��Ϸ��ʼ---------------------------------------------




int CALLBACK WinMain(HINSTANCE hInstance,HINSTANCE hPreInstance,LPSTR pCmdLine,int nCmdShow)
{
	InitIconPos();   //  ��ȡ����ͼ���λ��

	//---------------------------------------------------------------------------------
	/*ȡ���Զ�����ͼ��*/
	DWORD dwStyle = (DWORD)::GetWindowLong(hwndSysListView32, GWL_STYLE);
	bool bflag1 = false;
	if(dwStyle & LVS_AUTOARRANGE)
	{
		bflag1 = true;
		::SetWindowLong(hwndSysListView32,GWL_STYLE,dwStyle & ~LVS_AUTOARRANGE);
	}
	/*ȡ��ͼ�����������*/
	bool bflag2 = false;
	DWORD dwExStyle = (DWORD)ListView_GetExtendedListViewStyle(hwndSysListView32, GWL_EXSTYLE);	
	if(dwExStyle & LVS_EX_SNAPTOGRID)
	{
		bflag2 = true;
		ListView_SetExtendedListViewStyle(hwndSysListView32,dwExStyle & ~LVS_EX_SNAPTOGRID);
	}
	//---------------------------------------------------------------------------------

	hIns = hInstance;
	srand((unsigned int)time(NULL));
	HWND hWnd = ::CreateGameWindow();

	int inda = rand()%5;

	::SetTimer(hWnd,1,50,NULL);    //  ������Ϸ

	MSG message;
	while(GetMessage(&message,NULL,0,0))   //  ���ϵ�����Ϣ������ȡ����Ϣ
	{
		TranslateMessage(&message);     //������Ϣ
		DispatchMessage(&message);		// ������Ϣ
	}
	//  ��ԭͼ��
	RevertIconPos();

	//-----------------------------------------------------------------------
	/*�����Զ�����ͼ��*/
	if(bflag1 == true)
	{
		dwStyle = (DWORD)::GetWindowLong(hwndSysListView32, GWL_STYLE);
		if(!(dwStyle & LVS_AUTOARRANGE))
			::SetWindowLong(hwndSysListView32,GWL_STYLE,dwStyle | LVS_AUTOARRANGE);
	}
	/*ȡ��ͼ�����������*/
	if(bflag2 == true)
	{
		dwExStyle = (DWORD)ListView_GetExtendedListViewStyle(hwndSysListView32, GWL_EXSTYLE);	
		if(!(dwExStyle & LVS_EX_SNAPTOGRID))
		{
			ListView_SetExtendedListViewStyle(hwndSysListView32,dwExStyle | LVS_EX_SNAPTOGRID);
		}
	}
	//-----------------------------------------------------------------------

	return 0;
}




LRESULT CALLBACK MyProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)       //�жϷ��͹�������ʲô��Ϣ
	{
	case WM_TIMER:
		if(wParam == 1)  //  ��Ϸһ�����ͼ��
		{	
			::SetTimer(hwnd,4,1,0);       //  ������״̬
			g_nRandomDesktopIconTime++;
			RandomDesktopIcon();
			if(g_nRandomDesktopIconTime == 100)   //  ֹͣͼ�����
			{
				GameStartTime(g_nTimeArr3);
				::KillTimer(hwnd,1);
				::SetTimer(hwnd,2,1500,NULL);  
			}
		}
		else if(wParam == 2)   //  ��Ϸ��������ʱ
		{
			switch(g_nGameStartTime)
			{
			case 0:
				{
					GameStartTime(g_nTimeArr0);
					::KillTimer(hwnd,2);
					::Sleep(1500);
					//------------//  ��ʼ��Ϸ--------------------
					CreateSnake();       //  ������
					HideDesktopIcon();   //  ����ͼ��
					CreateBean();        //  ������
					ShakeShow();
					ShowBean();
					::SetTimer(hwnd,3,200,NULL);    //  �������ƶ�
				}
				break;
			case 1:
				GameStartTime(g_nTimeArr1);
				break;
			case 2:
				GameStartTime(g_nTimeArr2);
				break;
			}
		}
		else if(wParam == 3)   //  ��Ϸ������ʼ��Ϸ
		{
			SnakeMove();
			if(IsEatBean() == true)
			{
				GrowUp();
				CreateBean();
				ShowBean();
			}
			ShakeShow();

			if(GameOver() == true)  //  ��Ϸ����
			{ 
				Win();
				::KillTimer(hwnd,3);
				::PostMessage(hwnd,WM_CLOSE,0,0);
				Sleep(2000);
			}
		}
		else if(wParam == 4)   //  ��ȡ����״̬
		{
			if(::GetAsyncKeyState(VK_LEFT))
				FX = VK_LEFT;
			if(::GetAsyncKeyState(VK_RIGHT))
				FX = VK_RIGHT;
			if(::GetAsyncKeyState(VK_UP))
				FX = VK_UP;
			if(::GetAsyncKeyState(VK_DOWN))
				FX = VK_DOWN;
			if(::GetAsyncKeyState(VK_ESCAPE))
				::PostMessage(hwnd,WM_CLOSE,0,0);
		}
		break;
	//case WM_KEYDOWN:
	//	if(wParam == VK_ESCAPE)
	//		::PostMessage(hwnd,WM_CLOSE,0,0);
	//	else
	//		FX = wParam;
	//	break;
	case WM_DESTROY:  
		PostQuitMessage(0);   //������˳�����Ϣ   ���˳�
		break;
	}
	return DefWindowProc(hwnd,uMsg,wParam,lParam);  //  ִ����Ϣ��Ĭ�ϴ���
}
