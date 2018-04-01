#include"header.h"
#define WINDOW_TITLE L"������"//��Ϸ����
#define WINDOW_WIDTH 1000//���ڵĿ��
#define WINDOW_HEIGHT 730//���ڵĸ߶�
#define SIZE 36//С��Ĵ�С
typedef struct position//����С��λ��
{
	int x;
	int y;
} posType;
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int diaphaneity=255;//͸����
int game_state = 0;//��¼��Ϸ״̬
int gametime = 30;//��Ϸ��ʱ��
int backgroundnum=3;//�������
int Game_Init(HWND hwnd);//��Ϸ��ʼ��
void Game_Paint(HWND hwnd, int x, int y);//�Զ��庯�����õ����������С��λ��
void Game_Play(HWND hwnd, HDC g_hdc);//��Ϸ��ʼ��
int Game_CleanUp(HWND hwnd);//����
void drawercurser(HWND hwnd, posType pos,COLORREF c1);//�����
void drawtime(int);//��ʣ��ʱ��
void drawscore(int);//�����÷���
void ballsfall(HWND hwnd, HDC destDC);
void turn();//��С���Ƚ������򣬲�Ȼ���޹������
int balls_occurs(HWND hwnd, int n);
void getsamecolorballs(HWND hwnd, HDC g_hdc,posType cur, COLORREF c1);//�õ�ͬɫС��
int isvalued(HWND hwnd,HDC g_hdc,posType tympos, COLORREF c1);//ȷ����Χ��С���ǲ���ͬɫС��
void countdown(HWND hwnd);//����ʱ
void gameover(HWND hwnd);//��Ϸ����
void CreateMyMenu();//�����˵�  
void game_progress(HWND hwnd,HDC g_hdc);//��Ϸȫ������
int gamebegin_interface(HWND hwnd);//��Ϸ��ʼ����
int windowsshake(HWND hwnd);
HPEN hpen;//�ֱʾ��
HFONT font;//������
HBRUSH hbrush;//��ˢ���
HMENU hmenu;//�˵����
HDC g_hdc = NULL;
HDC g_mdc = NULL;
HBITMAP		g_hBitmap = NULL;  //����һ��λͼ���
HINSTANCE hInstance;
posType cur;//��ǰ���λ��
posType ballsarr[180]; //ͬɫС�����꣬����180��С���ֹ������ɫȫ��һ�������
int index;  //ͬɫС��ĸ���
int score = 0;
int result;//��Ϣ���ǻ��
clock_t t;//����ʱ�䣬�������Ѿ����е�ʱ��
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	CreateMyMenu();//�����˵�  
	srand((unsigned)time(NULL));//����ʱ�����ӣ����������
	static TCHAR szAppName[] = TEXT("WindowsAPI");
	HWND         hwnd;
	MSG          msg = { 0 };//��Ϣ�ṹ��
	WNDCLASSEX   wndclassex = { 0 };
	wndclassex.cbSize = sizeof(WNDCLASSEX);
	wndclassex.style = CS_HREDRAW | CS_VREDRAW;
	wndclassex.lpfnWndProc = WndProc;
	wndclassex.cbClsExtra = 0;
	wndclassex.cbWndExtra = 0;
	wndclassex.hInstance = hInstance;
	wndclassex.hIcon = (HICON)LoadImage(NULL, L"icon1.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	//wndclassex.hIcon = LoadIcon(NULL, IDI_APPLICATION);Ĭ��ͼ��
	wndclassex.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wndclassex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);//����ɫ
	wndclassex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	wndclassex.lpszMenuName = NULL;
	wndclassex.lpszClassName = szAppName;
	wndclassex.hIconSm = wndclassex.hIcon;
	//����Ƿ�ע�ᴰ��ʧ��
	if (!RegisterClassEx(&wndclassex))
	{
		MessageBox(NULL, TEXT("RegisterClassEx failed!"), szAppName, MB_ICONERROR);
		return 0;
	}
	int x = ((GetSystemMetrics(SM_CXSCREEN) / 2) - 500); //x����  
	int y = ((GetSystemMetrics(SM_CYSCREEN) / 2) - 365); //y���� 
	hwnd = CreateWindow
		(szAppName,
		WINDOW_TITLE,
		WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		x,//
		y, //
		WINDOW_WIDTH,//���ڿ��
		WINDOW_HEIGHT,//���ڸ߶�
		NULL,
		hmenu,//�˵�
		hInstance, //ʵ�����
		NULL);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	while ((msg.message != WM_QUIT))//������Ϣ		//ʹ��whileѭ���������Ϣ����WM_QUIT��Ϣ���ͼ���ѭ��
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))   //�鿴Ӧ�ó�����Ϣ���У�����Ϣʱ�������е���Ϣ�ɷ���ȥ��
		{
			TranslateMessage(&msg);		//���������Ϣת��Ϊ�ַ���Ϣ
			DispatchMessage(&msg);			//�ַ�һ����Ϣ�����ڳ���
		}
		else
		{
			game_progress(hwnd,g_hdc);//��Ϸ�Ľ��̣�������Ϸ��ʼ�����Լ���ʼ����Ϸ�Լ���������
		}
	}
	Game_CleanUp(hwnd);
	return msg.wParam;//������Ϣ
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	clock_t t=clock();
	HDC destDC=NULL;
	PAINTSTRUCT paintStruct;	//����һ��PAINTSTRUCT�ṹ������¼һЩ������Ϣ
	static HWND     hwndButton[5];   //��ť���
	switch (message)
	{
		
	  case WM_CREATE:
	  {
		SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(hwnd, RGB(255, 255, 255), 200, LWA_ALPHA);

		//����Ϸ����Ϸʱ����ִ�в���
		if (((t / CLOCKS_PER_SEC) >= 0) && ((t / CLOCKS_PER_SEC) < gametime))
		{
			hwndButton[0] = CreateWindow(L"BUTTON", L"��",
				WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				787, 262, 70, 27, hwnd, NULL,
				(HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE), NULL);
			hwndButton[1] = CreateWindow(L"BUTTON", L"��",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				787, 350, 70, 27, hwnd, NULL,
				(HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE), NULL);
			hwndButton[2] = CreateWindow(L"BUTTON", L"��",
				WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				762, 285, 27, 70, hwnd, NULL,
				(HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE), NULL);
			hwndButton[3] = CreateWindow(L"BUTTON", L"��",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				851, 285, 27, 70, hwnd, NULL,
				(HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE), NULL);
			hwndButton[4] = CreateWindow(L"BUTTON", L"OK",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				791, 291, 57, 57, hwnd, NULL,
				(HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE), NULL);
			return 0;
		}
		else if ((t / CLOCKS_PER_SEC) >= gametime)
		{
			hwndButton[0] = CreateWindow(L"BUTTON", L"��",
				 WS_CHILD | BS_DEFPUSHBUTTON,
				787, 262, 70, 27, hwnd, NULL,
				(HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE), NULL);
			//��ť2
			hwndButton[1] = CreateWindow(L"BUTTON", L"��",
				WS_TABSTOP | WS_CHILD | BS_DEFPUSHBUTTON,
				787, 350, 70, 27, hwnd, NULL,
				(HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE), NULL);
			hwndButton[2] = CreateWindow(L"BUTTON", L"��",
				WS_CHILD | BS_DEFPUSHBUTTON,
				762, 285, 27, 70, hwnd, NULL,
				(HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE), NULL);
			hwndButton[3] = CreateWindow(L"BUTTON", L"��",
				WS_TABSTOP | WS_CHILD | BS_DEFPUSHBUTTON,
				851, 285, 27, 70, hwnd, NULL,
				(HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE), NULL);
			hwndButton[4] = CreateWindow(L"BUTTON", L"OK",
				WS_TABSTOP | WS_CHILD | BS_DEFPUSHBUTTON,
				791, 291, 57, 57, hwnd, NULL,
				(HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE), NULL);
			return 0;
		}
	  }
	case WM_COMMAND:
    //����Ϸ����Ϸʱ����ִ�в���
		if (((t / CLOCKS_PER_SEC) >= 0) && ((t / CLOCKS_PER_SEC) < gametime))
		{
			if ((HWND)lParam == hwndButton[0] && (cur.y > 70))//��
			{
				g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC//�����.bmp", IMAGE_BITMAP, 140, 140, LR_LOADFROMFILE);   //����
				g_mdc = CreateCompatibleDC(g_hdc);
				Game_Paint(hwnd, -750, -250);
				PlaySound(L"MUSIC\\cursormove.wav", NULL, SND_FILENAME | SND_ASYNC);
				drawercurser(hwnd, cur, RGB(0, 0, 0));
				cur.x = cur.x;
				cur.y = cur.y - 40;
				drawercurser(hwnd, cur, RGB(255, 255, 255));
			}
			if ((HWND)lParam == hwndButton[1] && (cur.y < 610))//��
			{
				g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC//�����.bmp", IMAGE_BITMAP, 140, 140, LR_LOADFROMFILE);   //����
				g_mdc = CreateCompatibleDC(g_hdc);
				Game_Paint(hwnd, -750, -250);
				PlaySound(L"MUSIC\\cursormove.wav", NULL, SND_FILENAME | SND_ASYNC);
				drawercurser(hwnd, cur, RGB(0, 0, 0));
				cur.x = cur.x;
				cur.y = cur.y + 40;
				drawercurser(hwnd, cur, RGB(255, 255, 255));
			}
			if ((HWND)lParam == hwndButton[2] && (cur.x > 280))//��
			{
				g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC//�����.bmp", IMAGE_BITMAP, 140, 140, LR_LOADFROMFILE);   //����
				g_mdc = CreateCompatibleDC(g_hdc);
				Game_Paint(hwnd, -750, -250);
				PlaySound(L"MUSIC\\cursormove.wav", NULL, SND_FILENAME | SND_ASYNC);
				drawercurser(hwnd, cur, RGB(0, 0, 0));
				cur.y = cur.y;
				cur.x = cur.x - 40;
				drawercurser(hwnd, cur, RGB(255, 255, 255));

			}
			if ((HWND)lParam == hwndButton[3] && (cur.x < 680))//��
			{
				g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC//�����.bmp", IMAGE_BITMAP, 140, 140, LR_LOADFROMFILE);   //����
				g_mdc = CreateCompatibleDC(g_hdc);
				Game_Paint(hwnd, -750, -250);
				PlaySound(L"MUSIC\\cursormove.wav", NULL, SND_FILENAME | SND_ASYNC);
				drawercurser(hwnd, cur, RGB(0, 0, 0));
				cur.y = cur.y;
				cur.x = cur.x + 40;
				drawercurser(hwnd, cur, RGB(255, 255, 255));
			}
			if ((HWND)lParam == hwndButton[4])//ȷ��
			{
				g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC//�����.bmp", IMAGE_BITMAP, 140, 140, LR_LOADFROMFILE);   //����
				g_mdc = CreateCompatibleDC(g_hdc);
				Game_Paint(hwnd, -750, -250);
				//����ù��������Χλ�õ�ͬɫС�����꣬�������飬����¼������
				getsamecolorballs(hwnd, g_hdc, cur, GetPixel(g_hdc, cur.x, cur.y));
				//�������õ�ʱ��ʼ����
				if (index > 2)
				{
					PlaySound(L"MUSIC\\Win.wav", NULL, SND_FILENAME | SND_ASYNC);//����֮����ʾ����
					
					for (int k = 0; k < index; k++)
					{
						//ͬʱ����ͼƬ7���γ�������Ч��
						g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC//ͼ��3//7.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //����λͼ
						Game_Paint(hwnd, -(ballsarr[k].x - 18), -(ballsarr[k].y - 18));
					}
					windowsshake(hwnd);

					Sleep(500);
					//Sleep 500s ���䶯��
					ballsfall(hwnd, destDC);//С�����
					score += index;//�����ۼӲ����»���
					drawscore(score);
				}

				if (index <= 2)
				{
					PlaySound(L"MUSIC\\Error.wav", NULL, SND_FILENAME | SND_ASYNC);
				}
				//С��������ʱ�������ʾ��
				index = 0;//һ��������ɣ���ͬɫС���������
		    }
		}
		 switch (LOWORD(wParam))
		 {
			 case IDM_OPT1:

			 {
				 if (!Game_Init(hwnd))
				 {
					 MessageBox(hwnd, L"��Դ��ʼ��ʧ��", L"��Ϣ����", 0); //ʹ��MessageBox����������һ����Ϣ����
					 return 0;
				 }
				 game_state = 1;
				 //ת����Ϸ״̬���л�����ʼ����Ϸ
				 gametime += t / CLOCKS_PER_SEC;
				 //����Ϸδ��ʼʱ��ϵͳʱ�����
				 break;
			 }
			 case IDM_OPT2:
				 Game_Init(hwnd);
				 break;
			 case IDM_OPT12:
				gametime = 30+ t / CLOCKS_PER_SEC;
				 break;
			 case IDM_OPT13:
				 gametime = 60+t / CLOCKS_PER_SEC;
				 break;
			 case IDM_OPT14:
				 gametime = 90+ t / CLOCKS_PER_SEC;
				 break;
			 case IDM_OPT23:
				 gametime = 120 + t / CLOCKS_PER_SEC;
				 break;
			 case IDM_OPT15:
				 backgroundnum = 1;
				 Game_Init(hwnd);
				 break;
			 case IDM_OPT16:
				 backgroundnum = 2;
				 Game_Init(hwnd);
				 break;
			 case IDM_OPT17:
				 backgroundnum = 3;
				 Game_Init(hwnd);
				 break;
			 case IDM_OPT18:
				 backgroundnum = 4;
				 Game_Init(hwnd);
				break;
			 case IDM_OPT19:
				 ShellExecute(NULL, L"open",L"�ĵ�\\��ϵ����.html", NULL, NULL, SW_SHOWNORMAL);
				 break;
			 case IDM_OPT20:
				 ShellExecute(NULL, L"open", L"�ĵ�\\�����ĵ�.txt", NULL, NULL, SW_SHOWNORMAL);
				break;
			 case IDM_OPT21:
				 ShellExecute(NULL, L"open", L"http://pan.baidu.com/s/1slIQ8Lf", NULL, NULL, SW_SHOWNORMAL);
			
			 case IDM_OPT22:
				 result = MessageBox(NULL, L"��ȷ��Ҫ�˳���Ϸ��", L"��ʾ", MB_YESNO | MB_ICONQUESTION);
				 if (IDYES == result)
				 {
					 DestroyWindow(hwnd);
				 }
				 else if (IDNO == result)
				 {
					 //
				 }
				 return 0;
				 break;
		  }
		 break;
	case WM_PAINT: 
		g_hdc = BeginPaint(hwnd, &paintStruct);  //ָ�����ڽ��л�ͼ������׼�������ý��ͻ�ͼ�йص���Ϣ��䵽paintStruct�ṹ���С�
		EndPaint(hwnd, &paintStruct);			//EndPaint�������ָ�����ڵĻ滭���̽���
		ValidateRect(hwnd, NULL);		// ���¿ͻ�������ʾ
		break;		//������switch���
	case WM_CLOSE:
		if (IDYES == MessageBox(hwnd, L"���Ƿ�Ҫ�˳���Ϸ", L"��ʾ������", MB_YESNO | MB_ICONQUESTION))
		{
			DestroyWindow(hwnd);
		}
		else
		{
			//
		}
		return 0;//����break����ΪҪ����Ϣ��������������
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
}
	return DefWindowProc(hwnd, message, wParam, lParam);
		
}
//������Ϸ����
int Game_Init(HWND hwnd)//�����ʼ��
{
	g_hdc = GetDC(hwnd);  //��ȡ�豸����
	//��ȡ������ţ��Ա��������
	switch (backgroundnum)
	{
	case 1:
		g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\����\\2.bmp", IMAGE_BITMAP, 1000, 700, LR_LOADFROMFILE);   //����
		g_mdc = CreateCompatibleDC(g_hdc);
		Game_Paint(hwnd, 2, 0);
		break;
	case 2:
		g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\����\\3.bmp", IMAGE_BITMAP, 1030, 730, LR_LOADFROMFILE);   //����
		g_mdc = CreateCompatibleDC(g_hdc);
		Game_Paint(hwnd, 30, 17);
		break;
	case 3:
		g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\����\\4.bmp", IMAGE_BITMAP, 1030, 730, LR_LOADFROMFILE);   //����
		g_mdc = CreateCompatibleDC(g_hdc);
		Game_Paint(hwnd, 0, 0);
		break;
	case 4:
		g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\����\\5.bmp", IMAGE_BITMAP, 1000, 730, LR_LOADFROMFILE);   //����
		g_mdc = CreateCompatibleDC(g_hdc);
		Game_Paint(hwnd, 0, 0);
		break;

	}
	//ǰ����������ͼƬ
	for (int x = 240; x < 720; x += 40)
	{
		for (int y = 50; y < 610; y += 40)
		{
			int num = 1 + 1 + rand() % 5;
			balls_occurs(hwnd, num);
			Game_Paint(hwnd, -x, -y);
		}
	}
	//���һ��ָ������
	for (int x = 240; x < 480; x += 40)
	{
		
			balls_occurs(hwnd,1+((x-240)/40));
			Game_Paint(hwnd, -x, -610);
	}
	for (int x = 480; x < 720; x += 40)
	{

		balls_occurs(hwnd, 1 + ((x - 480) / 40));
		Game_Paint(hwnd, -x, -610);
	}
	g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\�����.bmp", IMAGE_BITMAP, 140, 140, LR_LOADFROMFILE);   //����
	g_mdc = CreateCompatibleDC(g_hdc);
	Game_Paint(hwnd, -750, -250);
	//��ǰ�Ĺ��λ��
	cur.x = 498;
	cur.y = 348;
	drawercurser(hwnd,cur,RGB(255,255,255));
	//����ꣻ
	drawtime(gametime);
	//����ʱ�䣻
	drawscore(0);
	//���Ʒ�����
	return 1;
}
//ָ��λ����ͼ
void Game_Paint(HWND hwnd,int x,int y)//�Զ��庯����ͬʱȷ��ͼƬλ��
{
	SelectObject(g_mdc, g_hBitmap);    
	BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_mdc, x, y, SRCCOPY);    //����BitBlt������ͼ����������Ϊ���ڴ�С
}
//��Ϸ��ʼ��
void Game_Play(HWND hwnd, HDC destDC)
{
	clock_t t = clock();//����ʱ��
	//����Ϸ����Ϸʱ����ִ�в���
	if (((t / CLOCKS_PER_SEC) >= 0) && ((t / CLOCKS_PER_SEC) < gametime))
	{
		if ((GetAsyncKeyState(VK_UP) < 0) && (cur.y > 70))	//��
		{
			PlaySound(L"MUSIC\\cursormove.wav", NULL, SND_FILENAME | SND_ASYNC);
			drawercurser(hwnd, cur, RGB(0, 0, 0));
			cur.x = cur.x;
			cur.y = cur.y - 40;
			drawercurser(hwnd, cur, RGB(255, 255, 255));
			
		}
		else if (GetAsyncKeyState(VK_DOWN) < 0 && (cur.y < 600))	//��
		{
			PlaySound(L"MUSIC\\cursormove.wav", NULL, SND_FILENAME | SND_ASYNC);
			drawercurser(hwnd, cur, RGB(0, 0, 0));
			cur.x = cur.x;
			cur.y = cur.y + 40;
			drawercurser(hwnd, cur, RGB(255, 255, 255));
		}
		else if (GetAsyncKeyState(VK_LEFT) < 0 && (cur.x > 280))	//��
		{
			PlaySound(L"MUSIC\\cursormove.wav", NULL, SND_FILENAME | SND_ASYNC);
			drawercurser(hwnd, cur, RGB(0, 0, 0));
			cur.y = cur.y;
			cur.x = cur.x - 40;
			drawercurser(hwnd, cur, RGB(255, 255, 255));
		}
		else if (GetAsyncKeyState(VK_RIGHT) < 0 && (cur.x < 680))	//��
		{
			PlaySound(L"MUSIC\\cursormove.wav", NULL, SND_FILENAME | SND_ASYNC);
			drawercurser(hwnd, cur, RGB(0, 0, 0));
			cur.y = cur.y;
			cur.x = cur.x + 40;
			drawercurser(hwnd, cur, RGB(255, 255, 255));
			

		}
		else  if (GetAsyncKeyState(VK_RETURN) < 0)
		{
			//����ù��������Χλ�õ�ͬɫС�����꣬�������飬����¼������
			getsamecolorballs(hwnd, g_hdc, cur, GetPixel(g_hdc, cur.x, cur.y));
			//�������õ�ʱ��ʼ����
			if (index > 2)
			{
				PlaySound(L"MUSIC\\Win.wav", NULL, SND_FILENAME | SND_ASYNC);//����֮����ʾ����
				for (int k = 0; k < index; k++)
				{
					//ͬʱ����ͼƬ7���γ�������Ч��
					g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\ͼ��3\\7.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //����λͼ
					Game_Paint(hwnd, -(ballsarr[k].x - 18), -(ballsarr[k].y - 18));
				}
				windowsshake(hwnd);
				Sleep(500);
				//Sleep 500s ���䶯��
				ballsfall(hwnd, destDC);//С�����
				score += index;//�����ۼӲ����»���
				drawscore(score);
			}
			
			if(index<=2)
			{
				PlaySound(L"MUSIC\\Error.wav", NULL, SND_FILENAME | SND_ASYNC);
			}
			//С��������ʱ�������ʾ��
			index = 0;//һ��������ɣ���ͬɫС���������
		}
	}
	//��ʱ�䳬����Ϸʱ�䣬�л�����ָ��
	else if((t / CLOCKS_PER_SEC)>=gametime)
	{
		if (GetAsyncKeyState(VK_UP) < 0)
		{
			font = CreateFont(20, // nHeight 
			0, // nWidth 
			0, // nEscapement 
			0, // nOrientation 
			FW_BOLD, // nWeight 
			FALSE, // bItalic 
			FALSE, // bUnderline 
			0, // cStrikeOut 
			ANSI_CHARSET, // nCharSet 
			OUT_DEFAULT_PRECIS, // nOutPrecision 
			CLIP_DEFAULT_PRECIS, // nClipPrecision 
			DEFAULT_QUALITY, // nQuality 
			DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
			L"����"); // lpszFac
		SelectObject(g_hdc, font);
		SetTextColor(g_hdc, RGB(255, 255, 255));
		SetBkColor(g_hdc, RGB(0, 0, 0));
			PlaySound(L"MUSIC\\waring.wav", NULL, SND_FILENAME | SND_ASYNC);
			TextOut(g_hdc, 400, 400, L" ��Ϸ�Ѿ�������",8);
			TextOut(g_hdc, 400, 440, L"��ѡ���˳���Ϸ��", 8);
		}
		else if (GetAsyncKeyState(VK_DOWN) < 0)
		{
			font = CreateFont(20, // nHeight 
				0, // nWidth 
				0, // nEscapement 
				0, // nOrientation 
				FW_BOLD, // nWeight 
				FALSE, // bItalic 
				FALSE, // bUnderline 
				0, // cStrikeOut 
				ANSI_CHARSET, // nCharSet 
				OUT_DEFAULT_PRECIS, // nOutPrecision 
				CLIP_DEFAULT_PRECIS, // nClipPrecision 
				DEFAULT_QUALITY, // nQuality 
				DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
				L"����"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 255));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			PlaySound(L"MUSIC\\waring.wav", NULL, SND_FILENAME | SND_ASYNC);
			TextOut(g_hdc, 400, 400, L" ��Ϸ�Ѿ�������", 8);
			TextOut(g_hdc, 400, 440, L"��ѡ���˳���Ϸ��", 8);
		}
		else if(GetAsyncKeyState(VK_LEFT) < 0)
		{
			font = CreateFont(20, // nHeight 
				0, // nWidth 
				0, // nEscapement 
				0, // nOrientation 
				FW_BOLD, // nWeight 
				FALSE, // bItalic 
				FALSE, // bUnderline 
				0, // cStrikeOut 
				ANSI_CHARSET, // nCharSet 
				OUT_DEFAULT_PRECIS, // nOutPrecision 
				CLIP_DEFAULT_PRECIS, // nClipPrecision 
				DEFAULT_QUALITY, // nQuality 
				DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
				L"����"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 255));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			PlaySound(L"MUSIC\\Error.wav", NULL, SND_FILENAME | SND_ASYNC);
			TextOut(g_hdc, 400, 400, L" ��Ϸ�Ѿ�������", 8);
			TextOut(g_hdc, 400, 440, L"��ѡ���˳���Ϸ��", 8);
		}
		else if (GetAsyncKeyState(VK_RIGHT) < 0)
		{
			font = CreateFont(20, // nHeight 
				0, // nWidth 
				0, // nEscapement 
				0, // nOrientation 
				FW_BOLD, // nWeight 
				FALSE, // bItalic 
				FALSE, // bUnderline 
				0, // cStrikeOut 
				ANSI_CHARSET, // nCharSet 
				OUT_DEFAULT_PRECIS, // nOutPrecision 
				CLIP_DEFAULT_PRECIS, // nClipPrecision 
				DEFAULT_QUALITY, // nQuality 
				DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
				L"����"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 255));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			PlaySound(L"MUSIC\\waring.wav", NULL, SND_FILENAME | SND_ASYNC);
			TextOut(g_hdc, 400, 400, L" ��Ϸ�Ѿ�������", 8);
			TextOut(g_hdc, 400, 440, L"��ѡ���˳���Ϸ��", 8);

		}
		else if (GetAsyncKeyState(VK_RETURN) < 0)
		{
			font = CreateFont(20, // nHeight 
				0, // nWidth 
				0, // nEscapement 
				0, // nOrientation 
				FW_BOLD, // nWeight 
				FALSE, // bItalic 
				FALSE, // bUnderline 
				0, // cStrikeOut 
				ANSI_CHARSET, // nCharSet 
				OUT_DEFAULT_PRECIS, // nOutPrecision 
				CLIP_DEFAULT_PRECIS, // nClipPrecision 
				DEFAULT_QUALITY, // nQuality 
				DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
				L"����"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 255));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			PlaySound(L"MUSIC\\waring.wav", NULL, SND_FILENAME | SND_ASYNC);
			TextOut(g_hdc, 400, 400, L" ��Ϸ�Ѿ�������", 8);
			TextOut(g_hdc, 400, 440, L"��ѡ���˳���Ϸ��", 8);
		}
	}
	countdown(hwnd);//����ʱ����ʱ
}
int Game_CleanUp(HWND hwnd)
{
	//�ͷ���Դ����
	DeleteObject(g_hBitmap);
	DeleteDC(g_mdc);
	ReleaseDC(hwnd, g_hdc);  //�ͷ��豸����
	return 1;
}
int balls_occurs(HWND hwnd, int n)
{
	
	g_hdc = GetDC(hwnd);  //��ȡ�豸�����ľ�����ز����٣������޷�����ͼƬ
	//swich��䣬�����������					 
	switch (n)
	{
	case 1:
		g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\ͼ��3\\1.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //����λͼ
		g_mdc = CreateCompatibleDC(g_hdc);    //���������豸�������ڴ�DC
		break;
	case 2:
		g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\ͼ��3\\2.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //����λͼ
		g_mdc = CreateCompatibleDC(g_hdc);    //���������豸�������ڴ�DC
		break;
	case 3:
		g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\ͼ��3\\3.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //����λͼ
		g_mdc = CreateCompatibleDC(g_hdc);    //���������豸�������ڴ�DC
		break;
	case 4:
		g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\ͼ��3\\4.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //����λͼ
		g_mdc = CreateCompatibleDC(g_hdc);    //���������豸�������ڴ�DC
		break;
	case 5:
		g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\ͼ��3\\5.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //����λͼ
		g_mdc = CreateCompatibleDC(g_hdc);    //���������豸�������ڴ�DC
		break;
	case 6:
		g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\ͼ��3\\6.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //����λͼ
		g_mdc = CreateCompatibleDC(g_hdc);    //���������豸�������ڴ�DC
		break;

	}
	return 1;
}
void drawercurser(HWND hwnd, posType pos, COLORREF c1)
{
	hpen = (HPEN)GetStockObject(NULL_PEN);
	SelectObject(g_hdc, hpen);
	DeleteObject(hpen);
	hpen = CreatePen(PS_DASH, 2, c1);
	SelectObject(g_hdc, hpen);
	//���ĸ�������������
	MoveToEx(g_hdc, pos.x-20, pos.y-20,NULL);
	//sleep  30ms,��������Ч��
	LineTo(g_hdc, pos.x-20, pos.y-20);
	Sleep(30);
	LineTo(g_hdc, pos.x+20, pos.y-20);
	Sleep(30);
	LineTo(g_hdc, pos.x+20, pos.y+20);
	Sleep(30);
	LineTo(g_hdc, pos.x-20, pos.y+20);
	Sleep(30);
	LineTo(g_hdc, pos.x-20, pos.y-20);
	Sleep(30);
	DeleteObject(hpen);//ɾ�����ʾ��
}
void drawscore(int score)
{
	int size;
	TCHAR a[] = L"��ķ���Ϊ��";
	TCHAR b[] = L"  ��";
	TCHAR szText[256];
	font = CreateFont(22, // nHeight 
		0, // nWidth 
		0, // nEscapement 
		0, // nOrientation 
		FW_BOLD, // nWeight 
		FALSE, // bItalic 
		FALSE, // bUnderline 
		0, // cStrikeOut 
		ANSI_CHARSET, // nCharSet 
		OUT_DEFAULT_PRECIS, // nOutPrecision 
		CLIP_DEFAULT_PRECIS, // nClipPrecision 
		DEFAULT_QUALITY, // nQuality 
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
	L"����"); // lpszFac
	SelectObject(g_hdc, font);
	SetTextColor(g_hdc, RGB(255, 0, 0));
	SetBkColor(g_hdc, RGB(0, 0, 0));
	size = wsprintf(szText, TEXT("%s%d%s"), a, score, b);
	//����wsprintf()��һϵ�е��ַ�����ֵ���뵽�������������ķ���ֵ��д��ĳ���  
	TextOut(g_hdc, 30, 100, szText, size);
}
void drawtime(int sec)
{
	clock_t t = clock();
	//ǰ20���ɫ
	if (((t / CLOCKS_PER_SEC) >=0)&&((t / CLOCKS_PER_SEC) <= gametime-10))
	{
		int size;
		TCHAR a[] = L"ʣ��ʱ��Ϊ��";
		TCHAR b[] = L"s";
		TCHAR szText[256];
		font = CreateFont(22, // nHeight 
			0, // nWidth 
			0, // nEscapement 
			0, // nOrientation 
			FW_BOLD, // nWeight 
			FALSE, // bItalic 
			FALSE, // bUnderline 
			0, // cStrikeOut 
			ANSI_CHARSET, // nCharSet 
			OUT_DEFAULT_PRECIS, // nOutPrecision 
			CLIP_DEFAULT_PRECIS, // nClipPrecision 
			DEFAULT_QUALITY, // nQuality 
			DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
			L"����"); // lpszFac
		SelectObject(g_hdc, font);
		SetTextColor(g_hdc, RGB(255, 255, 0));
		SetBkColor(g_hdc, RGB(0, 0, 0));
		size = wsprintf(szText, TEXT("%s%3d%s"), a, sec, b);
		//1.���Ͽո񸲸�֮ǰ������
		//2.����wsprintf()��һϵ�е��ַ�����ֵ���뵽�������������ķ���ֵ��д��ĳ���
		TextOut(g_hdc, 30, 550, szText, size);
	}
	//���10s
	else if (((t / CLOCKS_PER_SEC) > gametime-10) && ((t / CLOCKS_PER_SEC) <= gametime))
	{
		int size;
		TCHAR a[] = L"ʣ��ʱ��Ϊ��";
		TCHAR b[] = L"s";
		TCHAR szText[256];
		font = CreateFont(22, // nHeight 
			0, // nWidth ��Ϊ0��ʱ���׼����
			0, // nEscapement 
			0, // nOrientation 
			FW_BOLD, // nWeight 
			FALSE, // bItalic 
			FALSE, // bUnderline 
			0, // cStrikeOut 
			ANSI_CHARSET, // nCharSet 
			OUT_DEFAULT_PRECIS, // nOutPrecision 
			CLIP_DEFAULT_PRECIS, // nClipPrecision 
			DEFAULT_QUALITY, // nQuality 
			DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
			L"����"); // lpszFac
		SelectObject(g_hdc, font);
		SetTextColor(g_hdc, RGB(255, 0, 0));
		SetBkColor(g_hdc, RGB(0, 0, 0));//������͸��ɫ����ΪҪˢ��ʱ��
		size = wsprintf(szText, TEXT("%s  %d%s"), a, sec, b);
		//1.���Ͽո񸲸�֮ǰ������
		//2.����wsprintf()��һϵ�е��ַ�����ֵ���뵽�������������ķ���ֵ��д��ĳ���
		Sleep(50);
		TextOut(g_hdc, 30, 550, szText, size);
	}
}
void getsamecolorballs(HWND hwnd, HDC g_hdc,posType cur,COLORREF c1)//�õ�ͬɫС��ĸ���
{
	ballsarr[index].x = cur.x;
	ballsarr[index].y = cur.y;
	index++;//ͳ��С�����p;l.p,l
	posType tympos;
	for (int k = 0; k < 4; k++)
	{
		//switch����жϵ�ǰС����������ҵ�С�����
		//��ѭ�����жϻ᷽��ܶ�
		switch (k)
		{
			case 0:
				tympos.x = cur.x;
				tympos.y = cur.y - 40;
				break;
			case 1:
				tympos.x = cur.x;
				tympos.y = cur.y + 40;
				break;
			case 2:
				tympos.x = cur.x - 40;
				tympos.y = cur.y;
				break;
			case 3:
				tympos.x = cur.x + 40;
				tympos.y = cur.y;
				break;
		}
		if (isvalued(hwnd, g_hdc,tympos, c1))
		{
			getsamecolorballs(hwnd,g_hdc,tympos, c1);
			//�ֱ���������ҵ��������ҷ�λ��С������ж�
		}
	}
}
int isvalued(HWND hwnd,HDC g_hdc,posType cur, COLORREF c1)
{
	
	if (GetPixel(g_hdc,cur.x, cur.y) != c1)//��ȡ���ص�RGB�Ƿ��봫���һ��
	{
		return 0;
	}
	else
	{
		for (int i = 0; i < index; i++)
		{
			if ((cur.x == ballsarr[i].x) && (cur.y == ballsarr[i].y))
			{
				return 0;
			}
		}
		return 1;
	}
}
void ballsfall(HWND hwnd, HDC g_hdc )//С�����
{
	g_hdc = GetDC(hwnd);  //��ȡ�豸������� 
		turn();
		for (int i = 0; i < index; i++)
		{
			for (int k = ballsarr[i].y; k > 50; k -= 40)
			{
				COLORREF c1=GetPixel(g_hdc,ballsarr[i].x, k - 40);
				if (c1== GetPixel(g_hdc,240+18, 628))
				{
					g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\ͼ��3\\1.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //����λͼ
					g_mdc = CreateCompatibleDC(g_hdc);    //���������豸�������ڴ�DC
					Game_Paint(hwnd, -ballsarr[i].x+18, -k+18);
				}
				else if (c1 == GetPixel(g_hdc, 300-2, 628))
				{
					g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\ͼ��3\\2.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //����λͼ
					g_mdc = CreateCompatibleDC(g_hdc);    //���������豸�������ڴ�DC
					Game_Paint(hwnd, -ballsarr[i].x + 18, -k + 18);
				}
				else if (c1 == GetPixel(g_hdc, 340 - 2, 628))
				{
					g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\ͼ��3\\3.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //����λͼ
					g_mdc = CreateCompatibleDC(g_hdc);    //���������豸�������ڴ�DC
					Game_Paint(hwnd, -ballsarr[i].x + 18, -k + 18);
				}
				else if (c1 == GetPixel(g_hdc,380 - 2, 628))
				{
					g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\ͼ��3\\4.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //����λͼ
					g_mdc = CreateCompatibleDC(g_hdc);    //���������豸�������ڴ�DC
					Game_Paint(hwnd, -ballsarr[i].x+ 18, -k+ 18);
				}
				else if (c1 == GetPixel(g_hdc, 420 - 2, 628))
				{
					g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\ͼ��3\\5.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //����λͼ
					g_mdc = CreateCompatibleDC(g_hdc);    //���������豸�������ڴ�DC
					Game_Paint(hwnd, -ballsarr[i].x+18, -k+18);
				}
				else if (c1 == GetPixel(g_hdc, 460 - 2, 628))
				{
					g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\ͼ��3\\6.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //����λͼ
					g_mdc = CreateCompatibleDC(g_hdc);    //���������豸�������ڴ�DC
					Game_Paint(hwnd, -ballsarr[i].x + 18, -k + 18);
				}
				
			}//С�����
			balls_occurs(hwnd, 1 + rand() % 5);
			Game_Paint(hwnd, -(ballsarr[i].x - 18), -50);//�����Ϸ�С��
		}
}
void turn()//��С����������ֹͬʱ��������λ��
{
	int i, j;
	posType temp;
	for (j = 0; j<index - 1; j++)
		for (i = 0; i<index - 1 - j; i++)
		{
			if (ballsarr[i].x>ballsarr[i + 1].x)
			{
				temp = ballsarr[i];
				ballsarr[i] = ballsarr[i + 1];
				ballsarr[i + 1] = temp;
			}
			if (ballsarr[i].y>ballsarr[i + 1].y)
			{
				temp = ballsarr[i];
				ballsarr[i] = ballsarr[i + 1];
				ballsarr[i + 1] = temp;
			}
		}
}
void countdown(HWND hwnd)
{
	clock_t t= clock();
	if ((t / CLOCKS_PER_SEC)<gametime)
	{
		
		drawtime(gametime - t / CLOCKS_PER_SEC);
		Sleep(100);
	}
	else if ((t / CLOCKS_PER_SEC)== gametime)
	{
		gameover(hwnd);//��Ϸ����ʱ�л�����Ϸ��������
	}
	
}
void gameover(HWND hwnd)
{
	Sleep(100);
	g_hdc = GetDC(hwnd);  //��ȡ�豸����
	g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC//����\\1.bmp", IMAGE_BITMAP, 1000, 700, LR_LOADFROMFILE);   //����λͼ
	g_mdc = CreateCompatibleDC(g_hdc);    //���������豸�������ڴ�DC
	Game_Paint(hwnd, 0, 0);
	font = CreateFont(150, // nHeight 
		0, // nWidth 
		0, // nEscapement 
		0, // nOrientation 
		FW_BOLD, // nWeight 
		FALSE, // bItalic 
		FALSE, // bUnderline 
		0, // cStrikeOut 
		ANSI_CHARSET, // nCharSet 
		OUT_DEFAULT_PRECIS, // nOutPrecision 
		CLIP_DEFAULT_PRECIS, // nClipPrecision 
		DEFAULT_QUALITY, // nQuality 
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
		L"����"); // lpszFac
	SelectObject(g_hdc, font);
	SetTextColor(g_hdc, RGB(255,0,0));
	SetBkColor(g_hdc, RGB(0, 0, 0));
	TextOut(g_hdc, 180, 150, L"game over", strlen("game over"));
	int size;
	TCHAR a[] = L"��ķ���Ϊ��";
	TCHAR b[] = L"  ��";
	TCHAR szText[256];
	font = CreateFont(50, // nHeight 
		0, // nWidth 
		0, // nEscapement 
		0, // nOrientation 
		FW_BOLD, // nWeight 
		FALSE, // bItalic 
		FALSE, // bUnderline 
		0, // cStrikeOut 
		ANSI_CHARSET, // nCharSet 
		OUT_DEFAULT_PRECIS, // nOutPrecision 
		CLIP_DEFAULT_PRECIS, // nClipPrecision 
		DEFAULT_QUALITY, // nQuality 
		DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
		L"����"); // lpszFac
	SelectObject(g_hdc, font);
	SetTextColor(g_hdc, RGB(255, 255,255));
	SetBkColor(g_hdc, RGB(0, 0, 0));
	size = wsprintf(szText, TEXT("%s%d%s"), a, score, b);
	TextOut(g_hdc, 300, 500, szText, size);
	if (gametime == 30)
	{
		if ((score >= 0) && (score <= 20))
		{
			font = CreateFont(35, // nHeight 
				0, // nWidth 
				0, // nEscapement 
				0, // nOrientation 
				FW_BOLD, // nWeight 
				FALSE, // bItalic 
				FALSE, // bUnderline 
				0, // cStrikeOut 
				ANSI_CHARSET, // nCharSet 
				OUT_DEFAULT_PRECIS, // nOutPrecision 
				CLIP_DEFAULT_PRECIS, // nClipPrecision 
				DEFAULT_QUALITY, // nQuality 
				DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
				L"����"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 0));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			TextOut(g_hdc, 285, 600, L"��ķ����д���߼�������Ŷ", 13);
		}
		else if ((score >= 21) && (score <= 50))
		{
			font = CreateFont(35, // nHeight 
				0, // nWidth 
				0, // nEscapement 
				0, // nOrientation 
				FW_BOLD, // nWeight 
				FALSE, // bItalic 
				FALSE, // bUnderline 
				0, // cStrikeOut 
				ANSI_CHARSET, // nCharSet 
				OUT_DEFAULT_PRECIS, // nOutPrecision 
				CLIP_DEFAULT_PRECIS, // nClipPrecision 
				DEFAULT_QUALITY, // nQuality 
				DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
				L"����"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 0));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			TextOut(g_hdc, 360, 600, L"�ܲ����ˣ���������", 9);
		}
		else if (score >= 51)
		{
			font = CreateFont(35, // nHeight 
				0, // nWidth 
				0, // nEscapement 
				0, // nOrientation 
				FW_BOLD, // nWeight 
				FALSE, // bItalic 
				FALSE, // bUnderline 
				0, // cStrikeOut 
				ANSI_CHARSET, // nCharSet 
				OUT_DEFAULT_PRECIS, // nOutPrecision 
				CLIP_DEFAULT_PRECIS, // nClipPrecision 
				DEFAULT_QUALITY, // nQuality 
				DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
				L"����"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 0));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			TextOut(g_hdc, 420, 600, L"����̫����", 5);
		}
	}
	else if((gametime>30)&& (gametime)<90)
	{
		if ((score >= 0) && (score <= 40))
		{
			font = CreateFont(35, // nHeight 
				0, // nWidth 
				0, // nEscapement 
				0, // nOrientation 
				FW_BOLD, // nWeight 
				FALSE, // bItalic 
				FALSE, // bUnderline 
				0, // cStrikeOut 
				ANSI_CHARSET, // nCharSet 
				OUT_DEFAULT_PRECIS, // nOutPrecision 
				CLIP_DEFAULT_PRECIS, // nClipPrecision 
				DEFAULT_QUALITY, // nQuality 
				DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
				L"����"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 0));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			TextOut(g_hdc, 285, 600, L"��ķ����д���߼�������Ŷ", 13);
		}
		else if ((score >= 41) && (score <= 100))
		{
			font = CreateFont(35, // nHeight 
				0, // nWidth 
				0, // nEscapement 
				0, // nOrientation 
				FW_BOLD, // nWeight 
				FALSE, // bItalic 
				FALSE, // bUnderline 
				0, // cStrikeOut 
				ANSI_CHARSET, // nCharSet 
				OUT_DEFAULT_PRECIS, // nOutPrecision 
				CLIP_DEFAULT_PRECIS, // nClipPrecision 
				DEFAULT_QUALITY, // nQuality 
				DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
				L"����"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 0));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			TextOut(g_hdc, 360, 600, L"�ܲ����ˣ���������", 9);
		}
		else if (score >= 101)
		{
			font = CreateFont(35, // nHeight 
				0, // nWidth 
				0, // nEscapement 
				0, // nOrientation 
				FW_BOLD, // nWeight 
				FALSE, // bItalic 
				FALSE, // bUnderline 
				0, // cStrikeOut 
				ANSI_CHARSET, // nCharSet 
				OUT_DEFAULT_PRECIS, // nOutPrecision 
				CLIP_DEFAULT_PRECIS, // nClipPrecision 
				DEFAULT_QUALITY, // nQuality 
				DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
				L"����"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 0));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			TextOut(g_hdc, 420, 600, L"����̫����", 5);
		}
	}
	else if ((gametime>90) && (gametime)<120)
	{
		if ((score >= 0) && (score <= 60))
		{
			font = CreateFont(35, // nHeight 
				0, // nWidth 
				0, // nEscapement 
				0, // nOrientation 
				FW_BOLD, // nWeight 
				FALSE, // bItalic 
				FALSE, // bUnderline 
				0, // cStrikeOut 
				ANSI_CHARSET, // nCharSet 
				OUT_DEFAULT_PRECIS, // nOutPrecision 
				CLIP_DEFAULT_PRECIS, // nClipPrecision 
				DEFAULT_QUALITY, // nQuality 
				DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
				L"����"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 0));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			TextOut(g_hdc, 285, 600, L"��ķ����д���߼�������Ŷ", 13);
		}
		else if ((score >= 61) && (score <= 150))
		{
			font = CreateFont(35, // nHeight 
				0, // nWidth 
				0, // nEscapement 
				0, // nOrientation 
				FW_BOLD, // nWeight 
				FALSE, // bItalic 
				FALSE, // bUnderline 
				0, // cStrikeOut 
				ANSI_CHARSET, // nCharSet 
				OUT_DEFAULT_PRECIS, // nOutPrecision 
				CLIP_DEFAULT_PRECIS, // nClipPrecision 
				DEFAULT_QUALITY, // nQuality 
				DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
				L"����"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 0));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			TextOut(g_hdc, 360, 600, L"�ܲ����ˣ���������", 9);
		}
		else if (score >= 151)
		{
			font = CreateFont(35, // nHeight 
				0, // nWidth 
				0, // nEscapement 
				0, // nOrientation 
				FW_BOLD, // nWeight 
				FALSE, // bItalic 
				FALSE, // bUnderline 
				0, // cStrikeOut 
				ANSI_CHARSET, // nCharSet 
				OUT_DEFAULT_PRECIS, // nOutPrecision 
				CLIP_DEFAULT_PRECIS, // nClipPrecision 
				DEFAULT_QUALITY, // nQuality 
				DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
				L"����"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 0));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			TextOut(g_hdc, 420, 600, L"����̫����", 5);
		}
	}
	else if (gametime>= 120)
	{
		if ((score >= 0) && (score <= 80))
		{
			font = CreateFont(35, // nHeight 
				0, // nWidth 
				0, // nEscapement 
				0, // nOrientation 
				FW_BOLD, // nWeight 
				FALSE, // bItalic 
				FALSE, // bUnderline 
				0, // cStrikeOut 
				ANSI_CHARSET, // nCharSet 
				OUT_DEFAULT_PRECIS, // nOutPrecision 
				CLIP_DEFAULT_PRECIS, // nClipPrecision 
				DEFAULT_QUALITY, // nQuality 
				DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
				L"����"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 0));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			TextOut(g_hdc, 285, 600, L"��ķ����д���߼�������Ŷ", 13);
		}
		else if ((score >= 81) && (score <= 200))
		{
			font = CreateFont(35, // nHeight 
				0, // nWidth 
				0, // nEscapement 
				0, // nOrientation 
				FW_BOLD, // nWeight 
				FALSE, // bItalic 
				FALSE, // bUnderline 
				0, // cStrikeOut 
				ANSI_CHARSET, // nCharSet 
				OUT_DEFAULT_PRECIS, // nOutPrecision 
				CLIP_DEFAULT_PRECIS, // nClipPrecision 
				DEFAULT_QUALITY, // nQuality 
				DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
				L"����"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 0));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			TextOut(g_hdc, 360, 600, L"�ܲ����ˣ���������", 9);
		}
		else if (score >= 201)
		{
			font = CreateFont(35, // nHeight 
				0, // nWidth 
				0, // nEscapement 
				0, // nOrientation 
				FW_BOLD, // nWeight 
				FALSE, // bItalic 
				FALSE, // bUnderline 
				0, // cStrikeOut 
				ANSI_CHARSET, // nCharSet 
				OUT_DEFAULT_PRECIS, // nOutPrecision 
				CLIP_DEFAULT_PRECIS, // nClipPrecision 
				DEFAULT_QUALITY, // nQuality 
				DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
				L"����"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 0));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			TextOut(g_hdc, 420, 600, L"����̫����", 5);
		}
	}
}
int gamebegin_interface(HWND hwnd)
{
	clock_t t = clock();
		g_hdc = GetDC(hwnd);  //��ȡ�豸����
		g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\����\\background.bmp", IMAGE_BITMAP, 1000, 730, LR_LOADFROMFILE);   //����λͼ
		g_mdc = CreateCompatibleDC(g_hdc);    //���������豸�������ڴ�DC
		Game_Paint(hwnd, 0, 0);
		
		font = CreateFont(50, // nHeight 
			0, // nWidth 
			0, // nEscapement 
			0, // nOrientation 
			FW_BOLD, // nWeight 
			FALSE, // bItalic 
			FALSE, // bUnderline 
			0, // cStrikeOut 
			ANSI_CHARSET, // nCharSet 
			OUT_DEFAULT_PRECIS, // nOutPrecision 
			CLIP_DEFAULT_PRECIS, // nClipPrecision 
			DEFAULT_QUALITY, // nQuality 
			DEFAULT_PITCH | FF_SWISS, // nPitchAndFamily 
			L"����"); // lpszFac
		SelectObject(g_hdc, font);
		SetTextColor(g_hdc, RGB(2, 255, 255));
		SetBkMode(g_hdc, TRANSPARENT);
		TextOut(g_hdc, 350, 150, L"��ӭ������Ϸ",6);
		TextOut(g_hdc, 260, 230, L"�밴ENTER��������Ϸ", 13);
		//DeleteObject(g_hdc);
		Sleep(100);
		if ((GetAsyncKeyState(VK_RETURN) < 0))
		{
			
			if (!Game_Init(hwnd))
			{
				MessageBox(hwnd, L"��Դ��ʼ��ʧ��", L"��Ϣ����", 0); //ʹ��MessageBox����������һ����Ϣ����
				return 0;
			}
			game_state = 1;
			//ת����Ϸ״̬���л�����ʼ����Ϸ
			gametime += t / CLOCKS_PER_SEC;//����Ϸδ��ʼʱ��ϵͳʱ�����
		}
	
	return 1;
}
void CreateMyMenu()
{
	hmenu = CreateMenu();
    if (!hmenu)
    return;
	HMENU pop1 = CreatePopupMenu();
	HMENU pop1_1 = CreatePopupMenu();
	HMENU pop2 = CreatePopupMenu();
	HMENU pop2_1 = CreatePopupMenu();
	HMENU pop2_2 = CreatePopupMenu();
	HMENU pop2_3 = CreatePopupMenu();
	HMENU pop2_4 = CreatePopupMenu();
	HMENU pop3 = CreatePopupMenu();
	AppendMenu(hmenu, MF_POPUP,(UINT_PTR)pop1,L"����");//��һ�����˵�
	AppendMenu(pop1,MF_STRING,IDM_OPT1,L"��ʼ");
	AppendMenu(pop1, MF_STRING, IDM_OPT2, L"ˢ��");
	AppendMenu(hmenu, MF_POPUP, (UINT_PTR)pop2, L"����");//�ڶ������˵�
	AppendMenu(pop2, MF_POPUP, (UINT_PTR)pop2_3, L"��Ϸʱ��");
	AppendMenu(pop2_3, MF_POPUP, IDM_OPT12, L"30s");
	AppendMenu(pop2_3, MF_POPUP, IDM_OPT13, L"60s");
	AppendMenu(pop2_3, MF_POPUP, IDM_OPT14, L"90s");
	AppendMenu(pop2_3, MF_POPUP, IDM_OPT23, L"120s");
	AppendMenu(pop2, MF_POPUP, (UINT_PTR)pop2_4, L"��Ϸ����");
	AppendMenu(pop2_4, MF_POPUP, IDM_OPT15, L"background1");
	AppendMenu(pop2_4, MF_POPUP, IDM_OPT16, L"background2");
	AppendMenu(pop2_4, MF_POPUP, IDM_OPT17, L"background3");
	AppendMenu(pop2_4, MF_POPUP, IDM_OPT18, L"background4");
	AppendMenu(hmenu, MF_POPUP, (UINT_PTR)pop3, L"����");//���������˵�
	AppendMenu(pop3, MF_STRING, IDM_OPT19, L"��ϵ����");
	AppendMenu(pop3, MF_STRING, IDM_OPT20, L"�����ĵ�");
	AppendMenu(pop3, MF_STRING, IDM_OPT21, L"�������");
	AppendMenu(pop3, MF_STRING, IDM_OPT22, L"�˳���Ϸ");
}
void game_progress(HWND hwnd,HDC g_hdc)
{
	switch (game_state)
	{
	case 0:
		gamebegin_interface(hwnd);
		break;
	case 1:
		Game_Play(hwnd, g_hdc);
		break;
	}
}
int windowsshake(HWND hwnd)
{
	int x = ((GetSystemMetrics(SM_CXSCREEN) / 2) - 500); //x����  
	int y = ((GetSystemMetrics(SM_CYSCREEN) / 2) - 365); //y���� 
	for (int i = 0; i < 20;i++)
	{
		int shake_x = -4 + rand() % 8;
		int shake_y = -4 + rand() % 8;
	MoveWindow(hwnd, x+ shake_x, y+ shake_y, WINDOW_WIDTH, WINDOW_HEIGHT, 1);
	Sleep(30);
    }
	return 1;
}