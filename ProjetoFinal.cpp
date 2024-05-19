#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>

#define Max_mosquitoes 10

GLfloat tx = -4, ty = 0;
int win = 25;
//GLfloat dx=6, dy=0;
GLfloat ix=4, iy=2;
int count_time = 1, count_m=1, count_item=0, colide=0, dist=0, life_mosquito=2, life_player=3, count_mosquitoes = 1, aux_count_mosquito=1;
float tam = 2.0; // tamanho do jogador
float n = 28, m = 20, velocidade = 2.0;
int randomix=20, randomiy=20;
int randomx = 26, randomy = 26;

// Fazendo uma struct para cada mosquito
typedef struct mosquito{
	GLfloat dx, dy;
	int life;
}Mosquito;

// Definindo a quantidade máxima de mosquitos
Mosquito mosquitoes [Max_mosquitoes];

void desenha() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glClear(GL_COLOR_BUFFER_BIT);

	// Gota - Jogador
	glPushMatrix();
	glTranslatef(tx, ty, 0.0f);
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
 	dist = sqrt(pow(tx - ix, 2) + pow(ty - iy, 2));
    if (dist < tam && colide == 0) {
    	colide = 1;
    	count_item += 1;
    	printf("%d", count_item);
    	count_time = 1;
        glutPostRedisplay();
    }

    // Colisao com os mosquitos
    for (int i=0; i<count_mosquitoes; i++){
    	dist = sqrt(pow(tx - mosquitoes[i].dx, 2) + pow (ty - mosquitoes[i].dy, 2));
    	if (dist < tam ) {
    		life_player --;
    		printf("Vida do jogador %d", life_player);
    		if (life_player == 0)exit(0);
    	}
	}
    
}

void teclado(unsigned char key, int x, int y){
	switch(key){
		case 'a':	
			if (tx < -n-1) tx =  -n-1;
			tx -= 1;
			colisao();
		break;
		
		case 'd':
			
			if (tx > n+1) tx = n+1;
			tx += 1;
			colisao();
		break;
		
		case 'w':
			
    		if (ty > m +tam) ty = m + tam;
			ty += 1;
			colisao();
		break;

		case 's':
			
    		if (ty < -m -tam) ty = -tam -m;
    		ty -= 1;
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
		if (mosquitoes[i].dx < tx)mosquitoes[i].dx += velocidade;
   		else if (mosquitoes[i].dx > tx)mosquitoes[i].dx -= velocidade;
		if (mosquitoes[i].dy < ty)mosquitoes[i].dy += velocidade;
    	else if (mosquitoes[i].dy > ty)mosquitoes[i].dy -= velocidade;
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
	if (count_item == 5 *aux_count_mosquito){
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

