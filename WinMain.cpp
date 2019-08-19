#include <windows.h>
#include <commctrl.h>
#include <time.h>
#include "resource.h"
#include <atlstr.h>

//void test();
//
//#pragma comment(lib,"MyHook.lib")

//---------------获取屏幕信息------------------------------------------------
const int nScreenCX = ::GetSystemMetrics(SM_CXSCREEN);             //  屏幕大小
const int nScreenCY = ::GetSystemMetrics(SM_CYSCREEN);

const int nDesktopIconCX = ::GetSystemMetrics(SM_CXICONSPACING);   //  图标大小
const int nDesktopIconCY = ::GetSystemMetrics(SM_CYICONSPACING);
	
const int low = nScreenCX/nDesktopIconCX;                        //  行列个数
const int column = nScreenCY/nDesktopIconCY;

int lowStep = (nScreenCX-low*nDesktopIconCX)/low;             //  行列间距
int columnStep = (nScreenCY-column*nDesktopIconCY)/column;
//---------------------------------------------------------------


//------------------------获取屏幕图标-----------------------------------------------------
HWND  hwndParent = ::FindWindow("Progman", "Program Manager" );    //  查找窗口
HWND  hwndSHELLDLL_DefView = ::FindWindowEx( hwndParent, NULL, "SHELLDLL_DefView", NULL ); //  查找子窗口
HWND  hwndSysListView32 = ::FindWindowEx( hwndSHELLDLL_DefView, NULL, "SysListView32", "FolderView" );  
int Nm = ListView_GetItemCount( hwndSysListView32 );            //获取桌面网格项的个数
//-----------------------------------------------------------------------------




HINSTANCE hIns;
LRESULT CALLBACK MyProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);


HWND CreateGameWindow()   //  创建窗口
{
	WNDCLASSEX wndclass;
	wndclass.cbClsExtra = 0;//附加的信息 不需要开辟内存了
	wndclass.cbSize = sizeof(wndclass);
	wndclass.cbWndExtra = 0;//特有的数据 不需要了
	wndclass.hbrBackground = (HBRUSH)COLOR_WINDOW; //背景色
	wndclass.hCursor = ::LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW));  //光标
	wndclass.hIcon = ::LoadIcon(hIns,MAKEINTRESOURCE(IDI_ICON1));   //图标
	wndclass.hIconSm = NULL; //左上   图标
	wndclass.hInstance = hIns;// 应用程序实例句柄
	wndclass.lpfnWndProc = MyProc;
	wndclass.lpszClassName = "classname";//窗口类的名字
	wndclass.lpszMenuName = NULL;   //菜单
	wndclass.style = CS_HREDRAW|CS_VREDRAW; //刷新

	RegisterClassEx(&wndclass);//注册窗口
	//创建窗口   窗口类名，标题，样式，左上角坐标(X,Y)，窗口的长和宽，父亲窗口，菜单，应用程序实例句柄，多文档
	HWND hWnd = CreateWindow("classname","我的窗口",WS_OVERLAPPEDWINDOW,
		100,0,850+16,700+38,NULL,NULL,hIns,NULL);
	if (hWnd == NULL)     //    判断窗口是否创建成功
	{
		MessageBox(NULL,"窗口创建失败！","ERROR",MB_OK);
		return 0;
	}

	return hWnd;
}






//----------------------------蛇------------------------------------------------

struct Bean
{
	int nDesktopIconID;
	int x;
	int y;
};

struct Snake
{
	int nDesktopIconID;  //  桌面图标的ID
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
		g_pBean.nDesktopIconID = g_pSnakeEnd->nDesktopIconID+1;   //  把桌面图标的装到 豆里
		while(1)   
		{
			g_pBean.x = rand()%low * nDesktopIconCX;
			g_pBean.y = rand()%column * nDesktopIconCY;	
			//----  判断是否随机到蛇身
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
			//----  随机成功
			if(temp == NULL)
				return;
		}
	}
}

void ShowBean()
{
	//  给hwndSysListView32发送重置图标的位置消息              g_pBean.nDesktopIconID重置的图标ID      MAKELPARAM(g_pBean.x,g_pBean.y)重置的位置
	::SendMessage( hwndSysListView32,   LVM_SETITEMPOSITION, g_pBean.nDesktopIconID,  MAKELPARAM(g_pBean.x,g_pBean.y));
	//  绘制图标
	ListView_RedrawItems(hwndSysListView32, 0, ListView_GetItemCount(hwndSysListView32) - 1);
	//  更新窗口
	::UpdateWindow(hwndSysListView32); 
}

void HideDesktopIcon()
{
	// 隐藏其他图标
	for(int i=3;i<Nm;i++)
		::SendMessage( hwndSysListView32,   LVM_SETITEMPOSITION, i,   MAKELPARAM(-nDesktopIconCX,-nDesktopIconCY));

	ListView_RedrawItems(hwndSysListView32, 0, ListView_GetItemCount(hwndSysListView32) - 1);
	::UpdateWindow(hwndSysListView32); 
}


