#include <windows.h>
#include <ctime>
#include <cstdlib>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND MainWnd;
LPCTSTR lpszClass = TEXT("개미키우기");
RECT rt;
int workent_count; // 일개미 숫자
int soldier_count; // 병정개미 숫자
int enemy_count; // 적 숫자
int workerant[800][600]; // 위치값들 , 움직이기 위해 사용되었지만 비완성
int soldier[800][600];
int enemy[800][600]; 
int gold_XY[800][600];
///이까지 ////
int time_m; // 게임 시간초
int gold = 500; // 돈

int rand_x_index; // 골드를 생성하기 위해 x, y 좌표값 생성
int rand_y_index;


int anthouseX = rt.left + 10; // 개미집 위치를 나타내기 위해서 // 미완성 //
int anthouseY = rt.bottom + 1000;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hInst = hInstance;
	srand((unsigned int)time(NULL));
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hWnd, nCmdShow);

	while (GetMessage(&Message, NULL, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return (int)Message.wParam;
}
HANDLE hEvent;
#define X 800  
#define Y 600
DWORD WINAPI ThreadFunc(LPVOID temp) // 병사 개미 움직임 ///       미완성
{
	WaitForSingleObject(hEvent, INFINITE);
	static int  soliderindex_x[1000] = { 0 };
	static int soliderindex_y[1000] = { 0 };

	int pp[1000] = { 0 };
	while (1)
	{
		Sleep(1000);
		soldier[anthouseX + soliderindex_x[soldier_count]][anthouseY+ 100] = '0'; // 처음 개미 집에서 나온걸 먼저 없앤다
		soliderindex_x[soldier_count] = soliderindex_x[soldier_count] + 10; // 이동한다
		soldier[anthouseX + soliderindex_x[soldier_count]][anthouseY +100] = '1'; // x값으로 10만큼 이동 하였다
	}
	ResetEvent(hEvent);

	return 0;
}
DWORD WINAPI ThreadFunc2(LPVOID temp) // 일개미 움직임 ////     미완성
{
	WaitForSingleObject(hEvent, INFINITE);
	static int walkindex_x[1000] = { 0 };
	static int walkindex_y[1000] = { 0 };

	int pp[1000] = { 0 };
	while (1)
	{
		Sleep(1000);
		workerant[anthouseX + walkindex_x[workent_count]][anthouseY] = '0'; // 처음 개미 집에서 나온걸 먼저 없앤다
		walkindex_x[workent_count] = walkindex_x[workent_count] + 10; // 이동한다
		workerant[anthouseX + walkindex_x[workent_count]][anthouseY] = '1'; // x값으로 10만큼 이동 하였다

	}
	//	ResetEvent(hEvent);

	return 0;
}	
DWORD WINAPI ThreadFunc3(LPVOID temp) //  병사 개미 증가시 적 감소
{
	WaitForSingleObject(hEvent, INFINITE);

	while (1)
	{
		Sleep(2000);
		{
			if (soldier_count >= 3 &&  enemy_count >= 0)
			{
				enemy_count -=3;			
			}
			if (soldier_count >= 5 && enemy_count >= 0)
			{
				enemy_count -= 6;
			}
		}
	}

	return 0;
}

DWORD WINAPI ThreadFunc4(LPVOID temp) // 일개미 돈 증가
{
	WaitForSingleObject(hEvent, INFINITE);
	while (1)
	{
		Sleep(4000);
		{
			if (workent_count >= 1)
			{
				gold += 100;				
			}

			if (workent_count >= 3)
			{
				gold += 350;
			}

			if (workent_count >= 5)
			{
				gold += 500;
			}
		}
	}


	return 0;
}
int round = 0;
bool boss = false;
DWORD WINAPI ThreadFunc5(LPVOID temp) // 적,보스 출현시 병사개미 일개미 감소
{
	WaitForSingleObject(hEvent, INFINITE);
	while (1)
	{
		Sleep(4000);
		{
			if (enemy_count >= 3)
			{
				workent_count -= 3 + round;
				soldier_count -= 1 + round;
			}
			if (boss == true)
			{
				workent_count -= 10 + round;
				soldier_count -= 5 + round;
			}
		}
	}

	return 0;
}
//보스에 관련된 항목
int bosscount = 1; // 보스를 잡을때마다 증가하는데 증가한만큼 체력이 올라간다
int bosshp = 20; //체력
int boss_x; // 보스 위치를 생성 ,, 
int boss_y;


int attack = 1; // 사용자 공격력 

bool Restart = false; // 재 시작시 필수.

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	static HBRUSH hBrush;
	TCHAR str[256];
	HDC hdc;
	PAINTSTRUCT ps;
	HANDLE hThread, hThread2, hThread3, hThread4, hThread5; // 적,일개미에 생성과 감소에 대한 모든 쓰레드
	DWORD ThreadID, ThreadID2, ThreadID3, ThreadID4, ThreadID5;
	static int mouse_x;
	static int mouse_y;
	GetClientRect(hWnd, &rt); // 윈도우 크기를 의미한다
	
	
	switch (iMessage) {

	case WM_CREATE:
		hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);// 수동  3번째 인자 FLASE-> 비신호
		hThread2 = CreateThread(NULL, 0, ThreadFunc2, NULL, 0, &ThreadID2);
		hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &ThreadID);
		hThread3 = CreateThread(NULL, 0, ThreadFunc3, NULL, 0, &ThreadID3);
		hThread4 = CreateThread(NULL, 0, ThreadFunc4, NULL, 0, &ThreadID4);
		hThread5 = CreateThread(NULL, 0, ThreadFunc5, NULL, 0, &ThreadID5);

		boss_x = rand() % X;
		boss_y = rand() % Y;

		for (int count = 0; count < 1; count++) // 처음에 랜덤으로 골드를 배치 한다///////미완성
		{
			rand_x_index = rand() % X;
			rand_y_index = rand() % Y;
			gold_XY[rand_x_index][rand_y_index] = '1';
		}

		for (int i = 0; i < X; i++) // 개미들과 적 위치에 모든 항목을 '0'으로 한다 생성시에 '1'로 바꿔 이동시킬 예정 ///////미완성
		{
			for (int j = 0; j < Y; j++)
			{
				soldier[i][j] = '0';
				workerant[i][j] = '0';
				enemy[i][j] = '0';
			}
		}
		SetTimer(hWnd, 0, 1000, NULL);
		hBrush = CreateSolidBrush(RGB(0, 255, 0));
		SetClassLongPtr(hWnd, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);
		InvalidateRect(hWnd, NULL, FALSE);
		CreateWindow("button", "병사 개미(50)", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 20, 125, 30, hWnd, (HMENU)0, g_hInst, NULL);
		CreateWindow("button", "일 개미(30)", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 80, 125, 30, hWnd, (HMENU)1, g_hInst, NULL);
		CreateWindow("button", "보스 공격", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 140, 125, 30, hWnd, (HMENU)5, g_hInst, NULL);
		CreateWindow("button", "공격력+1(500)", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 150, 140, 125, 30, hWnd, (HMENU)4, g_hInst, NULL);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 0:

			hdc = BeginPaint(hWnd, &ps);
			if (gold >= 50 && time_m % 10 != 0 && time_m % 5 != 0) // 골드가 50 이상이면 병정 개미 생성
			{
				soldier[anthouseX][anthouseY + 100] = '1';
				soldier_count++;
				gold = gold - 50;
				time_m--;
			}
			else
			{

			}
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case 1:
			if (gold >= 30 && time_m % 10 != 0 && time_m % 5 != 0) // 골드 30 이상이면 일개미 생성
			{
				workerant[anthouseX][anthouseY] = '1';
				workent_count++;
				gold = gold - 30;
				time_m--;
			}
			else
			{

			}
			InvalidateRect(hWnd, NULL, TRUE);
			break;

		case 5:
			if (boss == true  && time_m % 10 != 0 && time_m % 5 != 0) // 보스가 생성되면 발동이 가능 . 체력을 깍아준다
			{
				bosshp -= attack;
				time_m--;
			}
			else
			{

			}
			InvalidateRect(hWnd, NULL, FALSE);
			break;

		case 4:
			if (time_m % 10 != 0 && time_m % 5 != 0 && gold > 500) //  공격력을 증가 시켜준다
			{				
				attack++;
				gold = gold - 500;
				time_m--;
			}
			else
			{
				
			}
			InvalidateRect(hWnd, NULL, FALSE);
			break;
			
		}
	case WM_TIMER:  //시간 지날때마다 적 출현 , 보스 출현 
	
		time_m++;
		if (time_m % 5 == 0)
		{
			enemy_count += 4;
		}
		if (time_m % 10 == 0)
		{
			enemy_count += 8;
			round++;
		}
		if (time_m % 65 == 0)
		{
			enemy_count += 12;
		}
		if (time_m % 20 == 0)
		{
			boss = true;
		}
		if (bosshp <= 0)
		{
			boss = false;
			bosscount++;
			bosshp = 20 * bosscount;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		SetEvent(hEvent); // 이벤트를 신호상태로 변경
		for (int i = rt.left; i < rt.right; i++) // 맵 전체를 탐색하여 개미가 생성된('1')곳을  네모로 움직이기 위해 바꿔준다 /// 미완성
		{
			for (int j = rt.top; j < rt.bottom; j++)
			{
				if (workerant[i][j] == '1')
				{
					Rectangle(hdc, i, j, i+5, j+5);  // left top right bottle
				}
				if (soldier[i][j] == '1')
				{
					Rectangle(hdc, i, j, i + 5, j + 5);  // left top right bottle
				}
				if (gold_XY[i][j] == '1')
				{
					Rectangle(hdc, i, j, i + 20, j + 20);  // left top right bottle
				}
			}
		}

		if (boss == true) // 보스가 출현하면
		{
			Rectangle(hdc, boss_x, boss_y, boss_x + 100, boss_y + 100); //랜덤으로 위치시킨다
		}
		else
		{

		}
		if (time_m >= 10 && (soldier_count <= 0 && workent_count <= 0)) // 시간이 10초 지난후 일개미와 병정개미가 없다면 재시작할수있다.
		{
			if (Restart == false)
			{
				Restart = true;
				if (MessageBox(hWnd, TEXT("-패배- 다시하시겠습니까?"), TEXT("개미키우기"), MB_YESNO) == IDYES)
				{
					gold = 500;
					time_m = 0;
					enemy_count = 0;
					bosscount = 0;
					bosshp = 20;
					boss = false;
					workent_count = 0;
					soldier_count = 0;
					round = 0;
					Restart = false;
				}
				else
				{
					SendMessage(hWnd, WM_DESTROY, NULL, NULL);
				}
			}
		}
		wsprintf(str, TEXT("%d"),time_m);
		TextOut(hdc, 600, 0, str, lstrlen(str));
		
		wsprintf(str, TEXT("병정개미 : %d 마리"), soldier_count);
		TextOut(hdc, rt.right - 300, 0, str, lstrlen(str));

		wsprintf(str, TEXT("일개미 %d :마리"), workent_count);
		TextOut(hdc, rt.right - 300, 50, str, lstrlen(str));

		wsprintf(str, TEXT("적 %d :마리"), enemy_count);
		TextOut(hdc, rt.right - 300, 150, str, lstrlen(str));

		wsprintf(str, TEXT("돈 %d " ), gold);
		TextOut(hdc, rt.right - 300, 100, str, lstrlen(str));

		if (boss == true)
		{
			wsprintf(str, TEXT("보스체력 %d"), bosshp);
			TextOut(hdc, rt.right - 300, 300, str, lstrlen(str));
		}
		wsprintf(str, TEXT("Round %d"), round);
		TextOut(hdc, rt.right - 300, 400, str, lstrlen(str));
		EndPaint(hWnd, &ps);

		return 0;
	case WM_DESTROY:

		PostQuitMessage(0);
		return 0;
	}
	return(DefWindowProc(hWnd, iMessage, wParam, lParam));
}
