#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>

#define Max_mosquitoes 100

#define timerloop 60
//definindo o tempo de chamada de timer a cada x milisegundos

int win = 25, count_timer_loop = 0;
GLfloat ix = 4, iy = 2;
int count_time = 1, count_time_game = 0, count_m = 1, colide = 0, dist = 0, count_mosquitoes = 1, aux_count_mosquito = 1, score = 0, level = 1;
float tam = 2.0; // tamanho do jogador
float n = 28, m = 20;
int randomix = 20, randomiy = 20;
int randomx = 26, randomy = 26;

time_t ultimoTempoPicada;

bool keys[256]; // Array para monitorar o estado das teclas

// Fazendo uma struct para cada mosquito
typedef struct mosquito {
    GLfloat dx, dy;
    int life;
    float velocidade;
} Mosquito;

typedef struct player {
    GLfloat tx, ty;
    float velocidade;
    int life;
    int count_item;
} Player;

typedef struct shield {
    GLfloat angle;
    float radius;
    int ativo;
} Shield;

Shield shield = {0, 6.0, 0}; // Inicializa o escudo com um ângulo de 0, raio de 3.0, e inativo
bool escudoGirando = false;


#define Max_shots 100 // Defina um número máximo para os tiros

typedef struct shot {
    GLfloat x, y;
    float velocidade;
    int ativo; // 0 = inativo, 1 = ativo
} Shot;

Shot shots[Max_shots];

// Definindo o jogador
Player player = {0, 0, 1.0, 5, 0};
// Definindo a quantidade máxima de mosquitos
Mosquito mosquitoes[Max_mosquitoes];


void desenharCoracao(float x, float y, float tamanho) {
    int i;
    float angulo;
    glBegin(GL_POLYGON);
    glColor3f(1.0, 0.0, 0.0); // Cor vermelha para os corações
    for (i = 0; i < 360; i++) {
        angulo = i * 3.14159 / 180;
        glVertex2f(x + tamanho * (0.75 * sin(angulo) * sin(angulo) * sin(angulo)), 
                   y + tamanho * (0.6 * cos(angulo) - 0.2 * cos(2 * angulo) - 0.1 * cos(3 * angulo) - 0.05 * cos(4 * angulo)));
    }
    glEnd();
}

void desenhaTexto (void *font, char *string){
	while (*string){
		glutBitmapCharacter(font, *string);
		string++;
	}
}

void desenhaTimer (void *font, char *string){
	while (*string){
		glutBitmapCharacter(font, *string);
		string++;
	}
}

void finaliza(){
    printf("Seu Score foi : %d\n", score);
    exit(0);
}