void CreateSnake()  //  创建蛇
{
	for(int i=0;i<3;i++)
	{
		Snake* temp = new Snake;
		temp->pNext = NULL;
		temp->pPre = NULL;
		temp->nDesktopIconID = i;    //  装入图标
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
void ShakeShow()  //  显示蛇
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
	//  所有节点迁移
	Snake* temp = g_pSnakeHead;
	int k=0;
	while(temp->pNext)
	{
		temp->x = temp->pNext->x;
		temp->y = temp->pNext->y;
		temp = temp->pNext;
	}
	//  修改最后一个节点
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



//-------------------------------获取桌面图标的位置---------------------------------------
Snake* m_pRevertHead = 0;
Snake* m_pRevertEnd = 0;
void InitIconPos()
{
	//--------------获取桌面图标位置装到链表里-------------------------------------------------------------------
	DWORD dwProcessId;
	//通过桌面窗口句柄获取此窗口所在进程的PID,其实就是explorer进程 
	GetWindowThreadProcessId(hwndSysListView32, &dwProcessId);
	//打开指定PID进程,取得进程句柄 
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
	//在指定进程里面申请一个POINI结构大小的空间.
	LPVOID lpvPt = VirtualAllocEx(hProcess,NULL,sizeof(POINT),MEM_COMMIT,PAGE_READWRITE);
		
	POINT pt;

	for(int i=0;i<Nm;i++)
	{
		Snake* temp = new Snake;
		temp->nDesktopIconID = i;
		temp->pNext = 0;
		temp->pPre = 0;
		
		//获取第一个图标的坐标,存入lpvPt
		ListView_GetItemPosition(hwndSysListView32,i,lpvPt);
		//lpvPt不是本进程里面的,不能使用,所以就要利用ReadProcessMemory从指定进程给读出来 
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
	// 释放申请的空间
	VirtualFreeEx(hProcess, lpvPt, 0, MEM_RELEASE);
	//关闭句柄
	CloseHandle(hProcess);
}
void RevertIconPos()  //  把图标放回到原来的位置
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
//-------------------------------获取桌面图标的位置---------------------------------------






//----------------------游戏开始---------------------------------------------
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

//----------------------游戏开始---------------------------------------------




int CALLBACK WinMain(HINSTANCE hInstance,HINSTANCE hPreInstance,LPSTR pCmdLine,int nCmdShow)
{
	InitIconPos();   //  获取所有图标的位置

	//---------------------------------------------------------------------------------
	/*取消自动排列图标*/
	DWORD dwStyle = (DWORD)::GetWindowLong(hwndSysListView32, GWL_STYLE);
	bool bflag1 = false;
	if(dwStyle & LVS_AUTOARRANGE)
	{
		bflag1 = true;
		::SetWindowLong(hwndSysListView32,GWL_STYLE,dwStyle & ~LVS_AUTOARRANGE);
	}
	/*取消图标对齐至网格*/
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

	::SetTimer(hWnd,1,50,NULL);    //  启动游戏

	MSG message;
	while(GetMessage(&message,NULL,0,0))   //  不断的在消息队列里取出消息
	{
		TranslateMessage(&message);     //翻译消息
		DispatchMessage(&message);		// 发送消息
	}
	//  还原图标
	RevertIconPos();

	//-----------------------------------------------------------------------
	/*设置自动排列图标*/
	if(bflag1 == true)
	{
		dwStyle = (DWORD)::GetWindowLong(hwndSysListView32, GWL_STYLE);
		if(!(dwStyle & LVS_AUTOARRANGE))
			::SetWindowLong(hwndSysListView32,GWL_STYLE,dwStyle | LVS_AUTOARRANGE);
	}
	/*取消图标对齐至网格*/
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
	switch (uMsg)       //判断发送过来的是什么消息
	{
	case WM_TIMER:
		if(wParam == 1)  //  游戏一，随机图标
		{	
			::SetTimer(hwnd,4,1,0);       //  检查键盘状态
			g_nRandomDesktopIconTime++;
			RandomDesktopIcon();
			if(g_nRandomDesktopIconTime == 100)   //  停止图标随机
			{
				GameStartTime(g_nTimeArr3);
				::KillTimer(hwnd,1);
				::SetTimer(hwnd,2,1500,NULL);  
			}
		}
		else if(wParam == 2)   //  游戏二，倒计时
		{
			switch(g_nGameStartTime)
			{
			case 0:
				{
					GameStartTime(g_nTimeArr0);
					::KillTimer(hwnd,2);
					::Sleep(1500);
					//------------//  开始游戏--------------------
					CreateSnake();       //  创建蛇
					HideDesktopIcon();   //  隐藏图标
					CreateBean();        //  创建豆
					ShakeShow();
					ShowBean();
					::SetTimer(hwnd,3,200,NULL);    //  控制蛇移动
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
		else if(wParam == 3)   //  游戏三，开始游戏
		{
			SnakeMove();
			if(IsEatBean() == true)
			{
				GrowUp();
				CreateBean();
				ShowBean();
			}
			ShakeShow();

			if(GameOver() == true)  //  游戏结束
			{ 
				Win();
				::KillTimer(hwnd,3);
				::PostMessage(hwnd,WM_CLOSE,0,0);
				Sleep(2000);
			}
		}
		else if(wParam == 4)   //  获取键盘状态
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
		PostQuitMessage(0);   //如果是退出的消息   就退出
		break;
	}
	return DefWindowProc(hwnd,uMsg,wParam,lParam);  //  执行消息的默认处理
}
