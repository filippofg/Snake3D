#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "lib/structures.h"	// Strutture dati personalizzate necessarie
#include "lib/snake.h"		// Procedure per il serpente
#include "lib/readBMP.h"

//variabili globali per le texture 
struct BitMapFile *image = NULL;
struct BitMapFile *image2 = NULL;
static GLenum textureID[2];

char *fileName = "res/mela.bmp";
char *fileName2 = "res/skin.bmp";

// Coordinate ausiliarie per la griglia virtuale (serpente).
int snakeAuxCoords[2] = {0, 0};
// Coordinate ausiliarie per la griglia virtuale (serpente fantasma).
int ghostAuxCoords[2] = {0, 0};

// Coordinate di spostamento della testa del serpente.
float xMov = 0, 
	  yMov = 0;

// Punteggio.
int score = 0;
int timeRefresh = 5000;

Material snakeMaterial = {
    {0.4f, 0.45f, 0.3f,0.92},{0.3,0.35,0.3,0.92},{0.7,0.7,0.7,1.0},{120.0}
    // {1.0,0.7,0.1,0.92},{1.0,0.7,0.1,0.92},{1.0,0.7,0.5,1.0},{70.0}
};

Material fruitMaterial = {
	{0.3f, 0.33f, 0.4f,0.92},{0.7,0.7,0.7,0.92},{0.3,0.3,0.3,1.0},{50.0}
    //{0.8f, 0.1f, 0.05f,0.92},{1.0,0.5,0.5,0.92},{0.7,0.7,0.7,1.0},{100.0}
    // {1.0,0.7,0.1,0.92},{1.0,0.7,0.1,0.92},{1.0,0.7,0.5,1.0},{70.0}
};

// Coordinate iniziali nella griglia per i cubi.
int initialBlocks[INITIAL_BLOCK_NO][2] = { 
	{0, 0},
	{0, -1}, 
	{0, -2},
	{0, -3},
	{0, -4},
	{0, -5}
};

// Vertici dei cubi del serpente della scena.
GLfloat cubeVertexArray[FACES_NO*VERTEX_NO*3] = { 
	//first face
		0.25, 0.25, 0.25,
        0.25, -0.25, 0.25,
        -0.25, 0.25, 0.25,
		-0.25, -0.25, 0.25,
	//second face
		-0.25, 0.25, 0.25,
        -0.25, -0.25, 0.25,
        -0.25, 0.25, -0.25,
		-0.25, -0.25, -0.25,
	//third face
		-0.25, 0.25, -0.25,
        -0.25, -0.25, -0.25,
        0.25, 0.25, -0.25,
		0.25, -0.25, -0.25,
	//fourth face
		0.25, 0.25, -0.25,
        0.25, -0.25, -0.25,
        0.25, 0.25, 0.25,
		0.25, -0.25, 0.25,
	//fifth face (lid)
		0.25, 0.25, -0.25,
        0.25, 0.25, 0.25,
        -0.25, 0.25, -0.25,
		-0.25, 0.25, 0.25
};

//Vertici dei frutti della scena.

GLfloat textCoordBodyArray[FACES_NO*VERTEX_NO*3] = {
	//first face
		1.0, 1.0, 0.0,
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
		0.0, 0.0, 0.0,
	//second face
		1.0, 1.0, 0.0, 
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
		0.0, 0.0, 0.0,
	//third face
		1.0, 1.0, 0.0,  
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
		0.0, 0.0, 0.0,
	//fourth face
		1.0, 1.0, 0.0,  
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
		0.0, 0.0, 0.0,
	//fifth face (lid)
		1.0, 1.0, 0.0, 
        1.0, 0.0, 0.0,
        0.0, 1.0, 0.0,
		0.0, 0.0, 0.0
};
GLfloat tNormal[FACES_NO*VERTEX_NO*3] = {
	//first face normals
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	//second face normals
	1.0,0.0, 0.0,
	1.0,0.0, 0.0,
	1.0,0.0, 0.0,
	1.0,0.0, 0.0,
	//third face normals
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	//fourth face normals
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	//fifth face
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
};

