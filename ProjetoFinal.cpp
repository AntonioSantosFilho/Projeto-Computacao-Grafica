#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>

#define Max_mosquitoes 10
int win = 25;
GLfloat ix = 4, iy = 2;
int count_time = 1, count_m = 1, colide = 0, dist = 0, count_mosquitoes = 1, aux_count_mosquito = 1;
float tam = 2.0; // tamanho do jogador
float n = 28, m = 20;
int randomix = 20, randomiy = 20;
int randomx = 26, randomy = 26;

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

#define Max_shots 100 // Defina um número máximo para os tiros

typedef struct shot {
    GLfloat x, y;
    float velocidade;
    int ativo; // 0 = inativo, 1 = ativo
} Shot;

Shot shots[Max_shots];

// Definindo o jogador
Player player = {0, 0, 2.0, 3, 0};
// Definindo a quantidade máxima de mosquitos
Mosquito mosquitoes[Max_mosquitoes];

void desenha() {
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

    // Item - Objeto
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

    glFlush();
}

void inicializa() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    mosquitoes[0].dx = 6;
    mosquitoes[0].dy = 0;
    mosquitoes[0].life = 0;
    mosquitoes[0].velocidade = 0.2;

    for (int i = 0; i < Max_shots; i++) {
        shots[i].ativo = 0;
    }
}

void colisao() {
    // Colisão com o Item
    dist = sqrt(pow(player.tx - ix, 2) + pow(player.ty - iy, 2));
    if (dist < tam && colide == 0) {
        colide = 1;
        player.count_item += 1;
        printf("%d\n", player.count_item);
        count_time = 1;
        glutPostRedisplay();
    }

    // Colisão com os mosquitos
    for (int i = 0; i < count_mosquitoes; i++) {
        dist = sqrt(pow(player.tx - mosquitoes[i].dx, 2) + pow(player.ty - mosquitoes[i].dy, 2));
        if (dist < tam) {
            player.life--;
            printf("Vida do jogador %d\n", player.life);
            if (player.life == 0) exit(0);
        }

        // Colisão entre tiros e mosquitos
        for (int j = 0; j < Max_shots; j++) {
            if (shots[j].ativo) {
                dist = sqrt(pow(shots[j].x - mosquitoes[i].dx, 2) + pow(shots[j].y - mosquitoes[i].dy, 2));
                if (dist < tam / 2) {
                    mosquitoes[i].life--;
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
            shots[i].velocidade = 1.0; // Aumentar a velocidade dos tiros
            shots[i].ativo = 1;
            break;
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

void teclado(unsigned char key, int x, int y) {
    keys[key] = true; // Marca a tecla como pressionada
    if (key == 'o') {
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
    // Mosquito persegue o jogador
    for (int i = 0; i < count_mosquitoes; i++) {
        if (mosquitoes[i].dx < player.tx) mosquitoes[i].dx += mosquitoes[i].velocidade;
        else if (mosquitoes[i].dx > player.tx) mosquitoes[i].dx -= mosquitoes[i].velocidade;
        if (mosquitoes[i].dy < player.ty) mosquitoes[i].dy += mosquitoes[i].velocidade;
        else if (mosquitoes[i].dy > player.ty) mosquitoes[i].dy -= mosquitoes[i].velocidade;
    }

    moverTiros(); // Atualiza a posição dos tiros
    atualizarPosicaoJogador(); // Atualiza a posição do jogador com base nas teclas pressionadas

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
            mosquitoes[count_mosquitoes].dx = rand() % (2 * randomx + 1) - randomx;
            mosquitoes[count_mosquitoes].dy = rand() % (2 * randomy + 1) - randomy;
            mosquitoes[count_mosquitoes].life = 1;
            mosquitoes[count_mosquitoes].velocidade = 0.2;
            count_mosquitoes++;
        }
    }

    glutTimerFunc(60, timer, 0); // Aumentar a frequência do timer para 50ms
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

    glutKeyboardFunc(teclado);
    glutKeyboardUpFunc(tecladoUp); // Registra o callback para quando a tecla é solta
    glutDisplayFunc(desenha);
    glutTimerFunc(50, timer, 0); // Chama a função de timer a cada 50ms

    inicializa();

    glutReshapeFunc(AlteraTamanhoJanela); // Registra a função AlteraTamanhoJanela

    glutMainLoop();
    return 0;
}

