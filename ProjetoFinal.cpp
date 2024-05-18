#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>

GLfloat tx = 0, ty = 0, win = 25;
GLfloat dx=6, dy=0;
GLfloat ix=4, iy=2;
int cont_time = 1, cont_m=1, cont_item=0, colide=0, dist=0, life_mosquito=2, life_player=3, aux_cont_mosquito = 1;
float tam = 2.0; // tamanho do jogador
float n = 28, m = 20, velocidade = 2.0;
int randomx=20, randomy=20;

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
	    
	// Triangulo - Mosquito
	glPushMatrix();
	glTranslatef(dx, dy, 0.0f); 
	glBegin(GL_TRIANGLES);
	    glColor3f(0.0, 0.0, 0.0);
	    glVertex2f(-tam/2, -tam/2);
	    glVertex2f(tam/2, -tam/2);
	    glVertex2f(0.0, tam/2);
	glEnd();
	glPopMatrix();
	
	
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
}
void colisao(){
	// Colisao com o Item
 	dist = sqrt(pow(tx - ix, 2) + pow(ty - iy, 2));
    if (dist < tam && colide == 0) {
    	colide = 1;
    	cont_item += 1;
    	printf("%d", cont_item);
    	cont_time = 1;
        glutPostRedisplay();
    }

    // Colisao com o mosquito
    dist = sqrt(pow(tx - dx, 2) + pow (ty - dy, 2));
    if (dist < tam ) {
    	life_player --;
    	if (life_player == 0)exit(0);
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
		
		case 'i':
            dx -= 2;
        break;

        case 'o':
            dy += 2;
        break;
		
		default:
		break;
	}
	
	glutPostRedisplay();	
		 
}

void timer(int value) {
// Que comece a perseguicao
	
    if (dx < tx)
        dx += velocidade;
    else if (dx > tx)
        dx -= velocidade;

    if (dy < ty)
        dy += velocidade;
    else if (dy > ty)
        dy -= velocidade;
	
// Spawnando outro item a cada 5 segundos
	if (cont_time == 10){
		srand(time(NULL)); // Inicializa a semente randômica com o tempo atual
        ix = rand() % (2 * randomx + 1) - randomx; // Randomiza ix entre -n e n
        iy = rand() % (2 * randomy + 1) - randomy; // Randomiza iy entre -m e m
		colide = 0;
		glutDisplayFunc(desenha);
	}
	cont_time++;
// Spawnando mosquito após o jogador possuir 5 itens

	if (cont_item == 5*aux_cont_mosquito){
		aux_cont_mosquito++;
		srand(time(NULL));
		printf ("Outro mosquito");
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

