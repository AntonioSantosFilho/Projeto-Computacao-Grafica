#include <GL/glut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <MMSystem.h>
#include <iostream>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define num_items 5  // Definindo o n�mero de itens presentes no jogo
#define Max_items 7  // quantidade maxima de itens que pode estar ao mesmo tempo no jogo
#define Max_mosquitoes 100
#define Max_shots 30  // Defina um n�mero m�ximo para os tiros
#define timerloop 25  // definindo o tempo de chamada de timer a cada x milisegundos

#define num_map 11

int win = 25, count_timer_loop = 0, count_message = 0, count_current_weapon,
    count_current_item;
int count_time = 1, count_time_game = 0, count_m = 1, colide = 0, dist = 0,
    count_mosquitoes = 0, aux_count_mosquito = 1, aux_count_time = 1,
    aux_count_item = 1, score = 0, level = 0;

float tam = 1.5;
float tamp = 5.0;     // tamanho do jogador
float tammos = 4.0;   // tamanho do mosquito
float tamitem = 3.0;  // tamanho dos itens

bool equipadoBorriflex = false;
bool equipadoNCMTK65 = false;
bool equipadoRepelex = false;
bool equipadoRaquetex = false;

bool spawnBorriflex = true;
bool spawnRepelex = true;
bool spawnRaquetex = true;
bool spawnNCMTK65 = true;

float n = 28, m = 20;
int randomx = 26, randomy = 26;
int intervalo_spawn = 3000;
char mensagem[20];
bool escudoGirando = false;
bool keys[256];  // Array para monitorar o estado das teclas
time_t ultimoTempoPicada;
time_t tempo_ultimo_spawn;
time_t tempo_mensagem;   // Tempo de in�cio da mensagem

// Tamanho da janela Antonio
int windowWidth = 840;
int windowHeight = 600;

int variable = 0;

int tpicada = 2;

#define NUM_TEXTURES 24
GLuint playerTextures[NUM_TEXTURES];
GLuint EfectTextures[NUM_TEXTURES];
int currentTextureIndex = 0;

GLuint mapa[num_map];

int screen_now = 1;
bool estaNoMenu =  true;
bool isPaused = true;
int isFinalizado = 0;

bool onInstrucao = false;
bool onItens = false;
bool onCreditos = false;
int tela_item = 1;

int mostrar_mensagem = 0;  // Flag para indicar se a mensagem deve ser mostrada

typedef enum { BORRIFEX, REPELEX, RAQUETEX, KIT, NCMTK65 } ItemType;
typedef enum { DEFAULT, CHAMA } ShotType;

typedef struct mosquito {//  Jo�o Pedro
  float dx;
  float dy;
  float velocidade;
  int life;
  int type;
  int textureIndex;  // �ndice da textura atual
} Mosquito;

Mosquito mosquitoes[Max_mosquitoes];
GLuint mosquitosTextures[24];  // Array para armazenar as texturas

typedef struct player {//  Jo�o Pedro
  GLfloat tx, ty;
  float velocidade;
  int life;
  int count_item;
  bool armado;
  ShotType currentShotType;  // tipo atual de tiro
  int currentAmmo;
  int maxAmmo;

} Player;

typedef struct shield {
  GLfloat angle;
  float radius;
  int ativo;
} Shield;

typedef struct shot {
  GLfloat x, y;
  GLfloat dx, dy;  // Direcao que o mesmo esta olhandos
  float velocidade;
  int ativo;  // 0 = inativo, 1 = ativo
  GLuint texture;
} Shot;

typedef struct item {//  Jo�o Pedro
  GLfloat x, y;
  ItemType type;
  int ativo;
} Item;

GLuint ItensTexture[4];  // Array para armazenar as texturas
int currentItemTextureIndex = 0;

// Definindo a quantidade m�xima de itens
Item item[num_items];
// Definindo a quantidade m�xima de mosquitos
Shot shots[Max_shots];
// Inicializa o escudo com um �ngulo de 0, raio de 3.0, e inativo
Shield shield = {0, 6.0, 0};
// Definindo o jogador
Player player = {0, 0, 0.5, 4, 0, false, DEFAULT, 0, 0};

void desenharCoracao(float x, float y, float tamanho) {//  Jo�o Pedro
  int i;
  float angulo;
  glBegin(GL_POLYGON);
  glColor3f(1.0, 0.0, 0.0);  // Cor vermelha para os cora��es
  for (i = 0; i < 360; i++) {
    angulo = i * 3.14159 / 180;
    glVertex2f(x + tamanho * (0.75 * sin(angulo) * sin(angulo) * sin(angulo)),
               y + tamanho * (0.6 * cos(angulo) - 0.2 * cos(2 * angulo) -
                              0.1 * cos(3 * angulo) - 0.05 * cos(4 * angulo)));
  }
  glEnd();
}

void desenhaTexto(void *font, char *string) {//  Jo�o Pedro
  while (*string) {
    glutBitmapCharacter(font, *string);
    string++;
  }
}

void desenhaInfo(void *font, char *string) {//  Jo�o Pedro
  while (*string) {
    glutBitmapCharacter(font, *string);
    string++;
  }
}

void desenhaTimer(void *font, char *string) {//  Jo�o Pedro
  while (*string) {
    glutBitmapCharacter(font, *string);
    string++;
  }
}
void desenhaMensagem(void *font, char *string) {//  Jo�o Pedro
  while (*string) {
    glutBitmapCharacter(font, *string);
    string++;
  }
}
void escolheMensagem(int level) {//  Jo�o Pedro
  switch (level) {
    case 1:
      sprintf(mensagem,
              "O nivel de Infestacao tornou-se %d. HORA DE USAR REPELENTES!!",
              level);
      break;
    case 2:
      sprintf(mensagem,
              "O nivel de Infestacao tornou-se %d. TALVEZ UMA ARMA DIFERENTE "
              "SEJA MELHOR?",
              level);
      break;
    case 3:
      sprintf(mensagem,
              "O nivel de Infestacao tornou-se %d. MELHOR SE MOVIMENTAR OU VAI "
              "SER FERRUADO!!",
              level);
      break;
    case 4:
      sprintf(
          mensagem,
          "O nivel de Infestacao tornou-se %d. CUIDADO COM O AEDES AEGYPTI!!",
          level);
      break;
    case 5:
      sprintf(mensagem,
              "O nivel de Infestacao tornou-se %d\n. NENHUM REPELENTE PODE TE "
              "AJUDAR AGORA!!",
              level);
      break;
    default:
      break;
  }
}