// Vertici della griglia.
GLfloat matrixVertexArray[MATRIX_VERTEX_NO][3] = {
	{0.0, 0.0, -5.0},	// colonne
	{0.0, 0.0, 5.0},
	{-5.0, 0.0, 0.0},	// righe
	{5.0, 0.0, 0.0}
};

// Colori della griglia.
GLfloat matrixColorArray[MATRIX_VERTEX_NO][3] = {
	{0.5, 0.5, 0.5},
	{0.5, 0.5, 0.5},
	{0.5, 0.5, 0.5},
	{0.5, 0.5, 0.5},
};

// Lista collegata rappresentante il serpente.
Snake 	*head,			// Testa del serpente (primo elemento)
		*lastBlock;		// Ultimo blocco inserito

// Frutto della scena.
Cube 	fruit;

// Direzione inserita dall'utente.
static Input userDirection;

// Vertex Array Object per VBO.
unsigned int vao[2];

// VBO per blocchi del serpente.
unsigned int cubeBuffers[2];
// VBO per frutti.
unsigned int fruitBuffers[2];
// VBO per griglia.
unsigned int matrixBuffers[2];
// Array di indici per VBO.
GLint cubeVertexIndices[FACES_NO*VERTEX_NO];
GLint fruitVertexIndices[FACES_NO*VERTEX_NO];
GLint matrixVertexIndices[MATRIX_VERTEX_NO];

// Flags.
short int allowIncrementalSpeed = 1,
		  requestHelp			= 0,
		  fullscreen 			= 0;

/* Variabile di blocco della posizione:
 * l'input dell'utente relativo alla posizione
 * viene scartato se il serpente non ha ancora
 * completato lo spostamento.
 */
short int lock = 0;

int main(int argc, char** argv) {
	GLenum glewErr;

	// Controllo degli argomenti
	if(argc > 1) {
		for(int i = 1; i < argc; i++) {
			if(!strcmp(argv[i], "--no-incremental-speed"))
				allowIncrementalSpeed = 0;
			else if(!strcmp(argv[i], "--fullscreen"))
				fullscreen = 1;
			else
				requestHelp = 1;
		}
		if(requestHelp) {
			printHelp(argv[0]);
			exit(0);
		}
	}

	glutInit(&argc, argv);

	// Double buffering
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	// Definizione proprieta' finestra principale
	glutInitWindowSize(1024, 768);
	glutInitWindowPosition(800, 800);
	glutCreateWindow("Snake 3D");

	if(fullscreen)
		glutFullScreen();
	
	// Inizializzazione glew
	glewErr = glewInit();
	if (glewErr != GLEW_OK) {
		fprintf(stderr, "GLEW init failed: %s\n", glewGetErrorString(glewErr));
		exit(1);
	}

	image = readBMP(fileName);
    if (image == NULL) {
        printf("readBMP: Could not openfile: %s \n", fileName);
        exit(1);
    }

	image2 = readBMP(fileName2);
    if (image2 == NULL) {
        printf("readBMP: Could not openfile: %s \n", fileName2);
        exit(1);
    }
	
	// Gestione input da tastiera:
	glutSpecialFunc(keyInput);		// tasti speciali (GLUT_KEY_*) per movimento
	glutKeyboardFunc(exitHandler);	// tasti (ESC...) per uscita
	
	init();
	glutDisplayFunc(display);
	glutMainLoop();

	return 0;
}

// Stampa a video del messaggio di aiuto.
void printHelp(char* programName) {
	printf("Uso: %s [ARGOMENTI]\n", programName);
	printf("\nArgomenti:\n");
	printf("\t--help\t\t\tStampa a video questo messaggio ed esce dal programma.\n");
	printf("\t--no-incremental-speed\tDisabilita l'aumento della velocità del serpente ad ogni frutto mangiato.\n");
	printf("\t--fullscreen\t\tLancia il programma a schermo intero.\n");
	exit(0);
}