void desenha() {
	char scoreText[20];
	sprintf(scoreText, "Score : %d", score);
	
	char time_gameText[20];
	sprintf(time_gameText, "Tempo: %d", count_time_game);
	
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT);

    // Gota - Jogador
    glPushMatrix();
    glTranslatef(player.tx, player.ty, 0.0f);
    glBegin(GL_QUADS);
    glColor3f(0.0, 1.0, 1.0);
    glVertex2f(-tam / 2, -tam / 2);
    glVertex2f(tam / 2, -tam / 2);
    glVertex2f(tam / 2, tam / 2);
    glVertex2f(-tam / 2, tam / 2);
    glEnd();
    glPopMatrix();

    // Tiros
    for (int i = 0; i < Max_shots; i++) {
        if (shots[i].ativo) {
            glPushMatrix();
            glTranslatef(shots[i].x, shots[i].y, 0.0f);
            glBegin(GL_QUADS);
            glColor3f(1.0, 0.0, 0.0); // Cor vermelha para os tiros
            glVertex2f(-tam / 8, -tam / 8);
            glVertex2f(tam / 8, -tam / 8);
            glVertex2f(tam / 8, tam / 8);
            glVertex2f(-tam / 8, tam / 8);
            glEnd();
            glPopMatrix();
        }
    }

    // Triangulo - Mosquitos
    for (int i = 0; i < count_mosquitoes; i++) {
        glPushMatrix();
        glTranslatef(mosquitoes[i].dx, mosquitoes[i].dy, 0.0f);
        glBegin(GL_TRIANGLES);
        glColor3f(0.0, 0.0, 0.0);
        glVertex2f(-tam / 2, -tam / 2);
        glVertex2f(tam / 2, -tam / 2);
        glVertex2f(0.0, tam / 2);
        glEnd();
        glPopMatrix();
    }

    // Item - ObjetoAzul
    if (colide == 0) {
        glPushMatrix();
        glTranslatef(ix, iy, 0.0f);
        glBegin(GL_QUADS);
        glColor3f(0.0, 0.0, 1.0);
        glVertex2f(-tam / 3, -tam / 3);
        glVertex2f(tam / 3, -tam / 3);
        glVertex2f(tam / 3, tam / 3);
        glVertex2f(-tam / 3, tam / 3);
        glEnd();
        glPopMatrix();
    }
    
    // item - Escudo
     if (shield.ativo) {
        glPushMatrix();
        glTranslatef(player.tx, player.ty, 0.0f);
        float escudoX = shield.radius * cos(shield.angle);
        float escudoY = shield.radius * sin(shield.angle);
        glTranslatef(escudoX, escudoY, 0.0f);
        glBegin(GL_QUADS);
        glColor3f(1.0, 1.0, 0.0); // Cor amarela para o escudo
        glVertex2f(-tam / 4, -tam / 4);
        glVertex2f(tam / 4, -tam / 4);
        glVertex2f(tam / 4, tam / 4);
        glVertex2f(-tam / 4, tam / 4);
        glEnd();
        glPopMatrix();
    }

     for (int i = 0; i < player.life; i++) {
        desenharCoracao(-n + i * 2.5f, m + 2.0f, 1.0f);
    }
    
    // Desenhando o Score na tela
    glPushMatrix();
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(-n + 25, m + 1.5f); // Posição do score na tela
    desenhaTexto(GLUT_BITMAP_HELVETICA_18, scoreText);
    glPopMatrix();
    
    // Desenhando o Timer na tela
    glPushMatrix();
    glColor3f(0.0,0.0,0.0);
    glRasterPos2f(-n + 50, m + 1.5f); // Posição do timer na tela
    desenhaTimer(GLUT_BITMAP_HELVETICA_18, time_gameText);
    glPopMatrix();
    
    glFlush();
}

void atualizarPosicaoEscudo() {
   if (shield.ativo && escudoGirando) {
        shield.angle += 0.2f; // Incrementa o ângulo para criar a rotação
        if (shield.angle >= 360.0f) shield.angle -= 360.0f; // Reseta o ângulo se ultrapassar 360 graus
    }
}


void inicializa() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    mosquitoes[0].dx = 10;
    mosquitoes[0].dy = 0;
    mosquitoes[0].life = 0;
    mosquitoes[0].velocidade = 0.2;
    
     for (int i = 1; i < 10; i++) {
        mosquitoes[i].dx = rand() % (2 * randomx + i*2) - randomx; // Randomiza dx entre -randomx e randomx
        mosquitoes[i].dy = rand() % (2 * randomy + i*3) - randomy; // Randomiza dy entre -randomy e randomy
        mosquitoes[i].life = 1;
        mosquitoes[i].velocidade = 0.1;
        count_mosquitoes++;
    }

    for (int i = 0; i < Max_shots; i++) {
        shots[i].ativo = 0;
    }
    
    ultimoTempoPicada = time(NULL) - 1;  // Inicializa como se tivesse passado 3 segundos

}