void desativarMensagem(int value) {//  Jo�o Pedro
  count_message = 0;
  glutPostRedisplay();
}
void finaliza() {//  Jo�o Pedro
	isFinalizado = 1;
	screen_now = 4;
	isPaused = true;	
	printf("Seu Score foi : %d\n", score);
	
//	exit(0);
}
void drawMosquito(Mosquito m) {
    glPushMatrix();
    glTranslatef(m.dx, m.dy, 0.0f);

    // Ativa a textura antes de vincul�-la
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, mosquitosTextures[m.textureIndex]);

    glBegin(GL_QUADS);
    glColor3f(1.0, 1.0, 1.0);  // Define a cor como branca para a textura ser vis�vel

    // Defina as coordenadas de textura para cada v�rtice
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-tammos / 2, -tammos / 2);

    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(tammos / 2, -tammos / 2);

    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(tammos / 2, tammos / 2);

    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(-tammos / 2, tammos / 2);

    glEnd();

    // Desativa a textura ap�s desenhar
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    glutSwapBuffers();
}

void drawAllMosquitoes() {
  for (int i = 0; i < Max_mosquitoes; i++) {
    if (mosquitoes[i].life >
        0) {  // Certifique-se de que apenas mosquitos vivos sejam desenhados
      drawMosquito(mosquitoes[i]);
    }
  }
}

void updateMosquitoes() {
  for (int i = 0; i < Max_mosquitoes; i++) {
    if (mosquitoes[i].life > 0) {  // Atualize apenas mosquitos vivos
      float dx = player.tx - mosquitoes[i].dx;
      float dy = player.ty - mosquitoes[i].dy;
      float dist = sqrt(dx * dx + dy * dy);
      float moveX = (dx / dist) * mosquitoes[i].velocidade;
      float moveY = (dy / dist) * mosquitoes[i].velocidade;

      mosquitoes[i].dx += moveX;
      mosquitoes[i].dy += moveY;

      // Calcular o �ngulo de movimento
      float angle = atan2(dy, dx) * 180.0 / M_PI;

      // Atualizar a textura do mosquito com base no �ngulo
	      if(mosquitoes[i].type == 0){
		 
	      if (angle >= -45 && angle < 45) {
	        mosquitoes[i].textureIndex = (variable == 1) ? 3 : 7;  // Direita
	      } else if (angle >= 45 && angle < 135) {
	        mosquitoes[i].textureIndex = (variable == 1) ? 0 : 4;  // Frente
	      } else if (angle >= -135 && angle < -45) {
	        mosquitoes[i].textureIndex = (variable == 1) ? 1 : 5;  // Costas
	      } else {
	        mosquitoes[i].textureIndex = (variable == 1) ? 2 : 6;  // Esquerda
	      }
	    } else if(mosquitoes[i].type == 1){
	    	
	    	 if (angle >= -45 && angle < 45) {
	        mosquitoes[i].textureIndex = (variable == 1) ? 14 : 15;  // Direita
	      } else if (angle >= 45 && angle < 135) {
	        mosquitoes[i].textureIndex = (variable == 1) ? 8 : 9;  // Frente
	      } else if (angle >= -135 && angle < -45) {
	        mosquitoes[i].textureIndex = (variable == 1) ? 10 : 11;  // Costas
	      } else {
	        mosquitoes[i].textureIndex = (variable == 1) ? 12 : 13;  // Esquerda
	      }
	    	
		}else if(mosquitoes[i].type == 2){
	    	
	    	 if (angle >= -45 && angle < 45) {
	        mosquitoes[i].textureIndex = (variable == 1) ? 22 : 23;  // Direita
	      } else if (angle >= 45 && angle < 135) {
	        mosquitoes[i].textureIndex = (variable == 1) ? 16 : 17;  // Frente
	      } else if (angle >= -135 && angle < -45) {
	        mosquitoes[i].textureIndex = (variable == 1) ? 18 : 19;  // Costas
	      } else {
	        mosquitoes[i].textureIndex = (variable == 1) ? 20 : 21;  // Esquerda
	      }
	    	
		}
	    
    }
  }
}


void menuInicial(){
	
}


void desenhaMapa() {
   glPushMatrix();
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, mapa[screen_now]);
  glBegin(GL_QUADS);
  glColor3f(1.0, 1.0, 1.0);

 glTexCoord2f(0.0f, 1.0f);
  glVertex2f(-n, -m);

  glTexCoord2f(1.0f, 1.0f);
  glVertex2f(n, -m);

  glTexCoord2f(1.0f, 0.0f);
  glVertex2f(n, m);

  glTexCoord2f(0.0f, 0.0f);
  glVertex2f(-n, m);

  glEnd();
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
  
}


void desenha() {//  Jo�o Pedro


	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);
   
	desenhaMapa();
	

