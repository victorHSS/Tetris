#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <allegro.h>

/*
 *     ________    _____    ________    _____     __    _____
 *    |********|  |*****|  |********|  |*****\   |**|  /*****
 *       |**|     |*|         |**|     |*    *    __   |*___
 *       |**|     |***|       |**|     |*** /    |**|  \****\
 *       |**|     |*|         |**|     |*| |*|   |**|   ____*
 *       |**|     |*****|     |**|     |*|  \*\  |**|   **** /
 *
 *      Autor:  Victor André
 *      Data:   12, Dez, 2009
 *      Versão: 0.8
 */

typedef struct {
    int bloco[20];
    int h,w;
} PARTE;

typedef struct {
    PARTE parte[4];
    int dir;
    int color;
} PECA;

typedef enum {
    NOVO = 0,
    PROCESSO,
    GAMEOVER
} ESTADO;

#define TOTAL_PECAS 9

#define LARGURA 14
#define ALTURA 20

#define TELA_H 640
#define TELA_W 448

#define BLOCO 32

#define BLOCO_TO_PIX(a) (32*a)

PECA pecas[] = {
        /* QUADRADO */
        {
            {
               {{1,1,
                 1,1}, 2, 2},
               {{1,1,
                 1,1}, 2, 2},
               {{1,1,
                 1,1}, 2, 2},
               {{1,1,
                 1,1}, 2, 2},
            }, 0, 0x00FF0000},
        /* ZIG-ZAG 1*/
        {
            {
               {{1,1,0,
                 0,1,1,
                 0,0,0}, 3, 3},
               {{0,0,1,
                 0,1,1,
                 0,1,0}, 3, 3},
               {{1,1,0,
                 0,1,1,
                 0,0,0}, 3, 3},
               {{0,0,1,
                 0,1,1,
                 0,1,0}, 3, 3}
            }, 0, 0x0000FF00},
        /* ZIG-ZAG 2*/
        {
            {
               {{0,1,1,
                 1,1,0,
                 0,0,0}, 3, 3},
               {{0,1,0,
                 0,1,1,
                 0,0,1}, 3, 3},
               {{0,1,1,
                 1,1,0,
                 0,0,0}, 3, 3},
               {{0,1,0,
                 0,1,1,
                 0,0,1}, 3, 3}
            }, 0, 0x00661177},
        /* PINTO */
        {
            {
               {{0,0,0,
                 1,1,1,
                 0,1,0}, 3, 3},
               {{0,1,0,
                 1,1,0,
                 0,1,0}, 3, 3},
               {{0,1,0,
                 1,1,1,
                 0,0,0}, 3, 3},
               {{0,1,0,
                 0,1,1,
                 0,1,0}, 3, 3}
            }, 0, 0x000000FF},
        /* RETAO */
        {
            {
               {{0,0,0,0,
                 1,1,1,1,
                 0,0,0,0,
                 0,0,0,0}, 4, 4},
               {{0,1,0,0,
                 0,1,0,0,
                 0,1,0,0,
                 0,1,0,0}, 4, 4},
               {{0,0,0,0,
                 1,1,1,1,
                 0,0,0,0,
                 0,0,0,0}, 4, 4},
               {{0,1,0,0,
                 0,1,0,0,
                 0,1,0,0,
                 0,1,0,0}, 4, 4}
            }, 0, 0x00F0F00F},
        /* BENGALA 1*/
        {
            {
               {{0,0,0,
                 1,1,1,
                 1,0,0}, 3, 3},
               {{1,1,0,
                 0,1,0,
                 0,1,0}, 3, 3},
               {{0,0,1,
                 1,1,1,
                 0,0,0}, 3, 3},
               {{0,1,0,
                 0,1,0,
                 0,1,1}, 3, 3}
            }, 0, 0x0000F0FF},
        /* BENGALA 2*/
        {
            {
               {{0,0,0,
                 1,1,1,
                 0,0,1}, 3, 3},
               {{0,1,0,
                 0,1,0,
                 1,1,0}, 3, 3},
               {{1,0,0,
                 1,1,1,
                 0,0,0}, 3, 3},
               {{0,1,1,
                 0,1,0,
                 0,1,0}, 3, 3}
            }, 0, 0x00000777},
        /* BENGALA DUPLA*/
        {
            {
               {{1,0,0,
                 1,1,1,
                 0,0,1}, 3, 3},
               {{0,1,1,
                 0,1,0,
                 1,1,0}, 3, 3},
               {{1,0,0,
                 1,1,1,
                 0,0,1}, 3, 3},
               {{0,1,1,
                 0,1,0,
                 1,1,0}, 3, 3}
            }, 0, 0x00AABBCC},
        /* CRUZ*/
        {
            {
               {{0,1,0,
                 1,1,1,
                 0,1,0}, 3, 3},
               {{0,1,0,
                 1,1,1,
                 0,1,0}, 3, 3},
               {{0,1,0,
                 1,1,1,
                 0,1,0}, 3, 3},
               {{0,1,0,
                 1,1,1,
                 0,1,0}, 3, 3}
            }, 0, 0x0000AABB},

            };