// Inizializzazione principale del programma.
void init() {
	GLenum glErr;

	initLight();

	initTexture();

	// Inizializzazione serpente
	snakeInit();

	// Seed per coordinate frutto
	srand(time(0));

	// Sfondo
	glClearColor(0.0, 0.0, 0.0, 1.0);
	
	// Depth buffer
	glEnable(GL_DEPTH_TEST);

	initVao();

	// Proiezione ortogonale
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-2.0,2.0,-2.0,2.0,1.0,10.0);

	// Controllo errori
    if ((glErr=glGetError()) != 0) {
        fprintf(stderr, "Errore = %d\n", glErr);
        //exit(-1);
		fprintf(stderr, "%s\n", gluErrorString(glErr));
    }
	gluLookAt(0.0, 5.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Animazione del serpente
	glutTimerFunc(timeRefresh/60, redisplayHelper, 0);
}

void initLight() {
	//define lighting
    float globAmb[] = {1.0, 1.0, 1.0, 1.0};
    float lightAmb[] = {0.8, 0.8,0.8, 1.0};
	float lightDif[] = {0.3, 0.3, 0.3, 1.0};
    float lightSpec[] = {0.2, 0.2, 0.2, 1.0};

    glEnable(GL_LIGHTING);
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpec);

    // Global ambient light.
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globAmb);

	// Enable two-sided lighting.
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
	// Enable local viewpoint.
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
};

