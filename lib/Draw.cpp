#include "../modelos/RGB.h"
#include "../modelos/Sprite.h"
#include "fileH/Screen.h"
#include "fileH/HandlerSprites.h"
#include "../modelos/Carta.h"
#include "../modelos/Player.h"
#include "../simulador/simulatorH/jogo.h"
#include "../simulador/simulatorH/HandlerCarta.h"
#include <string>
#include <chrono>
#include <thread>
using namespace std;

#define showCardsJ 200
#define screenWidth 360
const struct RGB BLACK = {0, 0, 0};
const struct RGB YELLOW_PAGE = {204, 201, 172};
const struct RGB WHITE = {255, 255, 255};
const struct RGB RED = {170, 0, 0};

struct Sprite *alfabeto;
struct Sprite *spriteCartas;
struct Sprite *extras;
struct Sprite *numbers;

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

void delay(int ms)
{
    this_thread::sleep_for(chrono::milliseconds(ms));
}

void initSprites()
{
    alfabeto = readSprites("sprites/alfabeto.txt");
    spriteCartas = readSprites("sprites/cartas.txt");
    extras = readSprites("sprites/extras.txt");
    numbers = readSprites("sprites/numero.txt");
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

void drawSprite(int lui, int luj, struct Sprite sprite, struct RGB color, int width_multipliar, int heigth_multipliar, bool drawBackground)
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
                else if (drawBackground)
                {
                    for (int jk = 0; jk < width_multipliar; jk++)
                    {
                        setPixel(lui - position_i, luj + j * width_multipliar + jk, WHITE);
                    }
                }
            }
            position_i++;
        }
    }
}
int getStringLength(string frase)
{
    int r = 0;
    for (int i = 0; i < frase.length(); i++)
    {
        if (frase[i] == ' ')
        {
            r += 3;
            continue;
        }
        if (frase[i] == '-')
        {

            r += numbers[9].width + 1;
        }
        else if (frase[i] >= '0' && frase[i] <= '9')
        {

            r += numbers[frase[i] - '0'].width + 1;
        }
        else
        {

            r += alfabeto[frase[i] - 'a'].width + 1;
        }
    }
    return r;
}
void drawString(string frase, int lui, int luj, struct RGB color, int width_multipliar, int heigth_multipliar)
{ // left upper corner
    for (int i = 0; i < frase.length(); i++)
    {
        if (frase[i] == ' ')
        {
            luj += 3 * width_multipliar;
            continue;
        }
        if (frase[i] == '-')
        {
            drawSprite(lui, luj, numbers[9], color, width_multipliar, heigth_multipliar, false);
            luj += (numbers[9].width + 1) * width_multipliar;
        }
        else if (frase[i] >= '0' && frase[i] <= '9')
        {
            drawSprite(lui, luj, numbers[frase[i] - '0'], color, width_multipliar, heigth_multipliar, false);
            luj += (numbers[frase[i] - '0'].width + 1) * width_multipliar;
        }
        else
        {
            drawSprite(lui, luj, alfabeto[frase[i] - 'a'], color, width_multipliar, heigth_multipliar, false);
            luj += (alfabeto[frase[i] - 'a'].width + 1) * width_multipliar;
        }
    }
}

void drawStringLimited(int lui, int luj, int heigth, int width, string frase, struct RGB color, int width_multipliar, int heigth_multipliar)
{
    int lujO = luj;
    for (int i = 0; i < frase.length(); i++)
    {
        if (frase[i] == ' ')
        {
            luj += 3 * width_multipliar;
            if (luj + 50 * width_multipliar >= width)
            {
                lui -= 8 * heigth_multipliar;
                luj = lujO;
            }
            continue;
        }
        if (frase[i] == '-')
        {
            drawSprite(lui, luj, numbers[9], color, width_multipliar, heigth_multipliar, false);
            luj += (numbers[9].width + 1) * width_multipliar;
        }
        else if (frase[i] >= '0' && frase[i] <= '9')
        {
            drawSprite(lui, luj, numbers[frase[i] - '0'], color, width_multipliar, heigth_multipliar, false);
            luj += (numbers[frase[i] - '0'].width + 1) * width_multipliar;
        }
        else
        {
            drawSprite(lui, luj, alfabeto[frase[i] - 'a'], color, width_multipliar, heigth_multipliar, false);
            luj += (alfabeto[frase[i] - 'a'].width + 1) * width_multipliar;
        }
    }
}

