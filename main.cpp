///////////////////////////////////
// Автор:         Калинин П.М.   //
// Имя файла:     WinApiTest.cpp //
// Дата создания: 23.11.2019     //
///////////////////////////////////

#ifndef UNICODE
#define UNICODE
#endif

// заголовочные для OpenGL
#include "glaux.h"
#include <gl\GL.h>
#include <gl\GLU.h>

// для библиотек OpenGL
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glaux.lib")

// необходимые для методов OpenGL объекты
HGLRC     hRC       = NULL;	// постоянный контекст рендеринга
HDC       hDC       = NULL;	// приватный контекст устройства GDI
HINSTANCE hInstance;		// дескриптор приложения

// объявляем 4 текстовых окна
HWND textX, textY, textW, textH;

// объявляем 4 окна ввода
HWND editX, editY, editW, editH;

// объявляем кнопку
HWND btn;

// объявляем главное окно
HWND mainWindow;

// объявляем окно графики
HWND graphWindow;

// id кнопки
#define ID_BUTTON 1

// размеры окон
#define MAIN_WIDTH    800
#define MAIN_HEIGHT   600
#define OPENGL_WIDTH  400
#define OPENGL_HEIGHT 500

// длина буферов для копирования текста ввода
const int BUF_LENGTH = 4;

// буферы для текста из форм ввода
wchar_t buffX[BUF_LENGTH] = (L"");
wchar_t buffY[BUF_LENGTH] = (L"");
wchar_t buffW[BUF_LENGTH] = (L"");
wchar_t buffH[BUF_LENGTH] = (L"");


// прототипы функций 
GLvoid KillGLWindow();
GLvoid ReSizeGLScene();
bool InitGL();           
GLvoid DrawGLScene(const GLfloat& rX, const GLfloat& rY, const GLfloat& rW, const GLfloat& rH);
LRESULT  CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT  CALLBACK GraphWindowProc(HWND, UINT, WPARAM, LPARAM);