void initTexture() {
	glGenTextures (2,textureID);
    
	//texture mela
    glBindTexture (GL_TEXTURE_2D, textureID[0]);
    glTexImage2D (GL_TEXTURE_2D,0,GL_RGBA,image->sizeX,image->sizeY,0,GL_RGBA,GL_UNSIGNED_BYTE,image->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//enable mipmap
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//texture corpo
    glBindTexture (GL_TEXTURE_2D, textureID[1]);
    glTexImage2D (GL_TEXTURE_2D,0,GL_RGBA,image2->sizeX,image2->sizeY,0,GL_RGBA,GL_UNSIGNED_BYTE,image2->data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //enable mipmap
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glEnable(GL_TEXTURE_2D);
}

// Inizializzazione della lista collegata del serpente.
void snakeInit() {
	// Inizializzazione lista vuota
	head 		= NULL;
	lastBlock 	= NULL;

	// Inserimento di un blocco in coda
	for(int i  = 0; i < INITIAL_BLOCK_NO; i++) {
		appendBlock(initialBlocks[i]);
		
		// Salvo la posizione della testa
		if(i == 0)
			head = lastBlock;
	}
	
	// Inizializzazione direzione iniziale
	userDirection = right;
}

// Aggiunta di un blocco al corpo del serpente.
void appendBlock(int *newCoordinates) {
	// Allocazione nuovo elemento
	Snake *aux = malloc(sizeof(Snake));
	
	// Copia dei dati
	aux->block.coords[0] = newCoordinates[0];
	aux->block.coords[1] = newCoordinates[1]; 

	// Inizializzazione del trigger per collisione
	aux->block.trigger = 0;
	
	// Imposto il nuovo elemento come ultimo
	aux->next = NULL;

	// Se la lista è vuota, imposto questo elemento come primo
	if(lastBlock != NULL)
		lastBlock->next = aux;
	lastBlock = aux;  
}

// VAO.
void initVao() {
	// Cubi
	/* buildVbo((GLfloat*)cubeVertexArray, FACES_NO*VERTEX_NO, NULL, (unsigned int*)cubeVertexIndices, (unsigned int*)cubeBuffers, 0);
	buildVbo((GLfloat*)fruitVertexArray, FACES_NO*VERTEX_NO, NULL, (unsigned int*)fruitVertexIndices, (unsigned int*)fruitBuffers, 1);
	buildVbo((GLfloat*)matrixVertexArray, MATRIX_VERTEX_NO, (GLfloat*)matrixColorArray, (unsigned int*)matrixVertexIndices, (unsigned int*)matrixBuffers, 2);
 */

	// Generazione di due buffer ID
	glGenVertexArrays(2, vao);

	// vao[0]: cubi
	glBindVertexArray(vao[0]);

	// VBO: abilitazione buffer per vertici e colori
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState (GL_TEXTURE_COORD_ARRAY);
    glEnableClientState (GL_NORMAL_ARRAY);

	glGenBuffers(2, cubeBuffers);
	// cubeBuffers[0]: vertici
	glBindBuffer(GL_ARRAY_BUFFER, cubeBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertexArray) + sizeof (textCoordBodyArray) + sizeof(tNormal), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cubeVertexArray), cubeVertexArray);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(cubeVertexArray), sizeof(textCoordBodyArray), textCoordBodyArray);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(cubeVertexArray) + sizeof(textCoordBodyArray), sizeof(tNormal), tNormal);

	// cubeBuffers[1]: indici
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeBuffers[1]);
	for(int i = 0; i < FACES_NO*VERTEX_NO; i++)
		cubeVertexIndices[i] = i;

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeVertexIndices), cubeVertexIndices, GL_STATIC_DRAW);

	glVertexPointer(3, GL_FLOAT, 0, 0);
	glTexCoordPointer(3, GL_FLOAT, 0, (GLvoid*)sizeof(textCoordBodyArray));
    glNormalPointer(GL_FLOAT, 0, tNormal);

	// vao[1]: frutti
	// glBindVertexArray(vao[1]);
	// glGenBuffers(2, fruitBuffers);

	// glEnableClientState(GL_VERTEX_ARRAY);

	// // fruitBuffers[0]: vertici
	// glBindBuffer(GL_ARRAY_BUFFER, fruitBuffers[0]);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(fruitVertexArray), NULL, GL_STATIC_DRAW);
	// glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(fruitVertexArray), fruitVertexArray);

	// // fruitBuffers[1]: indici
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fruitBuffers[1]);
	// for(int i = 0; i < FACES_NO * VERTEX_NO; i++)
	// 	fruitVertexIndices[i] = i;
	
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(fruitVertexIndices), fruitVertexIndices, GL_STATIC_DRAW);

	// glVertexPointer(3, GL_FLOAT, 0, 0);

	// vao[1]: griglia
	glBindVertexArray(vao[1]);
	glGenBuffers(2, matrixBuffers);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	// matrixBuffers[0]: vertici e colori
	glBindBuffer(GL_ARRAY_BUFFER, matrixBuffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(matrixVertexArray) + sizeof(matrixColorArray), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(matrixVertexArray), matrixVertexArray);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(matrixVertexArray), sizeof(matrixColorArray), matrixColorArray);

	// matrixBuffers[1]: indici
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, matrixBuffers[1]);
	for(int i = 0; i < MATRIX_VERTEX_NO; i++)
		matrixVertexIndices[i] = i;
	
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(matrixVertexIndices), matrixVertexIndices, GL_STATIC_DRAW);

	glVertexPointer(3, GL_FLOAT, 0, 0);
	glColorPointer(3, GL_FLOAT, 0, (GLvoid*)sizeof(matrixVertexArray));
}

