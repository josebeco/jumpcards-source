#include "../modelos/RGB.h"
#include "../modelos/Sprite.h"
#include "fileH/Screen.h"
#include "fileH/HandlerSprites.h"
#include "../modelos/Carta.h"
#include "../modelos/Player.h"
#include "../simulador/simulatorH/jogo.h"
#include <string>
using namespace std;

#define showCardsJ 220
#define screenWidth 360
const struct RGB BLACK = {0, 0, 0};
const struct RGB YELLOW_PAGE = {204, 201, 172};
const struct RGB DARK_BACKGROUND = {174, 171, 142};

struct Sprite *alfabeto;
struct Sprite *spriteCartas;
struct Sprite *extras;

int state = 0; // 0-menu 1-deck 2-game
int indKi = 0;
int indKj = 0;
bool confirmado;
bool cancelado;

void incKi()
{
    indKi++;
}

void incKj()
{
    indKj++;
}

void decKi()
{
    indKi--;
}

void decKj()
{
    indKj--;
}

void confirmar()
{
    confirmado = true;
}

void cancelar()
{
    cancelado = true;
}

void initSprites()
{
    alfabeto = readSprites("sprites/alfabeto.txt");
    spriteCartas = readSprites("sprites/cartas.txt");
    extras = readSprites("sprites/extras.txt");
}

void drawRectangle(int lui, int luj, int heigth, int width, struct RGB color)
{ // left upper corner
    while (--heigth >= 0)
    {
        for (int i = 0; i < width; i++)
        {
            setPixel(lui - heigth, luj + i, color);
        }
    }
}

void drawSprite(int lui, int luj, struct Sprite sprite, struct RGB color, int width_multipliar, int heigth_multipliar)
{ // left upper corner
    int position_i = 0;
    for (int i = 0; i < sprite.heigth; i++)
    {
        for (int ik = 0; ik < heigth_multipliar; ik++)
        {
            for (int j = 0; j < sprite.width; j++)
            {
                if (sprite.pixels[i * sprite.width + j])
                {
                    for (int jk = 0; jk < width_multipliar; jk++)
                    {
                        setPixel(lui - position_i, luj + j * width_multipliar + jk, color);
                    }
                }
            }
            position_i++;
        }
    }
}

void drawString(string frase, int lui, int luj, struct RGB color, int width_multipliar, int heigth_multipliar)
{ // left upper corner
    for (int i = 0; i < frase.length(); i++)
    {
        if (frase[i] == ' ')
        {
            luj += 3;
            continue;
        }

        drawSprite(lui, luj, alfabeto[frase[i] - 'a'], color, width_multipliar, heigth_multipliar);
        luj += alfabeto[frase[i] - 'a'].width + 1;
    }
}

void drawCardWithDescription(struct Carta carta){
    drawRectangle(239, 0, 240, screenWidth, YELLOW_PAGE);
    drawSprite(120 + spriteCartas[carta.indSprite].width, 20, spriteCartas[carta.indSprite], carta.jumpscareColor, 2, 2);
    
}

void drawGame(){

}

int showCards(struct Carta *cartas, int length)
{
    if (length < 0)
    {
        return -1;
    }
    drawRectangle(239, showCardsJ, 240, screenWidth - showCardsJ, YELLOW_PAGE);
    for (int i = 0; i < length; i++)
    {
        drawString(cartas[i].nome, 238 - i * 6 + 1, 231, BLACK, 1, 1);
    }

    indKi = 0;
    while(true){
        timerOverride();

        if
        if(confirmado){
            drawCardWithDescription(cartas(indKi))
        }
        drawSprite(238 - indKi * 6 + 1, screenWidth - showCardsJ + 1, extras[0], BLACK, 1, 1);
        confirmado = false;
        cancelado = false;
    }
}

void jumpscare(struct Carta carta)
{

    for (int i = 1; i <= carta.jumpscareSizeMultipliar; i++)
    {
        drawSprite(120 + spriteCartas[carta.indSprite].width * i / 2, screenWidth / 2, spriteCartas[carta.indSprite], carta.jumpscareColor, i, i);
        drawSprite(120 + spriteCartas[carta.indSprite].width * i / 2, screenWidth / 2, spriteCartas[carta.indSprite], YELLOW_PAGE, i, i);
    }
}

void drawMenu()
{
    if (indKi > 0)
    {
        indKi = 0;
    }
    else if (indKi < -1)
    {
        indKi = -1;
    }

    if (confirmado)
    {
        state = indKi + 2;
        if (state == 2)
        {
            initSimulation();
        }
    }
    drawRectangle(239, 0, 240, screenWidth, YELLOW_PAGE);
    drawSprite(130 + 20 * indKi, screenWidth / 2 - 6 * 4, extras[0], BLACK, 1, 1);
    drawString("play", 130, screenWidth / 2 - (4 * 4), BLACK, 1, 1);
    drawString("deck", 110, screenWidth / 2 - (4 * 4), BLACK, 1, 1);
    confirmado = false;
    cancelado = false;
}

void draw()
{
    if (state == 0)
    {
        drawMenu();
    }
    else if (state == 1)
    {
    }
    else if (state == 2)
    {
        if (!drawCard())
        {
            state == 0;
            drawRectangle(239, 0, 240, screenWidth, YELLOW_PAGE);
            drawString("you lose", 130, screenWidth / 2 - (8 * 4), BLACK, 1, 1);
        }
        while (true)
        {
            int resp = playCard();
            if (resp == 2)
            {
                break;
            }
        }
    }
}