if(!isPaused){

  char scoreText[20];
  sprintf(scoreText, "Score : %d", score);

  char time_gameText[20];
  sprintf(time_gameText, "Tempo: %d", count_time_game);

  char infoAMMO[20];
  sprintf(infoAMMO, "Municao: %d/%d", player.currentAmmo, player.maxAmmo);

  escolheMensagem(level);
 
 

  // Angelo - Player
  glPushMatrix();
  glTranslatef(player.tx, player.ty, 0.0f);
  glBindTexture(GL_TEXTURE_2D, playerTextures[currentTextureIndex]);
  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
  glColor3f(1.0, 1.0, 1.0);
  glTexCoord2f(0.0f, 0.0f);
  glVertex2f(-tamp / 2, -tamp / 2);
  glTexCoord2f(1.0f, 0.0f);
  glVertex2f(tamp / 2, -tamp / 2);
  glTexCoord2f(1.0f, 1.0f);
  glVertex2f(tamp / 2, tamp / 2);
  glTexCoord2f(0.0f, 1.0f);
  glVertex2f(-tamp / 2, tamp / 2);
  glEnd();
  glDisable(GL_TEXTURE_2D);
  glPopMatrix();
  glutSwapBuffers();

  // Tiros
  for (int i = 0; i < Max_shots; i++) {
    if (shots[i].ativo) {
      glPushMatrix();
      glTranslatef(shots[i].x, shots[i].y, 0.0f);
      if (player.currentShotType == DEFAULT) {
        glBegin(GL_QUADS);
        glColor3f(1.0, 0.0, 0.0);  // Cor vermelha para os tiros DEFAULT
        glVertex2f(-tam / 8, -tam / 8);
        glVertex2f(tam / 8, -tam / 8);
        glVertex2f(tam / 8, tam / 8);
        glVertex2f(-tam / 8, tam / 8);
        glEnd();
      } else if (player.currentShotType == CHAMA) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, EfectTextures[0]);
        glBegin(GL_QUADS);
        glColor3f(1.0, 1.0, 1.0);  // Cor branca para a textura ser vis�vel
        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-tamitem, -tamitem);
        glTexCoord2f(1.0f, 0.0f);
        glVertex2f(tamitem, -tamitem);
        glTexCoord2f(1.0f, 1.0f);
        glVertex2f(tamitem, tamitem);
        glTexCoord2f(0.0f, 1.0f);
        glVertex2f(-tamitem, tamitem);
        glEnd();
        glDisable(GL_TEXTURE_2D);
      }
      glPopMatrix();
    }
  }

 drawAllMosquitoes();
  // Desenha item
  for (int i = 0; i < Max_items; i++) {
    if (item[i].ativo) {
      glPushMatrix();
      glTranslatef(item[i].x, item[i].y, 0.0);

      switch (item[i].type) {
        case BORRIFEX:
          currentItemTextureIndex = 0;
          break;
        case REPELEX:
          currentItemTextureIndex = 1;
          break;
        case RAQUETEX:
          currentItemTextureIndex = 2;
          break;
        case KIT:
          currentItemTextureIndex = 3;
          break;
        case NCMTK65:
          currentItemTextureIndex = 5;
          break;
        default:
          printf("%d", item[i].type);
          break;
      }
      glEnable(GL_TEXTURE_2D);  // Habilitar texturas
      glBindTexture(GL_TEXTURE_2D,
                    ItensTexture[currentItemTextureIndex]);  // Bind a textura
      glBegin(GL_QUADS);

      // Definir coordenadas de textura e v�rtices
      glTexCoord2f(0.0, 1.0);
      glVertex2f(-tamitem / 3, -tamitem / 3);

      glTexCoord2f(1.0, 1.0);
      glVertex2f(tamitem / 3, -tamitem / 3);

      glTexCoord2f(1.0, 0.0);
      glVertex2f(tamitem / 3, tamitem / 3);

      glTexCoord2f(0.0, 0.0);
      glVertex2f(-tamitem / 3, tamitem / 3);

      glEnd();

      glDisable(GL_TEXTURE_2D);  // Desabilitar texturas

      glPopMatrix();
      glutSwapBuffers();
    }
    
    
    // Item - RAQUETEX
    if (shield.ativo) {
      glPushMatrix();
      glTranslatef(player.tx, player.ty, 0.0f);
      float escudoX = shield.radius * cos(shield.angle);
      float escudoY = shield.radius * sin(shield.angle);
      glEnable(GL_TEXTURE_2D);                        // Habilitar texturas
      glBindTexture(GL_TEXTURE_2D, ItensTexture[2]);  // Bind a textura

      glTranslatef(escudoX, escudoY, 0.0f);

      glBegin(GL_QUADS);

      glTexCoord2f(0.0, 1.0);
      glVertex2f(-tamitem / 3, -tamitem / 3);

      glTexCoord2f(1.0, 1.0);
      glVertex2f(tamitem / 3, -tamitem / 3);

      glTexCoord2f(1.0, 0.0);
      glVertex2f(tamitem / 3, tamitem / 3);

      glTexCoord2f(0.0, 0.0);
      glVertex2f(-tamitem / 3, tamitem / 3);

      glEnd();
      glDisable(GL_TEXTURE_2D);
      glPopMatrix();
      glutSwapBuffers();
    }
  }

  for (int i = 0; i < player.life; i++) {
    desenharCoracao((-n+3) + i * 3.5f, m - 4.5f, 1.0f);
  }

  // Desenhando o Score na tela
  glPushMatrix();
  glColor3f(0.0, 0.0, 0.0);
  glRasterPos2f(-n + 25, m - 3.0f);  // Posi��o do score na tela
  desenhaTexto(GLUT_BITMAP_HELVETICA_18, scoreText);
  glPopMatrix();

  // Desenhando o Timer na tela
  glPushMatrix();
  glColor3f(0.0, 0.0, 0.0);
  glRasterPos2f(-n + 40, m - 3.5f);  // Posi��o do timer na tela
  desenhaTimer(GLUT_BITMAP_HELVETICA_18, time_gameText);
  glPopMatrix();

  // Desenhando a mensagem
  if (level >= 0 && count_message) {
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(-n + 5, m-10);
    desenhaMensagem(GLUT_BITMAP_HELVETICA_18, mensagem);

    glutTimerFunc(5000, desativarMensagem, 0);
  }

  // Desenhando a arma atual na tela
  if (count_current_weapon) {
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(-23, -18);  // Posi��o da informacao da arma
    desenhaInfo(GLUT_BITMAP_HELVETICA_18, infoAMMO);
    glPopMatrix();

    // Desenhando a arma na parte inferior esquerda
    glPushMatrix();
    glTranslatef(-25, -17.5, 0.0f);
    glEnable(GL_TEXTURE_2D);  // Habilitar texturas
    glBindTexture(GL_TEXTURE_2D,
                  ItensTexture[count_current_item]);  // Bind a textura

    glBegin(GL_QUADS);

    glTexCoord2f(0.0, 1.0);
    glVertex2f(-tamitem / 3, -tamitem / 3);

    glTexCoord2f(1.0, 1.0);
    glVertex2f(tamitem / 3, -tamitem / 3);

    glTexCoord2f(1.0, 0.0);
    glVertex2f(tamitem / 3, tamitem / 3);

    glTexCoord2f(0.0, 0.0);
    glVertex2f(-tamitem / 3, tamitem / 3);

    glEnd();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    glutSwapBuffers();
  }
  }

  glutSwapBuffers();
  glFlush();
}

void carregarTextura(const char *filename, GLuint *textureID) {
  int width, height, channels;
  unsigned char *data = stbi_load(filename, &width, &height, &channels, 0);
  if (data) {
    glGenTextures(1, textureID);
    glBindTexture(GL_TEXTURE_2D, *textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, (channels == 4 ? GL_RGBA : GL_RGB), width,
                 height, 0, (channels == 4 ? GL_RGBA : GL_RGB),
                 GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);
    printf("Textura carregada com sucesso: %s\n", filename);
  } else {
    printf("Falha ao carregar a textura %s \n", filename);
    exit(1);
  }
}