/* void buildVbo(GLfloat *vertexArray, int vertexArraySize, GLfloat *colorArray, unsigned int *vertexIndices, unsigned int *buffer, int vaoIndex) {
	GLsizeiptr size;

	size = colorArray == NULL ? sizeof(vertexArray) : sizeof(vertexArray) + sizeof(colorArray);
	
	// debug
	switch(vaoIndex) {
		case 0:
			fprintf(stderr, "Vertex %d:\n", vaoIndex);
			for(int i = 0; i < FACES_NO*VERTEX_NO*3; i++) {
				fprintf(stderr, "%f ", vertexArray[i]);
			}
			break;
		case 1:
					fprintf(stderr, "Vertex %d:\n", vaoIndex);

			for(int i = 0; i < vertexArraySize*3; i++) {
				fprintf(stderr, "%f ", vertexArray[i]);
			}
			break;
		case 2:
					fprintf(stderr, "Vertex %d:\n", vaoIndex);

			for(int i = 0; i < vertexArraySize*3; i++) {
				fprintf(stderr, "%f ", vertexArray[i]);
			}
						fprintf(stderr, "\nColors %d:\n", vaoIndex);
			for(int i = 0; i < MATRIX_VERTEX_NO*3; i++) {
				fprintf(stderr, "%f ", colorArray[i]);
			}
			break;
		default:
			break;
	}
	fprintf(stderr, "\n\n");
	
	// vao[0]: cubi
	glBindVertexArray(vao[vaoIndex]);

	// VBO: abilitazione buffer per vertici e colori
	glEnableClientState(GL_VERTEX_ARRAY);
	if(colorArray != NULL)
		glEnableClientState(GL_COLOR_ARRAY);

	glGenBuffers(2, &buffer);
	// cubeBuffers[0]: vertici, colori
	glBindBuffer(GL_ARRAY_BUFFER, &buffer[0]);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertexArray), vertexArray);

	if(colorArray != NULL)
		glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertexArray), sizeof(colorArray), colorArray);

	// cubeBuffers[1]: indici
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, &buffer[1]);
	fprintf(stderr, "Indici: ");
	for(int i = 0; i < vertexArraySize; i++) {
		vertexIndices[i] = i;
		fprintf(stderr, "%d ", vertexIndices[i]);
	}
	fprintf(stderr, "\n");

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertexIndices), vertexIndices, GL_STATIC_DRAW);

	glVertexPointer(3, GL_FLOAT, 0, 0);
	if(colorArray != NULL)
		glColorPointer(3, GL_FLOAT, 0, (GLvoid*)(sizeof(vertexArray)));
}
*/
// Funzione per glutDisplayFunc.
void display() {
	GLfloat lightPos[4] = {0.0,0.5,-6,0.0};

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	drawGrill();
	// Input (movimento)
	processInput();

	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    
	glEnable(GL_LIGHT0);
	
	// Se il frutto è stato mangiato, ne creo un altro
	
	if(fruit.trigger)
		newFruit();

	glBindTexture(GL_TEXTURE_2D, textureID[0]);	
	// Frutto
	drawFruitHelper();
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, textureID[1]);
	// Serpente
	drawSnakeHelper();

	glBindTexture(GL_TEXTURE_2D, 0);

	// Disabilito le luci per scrivere lo score
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	//glBindTexture(GL_TEXTURE_2D, 0);
	writeScore();

	// movate
	glFlush();
	glFinish();

	// Double buffering
	glutSwapBuffers();

	lock = 0;
}

void drawGrill() {
	glBindVertexArray(vao[1]);
	for (int j = -N_COL/2; j<N_COL-N_COL/2; j++) {
		glPushMatrix();
			glTranslatef(j*CELL+STEP, 0, 0);
			glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (GLvoid*)0);
		glPopMatrix();
	}
	for (int i = -N_ROWS/2; i<N_ROWS-N_ROWS/2; i++) {
		glPushMatrix();
			glTranslatef(0, 0, i*CELL+STEP);
			glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, (GLvoid*)(sizeof(GLuint)*2));
		glPopMatrix();
	}
}

