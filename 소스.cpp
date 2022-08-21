#include <windows.h>
#include <ctime>
#include <cstdlib>
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hInst;
HWND MainWnd;
LPCTSTR lpszClass = TEXT("����Ű���");
RECT rt;
int workent_count; // �ϰ��� ����
int soldier_count; // �������� ����
int enemy_count; // �� ����
int workerant[800][600]; // ��ġ���� , �����̱� ���� ���Ǿ����� ��ϼ�
int soldier[800][600];
int enemy[800][600]; 
int gold_XY[800][600];
///�̱��� ////
int time_m; // ���� �ð���
int gold = 500; // ��

int rand_x_index; // ��带 �����ϱ� ���� x, y ��ǥ�� ����
int rand_y_index;


int anthouseX = rt.left + 10; // ������ ��ġ�� ��Ÿ���� ���ؼ� // �̿ϼ� //
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
DWORD WINAPI ThreadFunc(LPVOID temp) // ���� ���� ������ ///       �̿ϼ�
{
	WaitForSingleObject(hEvent, INFINITE);
	static int  soliderindex_x[1000] = { 0 };
	static int soliderindex_y[1000] = { 0 };

	int pp[1000] = { 0 };
	while (1)
	{
		Sleep(1000);
		soldier[anthouseX + soliderindex_x[soldier_count]][anthouseY+ 100] = '0'; // ó�� ���� ������ ���°� ���� ���ش�
		soliderindex_x[soldier_count] = soliderindex_x[soldier_count] + 10; // �̵��Ѵ�
		soldier[anthouseX + soliderindex_x[soldier_count]][anthouseY +100] = '1'; // x������ 10��ŭ �̵� �Ͽ���
	}
	ResetEvent(hEvent);

	return 0;
}
DWORD WINAPI ThreadFunc2(LPVOID temp) // �ϰ��� ������ ////     �̿ϼ�
{
	WaitForSingleObject(hEvent, INFINITE);
	static int walkindex_x[1000] = { 0 };
	static int walkindex_y[1000] = { 0 };

	int pp[1000] = { 0 };
	while (1)
	{
		Sleep(1000);
		workerant[anthouseX + walkindex_x[workent_count]][anthouseY] = '0'; // ó�� ���� ������ ���°� ���� ���ش�
		walkindex_x[workent_count] = walkindex_x[workent_count] + 10; // �̵��Ѵ�
		workerant[anthouseX + walkindex_x[workent_count]][anthouseY] = '1'; // x������ 10��ŭ �̵� �Ͽ���

	}
	//	ResetEvent(hEvent);

	return 0;
}	
DWORD WINAPI ThreadFunc3(LPVOID temp) //  ���� ���� ������ �� ����
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

DWORD WINAPI ThreadFunc4(LPVOID temp) // �ϰ��� �� ����
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
DWORD WINAPI ThreadFunc5(LPVOID temp) // ��,���� ������ ���簳�� �ϰ��� ����
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
//������ ���õ� �׸�
int bosscount = 1; // ������ ���������� �����ϴµ� �����Ѹ�ŭ ü���� �ö󰣴�
int bosshp = 20; //ü��
int boss_x; // ���� ��ġ�� ���� ,, 
int boss_y;


int attack = 1; // ����� ���ݷ� 

