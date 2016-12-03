#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define CAMSPEED 0.0001f // Camera Speed

//Offset
int O = 30;
//Noise
int Ni = 1;
//Size of the chart
int n = 5, last = pow(2,n) + 1;
float A[33][33] = {0};

// Directions
enum Direction {up, down, left, right};

float mPosX = 16, mPosY = 1, mPosZ = 3; // Position
float mViewX = 0, mViewY = 0, mViewZ = 0; // Target to view
float mUpX = 0, mUpY = 1, mUpZ = 0; // Up position

void hacerMatriz()
{
    srand (time(NULL));
    A[0][0] = -3;
    A[0][last - 1] = 7;
    A[last - 1][0] = 1;
    A[last - 1][last - 1] = -1;
    double aleatorio;
    //de 1 a longitud de la cuadricula
    for(int i = 1; i <= n; i++)
    {
        int N = last - 1;
        int d = floor( N / pow(2,i));
        //recorrido de la matriz ixj
        for (int j = 1; j <= pow(2,(i - 1)); j++)
        {
             for(int k = 1; k <= pow(2,(i - 1)); k++)
             {
                //Interpolate across rows
                aleatorio = (rand() % 100) / 100.0;
                A[(j-1)*2*d][(k-1)*2*d+d] = 1/2*(A[(j-1)*2*d][(k-1)*2*d]+A[(j-1)*2*d][k*2*d])+O*(2*aleatorio-1)*pow(2,(-Ni*n));
                aleatorio = (rand() % 100) / 100.0;
                A[j*2*d][(k-1)*2*d+d] = 1/2*(A[j*2*d][(k-1)*2*d]+A[j*2*d][k*2*d]) + O*(2*aleatorio-1)*pow(2,(-Ni*n));
                //Interpolate across columns
                aleatorio = (rand() % 100) / 100.0;
                A[(j-1)*2*d+d][(k-1)*2*d] = 1/2*(A[(j-1)*2*d][(k-1)*2*d]+A[j*2*d][(k-1)*2*d])+O*(2*aleatorio-1)*pow(2,(-Ni*n));
                aleatorio = (rand() % 100) / 100.0;
                A[(j-1)*2*d+d][k*2*d] = 1/2*(A[(j-1)*2*d][k*2*d]+A[j*2*d][k*2*d])+O*(2*aleatorio-1)*pow(2,(-Ni*n));
                //Interpolate the center
                aleatorio = (rand() % 100) / 100.0;
                A[(j-1)*2*d+d][(k-1)*2*d+d] = 1/4*(A[(j-1)*2*d][(k-1)*2*d+d]+A[j*2*d][(k-1)*2*d+d]+A[(j-1)*2*d+d][(k-1)*2*d]+A[(j-1)*2*d+d][k*2*d])+O*(2*aleatorio-1)*pow(2,(-Ni*n));

             }
        }
    }

    /*
    for(int i = 0; i < last; i++)
    {
        for (int j = 0; j < last; j++)
        {
            std::cout << A[i][j] << " ";
        }
        std::cout << std::endl;
    }
    */
}


/* GLUT callback Handlers */

static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

void MoveCamera(Direction direction){
    switch(direction) {
        case up:
            mPosY += CAMSPEED;
            break;
        case down:
            mPosY -= CAMSPEED;
            break;
        case left:
            mPosX += CAMSPEED;
            break;
        case right:
            mPosX -= CAMSPEED;
            break;
        default:
            std::cout << "Error";
    }
}

void idle() {
}

void RotateView(float speed)
{
    float vAuxX = mViewX - mPosX;
    float vAuxY = mViewY - mPosY;
    float vAuxZ = mViewZ - mPosZ;

    mViewZ = (float)(mPosZ + sin(speed)*vAuxX + cos(speed)*vAuxZ);
	mViewX = (float)(mPosX + cos(speed)*vAuxX - sin(speed)*vAuxZ);
}

void BoomCamera(float speed)
{
    mPosY = mPosY + speed;
    //mViewY = mViewY + speed;
}

void DollyCamera(float speed)
{
    mPosX = mPosX + speed;
    //mViewX = mViewX + speed;
}

void RollCamera(float speed)
{
    float vAuxX = mViewX - mPosX;
    float vAuxY = mViewY - mPosY;
    float vAuxZ = mViewZ - mPosZ;

    //mViewZ = (float)(mPosZ + sin(speed)*vAuxX + cos(speed)*vAuxZ);
	//mViewX = (float)(mPosX + cos(speed)*vAuxX - sin(speed)*vAuxZ);

    mViewX = (float)(mPosX + sin(speed)*vAuxY + cos(speed)*vAuxX);
	mViewY = (float)(mPosY + cos(speed)*vAuxY - sin(speed)*vAuxX);
}

void TiltCamera(float speed)
{
    mViewY = mViewY + (float) sin(speed);
    mUpY = mUpY + (float) sin(speed);
}

void Keyboard_Input(int key, int x, int y)
{
    switch(key)
    {
        case GLUT_KEY_UP:
            MoveCamera(up);
            break;
        case GLUT_KEY_DOWN:
            MoveCamera(down);
            break;
        case GLUT_KEY_RIGHT:
            MoveCamera(left);
            break;
        case GLUT_KEY_LEFT:
            MoveCamera(right);
            break;
    }
    glutPostRedisplay();
}

static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 'q':
            exit(0);
            break;
        case 'w':
            BoomCamera(CAMSPEED);
            break;
        case 's':
            BoomCamera(-CAMSPEED);
            break;
        case 'a':
            DollyCamera(-CAMSPEED);
            break;
        case 'd':
            DollyCamera(CAMSPEED);
            break;
        case 'r':
            RollCamera(-CAMSPEED);
            break;
        case 't':
            RollCamera(CAMSPEED);
            break;
        case 'f':
            TiltCamera(-CAMSPEED);
            break;
        case 'g':
            TiltCamera(CAMSPEED);
            break;
    }

    glutPostRedisplay();
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glColor3d(1,0,0);

    //glLoadIdentity();

    gluLookAt(mPosX, mPosY, mPosZ,
			  mViewX, mViewY, mViewZ,
			  mUpX, mUpY, mUpZ);


    glPushMatrix();
        glColor3f(0.0, 1.0, 0.0);
        //int i = 0, j = 0;
        for(int i = 0; i < last; i++)
        {
            for(int j = 0; j < last; j++)
            {

                glBegin(GL_QUAD_STRIP);
                glVertex3f(j, A[i+1][j], i+1);
                glVertex3f(j, A[i][j], i);
                glVertex3f(j+1, A[i+1][j+1], i+1);
                glVertex3f(j+1, A[i][j+1], i);
                glEnd();
            }
        }

    glPopMatrix();

    glutSwapBuffers();
}


const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };

/* Program entry point */

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(800,600);
    glutInitWindowPosition(10,10);
    hacerMatriz();
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Terrain");

    //glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(Keyboard_Input);

    glutIdleFunc(idle);

    glClearColor(1,1,1,1);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

    glutMainLoop();
}
