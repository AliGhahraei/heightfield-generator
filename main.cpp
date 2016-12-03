#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <time.h>

using namespace std;

#define CAMSPEED 0.01f // Camera Speed
#define CAMSPEED2 0.1f // Camera Speed

//Offset
int O = 30;
//Noise
int Ni = 1;
//Size of the chart
int n = 3, last = pow(2,n) + 1;
float A[33][33] = {0};

float mPosX = 16, mPosY = 10, mPosZ = 3; // Position
float mViewX = 0, mViewY = 0.5, mViewZ = 0; // Target to view
float mUpX = 0, mUpY = 1, mUpZ = 0; // Up position

float  mouse_x = 0, mouse_y = 0; //coordinates from mouse
float past_x = 0, past_y = 0;
bool mouse = false; // key for mouse
bool move_x = false, move_y = false, move_z = false; // simple moves flags
bool tilt = false, roll = false, pan = false; // transformations of camera
bool freeMove = false; // free camera movement
int iWidth, iHeight; // size of the window

void hacerMatriz()
{
    srand (time(NULL));
    A[0][0] = 1;
    A[0][last - 1] = 20;
    A[last - 1][0] = 5;
    A[last - 1][last - 1] = 200;
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
                A[(j-1)*2*d][(k-1)*2*d+d] = 1/2*(A[(j-1)*2*d][(k-1)*2*d]+A[(j-1)*2*d][k*2*d]);//+O*(2*aleatorio-1)*pow(2,(-Ni*n));
                aleatorio = (rand() % 100) / 100.0;
                A[j*2*d][(k-1)*2*d+d] = 1/2*(A[j*2*d][(k-1)*2*d]+A[j*2*d][k*2*d]);//+ O*(2*aleatorio-1)*pow(2,(-Ni*n));
                //Interpolate across columns
                aleatorio = (rand() % 100) / 100.0;
                A[(j-1)*2*d+d][(k-1)*2*d] = 1/2*(A[(j-1)*2*d][(k-1)*2*d]+A[j*2*d][(k-1)*2*d]);//+O*(2*aleatorio-1)*pow(2,(-Ni*n));
                aleatorio = (rand() % 100) / 100.0;
                A[(j-1)*2*d+d][k*2*d] = 1/2*(A[(j-1)*2*d][k*2*d]+A[j*2*d][k*2*d]);//+O*(2*aleatorio-1)*pow(2,(-Ni*n));
                //Interpolate the center
                aleatorio = (rand() % 100) / 100.0;
                A[(j-1)*2*d+d][(k-1)*2*d+d] = 1/4*(A[(j-1)*2*d][(k-1)*2*d+d]+A[j*2*d][(k-1)*2*d+d]+A[(j-1)*2*d+d][(k-1)*2*d]+A[(j-1)*2*d+d][k*2*d]);//+O*(2*aleatorio-1)*pow(2,(-Ni*n));

            }
        }
    }

    for(int i = 0; i < last; i++)
    {
        for (int j = 0; j < last; j++)
        {
            cout << A[i][j] << " ";
        }
        cout << endl;
    }
}

/* GLUT callback Handlers */

static void resize(int width, int height)
{
    const float ar = (float) width / (float) height;
    iWidth = width;
    iHeight = height;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-ar, ar, -1.0, 1.0, 2.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity() ;
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f(0.0, 1.0, 0.0);

    gluLookAt(mPosX, mPosY, mPosZ,
              mViewX, mViewY, mViewZ,
              mUpX, mUpY, mUpZ);

    glPushMatrix();
    for(int i = 0; i < last; i++)
    {
        glBegin(GL_QUAD_STRIP);
        for(int j = 0; j < last; j++)
        {

            glVertex3f(j, A[i+1][j], i+1);
            glVertex3f(j, A[i][j], i);
            glVertex3f(j + 1, A[i+1][j+1], i+1);
            glVertex3f(j + 1, A[i][j+1], i);

        }
        glEnd();
    }
    glPopMatrix();
    glutSwapBuffers();

}

void MoveCamera(float speed)
{
    float mAuxX = mViewX - mPosX;
    float mAuxY = mViewY - mPosY;
    float mAuxZ = mViewZ - mPosZ;

    mPosX = mPosX + mAuxX * speed;
    mPosZ = mPosZ + mAuxZ * speed;
    mViewX = mViewX + mAuxX * speed;
    mViewZ = mViewZ + mAuxZ * speed;
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
    mViewY = mViewY + speed;
}

