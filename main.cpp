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

#define CAMSPEED 0.1f // Camera Speed
#define CAMSPEED2 1.0f // Camera Speed
#define SCREENWIDTH 800
#define SCREENHEIGHT 600
#define BUTTONWIDTH 50
#define BUTTONHEIGHT 50
#define BUTTONNUMBER 12

int leftTop, rightTop, leftBottom, rightBottom;

//Offset
int O = 30;
//Noise
int Ni = 1;

//Size of the chart
int n = 5, MATRIX_LENGTH = pow(2,n) + 1;

//Buttons
float BUTTONS [BUTTONNUMBER][4] = {
        // WASD
        {0, SCREENHEIGHT - 2 * BUTTONHEIGHT},
        {2 * BUTTONWIDTH, SCREENHEIGHT - 2 * BUTTONHEIGHT},
        {BUTTONWIDTH, SCREENHEIGHT - BUTTONHEIGHT},
        {BUTTONWIDTH, SCREENHEIGHT - 3 * BUTTONHEIGHT},
        // Arrows
        {SCREENWIDTH - (3 * BUTTONWIDTH), SCREENHEIGHT - 2 * BUTTONHEIGHT},
        {SCREENWIDTH - BUTTONWIDTH, SCREENHEIGHT - 2 * BUTTONHEIGHT},
        {SCREENWIDTH - (2 * BUTTONWIDTH), SCREENHEIGHT - BUTTONHEIGHT},
        {SCREENWIDTH - (2 * BUTTONWIDTH), SCREENHEIGHT - 3 * BUTTONHEIGHT},
        // Roughness
        {3*SCREENWIDTH/8, SCREENHEIGHT - BUTTONHEIGHT},
        {3*SCREENWIDTH/8 + BUTTONWIDTH + 5, SCREENHEIGHT - BUTTONHEIGHT},
        // Size
        {5*SCREENWIDTH/8, SCREENHEIGHT - BUTTONHEIGHT},
        {(5*SCREENWIDTH/8) + BUTTONWIDTH + 5, SCREENHEIGHT - BUTTONHEIGHT},
};

float A[10000][10000] = {0};

float mPosX = MATRIX_LENGTH, mPosY = 10, mPosZ = 0; // Position
float mViewX = 0, mViewY = 0, mViewZ = 0; // Target to view
float mUpX = 0, mUpY = -1, mUpZ = 0; // Up position

float  mouse_x = 0, mouse_y = 0; //coordinates from mouse
float past_x = 0, past_y = 0;
bool mouse = false; // key for mouse
bool move_x = false, move_y = false, move_z = false; // simple moves flags
bool tilt = false, roll = false, pan = false; // transformations of camera
bool freeMove = false; // free camera movement
int iWidth, iHeight; // size of the window

void makeMatrix(float leftTop, float rightTop, float leftBottom, float rightBottom)
{

    srand (time(NULL));

    int lastIndex = MATRIX_LENGTH - 1;

    // Corner initialization
    A[0][0] = leftTop;
    A[0][lastIndex] = rightTop;
    A[lastIndex][0] = leftBottom;
    A[lastIndex][lastIndex] = rightBottom;

    // Random to add to each cell
    double random;

    // First cell to modify
    int interpolationStart = (int)floor(MATRIX_LENGTH/2);

    //Increment
    int increment = lastIndex - 1;

    for(int i = interpolationStart; i >= 1; i -= (int)ceil(i/2.0)){
        for(int j = i; j < MATRIX_LENGTH - 1; j+=increment){

            for(int k = i; k < MATRIX_LENGTH - 1; k+=increment){
                random = (rand()%100)/100;
                A[k + i][j] = (A[k+i][j+i] + A[k+i][j-i])/2 +O*(2*random-1)*pow(2,(-Ni*n));
                random = (rand()%100)/100;
                A[k - i][j] = (A[k-i][j+i] + A[k-i][j-i])/2 +O*(2*random-1)*pow(2,(-Ni*n));
                random = (rand()%100)/100;
                A[k][j + i] = (A[k+i][j+i] + A[k-i][j+i])/2 +O*(2*random-1)*pow(2,(-Ni*n));
                random = (rand()%100)/100;
                A[k][j - i] = (A[k+i][j-i] + A[k-i][j-i])/2 +O*(2*random-1)*pow(2,(-Ni*n));

                random = (rand()%100)/100;
                A[k][j] = (A[k+i][j] + A[k-i][j] + A[k][j + i] + A[k][j-i])/4
                + O*(2*random-1)*pow(2,(-Ni*n));
            }
        }

        interpolationStart /= 2;
        increment = (int)ceil(increment/2.0);
    }
}


