/*
Модифицировать программу из Задания 1 таким образом, чтобы шары отображались в случайной позиции, а за их удаление назначалось различное количество очков.
Предусмотреть вывод стоимости удаления на шаре, использовать собственные текстуры.
Должен быть реализован счетчик очков.
После удаления появляется новый шар другого размера. Предусмотреть невозможность наложения шаров друг на друга
*/

#include<iostream>
#include <SDL.h>
#include<SDL_ttf.h>
#include<conio.h>
#include <ctime>

const int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 600;

void DrawText(SDL_Window* window, SDL_Renderer* renderer, SDL_Rect rect, char* text);
void DrawCircle(SDL_Window * window, SDL_Renderer * renderer);
int isPointInside(int x, int y);
void DelCircle(int c);
void CreateCircle();
bool collision();

#pragma region circle
double x = 0, y = 0, min_radius = 15, u;
int max_radius = 25, n;
const int quantity = 40;
char text[256];

struct Circle
{
	double x, midx;
	double y, midy;
	double r;
	int points;
};
Circle circle[quantity];

void DrawCircle(SDL_Window * window, SDL_Renderer * renderer)
{

	SDL_Surface* myImage = SDL_LoadBMP("1.bmp");
	SDL_SetColorKey(myImage, SDL_TRUE, SDL_MapRGB(myImage->format, 255, 0, 255));
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, myImage);

	for (int i = 0; i < quantity; i++)
	{
		SDL_Rect ball = { (int)circle[i].x, (int)circle[i].y, 2 * (int)circle[i].r, 2 * (int)circle[i].r };
		SDL_RenderCopy(renderer, texture, NULL, &ball);
		sprintf_s(text, "%d", (int)circle[i].r);
		DrawText(window, renderer, ball, text);
	}

	SDL_DestroyTexture(texture);
}

int isPointInside(int x, int y)
{
	for (int i = 0; i < n; i++)
		if ((x - circle[i].midx)*(x - circle[i].midx) + (y - circle[i].midy)*(y - circle[i].midy) <= circle[i].r*circle[i].r) return i + 1;

	return false;
}

void DelCircle(int c)
{
	for (int i = c; i < n; i++)
	{
		circle[i].r = circle[i + 1].r;
		circle[i].x = circle[i + 1].x;
		circle[i].y = circle[i + 1].y;
		circle[i].midx = circle[i + 1].midx;
		circle[i].midy = circle[i + 1].midy;

	}
	n--;
}
void CreateCircle()
{
	do
	{
		circle[n].r = rand() % max_radius + min_radius;

		circle[n].x = rand() % (SCREEN_WIDTH - 4 * max_radius) + max_radius;
		circle[n].y = rand() % (SCREEN_HEIGHT - 4 * max_radius) + max_radius;

		circle[n].midx = circle[n].x + circle[n].r;
		circle[n].midy = circle[n].y + circle[n].r;

	} while (collision());
	n++;
}

bool collision()
{
	for (int j = 0; j < 360; j++)
	{
		u = 3.14 / 180 * j;
		if (isPointInside((int)circle[n].midx + (int)(circle[n].r * cos(u)), (int)circle[n].midy + (int)(circle[n].r * sin(u)))) return true;
	}
	return false;
}
#pragma endregion

TTF_Font* my_font;
void DrawText(SDL_Window* window, SDL_Renderer* renderer, SDL_Rect rect, char* text)
{
	SDL_Color fore_color = { 255,255,255 };
	SDL_Color back_color = { 0,0,0 };

	SDL_Surface* textSurface = TTF_RenderText_Shaded(my_font, text, fore_color, back_color);
	SDL_SetColorKey(textSurface, SDL_TRUE, SDL_MapRGB(textSurface->format, 0, 0, 0));
	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_RenderCopy(renderer, textTexture, NULL, &rect);
	SDL_DestroyTexture(textTexture);
	SDL_FreeSurface(textSurface);
}


int main(int argc, char** argv)
{
	const unsigned char* KeyboardState = SDL_GetKeyboardState(NULL);
	unsigned int t = SDL_GetTicks(), dt, nt;
	srand((int)time(0));

	int points = 0;
	SDL_Rect rectpoints = { 0,0,100,50 };

#pragma region Init
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf_s("SDL не смог запуститься! SDL_Error: %s\n", SDL_GetError());
		system("pause");
		return 0;
	}
	if (TTF_Init() < 0)
	{
		printf_s("TTF не смог запуститься! SDL_Error: %s\n", SDL_GetError());
		system("pause");
		SDL_Quit();
		return 0;
	}
	my_font = TTF_OpenFont("Text.ttf", 100);
	if (my_font == NULL)
	{
		printf_s("Не удалось подгрузить шрифт! SDL_Error: %s\n", SDL_GetError());
		system("pause");
		TTF_Quit();
		SDL_Quit();
		return 0;
	}


	SDL_Window* window = SDL_CreateWindow(u8"Lab22", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWEVENT_MOVED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (window == NULL)
	{
		printf_s("Окно не может быть создано! SDL_Error: %s\n", SDL_GetError());
		system("pause");
		TTF_CloseFont(my_font);
		TTF_Quit();
		SDL_Quit();
		return 0;
	}
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	if (renderer == NULL)
	{
		printf_s("Рисовальщику плохо! SDL_Error: %s\n", SDL_GetError());
		system("pause");
		SDL_DestroyWindow(window);
		TTF_CloseFont(my_font);
		TTF_Quit();
		SDL_Quit();
		return 0;
	}

#pragma endregion

	//================================================GameСycle=============================================================================================//
	SDL_Event event;
	bool draw = false, mouse = false;
	bool quit = false;
	while (!quit)
	{
		nt = SDL_GetTicks();
		dt = nt - t;
		t = nt;

		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT || KeyboardState[SDL_SCANCODE_ESCAPE])
				quit = true;

			if ((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_SPACE) && n < quantity)
				draw = true;

			if (event.button.button == SDL_BUTTON_LEFT)
				mouse = true;
		}
				
		
		if (draw)
		{
			SDL_SetRenderDrawColor(renderer, 50, 50, 150, 0);
			SDL_RenderClear(renderer);

			CreateCircle();
			DrawCircle(window, renderer);
			
			sprintf_s(text, "%d", points);
			
			DrawText(window, renderer, rectpoints, text);
			
			draw = false;
		}
		if (mouse)
		{
			int tmp = isPointInside(event.button.x, event.button.y);
			if (tmp)
			{
				SDL_SetRenderDrawColor(renderer, 50, 50, 150, 0);
				SDL_RenderClear(renderer);

				points += (int)circle[tmp - 1].r;
				sprintf_s(text, "%d", points);
				DrawText(window, renderer, rectpoints, text);

				DelCircle(tmp - 1);
				CreateCircle();
				n--;
				DrawCircle(window, renderer);
				n++;

			}
			mouse = false;
		}

		SDL_RenderPresent(renderer);

	}
	//================================================EndGameСycle=============================================================================================//
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_CloseFont(my_font);
	TTF_Quit();
	SDL_Quit();
	return 0;
}