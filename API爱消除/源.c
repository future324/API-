#include"header.h"
#define WINDOW_TITLE L"爱消除"//游戏名字
#define WINDOW_WIDTH 1000//窗口的宽度
#define WINDOW_HEIGHT 730//窗口的高度
#define SIZE 36//小球的大小
typedef struct position//定义小球位置
{
	int x;
	int y;
} posType;
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int diaphaneity=255;//透明度
int game_state = 0;//记录游戏状态
int gametime = 30;//游戏总时间
int backgroundnum=3;//背景序号
int Game_Init(HWND hwnd);//游戏初始化
void Game_Paint(HWND hwnd, int x, int y);//自定义函数，拿到句柄病画出小球位置
void Game_Play(HWND hwnd, HDC g_hdc);//游戏开始玩
int Game_CleanUp(HWND hwnd);//清理
void drawercurser(HWND hwnd, posType pos,COLORREF c1);//画光标
void drawtime(int);//画剩余时间
void drawscore(int);//画所得分数
void ballsfall(HWND hwnd, HDC destDC);
void turn();//对小球先进行排序，不然会无规则掉落
int balls_occurs(HWND hwnd, int n);
void getsamecolorballs(HWND hwnd, HDC g_hdc,posType cur, COLORREF c1);//得到同色小球
int isvalued(HWND hwnd,HDC g_hdc,posType tympos, COLORREF c1);//确定周围的小球是不是同色小球
void countdown(HWND hwnd);//倒计时
void gameover(HWND hwnd);//游戏结束
void CreateMyMenu();//创建菜单  
void game_progress(HWND hwnd,HDC g_hdc);//游戏全部进程
int gamebegin_interface(HWND hwnd);//游戏开始界面
int windowsshake(HWND hwnd);
HPEN hpen;//钢笔句柄
HFONT font;//字体句柄
HBRUSH hbrush;//画刷句柄
HMENU hmenu;//菜单句柄
HDC g_hdc = NULL;
HDC g_mdc = NULL;
HBITMAP		g_hBitmap = NULL;  //定义一个位图句柄
HINSTANCE hInstance;
posType cur;//当前光标位置
posType ballsarr[180]; //同色小球坐标，定义180个小球防止出现颜色全部一样的情况
int index;  //同色小球的个数
int score = 0;
int result;//消息框是或否
clock_t t;//定义时间，即程序已经运行的时间
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	CreateMyMenu();//创建菜单  
	srand((unsigned)time(NULL));//定义时间种子，产生随机数
	static TCHAR szAppName[] = TEXT("WindowsAPI");
	HWND         hwnd;
	MSG          msg = { 0 };//消息结构体
	WNDCLASSEX   wndclassex = { 0 };
	wndclassex.cbSize = sizeof(WNDCLASSEX);
	wndclassex.style = CS_HREDRAW | CS_VREDRAW;
	wndclassex.lpfnWndProc = WndProc;
	wndclassex.cbClsExtra = 0;
	wndclassex.cbWndExtra = 0;
	wndclassex.hInstance = hInstance;
	wndclassex.hIcon = (HICON)LoadImage(NULL, L"icon1.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	//wndclassex.hIcon = LoadIcon(NULL, IDI_APPLICATION);默认图标
	wndclassex.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wndclassex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);//背景色
	wndclassex.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
	wndclassex.lpszMenuName = NULL;
	wndclassex.lpszClassName = szAppName;
	wndclassex.hIconSm = wndclassex.hIcon;
	//检测是否注册窗口失败
	if (!RegisterClassEx(&wndclassex))
	{
		MessageBox(NULL, TEXT("RegisterClassEx failed!"), szAppName, MB_ICONERROR);
		return 0;
	}
	int x = ((GetSystemMetrics(SM_CXSCREEN) / 2) - 500); //x居中  
	int y = ((GetSystemMetrics(SM_CYSCREEN) / 2) - 365); //y居中 
	hwnd = CreateWindow
		(szAppName,
		WINDOW_TITLE,
		WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
		x,//
		y, //
		WINDOW_WIDTH,//窗口宽度
		WINDOW_HEIGHT,//窗口高度
		NULL,
		hmenu,//菜单
		hInstance, //实例句柄
		NULL);
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	while ((msg.message != WM_QUIT))//鼠标的消息		//使用while循环，如果消息不是WM_QUIT消息，就继续循环
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))   //查看应用程序消息队列，有消息时将队列中的消息派发出去。
		{
			TranslateMessage(&msg);		//将虚拟键消息转换为字符消息
			DispatchMessage(&msg);			//分发一个消息给窗口程序。
		}
		else
		{
			game_progress(hwnd,g_hdc);//游戏的进程，包括游戏开始界面以及开始玩游戏以及结束界面
		}
	}
	Game_CleanUp(hwnd);
	return msg.wParam;//返回消息
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	clock_t t=clock();
	HDC destDC=NULL;
	PAINTSTRUCT paintStruct;	//定义一个PAINTSTRUCT结构体来记录一些绘制信息
	static HWND     hwndButton[5];   //按钮句柄
	switch (message)
	{
		
	  case WM_CREATE:
	  {
		SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
		SetLayeredWindowAttributes(hwnd, RGB(255, 255, 255), 200, LWA_ALPHA);

		//当游戏在游戏时间内执行操作
		if (((t / CLOCKS_PER_SEC) >= 0) && ((t / CLOCKS_PER_SEC) < gametime))
		{
			hwndButton[0] = CreateWindow(L"BUTTON", L"上",
				WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				787, 262, 70, 27, hwnd, NULL,
				(HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE), NULL);
			hwndButton[1] = CreateWindow(L"BUTTON", L"下",
				WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				787, 350, 70, 27, hwnd, NULL,
				(HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE), NULL);
			hwndButton[2] = CreateWindow(L"BUTTON", L"左",
				WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
				762, 285, 27, 70, hwnd, NULL,
				(HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE), NULL);
			hwndButton[3] = CreateWindow(L"BUTTON", L"右",
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
			hwndButton[0] = CreateWindow(L"BUTTON", L"上",
				 WS_CHILD | BS_DEFPUSHBUTTON,
				787, 262, 70, 27, hwnd, NULL,
				(HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE), NULL);
			//按钮2
			hwndButton[1] = CreateWindow(L"BUTTON", L"下",
				WS_TABSTOP | WS_CHILD | BS_DEFPUSHBUTTON,
				787, 350, 70, 27, hwnd, NULL,
				(HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE), NULL);
			hwndButton[2] = CreateWindow(L"BUTTON", L"左",
				WS_CHILD | BS_DEFPUSHBUTTON,
				762, 285, 27, 70, hwnd, NULL,
				(HINSTANCE)GetWindowLong(hwnd, GWLP_HINSTANCE), NULL);
			hwndButton[3] = CreateWindow(L"BUTTON", L"右",
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
    //当游戏在游戏时间内执行操作
		if (((t / CLOCKS_PER_SEC) >= 0) && ((t / CLOCKS_PER_SEC) < gametime))
		{
			if ((HWND)lParam == hwndButton[0] && (cur.y > 70))//上
			{
				g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC//方向杆.bmp", IMAGE_BITMAP, 140, 140, LR_LOADFROMFILE);   //加载
				g_mdc = CreateCompatibleDC(g_hdc);
				Game_Paint(hwnd, -750, -250);
				PlaySound(L"MUSIC\\cursormove.wav", NULL, SND_FILENAME | SND_ASYNC);
				drawercurser(hwnd, cur, RGB(0, 0, 0));
				cur.x = cur.x;
				cur.y = cur.y - 40;
				drawercurser(hwnd, cur, RGB(255, 255, 255));
			}
			if ((HWND)lParam == hwndButton[1] && (cur.y < 610))//下
			{
				g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC//方向杆.bmp", IMAGE_BITMAP, 140, 140, LR_LOADFROMFILE);   //加载
				g_mdc = CreateCompatibleDC(g_hdc);
				Game_Paint(hwnd, -750, -250);
				PlaySound(L"MUSIC\\cursormove.wav", NULL, SND_FILENAME | SND_ASYNC);
				drawercurser(hwnd, cur, RGB(0, 0, 0));
				cur.x = cur.x;
				cur.y = cur.y + 40;
				drawercurser(hwnd, cur, RGB(255, 255, 255));
			}
			if ((HWND)lParam == hwndButton[2] && (cur.x > 280))//左
			{
				g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC//方向杆.bmp", IMAGE_BITMAP, 140, 140, LR_LOADFROMFILE);   //加载
				g_mdc = CreateCompatibleDC(g_hdc);
				Game_Paint(hwnd, -750, -250);
				PlaySound(L"MUSIC\\cursormove.wav", NULL, SND_FILENAME | SND_ASYNC);
				drawercurser(hwnd, cur, RGB(0, 0, 0));
				cur.y = cur.y;
				cur.x = cur.x - 40;
				drawercurser(hwnd, cur, RGB(255, 255, 255));

			}
			if ((HWND)lParam == hwndButton[3] && (cur.x < 680))//右
			{
				g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC//方向杆.bmp", IMAGE_BITMAP, 140, 140, LR_LOADFROMFILE);   //加载
				g_mdc = CreateCompatibleDC(g_hdc);
				Game_Paint(hwnd, -750, -250);
				PlaySound(L"MUSIC\\cursormove.wav", NULL, SND_FILENAME | SND_ASYNC);
				drawercurser(hwnd, cur, RGB(0, 0, 0));
				cur.y = cur.y;
				cur.x = cur.x + 40;
				drawercurser(hwnd, cur, RGB(255, 255, 255));
			}
			if ((HWND)lParam == hwndButton[4])//确定
			{
				g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC//方向杆.bmp", IMAGE_BITMAP, 140, 140, LR_LOADFROMFILE);   //加载
				g_mdc = CreateCompatibleDC(g_hdc);
				Game_Paint(hwnd, -750, -250);
				//将获得光标所在周围位置的同色小球坐标，存入数组，并记录个数；
				getsamecolorballs(hwnd, g_hdc, cur, GetPixel(g_hdc, cur.x, cur.y));
				//大于两得的时候开始消除
				if (index > 2)
				{
					PlaySound(L"MUSIC\\Win.wav", NULL, SND_FILENAME | SND_ASYNC);//消除之后提示音乐
					
					for (int k = 0; k < index; k++)
					{
						//同时插入图片7，形成消除的效果
						g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC//图标3//7.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //加载位图
						Game_Paint(hwnd, -(ballsarr[k].x - 18), -(ballsarr[k].y - 18));
					}
					windowsshake(hwnd);

					Sleep(500);
					//Sleep 500s 掉落动画
					ballsfall(hwnd, destDC);//小球掉落
					score += index;//分数累加并重新绘制
					drawscore(score);
				}

				if (index <= 2)
				{
					PlaySound(L"MUSIC\\Error.wav", NULL, SND_FILENAME | SND_ASYNC);
				}
				//小于两个的时候错误提示音
				index = 0;//一次消除完成，将同色小球个数置零
		    }
		}
		 switch (LOWORD(wParam))
		 {
			 case IDM_OPT1:

			 {
				 if (!Game_Init(hwnd))
				 {
					 MessageBox(hwnd, L"资源初始化失败", L"消息窗口", 0); //使用MessageBox函数，创建一个消息窗口
					 return 0;
				 }
				 game_state = 1;
				 //转换游戏状态，切换至开始玩游戏
				 gametime += t / CLOCKS_PER_SEC;
				 //把游戏未开始时的系统时间加上
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
				 ShellExecute(NULL, L"open",L"文档\\联系作者.html", NULL, NULL, SW_SHOWNORMAL);
				 break;
			 case IDM_OPT20:
				 ShellExecute(NULL, L"open", L"文档\\帮助文档.txt", NULL, NULL, SW_SHOWNORMAL);
				break;
			 case IDM_OPT21:
				 ShellExecute(NULL, L"open", L"http://pan.baidu.com/s/1slIQ8Lf", NULL, NULL, SW_SHOWNORMAL);
			
			 case IDM_OPT22:
				 result = MessageBox(NULL, L"您确定要退出游戏吗？", L"提示", MB_YESNO | MB_ICONQUESTION);
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
		g_hdc = BeginPaint(hwnd, &paintStruct);  //指定窗口进行绘图工作的准备，并用将和绘图有关的信息填充到paintStruct结构体中。
		EndPaint(hwnd, &paintStruct);			//EndPaint函数标记指定窗口的绘画过程结束
		ValidateRect(hwnd, NULL);		// 更新客户区的显示
		break;		//跳出该switch语句
	case WM_CLOSE:
		if (IDYES == MessageBox(hwnd, L"您是否要退出游戏", L"提示！！！", MB_YESNO | MB_ICONQUESTION))
		{
			DestroyWindow(hwnd);
		}
		else
		{
			//
		}
		return 0;//不能break，因为要把消息传到下面的语句里
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
}
	return DefWindowProc(hwnd, message, wParam, lParam);
		
}
//生成游戏界面
int Game_Init(HWND hwnd)//界面初始化
{
	g_hdc = GetDC(hwnd);  //获取设备环境
	//获取背景序号，以便更换背景
	switch (backgroundnum)
	{
	case 1:
		g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\背景\\2.bmp", IMAGE_BITMAP, 1000, 700, LR_LOADFROMFILE);   //加载
		g_mdc = CreateCompatibleDC(g_hdc);
		Game_Paint(hwnd, 2, 0);
		break;
	case 2:
		g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\背景\\3.bmp", IMAGE_BITMAP, 1030, 730, LR_LOADFROMFILE);   //加载
		g_mdc = CreateCompatibleDC(g_hdc);
		Game_Paint(hwnd, 30, 17);
		break;
	case 3:
		g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\背景\\4.bmp", IMAGE_BITMAP, 1030, 730, LR_LOADFROMFILE);   //加载
		g_mdc = CreateCompatibleDC(g_hdc);
		Game_Paint(hwnd, 0, 0);
		break;
	case 4:
		g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\背景\\5.bmp", IMAGE_BITMAP, 1000, 730, LR_LOADFROMFILE);   //加载
		g_mdc = CreateCompatibleDC(g_hdc);
		Game_Paint(hwnd, 0, 0);
		break;

	}
	//前面的随机载入图片
	for (int x = 240; x < 720; x += 40)
	{
		for (int y = 50; y < 610; y += 40)
		{
			int num = 1 + 1 + rand() % 5;
			balls_occurs(hwnd, num);
			Game_Paint(hwnd, -x, -y);
		}
	}
	//最后一行指定生成
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
	g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\方向杆.bmp", IMAGE_BITMAP, 140, 140, LR_LOADFROMFILE);   //加载
	g_mdc = CreateCompatibleDC(g_hdc);
	Game_Paint(hwnd, -750, -250);
	//当前的光标位置
	cur.x = 498;
	cur.y = 348;
	drawercurser(hwnd,cur,RGB(255,255,255));
	//画光标；
	drawtime(gametime);
	//绘制时间；
	drawscore(0);
	//绘制分数；
	return 1;
}
//指定位置贴图
void Game_Paint(HWND hwnd,int x,int y)//自定义函数能同时确定图片位置
{
	SelectObject(g_mdc, g_hBitmap);    
	BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_mdc, x, y, SRCCOPY);    //采用BitBlt函数贴图，参数设置为窗口大小
}
//游戏开始玩
void Game_Play(HWND hwnd, HDC destDC)
{
	clock_t t = clock();//定义时间
	//当游戏在游戏时间内执行操作
	if (((t / CLOCKS_PER_SEC) >= 0) && ((t / CLOCKS_PER_SEC) < gametime))
	{
		if ((GetAsyncKeyState(VK_UP) < 0) && (cur.y > 70))	//上
		{
			PlaySound(L"MUSIC\\cursormove.wav", NULL, SND_FILENAME | SND_ASYNC);
			drawercurser(hwnd, cur, RGB(0, 0, 0));
			cur.x = cur.x;
			cur.y = cur.y - 40;
			drawercurser(hwnd, cur, RGB(255, 255, 255));
			
		}
		else if (GetAsyncKeyState(VK_DOWN) < 0 && (cur.y < 600))	//下
		{
			PlaySound(L"MUSIC\\cursormove.wav", NULL, SND_FILENAME | SND_ASYNC);
			drawercurser(hwnd, cur, RGB(0, 0, 0));
			cur.x = cur.x;
			cur.y = cur.y + 40;
			drawercurser(hwnd, cur, RGB(255, 255, 255));
		}
		else if (GetAsyncKeyState(VK_LEFT) < 0 && (cur.x > 280))	//左
		{
			PlaySound(L"MUSIC\\cursormove.wav", NULL, SND_FILENAME | SND_ASYNC);
			drawercurser(hwnd, cur, RGB(0, 0, 0));
			cur.y = cur.y;
			cur.x = cur.x - 40;
			drawercurser(hwnd, cur, RGB(255, 255, 255));
		}
		else if (GetAsyncKeyState(VK_RIGHT) < 0 && (cur.x < 680))	//右
		{
			PlaySound(L"MUSIC\\cursormove.wav", NULL, SND_FILENAME | SND_ASYNC);
			drawercurser(hwnd, cur, RGB(0, 0, 0));
			cur.y = cur.y;
			cur.x = cur.x + 40;
			drawercurser(hwnd, cur, RGB(255, 255, 255));
			

		}
		else  if (GetAsyncKeyState(VK_RETURN) < 0)
		{
			//将获得光标所在周围位置的同色小球坐标，存入数组，并记录个数；
			getsamecolorballs(hwnd, g_hdc, cur, GetPixel(g_hdc, cur.x, cur.y));
			//大于两得的时候开始消除
			if (index > 2)
			{
				PlaySound(L"MUSIC\\Win.wav", NULL, SND_FILENAME | SND_ASYNC);//消除之后提示音乐
				for (int k = 0; k < index; k++)
				{
					//同时插入图片7，形成消除的效果
					g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\图标3\\7.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //加载位图
					Game_Paint(hwnd, -(ballsarr[k].x - 18), -(ballsarr[k].y - 18));
				}
				windowsshake(hwnd);
				Sleep(500);
				//Sleep 500s 掉落动画
				ballsfall(hwnd, destDC);//小球掉落
				score += index;//分数累加并重新绘制
				drawscore(score);
			}
			
			if(index<=2)
			{
				PlaySound(L"MUSIC\\Error.wav", NULL, SND_FILENAME | SND_ASYNC);
			}
			//小于两个的时候错误提示音
			index = 0;//一次消除完成，将同色小球个数置零
		}
	}
	//当时间超过游戏时间，切换键盘指令
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
			L"黑体"); // lpszFac
		SelectObject(g_hdc, font);
		SetTextColor(g_hdc, RGB(255, 255, 255));
		SetBkColor(g_hdc, RGB(0, 0, 0));
			PlaySound(L"MUSIC\\waring.wav", NULL, SND_FILENAME | SND_ASYNC);
			TextOut(g_hdc, 400, 400, L" 游戏已经结束！",8);
			TextOut(g_hdc, 400, 440, L"请选择退出游戏！", 8);
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
				L"黑体"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 255));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			PlaySound(L"MUSIC\\waring.wav", NULL, SND_FILENAME | SND_ASYNC);
			TextOut(g_hdc, 400, 400, L" 游戏已经结束！", 8);
			TextOut(g_hdc, 400, 440, L"请选择退出游戏！", 8);
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
				L"黑体"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 255));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			PlaySound(L"MUSIC\\Error.wav", NULL, SND_FILENAME | SND_ASYNC);
			TextOut(g_hdc, 400, 400, L" 游戏已经结束！", 8);
			TextOut(g_hdc, 400, 440, L"请选择退出游戏！", 8);
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
				L"黑体"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 255));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			PlaySound(L"MUSIC\\waring.wav", NULL, SND_FILENAME | SND_ASYNC);
			TextOut(g_hdc, 400, 400, L" 游戏已经结束！", 8);
			TextOut(g_hdc, 400, 440, L"请选择退出游戏！", 8);

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
				L"黑体"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 255));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			PlaySound(L"MUSIC\\waring.wav", NULL, SND_FILENAME | SND_ASYNC);
			TextOut(g_hdc, 400, 400, L" 游戏已经结束！", 8);
			TextOut(g_hdc, 400, 440, L"请选择退出游戏！", 8);
		}
	}
	countdown(hwnd);//倒计时倒计时
}
int Game_CleanUp(HWND hwnd)
{
	//释放资源对象
	DeleteObject(g_hBitmap);
	DeleteDC(g_mdc);
	ReleaseDC(hwnd, g_hdc);  //释放设备环境
	return 1;
}
int balls_occurs(HWND hwnd, int n)
{
	
	g_hdc = GetDC(hwnd);  //获取设备环境的句柄，必不可少，否则无法载入图片
	//swich语句，进行随机插入					 
	switch (n)
	{
	case 1:
		g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\图标3\\1.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //加载位图
		g_mdc = CreateCompatibleDC(g_hdc);    //建立兼容设备环境的内存DC
		break;
	case 2:
		g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\图标3\\2.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //加载位图
		g_mdc = CreateCompatibleDC(g_hdc);    //建立兼容设备环境的内存DC
		break;
	case 3:
		g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\图标3\\3.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //加载位图
		g_mdc = CreateCompatibleDC(g_hdc);    //建立兼容设备环境的内存DC
		break;
	case 4:
		g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\图标3\\4.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //加载位图
		g_mdc = CreateCompatibleDC(g_hdc);    //建立兼容设备环境的内存DC
		break;
	case 5:
		g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\图标3\\5.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //加载位图
		g_mdc = CreateCompatibleDC(g_hdc);    //建立兼容设备环境的内存DC
		break;
	case 6:
		g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\图标3\\6.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //加载位图
		g_mdc = CreateCompatibleDC(g_hdc);    //建立兼容设备环境的内存DC
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
	//画四个线条构建矩形
	MoveToEx(g_hdc, pos.x-20, pos.y-20,NULL);
	//sleep  30ms,做出动画效果
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
	DeleteObject(hpen);//删除画笔句柄
}
void drawscore(int score)
{
	int size;
	TCHAR a[] = L"你的分数为：";
	TCHAR b[] = L"  分";
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
	L"黑体"); // lpszFac
	SelectObject(g_hdc, font);
	SetTextColor(g_hdc, RGB(255, 0, 0));
	SetBkColor(g_hdc, RGB(0, 0, 0));
	size = wsprintf(szText, TEXT("%s%d%s"), a, score, b);
	//函数wsprintf()将一系列的字符和数值输入到缓冲区，函数的返回值是写入的长度  
	TextOut(g_hdc, 30, 100, szText, size);
}
void drawtime(int sec)
{
	clock_t t = clock();
	//前20秒黄色
	if (((t / CLOCKS_PER_SEC) >=0)&&((t / CLOCKS_PER_SEC) <= gametime-10))
	{
		int size;
		TCHAR a[] = L"剩余时间为：";
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
			L"黑体"); // lpszFac
		SelectObject(g_hdc, font);
		SetTextColor(g_hdc, RGB(255, 255, 0));
		SetBkColor(g_hdc, RGB(0, 0, 0));
		size = wsprintf(szText, TEXT("%s%3d%s"), a, sec, b);
		//1.加上空格覆盖之前的文字
		//2.函数wsprintf()将一系列的字符和数值输入到缓冲区，函数的返回值是写入的长度
		TextOut(g_hdc, 30, 550, szText, size);
	}
	//最后10s
	else if (((t / CLOCKS_PER_SEC) > gametime-10) && ((t / CLOCKS_PER_SEC) <= gametime))
	{
		int size;
		TCHAR a[] = L"剩余时间为：";
		TCHAR b[] = L"s";
		TCHAR szText[256];
		font = CreateFont(22, // nHeight 
			0, // nWidth ，为0的时候标准字体
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
			L"黑体"); // lpszFac
		SelectObject(g_hdc, font);
		SetTextColor(g_hdc, RGB(255, 0, 0));
		SetBkColor(g_hdc, RGB(0, 0, 0));//不能用透明色，因为要刷新时间
		size = wsprintf(szText, TEXT("%s  %d%s"), a, sec, b);
		//1.加上空格覆盖之前的文字
		//2.函数wsprintf()将一系列的字符和数值输入到缓冲区，函数的返回值是写入的长度
		Sleep(50);
		TextOut(g_hdc, 30, 550, szText, size);
	}
}
void getsamecolorballs(HWND hwnd, HDC g_hdc,posType cur,COLORREF c1)//得到同色小球的个数
{
	ballsarr[index].x = cur.x;
	ballsarr[index].y = cur.y;
	index++;//统计小球个数p;l.p,l
	posType tympos;
	for (int k = 0; k < 4; k++)
	{
		//switch语句判断当前小球的上下左右的小球情况
		//用循环来判断会方便很多
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
			//分别对上下左右的上下左右方位的小球进行判断
		}
	}
}
int isvalued(HWND hwnd,HDC g_hdc,posType cur, COLORREF c1)
{
	
	if (GetPixel(g_hdc,cur.x, cur.y) != c1)//获取像素点RGB是否与传入的一样
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
void ballsfall(HWND hwnd, HDC g_hdc )//小球掉落
{
	g_hdc = GetDC(hwnd);  //获取设备环境句柄 
		turn();
		for (int i = 0; i < index; i++)
		{
			for (int k = ballsarr[i].y; k > 50; k -= 40)
			{
				COLORREF c1=GetPixel(g_hdc,ballsarr[i].x, k - 40);
				if (c1== GetPixel(g_hdc,240+18, 628))
				{
					g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\图标3\\1.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //加载位图
					g_mdc = CreateCompatibleDC(g_hdc);    //建立兼容设备环境的内存DC
					Game_Paint(hwnd, -ballsarr[i].x+18, -k+18);
				}
				else if (c1 == GetPixel(g_hdc, 300-2, 628))
				{
					g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\图标3\\2.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //加载位图
					g_mdc = CreateCompatibleDC(g_hdc);    //建立兼容设备环境的内存DC
					Game_Paint(hwnd, -ballsarr[i].x + 18, -k + 18);
				}
				else if (c1 == GetPixel(g_hdc, 340 - 2, 628))
				{
					g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\图标3\\3.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //加载位图
					g_mdc = CreateCompatibleDC(g_hdc);    //建立兼容设备环境的内存DC
					Game_Paint(hwnd, -ballsarr[i].x + 18, -k + 18);
				}
				else if (c1 == GetPixel(g_hdc,380 - 2, 628))
				{
					g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\图标3\\4.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //加载位图
					g_mdc = CreateCompatibleDC(g_hdc);    //建立兼容设备环境的内存DC
					Game_Paint(hwnd, -ballsarr[i].x+ 18, -k+ 18);
				}
				else if (c1 == GetPixel(g_hdc, 420 - 2, 628))
				{
					g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\图标3\\5.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //加载位图
					g_mdc = CreateCompatibleDC(g_hdc);    //建立兼容设备环境的内存DC
					Game_Paint(hwnd, -ballsarr[i].x+18, -k+18);
				}
				else if (c1 == GetPixel(g_hdc, 460 - 2, 628))
				{
					g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\图标3\\6.bmp", IMAGE_BITMAP, SIZE, SIZE, LR_LOADFROMFILE);   //加载位图
					g_mdc = CreateCompatibleDC(g_hdc);    //建立兼容设备环境的内存DC
					Game_Paint(hwnd, -ballsarr[i].x + 18, -k + 18);
				}
				
			}//小球掉落
			balls_occurs(hwnd, 1 + rand() % 5);
			Game_Paint(hwnd, -(ballsarr[i].x - 18), -50);//绘制上方小球
		}
}
void turn()//对小球进行排序防止同时出现乱了位置
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
		gameover(hwnd);//游戏结束时切换至游戏结束界面
	}
	
}
void gameover(HWND hwnd)
{
	Sleep(100);
	g_hdc = GetDC(hwnd);  //获取设备环境
	g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC//背景\\1.bmp", IMAGE_BITMAP, 1000, 700, LR_LOADFROMFILE);   //加载位图
	g_mdc = CreateCompatibleDC(g_hdc);    //建立兼容设备环境的内存DC
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
		L"黑体"); // lpszFac
	SelectObject(g_hdc, font);
	SetTextColor(g_hdc, RGB(255,0,0));
	SetBkColor(g_hdc, RGB(0, 0, 0));
	TextOut(g_hdc, 180, 150, L"game over", strlen("game over"));
	int size;
	TCHAR a[] = L"你的分数为：";
	TCHAR b[] = L"  分";
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
		L"黑体"); // lpszFac
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
				L"黑体"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 0));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			TextOut(g_hdc, 285, 600, L"你的分数有待提高继续加油哦", 13);
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
				L"黑体"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 0));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			TextOut(g_hdc, 360, 600, L"很不错了，继续加油", 9);
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
				L"黑体"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 0));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			TextOut(g_hdc, 420, 600, L"真是太棒了", 5);
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
				L"黑体"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 0));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			TextOut(g_hdc, 285, 600, L"你的分数有待提高继续加油哦", 13);
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
				L"黑体"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 0));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			TextOut(g_hdc, 360, 600, L"很不错了，继续加油", 9);
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
				L"黑体"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 0));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			TextOut(g_hdc, 420, 600, L"真是太棒了", 5);
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
				L"黑体"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 0));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			TextOut(g_hdc, 285, 600, L"你的分数有待提高继续加油哦", 13);
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
				L"黑体"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 0));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			TextOut(g_hdc, 360, 600, L"很不错了，继续加油", 9);
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
				L"黑体"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 0));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			TextOut(g_hdc, 420, 600, L"真是太棒了", 5);
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
				L"黑体"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 0));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			TextOut(g_hdc, 285, 600, L"你的分数有待提高继续加油哦", 13);
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
				L"黑体"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 0));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			TextOut(g_hdc, 360, 600, L"很不错了，继续加油", 9);
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
				L"黑体"); // lpszFac
			SelectObject(g_hdc, font);
			SetTextColor(g_hdc, RGB(255, 255, 0));
			SetBkColor(g_hdc, RGB(0, 0, 0));
			TextOut(g_hdc, 420, 600, L"真是太棒了", 5);
		}
	}
}
int gamebegin_interface(HWND hwnd)
{
	clock_t t = clock();
		g_hdc = GetDC(hwnd);  //获取设备环境
		g_hBitmap = (HBITMAP)LoadImage(NULL, L"PIC\\背景\\background.bmp", IMAGE_BITMAP, 1000, 730, LR_LOADFROMFILE);   //加载位图
		g_mdc = CreateCompatibleDC(g_hdc);    //建立兼容设备环境的内存DC
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
			L"黑体"); // lpszFac
		SelectObject(g_hdc, font);
		SetTextColor(g_hdc, RGB(2, 255, 255));
		SetBkMode(g_hdc, TRANSPARENT);
		TextOut(g_hdc, 350, 150, L"欢迎进入游戏",6);
		TextOut(g_hdc, 260, 230, L"请按ENTER键进入游戏", 13);
		//DeleteObject(g_hdc);
		Sleep(100);
		if ((GetAsyncKeyState(VK_RETURN) < 0))
		{
			
			if (!Game_Init(hwnd))
			{
				MessageBox(hwnd, L"资源初始化失败", L"消息窗口", 0); //使用MessageBox函数，创建一个消息窗口
				return 0;
			}
			game_state = 1;
			//转换游戏状态，切换至开始玩游戏
			gametime += t / CLOCKS_PER_SEC;//把游戏未开始时的系统时间加上
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
	AppendMenu(hmenu, MF_POPUP,(UINT_PTR)pop1,L"功能");//第一个主菜单
	AppendMenu(pop1,MF_STRING,IDM_OPT1,L"开始");
	AppendMenu(pop1, MF_STRING, IDM_OPT2, L"刷新");
	AppendMenu(hmenu, MF_POPUP, (UINT_PTR)pop2, L"设置");//第二个主菜单
	AppendMenu(pop2, MF_POPUP, (UINT_PTR)pop2_3, L"游戏时间");
	AppendMenu(pop2_3, MF_POPUP, IDM_OPT12, L"30s");
	AppendMenu(pop2_3, MF_POPUP, IDM_OPT13, L"60s");
	AppendMenu(pop2_3, MF_POPUP, IDM_OPT14, L"90s");
	AppendMenu(pop2_3, MF_POPUP, IDM_OPT23, L"120s");
	AppendMenu(pop2, MF_POPUP, (UINT_PTR)pop2_4, L"游戏背景");
	AppendMenu(pop2_4, MF_POPUP, IDM_OPT15, L"background1");
	AppendMenu(pop2_4, MF_POPUP, IDM_OPT16, L"background2");
	AppendMenu(pop2_4, MF_POPUP, IDM_OPT17, L"background3");
	AppendMenu(pop2_4, MF_POPUP, IDM_OPT18, L"background4");
	AppendMenu(hmenu, MF_POPUP, (UINT_PTR)pop3, L"属性");//第三个主菜单
	AppendMenu(pop3, MF_STRING, IDM_OPT19, L"联系作者");
	AppendMenu(pop3, MF_STRING, IDM_OPT20, L"帮助文档");
	AppendMenu(pop3, MF_STRING, IDM_OPT21, L"软件下载");
	AppendMenu(pop3, MF_STRING, IDM_OPT22, L"退出游戏");
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
	int x = ((GetSystemMetrics(SM_CXSCREEN) / 2) - 500); //x居中  
	int y = ((GetSystemMetrics(SM_CYSCREEN) / 2) - 365); //y居中 
	for (int i = 0; i < 20;i++)
	{
		int shake_x = -4 + rand() % 8;
		int shake_y = -4 + rand() % 8;
	MoveWindow(hwnd, x+ shake_x, y+ shake_y, WINDOW_WIDTH, WINDOW_HEIGHT, 1);
	Sleep(30);
    }
	return 1;
}