void drawNumber(int num, int h, int j) // j = end
{
    int og = num;
    num = abs(num);

    if (num == 0)
    {
        drawSprite(h, j, numbers[num % 10], BLACK, 1, 1, false);
        return;
    }

    while (num != 0)
    {
        drawSprite(h, j, numbers[num % 10], BLACK, 1, 1, false);
        j -= 6;
        num /= 10;
    }

    if (og < 0)
    {
        drawSprite(h, j, numbers[10], BLACK, 1, 1, false);
    }
}

void drawStringBlankCenter(string frase, int wm, int hm)
{
    drawRectangle(239, 0, 240, screenWidth, YELLOW_PAGE);
    drawString(frase, 120 + 5 * hm / 2, screenWidth / 2 - getStringLength(frase) * wm / 2, BLACK, wm, hm);
}

void drawWaitingPlayer(int v)
{
    confirmado = false;
    cancelado = false;

    drawStringBlankCenter("player " + to_string(v), 2, 2);

    while (true)
    {
        timerOverride();

        if (confirmado)
        {
            break;
        }
    }

    confirmado = false;
    cancelado = false;
}

void drawCardWithDescription(struct Carta carta)
{
    drawRectangle(239, 0, 240, screenWidth, YELLOW_PAGE);
    drawSprite(230, 10, spriteCartas[carta.indSprite], carta.jumpscareColor, 2, 2, true);
    drawString(carta.nome, 230, 80, BLACK, 2, 2);
    drawStringLimited(210, 80, 230, screenWidth - 90, carta.descricao, BLACK, 1, 1);
}

void drawDeckBuilderMenu(struct Carta *original)
{
    drawRectangle(239, 0, 240, screenWidth, YELLOW_PAGE);
    drawRectangle(239, screenWidth / 2, 240, 1, BLACK);

    drawString("qtd", 230, screenWidth / 2 - getStringLength("player 1") - getStringLength("qtd") - 70, BLACK, 1, 1);
    drawString("player 1", 230, screenWidth / 2 - getStringLength("player 1") - 50, BLACK, 1, 1);
    drawNumber(getQtdCardDeck(0), 230, screenWidth / 2 - getStringLength("player 1") - 60);

    drawString("player 2", 230, screenWidth - getStringLength("player 2") - 50, BLACK, 1, 1);
    drawNumber(getQtdCardDeck(1), 230, screenWidth - getStringLength("player 2") - 60);
    drawString("qtd", 230, screenWidth - getStringLength("player 2") - getStringLength("qtd") - 70, BLACK, 1, 1);

    for (int k = 0; k < 2; k++)
    {
        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                drawSprite(220 - i * 45, 5 + j * 46 + k * screenWidth / 2, spriteCartas[original[i * 4 + j].indSprite], BLACK, 1, 1, true);
            }
        }
    }
}

int deckCardShow(struct Carta carta, int qtdO) // retorna qtd
{
    cancelado = false;
    confirmado = false;
    indKj = qtdO;
    int medKj;

    drawCardWithDescription(carta);
    while (true)
    {
        if (cancelado)
        {
            cancelado = false;
            confirmado = false;
           
            return qtdO;
        }

        if (confirmado)
        {
            cancelado = false;
            confirmado = false;
            
            return indKj;
        }

        if (indKj > 2)
        {
            indKj = 2;
        }
        else if (indKj < 0)
        {
            indKj = 0;
        }
        medKj = indKj;

        drawSprite(150, 22, extras[1], BLACK, 1, 1, false);
        drawSprite(150, 32, numbers[indKj], BLACK, 1, 1, false);
        drawSprite(150, 42, extras[0], BLACK, 1, 1, false);

        timerOverride();

        drawSprite(150, 32, numbers[medKj], YELLOW_PAGE, 1, 1, false);
    }
  
    return qtdO;
}