void colisao() {
    // Colisão com o Item
     dist = sqrt(pow(player.tx - ix, 2) + pow(player.ty - iy, 2));
    if (dist < tam && colide == 0) {
        colide = 1;
        score += 15;
        player.count_item += 1;
        printf("%d\n", player.count_item);
        if (player.count_item % 2 == 0) { // Ativa o escudo em cada segundo item coletado
            shield.ativo = 1;
        }
        count_time = 1;
        glutPostRedisplay();
    }
	// Colisão com o Escudo
    if (shield.ativo) {
        float escudoX = player.tx + shield.radius * cos(shield.angle);
        float escudoY = player.ty + shield.radius * sin(shield.angle);
        for (int i = 0; i < count_mosquitoes; i++) {
            dist = sqrt(pow(escudoX - mosquitoes[i].dx, 2) + pow(escudoY - mosquitoes[i].dy, 2));
            if (dist < tam / 2) {
                mosquitoes[i].life--;
                printf("Mosquito atingido pelo escudo! Vida restante: %d\n", mosquitoes[i].life);
                if (mosquitoes[i].life <= 0) {
                    // Remove o mosquito
                    for (int k = i; k < count_mosquitoes - 1; k++) {
                        mosquitoes[k] = mosquitoes[k + 1];
                    }
                    count_mosquitoes--;
                    score += 75;
                    i--; // Ajuste o índice para verificar o próximo mosquito corretamente
                    break;
                }
            }
        }
    }
	
	 time_t tempoAtual = time(NULL);
	 

    // Colisão com os mosquitos
    for (int i = 0; i < count_mosquitoes; i++) {
        dist = sqrt(pow(player.tx - mosquitoes[i].dx, 2) + pow(player.ty - mosquitoes[i].dy, 2));
        if (dist < tam && difftime(tempoAtual, ultimoTempoPicada) >= 2) {
            player.life--;
            printf("Vida do jogador %d\n", player.life);
            ultimoTempoPicada = tempoAtual; // Atualiza o tempo da última picada
            if (player.life == 0){
            	finaliza();
			}
        }

        // Colisão entre tiros e mosquitos
        for (int j = 0; j < Max_shots; j++) {
            if (shots[j].ativo) {
                dist = sqrt(pow(shots[j].x - mosquitoes[i].dx, 2) + pow(shots[j].y - mosquitoes[i].dy, 2));
                if (dist < tam / 2) {
                    mosquitoes[i].life--;
                    if (mosquitoes[i].life == 0){
                    	score += 75;
					}
                    shots[j].ativo = 0;
                    printf("Mosquito atingido! Vida restante: %d\n", mosquitoes[i].life);
                    if (mosquitoes[i].life <= 0) {
                        // Remove o mosquito
                        for (int k = i; k < count_mosquitoes - 1; k++) {
                            mosquitoes[k] = mosquitoes[k + 1];
                        }
                        count_mosquitoes--;
                        i--; // Ajuste o índice para verificar o próximo mosquito corretamente
                        break;
                    }
                }
            }
        }
    }
}


void atirar() {
    for (int i = 0; i < Max_shots; i++) {
        if (!shots[i].ativo) { // Encontre um tiro inativo
            shots[i].x = player.tx;
            shots[i].y = player.ty;
            shots[i].velocidade = 0.8; // Aumentar a velocidade dos tiros
            shots[i].ativo = 1;
            break;
        }
    }
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON) {
        if (state == GLUT_DOWN) {
            escudoGirando = true; // Inicia a rotação do escudo
        } else if (state == GLUT_UP) {
            escudoGirando = false; // Para a rotação do escudo
        }
    }
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
           atirar();
	    }
	} 
}