void DollyCamera(float speed)
{
    mPosX = mPosX + speed;
    mViewX = mViewX + speed;
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

void mouse_Movement(int x, int y)
{
    int iSign = 1, iSignX = 1;
    past_x = mouse_x;
    past_y = mouse_y;
    mouse_x = x;
    mouse_y = y;
    iSign = (past_y > mouse_y)? 1 : -1 ;
    iSignX = (past_x > mouse_x)? 1 : -1 ;
    //cout << mouse_x << " " << mouse_y << endl;
    if(mouse)
    {
        if(move_x)
        {
            DollyCamera((float) 0.0001 * (mouse_x - (iWidth / 2.0)));
        }
        if(move_y)
        {
            BoomCamera((float) 0.0001 * (mouse_y - (iHeight / 2.0)));
        }
        if(move_z)
        {
            MoveCamera((float) 0.0001 * iSign * (mouse_x - (iWidth / 2.0)));
        }
        if(tilt)
        {
            TiltCamera((float) 0.0005 * iSign * (mouse_x - (iWidth / 2.0)));
        }
        if(roll)
        {
            RollCamera((float) 0.0001 * iSign * (mouse_x - (iWidth / 2.0)));
        }
        if(pan)
        {
            RotateView((float) 0.00001 * (mouse_x - (iWidth / 2.0)) );
        }
        if(freeMove)
        {
            mViewX = (float) 0.01 * (mouse_x - (iWidth / 2.0));
            mViewY = (float) 0.01 * (mouse_y - (iHeight / 2.0));
            mViewZ = 0.5;
        }

    }
}

void Keyboard_Input(int key, int x, int y)
{
    switch(key)
    {
        case GLUT_KEY_UP:
            //cout << "Arriba" << endl;
            MoveCamera(CAMSPEED);
            break;
        case GLUT_KEY_DOWN:
            //cout << "Abajo" << endl;
            MoveCamera(-CAMSPEED);
            break;
        case GLUT_KEY_RIGHT:
            //cout << "Derecha" << endl;
            RotateView(CAMSPEED);
            break;
        case GLUT_KEY_LEFT:
            //cout << "Abajo" << endl;
            RotateView(-CAMSPEED);
            break;
    }
    glutPostRedisplay();
}

static void key(unsigned char key, int x, int y)
{
    switch (key)
    {
        case 27 :
        case 'q':
            exit(0);
            break;

        case 'x':
            move_x = (move_x)? false : true;
            freeMove = false;
            cout << "move in x: " << move_x << endl;
            break;
        case 'y':
            move_y = (move_y)? false : true;
            move_z = false;
            freeMove = false;
            cout << "move in y: " << move_y << endl;
            break;
        case 'z':
            move_z = (move_z)? false : true;
            move_y = false;
            freeMove = false;
            cout << "move in z: " << move_z << endl;
            break;
        case 't':
            tilt = (tilt)? false : true;
            move_z = move_y = false;
            pan = roll = false;
            freeMove = false;
            cout << "move in tilt: " << tilt << endl;
            break;
        case 'p':
            pan = (pan)? false : true;
            move_x = false;
            tilt = roll = false;
            freeMove = false;
            cout << "move in pan: " << pan << endl;
            break;
        case 'r':
            roll = (roll)? false : true;
            move_z = move_y = false;
            pan = tilt = false;
            freeMove = false;
            cout << "move in roll: " << roll << endl;
            break;
        case 'f':
            freeMove = (freeMove)? false : true;
            move_x = move_y = move_z = false;
            roll = pan = tilt = false;
            cout << "Free Move: " << freeMove << endl;
            break;
        case 'm':
            mouse = (mouse)? false : true;
            cout << "move mouse: " << mouse << endl;
            break;
        case 'w':
            BoomCamera(CAMSPEED2);
            break;
        case 's':
            BoomCamera(-CAMSPEED2);
            break;
        case 'a':
            DollyCamera(-CAMSPEED2);
            break;
        case 'd':
            DollyCamera(CAMSPEED2);
            break;

    }

    glutPostRedisplay();
}

static void idle(void)
{
    // Clear Color and Depth Buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Reset transformations
    glLoadIdentity();
    // Set the camera
    gluLookAt(mPosX, mPosY, mPosZ,
              mViewX, mViewY, mViewZ,
              mUpX, mUpY, mUpZ);

    glutPostRedisplay();
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

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(Keyboard_Input);
    glutIdleFunc(idle);
    glutPassiveMotionFunc( mouse_Movement );

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

    return EXIT_SUCCESS;
}