bool Restart = false; // �� ���۽� �ʼ�.

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	static HBRUSH hBrush;
	TCHAR str[256];
	HDC hdc;
	PAINTSTRUCT ps;
	HANDLE hThread, hThread2, hThread3, hThread4, hThread5; // ��,�ϰ��̿� ������ ���ҿ� ���� ��� ������
	DWORD ThreadID, ThreadID2, ThreadID3, ThreadID4, ThreadID5;
	static int mouse_x;
	static int mouse_y;
	GetClientRect(hWnd, &rt); // ������ ũ�⸦ �ǹ��Ѵ�
	
	
	switch (iMessage) {

	case WM_CREATE:
		hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);// ����  3��° ���� FLASE-> ���ȣ
		hThread2 = CreateThread(NULL, 0, ThreadFunc2, NULL, 0, &ThreadID2);
		hThread = CreateThread(NULL, 0, ThreadFunc, NULL, 0, &ThreadID);
		hThread3 = CreateThread(NULL, 0, ThreadFunc3, NULL, 0, &ThreadID3);
		hThread4 = CreateThread(NULL, 0, ThreadFunc4, NULL, 0, &ThreadID4);
		hThread5 = CreateThread(NULL, 0, ThreadFunc5, NULL, 0, &ThreadID5);

		boss_x = rand() % X;
		boss_y = rand() % Y;

		for (int count = 0; count < 1; count++) // ó���� �������� ��带 ��ġ �Ѵ�///////�̿ϼ�
		{
			rand_x_index = rand() % X;
			rand_y_index = rand() % Y;
			gold_XY[rand_x_index][rand_y_index] = '1';
		}

		for (int i = 0; i < X; i++) // ���̵�� �� ��ġ�� ��� �׸��� '0'���� �Ѵ� �����ÿ� '1'�� �ٲ� �̵���ų ���� ///////�̿ϼ�
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
		CreateWindow("button", "���� ����(50)", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 20, 125, 30, hWnd, (HMENU)0, g_hInst, NULL);
		CreateWindow("button", "�� ����(30)", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 80, 125, 30, hWnd, (HMENU)1, g_hInst, NULL);
		CreateWindow("button", "���� ����", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 20, 140, 125, 30, hWnd, (HMENU)5, g_hInst, NULL);
		CreateWindow("button", "���ݷ�+1(500)", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 150, 140, 125, 30, hWnd, (HMENU)4, g_hInst, NULL);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case 0:

			hdc = BeginPaint(hWnd, &ps);
			if (gold >= 50 && time_m % 10 != 0 && time_m % 5 != 0) // ��尡 50 �̻��̸� ���� ���� ����
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
			if (gold >= 30 && time_m % 10 != 0 && time_m % 5 != 0) // ��� 30 �̻��̸� �ϰ��� ����
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
			if (boss == true  && time_m % 10 != 0 && time_m % 5 != 0) // ������ �����Ǹ� �ߵ��� ���� . ü���� ����ش�
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
			if (time_m % 10 != 0 && time_m % 5 != 0 && gold > 500) //  ���ݷ��� ���� �����ش�
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
	case WM_TIMER:  //�ð� ���������� �� ���� , ���� ���� 
	
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
		SetEvent(hEvent); // �̺�Ʈ�� ��ȣ���·� ����
		for (int i = rt.left; i < rt.right; i++) // �� ��ü�� Ž���Ͽ� ���̰� ������('1')����  �׸�� �����̱� ���� �ٲ��ش� /// �̿ϼ�
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

		if (boss == true) // ������ �����ϸ�
		{
			Rectangle(hdc, boss_x, boss_y, boss_x + 100, boss_y + 100); //�������� ��ġ��Ų��
		}
		else
		{

		}
		if (time_m >= 10 && (soldier_count <= 0 && workent_count <= 0)) // �ð��� 10�� ������ �ϰ��̿� �������̰� ���ٸ� ������Ҽ��ִ�.
		{
			if (Restart == false)
			{
				Restart = true;
				if (MessageBox(hWnd, TEXT("-�й�- �ٽ��Ͻðڽ��ϱ�?"), TEXT("����Ű���"), MB_YESNO) == IDYES)
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
		
		wsprintf(str, TEXT("�������� : %d ����"), soldier_count);
		TextOut(hdc, rt.right - 300, 0, str, lstrlen(str));

		wsprintf(str, TEXT("�ϰ��� %d :����"), workent_count);
		TextOut(hdc, rt.right - 300, 50, str, lstrlen(str));

		wsprintf(str, TEXT("�� %d :����"), enemy_count);
		TextOut(hdc, rt.right - 300, 150, str, lstrlen(str));

		wsprintf(str, TEXT("�� %d " ), gold);
		TextOut(hdc, rt.right - 300, 100, str, lstrlen(str));

		if (boss == true)
		{
			wsprintf(str, TEXT("����ü�� %d"), bosshp);
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