void atualizarPosicaoEscudo() {
  if (shield.ativo && escudoGirando) {
    shield.angle += 0.2f;  // Incrementa o �ngulo para criar a rota��o
    if (shield.angle >= 360.0f)
      shield.angle -= 360.0f;  // Reseta o �ngulo se ultrapassar 360 graus
  }
}

void inicializa() {
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  //=====================================================================================================
  carregarTextura("AngeloSprite/Andando pra Frente - Movendo.png",
                  &playerTextures[0]);
  carregarTextura("AngeloSprite/Andando pra Direita.png", &playerTextures[1]);
  carregarTextura("AngeloSprite/Andando pra Tr�s.png", &playerTextures[2]);
  carregarTextura("AngeloSprite/Andando pra Esquerda.png", &playerTextures[3]);

  carregarTextura("AngeloSprite/Andando pra Frente - Movendo 2.png",
                  &playerTextures[4]);
  carregarTextura("AngeloSprite/Andando pra Direita - Movendo 2.png",
                  &playerTextures[5]);
  carregarTextura("AngeloSprite/Andando pra Tr�s 2.png", &playerTextures[6]);
  carregarTextura("AngeloSprite/Andando pra Esquerda 2.png",
                  &playerTextures[7]);
  //=====================================================================================================
  // Tiros DEFAULT
  carregarTextura("AngeloSprite/Atirando pra Frente - Movendo.png",
                  &playerTextures[8]);
  carregarTextura("AngeloSprite/Atirando pra Direita - Movendo.png",
                  &playerTextures[10]);
  carregarTextura("AngeloSprite/Atirando pra Tr�s - Movendo.png",
                  &playerTextures[12]);
  carregarTextura("AngeloSprite/Atirando pra Esquerda - Movendo.png",
                  &playerTextures[14]);

  carregarTextura("AngeloSprite/Atirando pra Frente - Movendo 2.png",
                  &playerTextures[9]);
  carregarTextura("AngeloSprite/Atirando pra Direita - Movendo 2.png",
                  &playerTextures[11]);
  carregarTextura("AngeloSprite/Atirando pra Tr�s - Movendo 2.png",
                  &playerTextures[13]);
  carregarTextura("AngeloSprite/Atirando pra Esquerda - Movendo 2.png",
                  &playerTextures[15]);

  // Tiros CHAMA
  carregarTextura(
      "AngeloSprite/Angelo com lancachamas apontando pra frente.png",
      &playerTextures[16]);
  carregarTextura(
      "AngeloSprite/Angelo com lancachamas apontando pra frente2.png",
      &playerTextures[17]);
  carregarTextura("AngeloSprite/Angelo com lancachamas apontando pra tr�s.png",
                  &playerTextures[18]);
  carregarTextura("AngeloSprite/Angelo com lancachamas apontando pra tr�s2.png",
                  &playerTextures[19]);
  carregarTextura(
      "AngeloSprite/Angelo com lancachamas apontando pra esquerda.png",
      &playerTextures[20]);
  carregarTextura(
      "AngeloSprite/Angelo com lancachamas apontando pra esquerda2.png",
      &playerTextures[21]);
  carregarTextura(
      "AngeloSprite/Angelo com lancachamas apontando pra direita.png",
      &playerTextures[22]);
  carregarTextura(
      "AngeloSprite/Angelo com lancachamas apontando pra direita2.png",
      &playerTextures[23]);

  //=====================================================================================================

  carregarTextura("Mosquito/Mosquito de frente.png", &mosquitosTextures[0]);
  carregarTextura("Mosquito/Mosquito de costas.png", &mosquitosTextures[1]);
  carregarTextura("Mosquito/Mosquito de esquerda.png", &mosquitosTextures[2]);
  carregarTextura("Mosquito/Mosquito de direita.png", &mosquitosTextures[3]);

  carregarTextura("Mosquito/Mosquito de frente2.png", &mosquitosTextures[4]);
  carregarTextura("Mosquito/Mosquito de costas2.png", &mosquitosTextures[5]);
  carregarTextura("Mosquito/Mosquito de esquerda2.png", &mosquitosTextures[6]);
  carregarTextura("Mosquito/Mosquito de direita2.png", &mosquitosTextures[7]);
  //=====================================================================================================

  carregarTextura("Mosquito/Mosquito especial de frente.png",
                  &mosquitosTextures[8]);
  carregarTextura("Mosquito/Mosquito especial de costas.png",
                  &mosquitosTextures[10]);
  carregarTextura("Mosquito/Mosquito especial de esquerda.png",
                  &mosquitosTextures[12]);
  carregarTextura("Mosquito/Mosquito especial de direita.png",
                  &mosquitosTextures[14]);

  carregarTextura("Mosquito/Mosquito especial de frente 2.png",
                  &mosquitosTextures[9]);
  carregarTextura("Mosquito/Mosquito especial de costas 2.png",
                  &mosquitosTextures[11]);
  carregarTextura("Mosquito/Mosquito especial de esquerda 2.png",
                  &mosquitosTextures[13]);
  carregarTextura("Mosquito/Mosquito especial de direita 2.png",
                  &mosquitosTextures[15]);
  //=====================================================================================================

  carregarTextura("Mosquito/Mosquitao Boss de frente.png",
                  &mosquitosTextures[16]);
  carregarTextura("Mosquito/Mosquitao Boss de costas.png",
                  &mosquitosTextures[18]);
  carregarTextura("Mosquito/Mosquitao Boss de esquerda.png",
                  &mosquitosTextures[20]);
  carregarTextura("Mosquito/Mosquitao Boss de direita.png",
                  &mosquitosTextures[22]);

  carregarTextura("Mosquito/Mosquitao Boss de frente 2.png",
                  &mosquitosTextures[17]);
  carregarTextura("Mosquito/Mosquitao Boss de costas 2.png",
                  &mosquitosTextures[19]);
  carregarTextura("Mosquito/Mosquitao Boss de esquerda 2.png",
                  &mosquitosTextures[21]);
  carregarTextura("Mosquito/Mosquitao Boss de direita 2.png",
                  &mosquitosTextures[23]);
  //=====================================================================================================

  carregarTextura("Itens/Borriflex.png", &ItensTexture[0]);
  carregarTextura("Itens/Repelentex.png", &ItensTexture[1]);
  carregarTextura("Itens/raquete.png", &ItensTexture[2]);
  carregarTextura("Itens/Kit.png", &ItensTexture[3]);
  carregarTextura("itens/NCM-TK65.png", &ItensTexture[5]);
  //=====================================================================================================
  carregarTextura("itens/Efeitos/Fogo do lancachamas.png", &EfectTextures[0]);
  //=====================================================================================================
 
  carregarTextura("mapa/mapa.png", &mapa[0]);
  carregarTextura("telas/inicial.png", &mapa[1]);
  carregarTextura("telas/instrucao.png", &mapa[2]);
  carregarTextura("telas/pause.png", &mapa[3]);
  carregarTextura("telas/fim.png", &mapa[4]);
  
  carregarTextura("telas/item1.png", &mapa[5]);
  carregarTextura("telas/item2.png", &mapa[6]);
  carregarTextura("telas/item3.png", &mapa[7]);
  carregarTextura("telas/item4.png", &mapa[8]);
  carregarTextura("telas/item5.png", &mapa[9]);
  
  carregarTextura("telas/creditos.png", &mapa[10]);
  
  //=====================================================================================================
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  for (int i = 0; i < Max_shots; i++) {
    shots[i].ativo = 0;
  }

  ultimoTempoPicada =
      time(NULL) - 1;  // Inicializa como se tivesse passado 3 segundos
}