// главная функция
// [in/out] HINSTANCE - дескриптор приложения
// [in/out] HINSTANCE - дескриптор	родительского приложения
// [in/out] LPSTR     - параметры командной строки
// [in/out] int       - состояние отображения окна
int WINAPI WinMain(HINSTANCE  hInstance, HINSTANCE  hPrevInstance, LPSTR lpCmdLine, int nCmdShow)     
{         
		// создание главного окна

	// объявляем класс главного окна
	WNDCLASS wc = { };

	// инициализируем название класса
	const wchar_t MAIN_NAME[] = L"MainWindow";

	// присваиваем параметры
	wc.lpfnWndProc = WndProc;                 // процедура обработки сообщений
	wc.hInstance = hInstance;                 // устанавливаем дескриптор
	wc.lpszClassName = MAIN_NAME;             // устанавливаем имя классу
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); // загружаем указатель мышки

	// регистрируем класс
	if (!RegisterClass(&wc))         
	{
		MessageBox(NULL, L"Класс главного окна не зарегистрирован!", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return 0;            
	};

	// инициализируем главное окно
	mainWindow = CreateWindowEx(
		0,                   // расширенный стиль для окна      
		MAIN_NAME,           // класс 
		L"WinApiTest",       // имя 
		WS_OVERLAPPEDWINDOW, // стиль

		// установка позиции и размеров
		100, 100, MAIN_WIDTH, MAIN_HEIGHT,

		NULL, NULL, hInstance, NULL);

	// проверка на успешность создания главного окна
	if (!mainWindow)
	{
		MessageBox(NULL, L"Главное окно не создано!", L"ERROR", MB_OK | MB_ICONINFORMATION);
		return 0;
	};

	// показать главное окно
	ShowWindow(mainWindow, nCmdShow);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////

		// создание GL-окна

	// объявляем класс GL-окна
	WNDCLASS wcGL = { };

	// инициализируем название класса
	const wchar_t GL_NAME[] = L"GLWindow";

	// параметры
	wcGL.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // перерисуем при перемещении и создаём скрытый DC
	wcGL.lpfnWndProc   = (WNDPROC)GraphWindowProc;           // процедура обработки сообщений
	wcGL.cbClsExtra    = 0;                                  // нет дополнительной информации для окна
	wcGL.cbWndExtra    = 0;                                  // нет дополнительной информации для окна
	wcGL.hInstance     = hInstance;                          // устанавливаем дескриптор
	wcGL.hIcon         = LoadIcon(NULL, IDI_WINLOGO);        // загружаем иконку по умолчанию
	wcGL.hCursor       = LoadCursor(NULL, IDC_ARROW);        // загружаем указатель мышки
	wcGL.hbrBackground = NULL;                               // фон не требуется для GL
	wcGL.lpszMenuName  = NULL;                               // меню в окне не будет
	wcGL.lpszClassName = GL_NAME;                            // устанавливаем имя классу

	// регистрируем класс
	if (!RegisterClass(&wcGL))          
	{
		MessageBox(NULL, L"Класс граф. окна не зарегистрирован!", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return 0;           
	};

	// инициализируем графическое окно
	graphWindow = CreateWindowEx(
		WS_EX_APPWINDOW | WS_EX_WINDOWEDGE, // расширенный стиль для окна
		GL_NAME,                            // имя класса
		(LPCTSTR)NULL,                      // заголовок окна
		WS_CHILD | WS_BORDER | WS_VISIBLE,  // требуемый стиль для окна
		25, 25,                             // позиция окна
		OPENGL_WIDTH,                       // ширина окна 
		OPENGL_HEIGHT,                      // высота окна
		mainWindow,                         // родитель
		NULL,                               // нет меню
		hInstance,                          // дескриптор приложения
		NULL);

	// проверка на успешность создания графического окна
	if (!graphWindow)
	{
		MessageBox(NULL, L"Графическое окно не создано!", L"ERROR", MB_OK | MB_ICONINFORMATION);
		return 0;
	};

	// показать графическое окно
	ShowWindow(graphWindow, SW_NORMAL);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// производим настройки графики

	// pfd сообщает Windows каким будет вывод на экран каждого пикселя
	static  PIXELFORMATDESCRIPTOR pfd =            
	{
		sizeof(PIXELFORMATDESCRIPTOR),  // размер дескриптора данного формата пикселей
		1,                              // номер версии
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // формат для окна, OpenGL, двойного буфера
		PFD_TYPE_RGBA,                  // требуется RGBA формат
		32,                             // выбирается бит глубины цвета
		0, 0, 0, 0, 0, 0,               // игнорирование цветовых битов
		0,                              // нет буфера прозрачности
		0,                              // сдвиговый бит игнорируется
		0,                              // нет буфера накопления
		0, 0, 0, 0,                     // биты накопления игнорируются
		32,                             // 32 битный Z-буфер (буфер глубины)
		0,                              // нет буфера трафарета
		0,                              // нет вспомогательных буферов
		PFD_MAIN_PLANE,                 // главный слой рисования
		0,                              // зарезервировано
		0, 0, 0                         // маски слоя игнорируются
	};

	// проверка на возможность получения контекста устройства
	if (!(hDC = GetDC(graphWindow)))          
	{
		KillGLWindow();      
		MessageBox(NULL, L"Невозможно получить контекст устройства", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return 0;                
	}

	// формат пикселя
	GLuint pixelFormat;            

	// нахождение подходящего формата пикселя
	if (!(pixelFormat = ChoosePixelFormat(hDC, &pfd)))        
	{
		KillGLWindow();            
		MessageBox(NULL, L"Не найден подходящий формат пикселя!", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;           
	}

	// проверка на установку формата пикселя
	if (!SetPixelFormat(hDC, pixelFormat, &pfd))          
	{
		KillGLWindow();               
		MessageBox(NULL, L" Формат пикселя не установлен!", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;             
	}

	// проверка на возможность установки контекста рендеринга
	if (!(hRC = wglCreateContext(hDC)))     
	{
		KillGLWindow();           
		MessageBox(NULL, L"Невозможно установить контекст рендеринга!", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;       
	}

	// попытка активации контектста рендеринга
	if (!wglMakeCurrent(hDC, hRC))       
	{
		KillGLWindow();             
		MessageBox(NULL, L"Контекст рендеринга не активирован!", L"ERROR", MB_OK | MB_ICONEXCLAMATION);
		return false;           
	}

	// показать окно
	ShowWindow(graphWindow, SW_SHOW);              

	// настроить перспективу для OpenGL экрана
	ReSizeGLScene();        

	// инициализация созданного окна
	if (!InitGL())            
	{
		KillGLWindow();              
		MessageBox(NULL, (L"Инициализация GL-окна не удалась!"), (L"ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;          
	};

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
		// инициализация окон текста, ввода, кнопок

	// 4 окна с текстом
	textX = CreateWindow(TEXT("STATIC"), TEXT("Введите X:"),
		WS_VISIBLE | WS_CHILD,
		450, 50, 100, 25,
		mainWindow, (HMENU)NULL, NULL, NULL);

	textY = CreateWindow(TEXT("STATIC"), TEXT("Введите Y:"),
		WS_VISIBLE | WS_CHILD,
		450, 85, 100, 25,
		mainWindow, (HMENU)NULL, NULL, NULL);

	textW = CreateWindow(TEXT("STATIC"), TEXT("Длина:"),
		WS_VISIBLE | WS_CHILD,
		625, 50, 100, 25,
		mainWindow, (HMENU)NULL, NULL, NULL);

	textH = CreateWindow(TEXT("STATIC"), TEXT("Высота:"),
		WS_VISIBLE | WS_CHILD,
		625, 85, 100, 25,
		mainWindow, (HMENU)NULL, NULL, NULL);

	// 4 окна окна с вводом
	editX = CreateWindow(TEXT("EDIT"), TEXT("100"),
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
		550, 50, 50, 25,
		mainWindow, (HMENU)NULL, NULL, NULL);

	editY = CreateWindow(TEXT("EDIT"), TEXT("50"),
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
		550, 85, 50, 25,
		mainWindow, (HMENU)NULL, NULL, NULL);

	editW = CreateWindow(TEXT("EDIT"), TEXT("200"),
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
		700, 50, 50, 25,
		mainWindow, (HMENU)NULL, NULL, NULL);

	editH = CreateWindow(TEXT("EDIT"), TEXT("150"),
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
		700, 85, 50, 25,
		mainWindow, (HMENU)NULL, NULL, NULL);

	// ограничиваем длину ввода 3-мя символами
	SendMessage(editX, EM_LIMITTEXT, 3, 0);
	SendMessage(editY, EM_LIMITTEXT, 3, 0);
	SendMessage(editW, EM_LIMITTEXT, 3, 0);
	SendMessage(editH, EM_LIMITTEXT, 3, 0);

	// копирование информации в буферы из окон ввода
	SendMessage(editX, WM_GETTEXT, (WPARAM)BUF_LENGTH, (LPARAM)buffX);
	SendMessage(editY, WM_GETTEXT, (WPARAM)BUF_LENGTH, (LPARAM)buffY);
	SendMessage(editW, WM_GETTEXT, (WPARAM)BUF_LENGTH, (LPARAM)buffW);
	SendMessage(editH, WM_GETTEXT, (WPARAM)BUF_LENGTH, (LPARAM)buffH);

	// создаем кнопку
	btn = CreateWindow(TEXT("BUTTON"), TEXT("Перерисовать"),
		WS_VISIBLE | WS_CHILD,
		525, 150, 130, 25,
		mainWindow, (HMENU)ID_BUTTON, NULL, NULL);

	// обновить главное окно
	UpdateWindow(mainWindow);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// структура для хранения сообщения Windows
	MSG msg;

	// цикл обработки сообщений
	while (GetMessage(&msg, NULL, 0, 0))
	{
		// переводим сообщение
		TranslateMessage(&msg);

		// отсылаем сообщение WinProc()
		DispatchMessage(&msg);
	};

	// разрушаем окно
	KillGLWindow();        

	// выходим из программы
	return (msg.wParam);             
}

// инициализация окна OpenGL
GLvoid ReSizeGLScene()
{
	// cброс текущей области вывода
    glViewport(0, 0, OPENGL_WIDTH, OPENGL_HEIGHT);

	// выбор матрицы проекций
	glMatrixMode(GL_PROJECTION);          

	// сделать единичной матрицу проекций
    glLoadIdentity();             
 
	// для вывода в 2D
	glOrtho(0, OPENGL_WIDTH, OPENGL_HEIGHT, 0, 0, 1);

	// Выбор матрицы вида модели
    glMatrixMode( GL_MODELVIEW );  

	// сделать единичной матрицу проекций
    glLoadIdentity(); 
}

// установки OpenGL
// [out] bool - успешность метода
bool InitGL()               
{
	// разрешить плавное цветовое сглаживание
	glShadeModel(GL_SMOOTH);     

	// очистка экрана в белый цвет
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	
	// разрешить очистку буфера глубины
	glClearDepth(1.0f);      

	// разрешить тест глубины
    glEnable(GL_DEPTH_TEST); 

	// тип теста глубины
    glDepthFunc(GL_LEQUAL);   

	// улучшение в вычислении перспективы
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  

	// инициализация прошла успешно
	return true;                
}

// прорисовка сцены
// [in]  const GLfloat& - координата верхней левой точки по Х
// [in]  const GLfloat& - координата верхней левой точки по Y
// [in]  const GLfloat& - ширина прямоугольника
// [in]  const GLfloat& - высота прямоугольника
GLvoid DrawGLScene(const GLfloat& rX, const GLfloat& rY, const GLfloat& rW, const GLfloat& rH)        
{
	// очищаем экран и буфер глубины
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

	// сброс просмотра
	glLoadIdentity();                               

	// масштаб
	glScalef(1.0, 1.0, 0.0); 

	// сдвиг координат
	glTranslatef(-1.0f, 1.0f, 0.0f);         

	// выбор черного цвета
	glColor3f(0.0f, 0.0f, 0.0f);    

	// текстура для рисования прямоугольника
	GLuint tex1 = 1;

	//выбирает указанную текстуру как активную для наложения ее на объекты
	glBindTexture(GL_TEXTURE_2D, tex1);

	// начинаем рисовать 4-хугольник
	glBegin(GL_QUADS);

	// верхняя левая координата
	glTexCoord2f(rX, rY); glVertex3f(rX, rY, 0.0f);

	// верхняя правая координата
	glTexCoord2f(rX + rW, rY); glVertex3f(rX + rW, rY, 0.0f);

	// нижняя правая координата
	glTexCoord2f(rX + rW, rY + rH); glVertex3f(rX + rW, rY + rH, 0.0f);

	// нижняя левая координата
	glTexCoord2f(rX, rY + rH); glVertex3f(rX, rY + rH, 0.0f);

	// закончили рисование
	glEnd();          
}

// корректное разрушение GL-окна
GLvoid KillGLWindow()              
{
	// проверка на существование контекста рендеринга
	if (hRC)
	{
		// возможно ли освободить RC и DC 
		if (!wglMakeCurrent(NULL, NULL))
		{
			MessageBox(NULL, L"Невозможно освободить RC и DC!", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}

		// возможно ли удалить RC
		if (!wglDeleteContext(hRC))
		{
			MessageBox(NULL, L"Невозможно удалить RC!", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		};

		// установить RC в NULL
		hRC = NULL;
	};

	// возможно ли уничтожить DC
	if (hDC && !ReleaseDC(graphWindow, hDC))
	{
		MessageBox(NULL, L"Невозможно уничтожить DC!", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);

		// установить DC в NULL
		hDC = NULL;      
	};

	// возможно ли уничтожить окно
	if (graphWindow && !DestroyWindow(graphWindow))
	{
		MessageBox(NULL, L"Невозможно уничтожить графическое окно!", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);

		// установливаем graphWindow в NULL
		graphWindow = NULL;
	};
}

// функция обратного вызова для главного окна
// [in/out] HWND   - дескриптор нужного окна
// [in/out] UINT   - сообщение для этого окна
// [in/out] WPARAM - дополнительная информация
// [in/out] LPARAM - дополнительная информация
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)          
{
	// проверка сообщения для окна
	switch (uMsg)                
	{
		case WM_COMMAND:
		{
			switch (wParam)
			{
				// нажата кнопка
				case ID_BUTTON:
				{
					// копирование информации в буферы из окон ввода
					SendMessage(editX, WM_GETTEXT, (WPARAM)BUF_LENGTH, (LPARAM)buffX);
					SendMessage(editY, WM_GETTEXT, (WPARAM)BUF_LENGTH, (LPARAM)buffY);
					SendMessage(editW, WM_GETTEXT, (WPARAM)BUF_LENGTH, (LPARAM)buffW);
					SendMessage(editH, WM_GETTEXT, (WPARAM)BUF_LENGTH, (LPARAM)buffH);

					// перерисовываем сцену
					DrawGLScene((GLfloat)_wtoi(buffX), (GLfloat)_wtoi(buffY), (GLfloat)_wtoi(buffW), (GLfloat)_wtoi(buffH));
					
					// меняем буфер (двойная буферизация)
					SwapBuffers(hDC);    

					break;
				}

				default:
				{
					// пересылаем все необработанные сообщения DefWindowProc
					DefWindowProc(hWnd, uMsg, wParam, lParam);
				}
			};
			break;
		};

		// перерисовка
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW));

			EndPaint(hWnd, &ps);

			break;
		}

		// сообщение о закрытии
		case WM_CLOSE:              
		{
			PostQuitMessage(0);          
			return 0;           
		}

		default:
		{
			// пересылаем все необработанные сообщения DefWindowProc
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}
}


// функция обратного вызова для графич. окна
// [in/out] HWND   - дескриптор нужного окна
// [in/out] UINT   - сообщение для этого окна
// [in/out] WPARAM - дополнительная информация
// [in/out] LPARAM - дополнительная информация
LRESULT CALLBACK GraphWindowProc(HWND  hWnd, UINT  uMsg, WPARAM  wParam, LPARAM  lParam)   
{
	switch (uMsg)
	{ 
		// завершение программы
		case WM_CLOSE:
		{
			KillGLWindow();
		}

		// перерисовка
		case WM_PAINT:
		{
			// перерисовываем сцену
			DrawGLScene((GLfloat)_wtoi(buffX), (GLfloat)_wtoi(buffY), (GLfloat)_wtoi(buffW), (GLfloat)_wtoi(buffH));
			
			// меняем буфер (двойная буферизация)
			SwapBuffers(hDC);    

			break;
		}

		default: 
		{
			// пересылаем все необработанные сообщения DefWindowProc
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
		}
	}
}