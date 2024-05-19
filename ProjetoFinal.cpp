#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>

#define Max_mosquitoes 10
int win = 25;
//GLfloat dx=6, dy=0;
GLfloat ix=4, iy=2;
int count_time = 1, count_m=1, colide=0, dist=0, count_mosquitoes = 1, aux_count_mosquito=1;
float tam = 2.0; // tamanho do jogador
float n = 28, m = 20;
int randomix=20, randomiy=20;
int randomx = 26, randomy = 26;

// Fazendo uma struct para cada mosquito
typedef struct mosquito{
	GLfloat dx, dy;
	int life = 2;
	float velocidade = 2.0;
}Mosquito;

typedef struct player{
	GLfloat tx = 0, ty= 0;
	float velocidade = 2.0;
	int life = 3;
	int count_item = 0;
}Player;

// Definindo o jogador
Player player;
// Definindo a quantidade máxima de mosquitos
Mosquito mosquitoes [Max_mosquitoes];

void desenha() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT);

	// Gota - Jogador
	glPushMatrix();
	glTranslatef(player.tx, player.ty, 0.0f);
	glBegin(GL_QUADS);
	    glColor3f(0.0, 1.0, 1.0);
	    glVertex2f(-tam/2, -tam/2);
	    glVertex2f(tam/2, -tam/2);
	    glVertex2f(tam/2, tam/2);
	    glVertex2f(-tam/2, tam/2);
	glEnd();
	glPopMatrix();
	    
	// Triangulo - Mosquitos
	for (int i=0 ; i< count_mosquitoes; i++){
		glPushMatrix();
		glTranslatef(mosquitoes[i].dx, mosquitoes[i].dy, 0.0f); 
		glBegin(GL_TRIANGLES);
	   	 	glColor3f(0.0, 0.0, 0.0);
	    	glVertex2f(-tam/2, -tam/2);
	    	glVertex2f(tam/2, -tam/2);
	    	glVertex2f(0.0, tam/2);
		glEnd();
		glPopMatrix();
			
	}
	
	//Item - Objeto
    if (colide == 0){
	    glPushMatrix();
	    glTranslatef(ix,iy,0.0f);
	    glBegin(GL_QUADS);
	    	glColor3f(0.0,0.0,1.0);
	    	glVertex2f(-tam/3, -tam/3);
	        glVertex2f(tam/3, -tam/3);
	        glVertex2f(tam/3, tam/3);
	        glVertex2f(-tam/3, tam/3);
	    glEnd();
	    glPopMatrix();
	}
    
    glFlush();
}
void inicializa(){
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    mosquitoes[0].dx = 6;
    mosquitoes[0].dy = 0;
    mosquitoes[0].life = 2;
}
void colisao(){
	// Colisao com o Item
 	dist = sqrt(pow(player.tx - ix, 2) + pow(player.ty - iy, 2));
    if (dist < tam && colide == 0) {
    	colide = 1;
    	player.count_item += 1;
    	printf("%d", player.count_item);
    	count_time = 1;
        glutPostRedisplay();
    }

    // Colisao com os mosquitos
    for (int i=0; i<count_mosquitoes; i++){
    	dist = sqrt(pow(player.tx - mosquitoes[i].dx, 2) + pow (player.ty - mosquitoes[i].dy, 2));
    	if (dist < tam ) {
    		player.life--;
    		printf("Vida do jogador %d", player.life);
    		if (player.life == 0)exit(0);
    	}
	}
    
}

void teclado(unsigned char key, int x, int y){
	switch(key){
		case 'a':	
			if (player.tx < -n-1) player.tx =  -n-1;
			player.tx -= 1;
			colisao();
		break;
		
		case 'd':
			
			if (player.tx > n+1) player.tx = n+1;
			player.tx += 1;
			colisao();
		break;
		
		case 'w':
			
    		if (player.ty > m +tam) player.ty = m + tam;
			player.ty += 1;
			colisao();
		break;

		case 's':
			
    		if (player.ty < -m -tam) player.ty = -tam -m;
    		player.ty -= 1;
    		colisao();
		break;
		
		default:
		break;
	}
	
	glutPostRedisplay();	 
}

void timer(int value) {
// Mosquito persegue o jogador
	
	for (int i=0; i<count_mosquitoes; i++){
		if (mosquitoes[i].dx < player.tx)mosquitoes[i].dx += mosquitoes[i].velocidade;
   		else if (mosquitoes[i].dx > player.tx)mosquitoes[i].dx -= mosquitoes[i].velocidade;
		if (mosquitoes[i].dy < player.ty)mosquitoes[i].dy += mosquitoes[i].velocidade;
    	else if (mosquitoes[i].dy > player.ty)mosquitoes[i].dy -= mosquitoes[i].velocidade;
	}
    
// Spawnando outro item a cada 5 segundos
	if (count_time == 10){
		srand(time(NULL)); // Inicializa a semente randômica com o tempo atual
        ix = rand() % (2 * randomix + 1) - randomix; // Randomiza ix entre -n e n
        iy = rand() % (2 * randomiy + 1) - randomiy; // Randomiza iy entre -m e m
		colide = 0;
		glutDisplayFunc(desenha);
	}
	count_time++;
	
// Spawnando outro mosquito após o jogador possuir 5 itens
	if (player.count_item == 2 *aux_count_mosquito){
		if (count_mosquitoes < Max_mosquitoes){
			aux_count_mosquito++;
			mosquitoes[count_mosquitoes].dx = rand() % (2 * randomx + 1) - randomx;
			mosquitoes[count_mosquitoes].dy = rand() % (2 * randomy + 1) - randomy;
			mosquitoes[count_mosquitoes].life = 2;
			count_mosquitoes++;
		}
	}

    glutPostRedisplay();
    glutTimerFunc(500, timer, 0); // Define o temporizador para chamar a função novamente após 0.5 segundos
}

void AlteraTamanhoJanela(GLsizei w, GLsizei h)
{
	GLsizei largura, altura;
                   
	if(h == 0) h = 1;

	largura = w;
	altura = h;
                                              
	glViewport(0, 0, largura, altura);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (largura <= altura) 
	{ 
		gluOrtho2D (-25.0f, 25.0f, -25.0f*altura/largura, 25.0f*altura/largura);
		win = 25.0f;
	}              
	else 
	{
		gluOrtho2D (-25.0f*largura/altura, 25.0f*largura/altura, -25.0f, 25.0f);
		win = 25.0f*largura/altura;           
	}
}

int main(){
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(900, 700);
    glutCreateWindow("Projeto Final - Computacao Grafica\n");
    
    glutKeyboardFunc(teclado);
    glutDisplayFunc(desenha);
    glutTimerFunc(500, timer, 0);
    
    inicializa();
    
    glutReshapeFunc(AlteraTamanhoJanela); // Registra a função AlteraTamanhoJanela
   
    glutMainLoop();
    return 0;
}