int mapa[LARGURA*ALTURA] ;
int velocidade = 25;

BITMAP *back_buffer,*foto;

//timer variables
volatile int counter;
volatile int ticks;
volatile int framerate;
volatile int resting, rested;

void timer1(void) {
    counter++;
    framerate = ticks;
    ticks = 0;
    rested = resting;
}
END_OF_FUNCTION(timer1)

//lock interrupt variables
LOCK_VARIABLE(counter);
LOCK_VARIABLE(framerate);
LOCK_VARIABLE(ticks);
LOCK_FUNCTION(timer1);

void rest1(void) {
    resting++;
}

/* Funções gráficas */
void draw_bloco(int x, int y, int color) {
    rectfill(back_buffer, BLOCO_TO_PIX(x), BLOCO_TO_PIX(y), BLOCO_TO_PIX(x) + BLOCO, BLOCO_TO_PIX(y) + BLOCO, color);
    rect(back_buffer,  BLOCO_TO_PIX(x), BLOCO_TO_PIX(y), BLOCO_TO_PIX(x) + BLOCO, BLOCO_TO_PIX(y) + BLOCO, makecol(255,255,255));
}

void draw_back(){
    for ( int i = 0 ; i < LARGURA ; i++)
        for ( int j = 0 ; j < ALTURA ; j++)
            if (mapa[j*LARGURA + i])
                draw_bloco(i,j,mapa[j*LARGURA + i]);
}

void draw_piece(int x, int y, int atual, int dir){
    for ( int i = 0 ; i < pecas[atual].parte[dir].w ; i++)
        for ( int j = 0 ; j < pecas[atual].parte[dir].h ; j++)
            if (pecas[atual].parte[dir].bloco[j*pecas[atual].parte[dir].w+i])
                draw_bloco(x+i,y+j,pecas[atual].color);
}
/* Fim fuções gráficas */


void copy_mapa(int x, int y, int atual, int dir){
    for ( int i = 0 ; i < pecas[atual].parte[dir].w ; i++)
        for ( int j = 0 ; j < pecas[atual].parte[dir].h ; j++)
            if (pecas[atual].parte[dir].bloco[j*pecas[atual].parte[dir].w+i])
                mapa[(y+j)*LARGURA + (x+i)] = pecas[atual].color;
}

void come_linhas(int jj) {
    for ( int i = 0 ; i < LARGURA ; i++)
        for ( int j = jj ; j > 0 ; j--)
            mapa[( j )*LARGURA + i] = mapa[(j - 1)*LARGURA + i];
    for ( int j = 0 ; j < LARGURA ; j++)
        mapa[j] = 0;
}

void check_linhas(){
    int flag;
    for ( int j = ALTURA - 1 ; j >= 0 ; j--) {
        flag = 1;
        for ( int i = 0 ; i < LARGURA ; i++) {
            if (!mapa[j*LARGURA + i]){
                flag = 0;
                break;
            }
        }
        if (flag) {
            come_linhas(j);
            j++;
        }
    }
}