void printMatrix(){
    for(int i = 0; i < MATRIX_LENGTH; i++)
    {
        for (int j = 0; j < MATRIX_LENGTH; j++)
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

void drawButton(float* button){
    glVertex2f(button[0], button[1]);
    glVertex2f(button[2], button[1]);
    glVertex2f(button[2], button[3]);
    glVertex2f(button[0], button[3]);
}

void drawButtons(){
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, SCREENWIDTH, SCREENHEIGHT, 0.0, -1.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glDisable(GL_CULL_FACE);

    glClear(GL_DEPTH_BUFFER_BIT);

    glColor3f(0.5, 0.5, 0.5);

    glBegin(GL_QUADS);
    for(int i = 0; i < BUTTONNUMBER; i++){
        drawButton(BUTTONS[i]);
    }
    glEnd();

    // Making sure we can render 3d again
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glColor3f(0.0, 1.0, 0.0);

    gluLookAt(mPosX, mPosY, mPosZ,
              mViewX, mViewY, mViewZ,
              mUpX, mUpY, mUpZ);

    glPushMatrix();
    glBegin(GL_TRIANGLES);
    for(int i = 0; i < MATRIX_LENGTH; i++)
    {

        for(int j = 0; j < MATRIX_LENGTH; j++)
        {
            // Fst
            glNormal3f(j, -A[i][j], i);
            glVertex3f(j, -A[i][j], i);

            glNormal3f(j, -A[i+1][j], i+1);
            glVertex3f(j, -A[i+1][j], i+1);

            glNormal3f(j + 1, -A[i][j+1], i);
            glVertex3f(j + 1, -A[i][j+1], i);

            // Snd
            glNormal3f(j + 1, -A[i][j+1], i);
            glVertex3f(j + 1, -A[i][j+1], i);

            glNormal3f(j, -A[i+1][j], i+1);
            glVertex3f(j, -A[i+1][j], i+1);

            glNormal3f(j + 1, -A[i+1][j+1], i+1);
            glVertex3f(j + 1, -A[i+1][j+1], i+1);
        }

    }
    glEnd();
    glPopMatrix();

    drawButtons();

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
            DollyCamera(CAMSPEED2);
            break;
        case 'd':
            DollyCamera(-CAMSPEED2);
            break;

    }

    glutPostRedisplay();
}

bool clicked(float* button, int x, int y){
    return (x>=button[0] && x<=button[2]) && (y>=button[1] && y<=button[3]);
}

void mouseEvent(int button, int state, int x, int y){
    bool isLeftMouseButton = button == GLUT_LEFT_BUTTON;
    bool isReleased = state == GLUT_UP;

    if(isLeftMouseButton && isReleased){
        if(clicked(BUTTONS[0], x, y)){
            cout << "a" << endl;
            DollyCamera(CAMSPEED2);
            glutPostRedisplay();
        }

        else if(clicked(BUTTONS[1], x, y)){
            cout << "d" << endl;
            DollyCamera(-CAMSPEED2);
            glutPostRedisplay();
        }

        else if(clicked(BUTTONS[2], x, y)){
            cout << "s" << endl;
            BoomCamera(-CAMSPEED2);
            glutPostRedisplay();
        }

        else if(clicked(BUTTONS[3], x, y)){
            cout << "w" << endl;
            BoomCamera(CAMSPEED2);
            glutPostRedisplay();
        }

        else if(clicked(BUTTONS[4], x, y)){
            cout << "izq" << endl;
            RotateView(-CAMSPEED);
            glutPostRedisplay();
        }

        else if(clicked(BUTTONS[5], x, y)){
            cout << "der" << endl;
            RotateView(CAMSPEED);
            glutPostRedisplay();
        }

        else if(clicked(BUTTONS[6], x, y)){
            cout << "abajo" << endl;
            MoveCamera(-CAMSPEED);
            glutPostRedisplay();
        }

        else if(clicked(BUTTONS[7], x, y)){
            cout << "arriba" << endl;
            MoveCamera(CAMSPEED);
            glutPostRedisplay();
        }

        else if(clicked(BUTTONS[8], x, y)){
            cout << "roughnessDown" << endl;
            if(O != 10){
                O-=10;
                makeMatrix(leftTop, rightTop, leftBottom, rightBottom);
                glutPostRedisplay();
            }
        }

        else if(clicked(BUTTONS[9], x, y)){
            cout << "roughnessUp" << endl;
            O+=10;
            makeMatrix(leftTop, rightTop, leftBottom, rightBottom);
            glutPostRedisplay();
        }

        else if(clicked(BUTTONS[10], x, y)){
            cout << "SizeDown" << endl;
            if(n != 1){
                n-=1;
                MATRIX_LENGTH = pow(2, n) + 1;
                makeMatrix(leftTop, rightTop, leftBottom, rightBottom);
                glutPostRedisplay();
            }
        }

        else if(clicked(BUTTONS[11], x, y)){
            cout << "SizeUp" << endl;
            n+=1;
            MATRIX_LENGTH = pow(2, n) + 1;
            makeMatrix(leftTop, rightTop, leftBottom, rightBottom);
            glutPostRedisplay();
        }
    }
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
/*  cin >> leftTop;
    cin >> rightTop;
    cin >> leftBottom;
    cin >> rightBottom;*/

    for(int i=0; i<BUTTONNUMBER; i++){
        BUTTONS[i][2] = BUTTONS[i][0] + BUTTONWIDTH;
        BUTTONS[i][3] = BUTTONS[i][1] + BUTTONHEIGHT;
    }

    for(int i=0; i < MATRIX_LENGTH; i++){
        for(int j=0; j < MATRIX_LENGTH; j++){
            A[i][j] = 0;
        }
    }

    /*
    for(int i=0; i<BUTTONNUMBER; i++){
        cout << "a" << BUTTONS[i][0] << "b" << BUTTONS[i][1] << "c" << BUTTONS[i][2] <<"d" << BUTTONS[i][3] << endl;
    }
     */

    leftTop = 7;
    rightTop = 3;
    leftBottom = 5;
    rightBottom = 4;

    glutInit(&argc, argv);
    glutInitWindowSize(SCREENWIDTH,SCREENHEIGHT);
    glutInitWindowPosition(10,10);


    makeMatrix(leftTop, rightTop, leftBottom, rightBottom);
    //printMatrix();

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Terrain");

    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutKeyboardFunc(key);
    glutSpecialFunc(Keyboard_Input);
    glutIdleFunc(idle);
    glutPassiveMotionFunc( mouse_Movement );
    glutMouseFunc(mouseEvent);

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