void deckBuilder()
{
    indKi = 0;
    indKj = 0;
    cancelado = false;
    confirmado = false;

    int medKj, medKi;
    struct Carta *original = getCartasOriginal();
    struct Player *jogadores = getJogadores();

    drawDeckBuilderMenu(original);
    while (true)
    {
        medKi = indKi;
        medKj = indKj;

        drawSprite(185 + indKi * 45, 19 + (indKj % 4) * 46 + (indKj / 4) * screenWidth / 2, extras[2], BLACK, 1, 1, false);

        timerOverride();

        drawSprite(185 + medKi * 45, 19 + (medKj % 4) * 46 + (medKj / 4) * screenWidth / 2, extras[2], YELLOW_PAGE, 1, 1, false);

        if (cancelado)
        {
            cancelado = false;
            confirmado = false;
            state = 0;
            return;
        }

        if (indKj > 7)
        {
            indKj = 7;
        }
        else if (indKj < 0)
        {
            indKj = 0;
        }

        if (indKi > 0)
        {
            indKi = 0;
        }
        else if (indKi < -4)
        {
            indKi = -4;
        }

        if (confirmado)
        {
            cancelado = false;
            confirmado = false;
            medKi = indKi;
            medKj = indKj;

            if (medKj < 4)
            {
                jogadores[0].deck[medKj % 4 + abs(medKi) * 4] = deckCardShow(original[medKj % 4 + abs(medKi) * 4], jogadores[0].deck[medKj % 4 + abs(medKi) * 4]);
            }
            else
            {
                jogadores[1].deck[medKj % 4 + abs(medKi) * 4] = deckCardShow(original[medKj % 4 + abs(medKi) * 4], jogadores[1].deck[medKj % 4 + abs(medKi) * 4]);
            }

            indKj = medKj;
            indKi = medKi;
            cancelado = false;
            confirmado = false;
            drawDeckBuilderMenu(original);
        }
    }
}

void drawGame()
{
    struct Player atual = getPlayer(true);
    struct Player oponente = getPlayer(false);

    if (oponente.adr <= 0)
    {
        drawRectangle(239, 50, 10, 0, RED);
    }
    else if (oponente.adr >= 100)
    {
        drawRectangle(239, 50, 10, 100, RED);
    }
    else
    {
        drawRectangle(239, 50, 10, oponente.adr, RED);
    }

    drawNumber(oponente.adr, 237, 40);

    if (atual.adr <= 0)
    {
        drawRectangle(10, 50, 10, 0, RED);
    }
    else if (atual.adr >= 100)
    {
        drawRectangle(10, 50, 10, 100, RED);
    }
    else
    {
        drawRectangle(10, 50, 10, oponente.adr, RED);
    }

    drawNumber(atual.adr, 8, 40);

    for (int i = 0; i < 5; i++)
    {
        if (!oponente.livreEmJogo[i])
        {
            drawSprite(196, 10 + i * 37, spriteCartas[oponente.emJogo[i].indSprite], BLACK, 1, 1, true);
        }
        else
        {
            drawSprite(196, 10 + i * 37, extras[5], BLACK, 1, 1, false);
        }

        if (!atual.livreEmJogo[i])
        {
            drawSprite(76, 10 + i * 37, spriteCartas[atual.emJogo[i].indSprite], BLACK, 1, 1, true);
        }
        else
        {
            drawSprite(76, 10 + i * 37, extras[5], BLACK, 1, 1, false);
        }
    }
}

void drawCardArray(struct Carta *cartas, int length)
{
    int less = 0;
    for (int i = length - 1; i >= 0; i--)
    {
        if (cartas[i].nula)
        {
            less++;
            continue;
        }
        drawString(cartas[i].nome, 235 - (length - i - 1 + less) * 6 + 1, showCardsJ + 9, BLACK, 1, 1);
    }
}

int showCards(struct Carta *cartas, int length)
{
    confirmado = false;
    cancelado = false;

    if (length <= 0)
    {
        return -1;
    }

    int nulls[length];
    nulls[0] = 0;

    if (cartas[0].nula)
    {
        nulls[0] = 1;
    }

    for (int i = 1; i < length; i++)
    {
        nulls[i] = nulls[i - 1];
        if (cartas[i].nula)
        {
            nulls[i] += 1;
        }
    }

    length -= nulls[length - 1];

    if (length <= 0)
    {
        return -1;
    }

    indKi = length - 1;
    while (true)
    {
        drawRectangle(239, 0, 240, screenWidth, YELLOW_PAGE);
        drawGame();
        drawCardArray(cartas, length);
        drawRectangle(239, showCardsJ, 240, 1, BLACK);
        drawSprite(235 - (length - indKi - 1) * 6 + 1, showCardsJ + 2, extras[0], BLACK, 1, 1, false);

        timerOverride();

        if (cancelado)
        {
            confirmado = false;
            cancelado = false;
            drawRectangle(239, 0, 240, screenWidth, YELLOW_PAGE);
            drawString("pass turn", 120, screenWidth / 2 - getStringLength("pass turn") / 2, BLACK, 1, 1);

            while (true)
            {
                timerOverride();

                if (confirmado)
                {
                    confirmado = false;
                    cancelado = false;
                    drawRectangle(239, 0, 240, screenWidth, YELLOW_PAGE);
                    drawGame();
                    drawCardArray(cartas, length);
                    drawRectangle(239, showCardsJ, 240, 1, BLACK);
                    drawSprite(235 - (length - indKi - 1) * 6 + 1, showCardsJ + 2, extras[0], BLACK, 1, 1, false);
                    return -1;
                }
                else if (cancelado)
                {
                    confirmado = false;
                    cancelado = false;
                    break;
                }
            }
        }

        if (indKi < 0)
        {
            indKi = 0;
        }
        else if (indKi >= length)
        {
            indKi = length - 1;
        }

        if (confirmado)
        {
            confirmado = false;
            cancelado = false;

            drawCardWithDescription(cartas[indKi + nulls[indKi]]);

            while (true)
            {
                timerOverride();
                if (confirmado)
                {
                    confirmado = false;
                    cancelado = false;

                    drawRectangle(239, 0, 240, screenWidth, YELLOW_PAGE);
                    drawGame();
                    drawCardArray(cartas, length);
                    drawRectangle(239, showCardsJ, 240, 1, BLACK);
                    drawSprite(235 - (length - indKi - 1) * 6 + 1, showCardsJ + 2, extras[0], BLACK, 1, 1, false);

                    return indKi;
                }
                else if (cancelado)
                {
                    confirmado = false;
                    cancelado = false;
                    break;
                }
            }
        }
        confirmado = false;
        cancelado = false;
    }
}