int main()
{
    //initialize the program
    allegro_init();
    install_keyboard();
    install_timer();
    set_color_depth(32);
    text_mode(-1);
    set_gfx_mode(GFX_AUTODETECT_WINDOWED, TELA_W, TELA_H, 0, 0);
    install_int(timer1, 1000);
    resting = 0;
    rest_callback(8, rest1);
    srand(time(NULL));
    memset(mapa,0,sizeof(int)*LARGURA*ALTURA);

    ESTADO st = NOVO;

    int atual,dir,end;
    int x,y;
    int retardo = 0;
    int key_ant = key[KEY_UP];

    back_buffer = create_bitmap(TELA_W,TELA_H);
    foto = load_bitmap("bitmaps/fundo_bola.bmp",0);
    //main loop
    while(!key[KEY_ESC]){

        //processar logica do jogo
        switch(st) {
            case NOVO: atual = rand()%TOTAL_PECAS; x = 4,y = 0;dir = 0;st = PROCESSO;break;
            case PROCESSO:
                //processar entrada

                /* PRESSIONANDO CIMA (^) */
                end = 0;
                if (key[KEY_UP]) {
                    if (key_ant != key[KEY_UP]) {
                        int ant = dir;
                        dir = (dir + 1)%4;
                        for ( int i =  0 ; i < pecas[atual].parte[dir].h  && !end; i++)
                            for ( int j = 0 ; j < pecas[atual].parte[dir].w ; j++)
                                if (pecas[atual].parte[dir].bloco[i*pecas[atual].parte[dir].w+j])
                                    if( (x + j  < 0) || (x + j + 1 > LARGURA) || mapa[ ( y + i ) * LARGURA + ( x + j )]) {
                                        end = 1;
                                        break;
                                    }
                        if (end) dir = ant;
                    }
                }
                key_ant = key[KEY_UP];

                //retardar a descida... Aumentar ficar mais lento. Diminuir fica mais rapido.
                if (retardo++ >= velocidade || key[KEY_DOWN]) {
                    retardo = 0;
                    y++;
                    /* Detecta a colisão com outras peças ou o chão*/
                    end = 0;
                    for ( int i = pecas[atual].parte[dir].h - 1 ; i >=0 && !end; i--)
                        for ( int j = 0 ; j < pecas[atual].parte[dir].w ; j++)
                            if (pecas[atual].parte[dir].bloco[i*pecas[atual].parte[dir].w+j])
                                if ( ( y + i == ALTURA) || ( mapa[ ( y + i ) * LARGURA + ( x + j ) ] ) ) {//choque ou fim da linha?
                                    end = 1;
                                    y--;
                                    st = NOVO;
                                    copy_mapa(x,y,atual,dir);
                                    break;
                                }
                    check_linhas();
                    for ( int j = 0 ; j < LARGURA ; j++)
                        if (mapa[j])
                            st = GAMEOVER;
                } else {
                    /* PRESSIONANDO ESQUERDA (<) */
                    end = 0;
                    if (key[KEY_LEFT]) {
                        for ( int i =  0 ; i < pecas[atual].parte[dir].h  && !end; i++)
                            for ( int j = 0 ; j < pecas[atual].parte[dir].w ; j++)
                                if (pecas[atual].parte[dir].bloco[i*pecas[atual].parte[dir].w+j])
                                    if( (x + j  == 0) || mapa[ ( y + i ) * LARGURA + ( x + j - 1 )]) {
                                        end = 1;
                                        break;
                                    }
                        if (!end) x--;
                    }

                    /* PRESSIONANDO DIREITA (>) */
                    end = 0;
                    if (key[KEY_RIGHT]) {
                        for ( int i =  0 ; i < pecas[atual].parte[dir].h  && !end; i++)
                            for ( int j = pecas[atual].parte[dir].w - 1 ; j >=0 ; j--)
                                if (pecas[atual].parte[dir].bloco[i*pecas[atual].parte[dir].w+j])
                                    if( (x + j + 1 == LARGURA) || mapa[ ( y + i ) * LARGURA + ( x + j + 1 )]) {
                                        end = 1;
                                        break;
                                    }
                        if (!end) x++;
                    }
                }
                clear_keybuf();

                break;
                case GAMEOVER: break;
        }
        //update ticks
        ticks++;

        //desenhos aqui
        acquire_screen();
        //clear_to_color(back_buffer,makecol(255,255,255));
        blit(foto,back_buffer,0,0,0,0,foto->w,foto->h);
        draw_back();
        draw_piece(x,y,atual,dir);

        blit(back_buffer,screen,0,0,0,0,back_buffer->w,back_buffer->h);
        release_screen();
        //fim desenhos

        rest(30);
    }
    destroy_bitmap(back_buffer);
    destroy_bitmap(foto);
    return 0;
}
END_OF_MAIN();