// Modifica la scena in base all'input dell'utente.
void processInput() {
	// Avanza la testa del serpente
	updateSnakeHead();

	// Se i blocchi collidono, allora il serpente muore e il gioco finisce
	if(isDying()) {
		printf("Game over!\n");
		printf("Punteggio ottenuto: %d\n", score);
		killSnake();
	}
	
	// Rilevamento collisione con il frutto: se la testa collide allora viene aggiunto un blocco
	if(detectCollision(fruit.coords, head->block.coords)) {
		fruit.trigger = 1;
		appendBlock(fruit.coords);
		// Aggiorna punteggio
		score += FRUIT_SCORE;
		// Aumenta il refresh rate se l'utente non l'ha disabilitato
		if(allowIncrementalSpeed)
			timeRefresh -= 100;
	}
}

// Aggiornamento della posizione della testa del serpente.
void updateSnakeHead() {
	/* Aggiornamento della posizione del serpente in base alle coordinate specificate
	 * dall'utente; gestione "effetto pacman"
	 */
	switch(userDirection) {
		case up:
			if( head->block.coords[1] < MATRIX_BORDER_TOP) { 
				yMov = 1;
                xMov = 0;
 			} else {
				yMov = MATRIX_BORDER_BOTTOM*2;
                xMov = 0;
			}
			break;
		case down:    
			if( head->block.coords[1] > MATRIX_BORDER_BOTTOM) {
				yMov = -1;
                xMov = 0;				
 			} else {
				yMov = MATRIX_BORDER_TOP*2;
				xMov =  0;
			}
			break;
		case left:
 			if( head->block.coords[0] > MATRIX_BORDER_LEFT) {
				xMov = -1;
                yMov = 0;
				
 			} else {
				xMov = MATRIX_BORDER_RIGHT*2;
				yMov = 0;
			}
			break;
		case right:
 			if( head->block.coords[0] < MATRIX_BORDER_RIGHT ) {
				xMov = 1;
                yMov = 0;
 			} else {
				xMov = MATRIX_BORDER_LEFT*2;
                yMov = 0;
			}
			break;
		default:
			break;
	}

	// Salvo la posizione originale della testa
	for(int i = 0; i < 2; i++)
		ghostAuxCoords[i] = head->block.coords[i];
	
	// Aggiorno la posizione della testa
    head->block.coords[0] += xMov;
    head->block.coords[1] += yMov;
}

// Rilevamento morte del serpente.
int isDying() {
	// Rilevamento collisione tra blocchi: se almeno due vertici sono in comune, i blocchi collidono e il serpente muore
	for(Snake *death = head->next; death != NULL; death = death->next) {
		if(detectCollision(head->block.coords, death->block.coords))
			return 1;
	}
	return 0;
}

// Deallocazione dei blocchi del serpente e uscita dal programma.
void killSnake() {
	Snake *ptr = head;
	// Deallocazione di tutti i blocchi del serpente
	while(ptr != NULL) {
		Snake* aux = ptr;
		ptr = ptr->next;
		free(aux);
	}
	// Uscita
	exit(0);
}

/* Rilevamento collisione tra blocchi.
 * Confronta le coordinate dei due blocchi: se coincidono
 * allora sono sulla stessa cella e collidono.
 */
int detectCollision(int *block1, int *block2) {
	return block1[0] == block2[0] && block1[1] == block2[1];
}

// Calcolo della posizione del frutto.
void newFruit() {
	int aux[2];
	short int collision;

	/* Effettua un controllo con la posizione di tutti i blocchi del serpente
	 * per non spawnarci sopra.
	 */
	do {
		collision = 0;

		for(int i = 0; i < 2; i++)
			aux[i] = -10+(int) rand() % 20;

		for(Snake* ptr = head; ptr != NULL; ptr = ptr->next) {
			if(detectCollision(aux, ptr->block.coords))
				collision = 1;
		}
	} while(collision);

	for(int i = 0; i < 2; i++)
		fruit.coords[i] = aux[i];
	
	fruit.trigger = 0;
}