void colisao() {//  Jo�o Pedro

time_t tempoAtual = time(NULL);


  // Colis�o com o Item
  for (int i = 0; i < Max_items; i++) {
    if (item[i].ativo) {
      dist =
          sqrt(pow(player.tx - item[i].x, 2) + pow(player.ty - item[i].y, 2));
      if (dist < tam) {
        switch (item[i].type) {
          case 0:
            player.armado = true;
            equipadoBorriflex = true;
            equipadoNCMTK65 = false;
            spawnBorriflex = false;
            player.currentShotType = DEFAULT;
            player.currentAmmo = 50;
            player.maxAmmo = 50;
            count_current_weapon = 1;
            count_current_item = currentItemTextureIndex;
            break;
          case 1:
            score += 15;
            equipadoRepelex = true;
            spawnRepelex = false;
            tpicada = 10;
            ultimoTempoPicada = tempoAtual; 
          
            break;
          case 2:
            shield.ativo = 1;
            equipadoRaquetex = true;
            spawnRaquetex = false;
            break;
          case 3:
            player.life++;
            break;
          case 4:
            player.armado = true;
            equipadoNCMTK65 = true;
            spawnNCMTK65 = false;
            equipadoBorriflex = false;
            player.currentShotType = CHAMA;
            player.currentAmmo = 25;
            player.maxAmmo = 25;
            count_current_weapon = 1;
            count_current_item = currentItemTextureIndex;
            break;
          default:
            break;
        }
        item[i].ativo = 0;
        printf("Item coletado: %d\n", item[i].type);
      }
    }
    // Colis�o com o Escudo
    if (shield.ativo) {
      float escudoX = player.tx + shield.radius * cos(shield.angle);
      float escudoY = player.ty + shield.radius * sin(shield.angle);
      for (int i = 0; i < count_mosquitoes; i++) {
        dist = sqrt(pow(escudoX - mosquitoes[i].dx, 2) +
                    pow(escudoY - mosquitoes[i].dy, 2));
        if (dist < tam / 2) {
          mosquitoes[i].life--;
          printf("Mosquito atingido pelo escudo! Vida restante: %d\n",
                 mosquitoes[i].life);
          if (mosquitoes[i].life <= 0) {
            // Remove o mosquito
            for (int k = i; k < count_mosquitoes - 1; k++) {
              mosquitoes[k] = mosquitoes[k + 1];
            }
            count_mosquitoes--;
            score += 50;
            i--;  // Ajuste o �ndice para verificar o pr�ximo mosquito
                  // corretamente
            break;
          }
        }
      }
    }
  }
  

  // Colis�o com os mosquitos
  for (int i = 0; i < count_mosquitoes; i++) {//  Jo�o Pedro
    dist = sqrt(pow(player.tx - mosquitoes[i].dx, 2) +
                pow(player.ty - mosquitoes[i].dy, 2));
    if (dist < tam && difftime(tempoAtual, ultimoTempoPicada) >= tpicada) {
    	tpicada = 2;
      if (shield.ativo) {
        shield.ativo = false;  // desativa o Raquetex caso tome dano
        spawnRaquetex = true;  // Ligando o spawn do item novamente
      }
      player.life--;
      printf("Vida do jogador %d\n", player.life);
      ultimoTempoPicada = tempoAtual;  // Atualiza o tempo da �ltima picada
      if (player.life <= 0) {
       
        finaliza();
      }
    }

    // Colis�o entre tiros e mosquitos
    for (int j = 0; j < Max_shots; j++) {
      if (shots[j].ativo) {
        dist = sqrt(pow(shots[j].x - mosquitoes[i].dx, 2) +
                    pow(shots[j].y - mosquitoes[i].dy, 2));
        
		if (player.currentShotType == DEFAULT){
			if (dist < tam / 2) {
	          mosquitoes[i].life--;
	          if (mosquitoes[i].life == 0) {
	            score += 50;
          }
          shots[j].ativo = 0;
          printf("Mosquito atingido! Vida restante: %d\n", mosquitoes[i].life);

          	if (mosquitoes[i].life < 1) {
            // Remove o mosquito
            for (int k = i; k < count_mosquitoes - 1; k++) {
              mosquitoes[k] = mosquitoes[k + 1];
              glutSwapBuffers();
            }

            count_mosquitoes--;
            i--;  // Ajuste o �ndice para verificar o pr�ximo mosquito
                  // corretamente
            break;
          }
        	}
		}
		else if (player.currentShotType == CHAMA){
			if (dist < 2*tam) {
				
          		mosquitoes[i].life--;
          	if (mosquitoes[i].life == 0) {
            	score += 50;
          	}
          shots[j].ativo = 1;
          printf("Mosquito atingido! Vida restante: %d\n", mosquitoes[i].life);

          if (mosquitoes[i].life < 1) {
            // Remove o mosquito
            for (int k = i; k < count_mosquitoes - 1; k++) {
              mosquitoes[k] = mosquitoes[k + 1];
              glutSwapBuffers();
            }

            count_mosquitoes--;
            i--;  // Ajuste o �ndice para verificar o pr�ximo mosquito
                  // corretamente
            break;
          }
          }
		}
		
      }
    }
  }
}