void drawLost()
{
    state = 0;
    drawStringBlankCenter("you lost", 2, 2);
    timerOverride();
    delay(2000);
}

void jumpscare(struct Carta carta)
{

    for (int i = 1; i <= carta.jumpscareSizeMultipliar; i++)
    {
        drawSprite(120 + spriteCartas[carta.indSprite].heigth * i / 2, screenWidth / 2 - spriteCartas[carta.indSprite].width * i / 2,
                   spriteCartas[carta.indSprite], carta.jumpscareColor, i, i, true);
        timerOverride();
        drawRectangle(120 + spriteCartas[carta.indSprite].heigth * i / 2, screenWidth / 2 - spriteCartas[carta.indSprite].width * i / 2,
                      spriteCartas[carta.indSprite].heigth * i, spriteCartas[carta.indSprite].width * i, YELLOW_PAGE);
        delay(700);
    }
}

void drawMenu()
{
    if (indKi > 0)
    {
        indKi = 0;
    }
    else if (indKi < -2)
    {
        indKi = -2;
    }

    if (confirmado)
    {
        if (indKi == -2)
        {
            exit(0);
        }
        state = indKi + 2;
        if (state == 2)
        {
            if (!initSimulation())
            {
                state = 0;
                drawStringBlankCenter("algum deck nao esta com a quantidade de cartas certas", 1, 1);
                timerOverride();
                delay(3500);
            }
        }
    }
    drawRectangle(239, 0, 240, screenWidth, YELLOW_PAGE);
    drawString("versao", 6, 10, BLACK, 1, 1);
    drawNumber(1004, 6, 70);

    drawSprite(227, 12, extras[4], BLACK, 7, 6, false);

    drawRectangle(235, screenWidth / 2 - getStringLength("jumpscard") - 1, 20, getStringLength("jumpscard") * 2, YELLOW_PAGE);
    drawRectangle(140, screenWidth / 2 - getStringLength("play"), 60, getStringLength("play") * 2, YELLOW_PAGE);

    drawSprite(130 + 20 * indKi, screenWidth / 2 - 4 * 5, extras[0], BLACK, 1, 1, false);

    drawString("jumpscard", 230, screenWidth / 2 - getStringLength("jumpscard"), BLACK, 2, 2);
    drawString("play", 130, screenWidth / 2 - getStringLength("play") / 2, BLACK, 1, 1);
    drawString("deck", 110, screenWidth / 2 - getStringLength("deck") / 2, BLACK, 1, 1);
    drawString("exit", 90, screenWidth / 2 - getStringLength("exit") / 2, BLACK, 1, 1);


    confirmado = false;
    cancelado = false;
}

void cycle()
{
    if (state == 0)
    {
        drawMenu();
    }
    else if (state == 1)
    {
        deckBuilder();
    }
    else if (state == 2)
    {
       
        if (!drawCard())
        {
            drawLost();
            return;
        }

        drawRectangle(239, 0, 240, screenWidth, YELLOW_PAGE);
        drawGame();

        while (true)
        {
            int resp = playCard();
            if (resp == 2)
            {
                break;
            }
        }

        if (checkDeath())
        {
            drawLost();
            return;
        }

        passTurn();
    }
}