// Preparazione al disegno del serpente nella scena.
void drawSnakeHelper() {
	// Specifico il materiale del serpente
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   snakeMaterial.matAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   snakeMaterial.matDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  snakeMaterial.matSpecular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, snakeMaterial.shine);

	// Specifico il rispettivo Vertex Array
	glBindVertexArray(vao[0]);

	for(Snake *ptr = head; ptr != NULL; ptr = ptr->next) {
		if(ptr != head) {
			// Corpo
			/* Per far sì che il serpente si muova, è necessario che
			 * l'i-esimo blocco segua la posizione di quello precedente:
			 * si effettua quindi uno scambio di valori
			 * tramite una variabile ausiliaria.
			 */
			for(int i = 0; i < 2; i++) {
				snakeAuxCoords[i] 	 = ptr->block.coords[i];
				ptr->block.coords[i] = ghostAuxCoords[i];
				ghostAuxCoords[i] 	 = snakeAuxCoords[i];
			}
		}
		// Disegno a video
		glPushMatrix();
			// Utilizzo l'offset specificato in coordinate x,y
			glTranslatef(ptr->block.coords[0]*CELL, 0 ,-ptr->block.coords[1]*CELL);
			// Prendo i vertici da disegnare dai VBO
			for (int indFace = 0; indFace < FACES_NO; indFace++)
				glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (GLvoid*)(indFace * VERTEX_NO * sizeof(GLuint)));
		glPopMatrix();
	}
}

// Preparazione al disegno del frutto nella scena.
void drawFruitHelper() {
	// Specifico il materiale del frutto
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   fruitMaterial.matAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   fruitMaterial.matDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  fruitMaterial.matSpecular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, fruitMaterial.shine);
	// Specifico l'array object per i vertici
	//glBindVertexArray(vao[1]);
	
	GLUquadric *quad;											
	quad = gluNewQuadric();										
	gluQuadricDrawStyle(quad, GLU_FILL);
	
	//glBindTexture(GL_TEXTURE_2D, textureID[0]);
	gluQuadricTexture(quad, GL_TRUE);
	gluQuadricNormals(quad, GLU_SMOOTH);
	
	glPushMatrix();
		glTranslatef(fruit.coords[0]*CELL, 0 ,-fruit.coords[1]*CELL);
		gluSphere(quad,0.4,100,100);
	glPopMatrix();

}

// Disegna a video il punteggio.
void writeBitmapString(void *font, char *string) {
	char *c;
   	for (c = string; *c != '\0'; c++) glutBitmapCharacter(font, *c);
}

void writeScore() {
	char stringScore[25];
	int xText,yText;

	xText = 3;
	yText = 8;

	glRasterPos3f(xText*CELL, yText*CELL, 0);
	glPushMatrix();
	// glTranslatef (xText*CELL,yText*CELL,0);
	snprintf(stringScore,25,"score : %d",score);
    writeBitmapString(GLUT_BITMAP_8_BY_13,stringScore);
	glPopMatrix();
};

// Input da tastiera.
void keyInput(int key, int x, int y){
	if(lock == 1)
		return;
	switch(key) {
		case GLUT_KEY_UP:
			if(userDirection == down) 
				break;
			userDirection = up;
			lock = 1;
			break;
		case GLUT_KEY_DOWN:
			if(userDirection == up) 
				break;
			userDirection = down;
			lock = 1;
			break;
		case GLUT_KEY_RIGHT:
			if(userDirection == left) 
				break;
			userDirection = right;
			lock = 1;
			break;
		case GLUT_KEY_LEFT:
			if(userDirection == right) 
				break;
			userDirection = left;
			lock = 1;
			break;
		default:
			break;
	}
}

// Gestione tasti per uscita.
void exitHandler(unsigned char key, int x, int y) {
	switch(key) {
		case 27:	// Esc
		case 'q':	// q
			// rilascio risorse e uscita
			killSnake();
		default:
			break;
	}
}

// Funzione helper per chiamata da glutTimerFunc.
void redisplayHelper(int val) {
	glutPostRedisplay();
	// Animazione del serpente
	glutTimerFunc(timeRefresh/60, redisplayHelper, 0);
}