void atirar(int mouseX, int mouseY) {
  for (int i = 0; i < Max_shots; i++) {
    if (!shots[i].ativo) {
      shots[i].x = player.tx;
      shots[i].y = player.ty;

      // Transforma coordenadas do mouse para coordenadas do mundo
      GLdouble modelview[16], projection[16];
      GLint viewport[4];
      GLdouble worldX, worldY, worldZ;
      glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
      glGetDoublev(GL_PROJECTION_MATRIX, projection);
      glGetIntegerv(GL_VIEWPORT, viewport);
      gluUnProject(mouseX, viewport[1] + viewport[3] - mouseY, 0.0, modelview,
                   projection, viewport, &worldX, &worldY, &worldZ);

      // Calcular a dire��o do tiro em rela��o � posi��o transformada do mouse
      float dx = worldX - player.tx;
      float dy = worldY - player.ty;
      float magnitude = sqrt(dx * dx + dy * dy);
      shots[i].dx = dx / magnitude;
      shots[i].dy = dy / magnitude;
      shots[i].velocidade = 0.9;
      shots[i].ativo = 1;
      player.currentAmmo--;
      break;
    }
  }
}

void mouse(int button, int state, int x, int y) {
	if(!isPaused){
	
  if (button == GLUT_RIGHT_BUTTON) {
    if (state == GLUT_DOWN) {
      escudoGirando = true;  // Inicia a rota��o do escudo
    } else if (state == GLUT_UP) {
      escudoGirando = false;  // Para a rota��o do escudo
    }
  }
  if (button == GLUT_LEFT_BUTTON &&
      state == GLUT_DOWN) {  // Apenas atira se o bot�o esquerdo do mouse for
                             // pressionado
    if (player.armado == true) {
      if (player.currentAmmo < 1) {
      } else
      	if(player.currentShotType == CHAMA){
      		PlaySound (TEXT("musica/fogareu.wav"), NULL, SND_ASYNC); 
			  atirar(x, y);
		  } else{
		  	PlaySound (TEXT("musica/borrifada.wav"), NULL, SND_ASYNC); 
			  atirar(x, y);
		  }
    }
  }
}
}
void spawnItem() {//  Jo�o Pedro
  for (int i = 0; i < Max_items; i++) {
    if (!item[i].ativo) {
      item[i].type = static_cast<ItemType>(rand() % num_items);
      int chance = rand() % 100;
        
      if (item[i].type == 0) {
        if (!spawnBorriflex) {
          break;
        }
		else {
          if (chance < 100) {
            item[i].x = rand() % 21 - rand () % 21;
            item[i].y = rand() % 21 - rand () % 21;
            if (item[i].x == player.tx && item[i].y == player.ty) break; // para n�o Spawnar o item em cima do personagem
            item[i].ativo = 1;
            spawnBorriflex = false;  // tem que arrumar pra ele poder spawnar novamente
            printf("Item spawnado na posi��o (%f, %f) do tipo %d\n", item[i].x,
                   item[i].y, item[i].type);
          }
          break;
        }
      }

      if (item[i].type == 1) {
        if (equipadoRepelex || !spawnRepelex) {
          break;
        } 
		else {
          if (chance < 50) {
            item[i].x = rand() % 21 - rand () % 21;
            item[i].y = rand() % 21 - rand () % 21;
            if (item[i].x == player.tx && item[i].y == player.ty) break; // para n�o Spawnar o item em cima do personagem
            item[i].ativo = 1;
            spawnRepelex =
                false;  // tem que arrumar pra ele poder spawnar novamente
            printf("Item spawnado na posi��o (%f, %f) do tipo %d\n", item[i].x,
                   item[i].y, item[i].type);
          }
          break;
        }
      }
      if (item[i].type == 2) {
        if (shield.ativo || !spawnRaquetex) {
          break;
        } else {
          if (chance < 50) {
            item[i].x = rand() % 21 - rand () % 21;
            item[i].y = rand() % 21 - rand () % 21;
            if (item[i].x == player.tx && item[i].y == player.ty) break;// para n�o Spawnar o item em cima do personagem
            item[i].ativo = 1;
            spawnRaquetex =
                false;  // tem que arrumar pra ele poder spawnar novamente
            printf("Item spawnado na posi��o (%f, %f) do tipo %d\n", item[i].x,
                   item[i].y, item[i].type);
          }
          break;
        }
      }
      if (item[i].type == 3) {
        if (player.life > 5) {
          break;
        }
		else {
          if (chance < 50) {
            item[i].x = rand() % 21 - rand () % 21;
            item[i].y = rand() % 21 - rand () % 21;
            if (item[i].x == player.tx && item[i].y == player.ty) break;// para n�o Spawnar o item em cima do personagem
            item[i].ativo = 1;
            printf("Item spawnado na posi��o (%f, %f) do tipo %d\n", item[i].x,
                   item[i].y, item[i].type);
          }
          break;
        }
      }
      if (item[i].type == 4) {
        if (equipadoNCMTK65 || !spawnNCMTK65) {
          break;
        } else {
          if (chance < 60) {
            item[i].x = rand() % 21 - rand () % 21;
            item[i].y = rand() % 21 - rand () % 21;
            if (item[i].x == player.tx && item[i].y == player.ty) break;// para n�o Spawnar o item em cima do personagem
            item[i].ativo = 1;
            spawnNCMTK65 =
                false;  // tem que arrumar pra ele poder spawnar novamente
            printf("Item spawnado na posi��o (%f, %f) do tipo %d\n", item[i].x,
                   item[i].y, item[i].type);
          }
          break;
        }
      }
    }
  }
}

void initRandom() {
    srand(time(NULL));
}


int getRandomMosquitoType() {
    float r = (float)rand() / RAND_MAX;  // Gera um n�mero entre 0 e 1
	
    if (r < 0.80) {
        return 0;  // 80% de chance
    } else if (r < 0.995) {
        return 1;  // 19.5% de chance
    } else {
        return 2;  // 0.5% de chance
    }
}