void moverTiros() {
    for (int i = 0; i < Max_shots; i++) {
        if (shots[i].ativo) {
            colisao();
            shots[i].x += shots[i].velocidade; // Mover os tiros mais rapidamente
            // Desative o tiro se ele sair da tela
            if (shots[i].x > n || shots[i].x < -n || shots[i].y > m || shots[i].y < -m) {
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

            if (distancia < tam) { // Se a distância for menor que o tamanho dos mosquitos, há uma colisão
                // Calcular o vetor de separação
                float overlap = tam - distancia;
                float separacaoX = (dx / distancia) * (overlap / 2);
                float separacaoY = (dy / distancia) * (overlap / 2);

                // Ajustar a posição dos dois mosquitos para evitar a colisão
                mosquitoes[i].dx -= separacaoX;
                mosquitoes[i].dy -= separacaoY;
                mosquitoes[j].dx += separacaoX;
                mosquitoes[j].dy += separacaoY;
            }
        }
    }
}

void teclado(unsigned char key, int x, int y) {
    keys[key] = true; // Marca a tecla como pressionada
    if (key == 'c') {
        atirar();
    }
}

void tecladoUp(unsigned char key, int x, int y) {
    keys[key] = false; // Marca a tecla como solta
}

void atualizarPosicaoJogador() {
    if (keys['a']) {
        if (player.tx > -n) player.tx -= player.velocidade;
    }
    if (keys['d']) {
        if (player.tx < n) player.tx += player.velocidade;
    }
    if (keys['w']) {
        if (player.ty < m) player.ty += player.velocidade;
    }
    if (keys['s']) {
        if (player.ty > -m) player.ty -= player.velocidade;
    }

    colisao();
    glutPostRedisplay();
}

void timer(int value) {
	count_timer_loop += timerloop;
	if (count_timer_loop >= 1000){// Convertendo milissegundos em segundos
	count_time_game++; // adicionando 1 segundo no contador do jogo
	count_timer_loop = 0;
	}
	
    // Mosquito persegue o jogador
    for (int i = 0; i < count_mosquitoes; i++) {
        if (mosquitoes[i].dx < player.tx) mosquitoes[i].dx += mosquitoes[i].velocidade;
        else if (mosquitoes[i].dx > player.tx) mosquitoes[i].dx -= mosquitoes[i].velocidade;
        if (mosquitoes[i].dy < player.ty) mosquitoes[i].dy += mosquitoes[i].velocidade;
        else if (mosquitoes[i].dy > player.ty) mosquitoes[i].dy -= mosquitoes[i].velocidade;
    }
	checarColisaoEntreMosquitos();
    moverTiros(); // Atualiza a posição dos tiros
    atualizarPosicaoJogador(); // Atualiza a posição do jogador com base nas teclas pressionadas
    atualizarPosicaoEscudo(); // Atualiza a posição do escudo

    // Verifica colisões após atualizar os tiros
    colisao();

    // Spawnando outro item a cada 5 segundos
    if (count_time == 10) {
        srand(time(NULL)); // Inicializa a semente randômica com o tempo atual
        ix = rand() % (2 * randomix + 1) - randomix; // Randomiza ix entre -n e n
        iy = rand() % (2 * randomiy + 1) - randomiy; // Randomiza iy entre -m e m
        colide = 0;
        glutDisplayFunc(desenha);
    }
    count_time++;

    // Spawnando outro mosquito após o jogador possuir 5 itens
    if (player.count_item == 2 * aux_count_mosquito) {
        if (count_mosquitoes < Max_mosquitoes) {
            aux_count_mosquito++;
            mosquitoes[count_mosquitoes].dx = rand() % (2 * randomx + 4) - randomx;
            mosquitoes[count_mosquitoes].dy = rand() % (2 * randomy + 6) - randomy;
            mosquitoes[count_mosquitoes].life = 1;
            mosquitoes[count_mosquitoes].velocidade = 0.2;
            count_mosquitoes++;
        }
    }

    glutTimerFunc(timerloop, timer, 0); // Aumentar a frequência do timer
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

    if (largura <= altura) 
    { 
        gluOrtho2D(-25.0f, 25.0f, -25.0f * altura / largura, 25.0f * altura / largura);
        win = 25.0f;
    }              
    else 
    {
        gluOrtho2D(-25.0f * largura / altura, 25.0f * largura / altura, -25.0f, 25.0f);
        win = 25.0f * largura / altura;           
    }
}

int main() {
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(900, 700);
    glutCreateWindow("Projeto Final - Computacao Grafica\n");
 	glutMouseFunc(mouse);
    glutKeyboardFunc(teclado);
    glutKeyboardUpFunc(tecladoUp); // Registra o callback para quando a tecla é solta
    glutDisplayFunc(desenha);
    glutTimerFunc(50, timer, 0); // Chama a função de timer a cada 50ms
    inicializa();
    glutReshapeFunc(AlteraTamanhoJanela); // Registra a função AlteraTamanhoJanela

    glutMainLoop();
    return 0;
}