void spawnMosquito() {//  Jo�o Pedro
  if (count_mosquitoes < Max_mosquitoes) {
   
	int selected_type = getRandomMosquitoType();
    int side = rand() % 4;  // 0: top, 1: bottom, 2: left, 3: right

    switch (side) {
      case 0:  // Top
        mosquitoes[count_mosquitoes].dx =
            rand() % (2 * randomx) - randomx;       // Random x within bounds
        mosquitoes[count_mosquitoes].dy = randomy;  // Top edge
        break;
      case 1:  // Bottom
        mosquitoes[count_mosquitoes].dx =
            rand() % (2 * randomx) - randomx;        // Random x within bounds
        mosquitoes[count_mosquitoes].dy = -randomy;  // Bottom edge
        break;
      case 2:                                        // Left
        mosquitoes[count_mosquitoes].dx = -randomx;  // Left edge
        mosquitoes[count_mosquitoes].dy =
            rand() % (2 * randomy) - randomy;  // Random y within bounds
        break;
      case 3:                                       // Right
        mosquitoes[count_mosquitoes].dx = randomx;  // Right edge
        mosquitoes[count_mosquitoes].dy =
            rand() % (2 * randomy) - randomy;  // Random y within bounds
        break;
    }
 
  mosquitoes[count_mosquitoes].type = selected_type; 
 
 if(mosquitoes[count_mosquitoes].type == 0){
 	 aux_count_mosquito++;
 	mosquitoes[count_mosquitoes].life = 1;
    mosquitoes[count_mosquitoes].velocidade = 0.2;
    mosquitoes[count_mosquitoes].type = selected_type; //use selected_type para receber a logica 
    count_mosquitoes++;
 	
 }else if(mosquitoes[count_mosquitoes].type == 1){
 	 aux_count_mosquito++;
 	mosquitoes[count_mosquitoes].life = 3;
    mosquitoes[count_mosquitoes].velocidade = 0.4;
   //use selected_type para receber a logica 
    count_mosquitoes++;
 	
 } else if(mosquitoes[count_mosquitoes].type == 2)	{
 aux_count_mosquito++;
 	mosquitoes[count_mosquitoes].life = 5;
    mosquitoes[count_mosquitoes].velocidade = 1.4;
   
    count_mosquitoes++;
 	
}
  }
}

void moverTiros() {
  for (int i = 0; i < Max_shots; i++) {
    if (shots[i].ativo) {
      colisao();
      shots[i].x += shots[i].dx *
                    shots[i].velocidade;  // Mover os tiros mais rapidamente na
                                          // dire��o x que ele esta olhando
      shots[i].y += shots[i].dy *
                    shots[i].velocidade;  // Mover os tiros mais rapidamente na
                                          // dire��o y que ele esta olhando
      // Desative o tiro se ele sair da tela
      if (shots[i].x > n || shots[i].x < -n || shots[i].y > m ||
          shots[i].y < -m) {
        shots[i].ativo = 0;
      }
    }
  }
}



void checarColisaoEntreMosquitos() {
  for (int i = 0; i < count_mosquitoes; i++) {
    for (int j = i + 1; j < count_mosquitoes; j++) {
      float dx = mosquitoes[j].dx - mosquitoes[i].dx;
      float dy = mosquitoes[j].dy - mosquitoes[i].dy;
      float distancia = sqrt(dx * dx + dy * dy);

      if (distancia < tam) {  // Se a dist�ncia for menor que o tamanho dos
                              // mosquitos, h� uma colis�o
        // Calcular o vetor de separa��o
        float overlap = tam - distancia;
        float separacaoX = (dx / distancia) * (overlap / 2);
        float separacaoY = (dy / distancia) * (overlap / 2);

        // Ajustar a posi��o dos dois mosquitos para evitar a colis�o
        mosquitoes[i].dx -= separacaoX;
        mosquitoes[i].dy -= separacaoY;
        mosquitoes[j].dx += separacaoX;
        mosquitoes[j].dy += separacaoY;
      }
    }
  }
}
void teclado(unsigned char key, int x, int y) {
  switch (key) {
    case '\r':  // Caso da tecla Enter
      if (estaNoMenu) {
        isPaused = !isPaused;
        estaNoMenu = false;
        screen_now = 0;
         PlaySound(NULL, 0, 0);
        printf("Saiu do menu: %s\n", isPaused ? "true" : "false");
      }
      break;

    case 'p':
      
     if(isFinalizado== 1) break;
	 
        isPaused = !isPaused;
        isPaused ? screen_now = 3 : screen_now = 0;
        printf("isPaused: %s\n", isPaused ? "true" : "false");
      

      break;
      
       case 'i':
       	
      if (estaNoMenu) {
      	tela_item = 1;
      	onItens = true;
      	screen_now = 5;
      	
}
        break;
        
        case 'c':
       	onInstrucao = true;
    screen_now = 2;
        break;
      
      case 27:
      	  if (estaNoMenu) {
      	onCreditos = true;
      	screen_now = 10;
      	
}else
      	exit(0);
    	break;

    default: 
      break;
  }
  keys[key] = true; 
  // Marca a tecla como pressionada
}



void teclasEspeciais(int key, int x, int y) {
  switch (key) {
    case GLUT_KEY_PAGE_UP:
      
      if(estaNoMenu && onItens){
      	if(tela_item == 5) break;
      	tela_item++;
      	screen_now = tela_item+4;
      	printf("%d", tela_item); 
      
      	
	  }
      
      break;
      
    case GLUT_KEY_PAGE_DOWN:
    
    if(onCreditos){
    	onCreditos = false;
    	screen_now = 1;
	}
	
	 if(onInstrucao){
    	onInstrucao = false;
    	screen_now = 1;
	}
    
    if(onItens)
	{    	
    	if(tela_item == 1)
		{
	    	screen_now = 1;
	    	onItens = false;
		}else{
	
	   		tela_item--;
	      	screen_now = tela_item+4;
	      	printf("%d", tela_item); 
		}
	}
      	
      
      	

    
      break;
       case GLUT_KEY_END:
      printf("End pressed\n");
     screen_now = 10;
      break;
    default:
      break;
  }
}

void tecladoUp(unsigned char key, int x, int y) {
  keys[key] = false;  // Marca a tecla como solta
}

void atualizarPosicaoJogador() {
  if (keys['a']) {
  	
    if (player.tx > -n) player.tx -= player.velocidade;
    if (equipadoBorriflex || equipadoNCMTK65) {
      if (equipadoBorriflex) {
        equipadoNCMTK65 = false;
        spawnNCMTK65 = true;
        if (variable == 1) {
          currentTextureIndex = 14;
        } else {
          currentTextureIndex = 15;
        }
      }
      if (equipadoNCMTK65) {
        equipadoBorriflex = false;
        spawnBorriflex = true;
        if (variable == 1) {
          currentTextureIndex = 20;
        } else {
          currentTextureIndex = 21;
        }
      }

    } else if (!equipadoBorriflex || !equipadoNCMTK65) {
      if (variable == 1) {
        currentTextureIndex = 3;
      } else {
        currentTextureIndex = 7;
      }
    }
  }
  if (keys['d']) {
    if (player.tx < n) player.tx += player.velocidade;
    if (equipadoBorriflex || equipadoNCMTK65) {
      if (equipadoBorriflex) {
        equipadoNCMTK65 = false;
        spawnNCMTK65 = true;
        if (variable == 1) {
          currentTextureIndex = 10;
        } else {
          currentTextureIndex = 11;
        }
      }
      if (equipadoNCMTK65) {
        equipadoBorriflex = false;
        spawnBorriflex = true;
        if (variable == 1) {
          currentTextureIndex = 22;
        } else {
          currentTextureIndex = 23;
        }
      }

    } else if (!equipadoBorriflex || !equipadoNCMTK65) {
      if (variable == 1) {
        currentTextureIndex = 1;
      } else {
        currentTextureIndex = 5;
      }
    }
  }
  if (keys['w']) {
    if (player.ty < m) player.ty += player.velocidade;

    if (equipadoBorriflex || equipadoNCMTK65) {
      if (equipadoBorriflex) {
        equipadoNCMTK65 = false;
        spawnNCMTK65 = true;
        if (variable == 1) {
          currentTextureIndex = 12;
        } else {
          currentTextureIndex = 13;
        }
      }
      if (equipadoNCMTK65) {
        equipadoBorriflex = false;
        spawnBorriflex = true;
        if (variable == 1) {
          currentTextureIndex = 18;
        } else {
          currentTextureIndex = 19;
        }
      }

    } else if (!equipadoBorriflex || !equipadoNCMTK65) {
      if (variable == 1) {
        currentTextureIndex = 2;
      } else {
        currentTextureIndex = 6;
      }
    }
  }
  if (keys['s']) {
    if (player.ty > -m) player.ty -= player.velocidade;

    if (equipadoBorriflex || equipadoNCMTK65) {
      if (equipadoBorriflex) {
        equipadoNCMTK65 = false;
        spawnNCMTK65 = true;
        if (variable == 1) {
          currentTextureIndex = 8;
        } else {
          currentTextureIndex = 9;
        }
      }
      if (equipadoNCMTK65) {
        equipadoBorriflex = false;
        spawnBorriflex = true;
        if (variable == 1) {
          currentTextureIndex = 16;
        } else {
          currentTextureIndex = 17;
        }
      }

    } else if (!equipadoBorriflex || !equipadoNCMTK65) {
      if (variable == 1) {
        currentTextureIndex = 0;
      } else {
        currentTextureIndex = 4;
      }
    }
  }

  colisao();
}

void timer(int value) {//  Jo�o Pedro

  if (!isPaused) {
    count_timer_loop += timerloop;
    if (count_timer_loop >= 1000) {  // Convertendo milissegundos em segundos
      count_time_game++;  // adicionando 1 segundo no contador do jogo
      count_timer_loop = 0;
    }
    // Desenha todos os mosquitos    
	
    

    checarColisaoEntreMosquitos();
    moverTiros();               // Atualiza a posi��o dos tiros
    atualizarPosicaoJogador();  // Atualiza a posi��o do jogador com base nas
                                // teclas pressionadas
                                // Atualiza a posi��o do escudo
    atualizarPosicaoEscudo();
    // Verifica colis�es ap�s atualizar os tiros
    colisao();

    // Spawn de item
    if (count_time_game == 3 * aux_count_item) {// Exemplo: a cada x segundos
      aux_count_item++;
      spawnItem();
    }
    // Atualizando o n�vel a cada 30 segundos
    if (count_time_game == 30 * aux_count_time) {
      aux_count_time++;
      level++;
      count_message = 1;
      // Reduz o intervalo de spawn conforme o n�vel aumenta
      intervalo_spawn =
          3000 /
          (level + 1);  // Exemplo: n�vel 0 => 3000ms, n�vel 1 => 1500ms, etc.
    }
    // Spawnando mosquito de acordo com o intervalo de tempo do n�vel de
    // infesta��o
    if (difftime(time(NULL), tempo_ultimo_spawn) * 1000 >= intervalo_spawn) {
      spawnMosquito();
      tempo_ultimo_spawn = time(NULL);
    }
  }

  glutPostRedisplay();

  glutTimerFunc(timerloop, timer, 0);  // Aumentar a frequ�ncia do timer
}

void timeranim(int value) {
  // Alterar o valor da vari�vel
  variable = !variable;
  
   updateMosquitoes();

  // Redesenha a cena
  glutPostRedisplay();

  // Define o temporizador novamente
  glutTimerFunc(60, timeranim, 0);
}

void timerup(int value) {
  
 
  // Define o temporizador novamente
  glutTimerFunc(10, timerup, 0);
}

void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{
    GLsizei largura, altura;

    if (h == 0) h = 1;

    largura = w;
    altura = h;

    glViewport(0, 0, largura, altura);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    GLfloat fatorAumento = 0.8f;  // 10% a mais

    if (largura <= altura) 
    { 
        gluOrtho2D(-25.0f * fatorAumento, 25.0f * fatorAumento, 
                   -25.0f * altura / largura * fatorAumento, 
                    25.0f * altura / largura * fatorAumento);
        win = 45.0f * fatorAumento;
    }              
    else 
    {
        gluOrtho2D(-25.0f * largura / altura * fatorAumento, 
                    25.0f * largura / altura * fatorAumento, 
                   -25.0f * fatorAumento, 
                    25.0f * fatorAumento);
        win = 45.0f * largura / altura * fatorAumento;           
    }
}

int main() {//  Jo�o Pedro

  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  initRandom();
  glutInitWindowPosition(250, 100);
  glutInitWindowSize(windowWidth, windowHeight);
  glutCreateWindow("Projeto Final - Computacao Grafica\n");
  glutMouseFunc(mouse);
  glutKeyboardFunc(teclado);
  glutSpecialFunc(teclasEspeciais);
  glutKeyboardUpFunc(tecladoUp);  // Registra o callback para quando a tecla � solta
  glutDisplayFunc(desenha);
  glutReshapeFunc(AlteraTamanhoJanela);
  glutTimerFunc(50, timer, 0);  // Chama a fun��o de timer a cada 50ms
  glutTimerFunc(0, timeranim, 0);
if(estaNoMenu) PlaySound (TEXT("musica/start.wav"), NULL, SND_ASYNC); else{
}
  inicializa();
  
	
	glutTimerFunc(0, timerup, 0);
  glutMainLoop();

  return 0;
}
