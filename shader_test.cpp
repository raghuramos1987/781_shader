////////////////////////////////////////////////////////
//
// 3D sample program
//
// Han-Wei Shen
//
////////////////////////////////////////////////////////

#include <iostream>
//#include <pngwriter.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <cmath>

#include <GL/glew.h>
#include <GL/glut.h> 
#include <GL/gl.h>
//#include <GL/glext.h>

#include "shader_test.h"
#include "bitmap.h"
#include "shaderSetup.h"
#define TITLE "781 Lab3"
#define X_RES 600
#define Y_RES 400
#define X_POS 600
#define Y_POS 400

GLhandleARB programObject;
GLhandleARB vertexShaderObject;
GLhandleARB fragmentShaderObject;
    
GLhandleARB VertexShaderObject = 0;
GLhandleARB FragmentShaderObject = 0;

GLcharARB *vertexShaderSource;
GLcharARB *fragmentShaderSource;


GLfloat globalMatrix[4][4] = {
    {1,0,0,0}, 
    {0,1,0,0},
    {0,0,1,0},
    {0,0,0,1}
}; 

int win_width, win_height, debug; 

float eye_x=0, eye_y=1.8, eye_z=.5; 

int press_x, press_y; 
int release_x, release_y; 
float x_angle = 0.0; 
float y_angle = 0.0; 
float scale_size = 1; 

int xform_mode = 0; 

#define XFORM_NONE    0 
#define XFORM_ROTATE  1
#define XFORM_SCALE 2

int show_axis=-1; 
int poly_fill = 0;

bool transform_global = true; 
bool use_glsl = true; 

////////////////////////////////////////////////////////
// 4 x 4 matrix inversion - this is a hack because I return the 
// result to a global ...
//
float result[4][4] = {
  {1,0,0,0},
  {0,1,0,0},
  {0,0,1,0},
  {0,0,0,1}
}; 

using namespace std;

GLuint texid;
GLubyte disp_map[317][324];
GLubyte grad_map[315][322];
BITMAPINFO *BitmapInfo; /* Bitmap information */
GLubyte    *BitmapBits; /* Bitmap data */
GLubyte* read_disp_map(char *argv[]);
bool read_file(char** argv);

///////////////////////////////////////////

void draw_axes()
{
  glLineWidth(3);        
  glBegin(GL_LINES); 
    glColor3f(1,0,0);    // red: x; green: y; blue: z
    glVertex3f(0,0,0); 
    glVertex3f(4,0,0); 
    glColor3f(0,1,0);  
    glVertex3f(0,0,0); 
    glVertex3f(0,4,0); 
    glColor3f(0,0,1); 
    glVertex3f(0,0,0);
    glVertex3f(0,0,4);
  glEnd(); 
  glLineWidth(1.0); 
}

///////////////////////////////////////

void set_lighting()
{
  GLfloat light_ambient[] = {1.0,1.0,1.0,1}; 
  GLfloat light_diffuse[] = {2.8,2.8,2.8,1};
  GLfloat light_specular[] = {1,1,1,1}; 

  GLfloat mat_specular[] = {.7, .7, .7,1}; 
  GLfloat mat_shine[] = {60}; 

  //  glShadeModel(GL_FLAT); 
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE); 
  glEnable(GL_NORMALIZE); 
  glEnable(GL_COLOR_MATERIAL);  
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE); 

  glLightfv(GL_LIGHT0,GL_AMBIENT, light_ambient); 
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse); 
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); 
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular); 
  glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shine); 

}

bool compare_float(float f1, float f2)
{
    float precision = 0.001;
    if (((f1 - precision) < f2) && 
    ((f1 + precision) > f2))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void DrawTorus()
{
    glPushMatrix();
    float u, v;
    Point v1, v2, v3, v4;
    Vector n1, n2, n3, n4;
    Vector temp1, temp2;
    glScalef(.05,.05,.05);
    glTranslatef(0, 5, 0);
    glRotatef(90, 1, 0, 0);
    GLfloat len;
    glColor3f(1,0,0); 
    float level = 10;
    //GLint loc = glGetAttribLocation(programObject, "tangent");
    //glBindAttribLocation(programObject, loc, "tangent");
    glBindTexture(GL_TEXTURE_2D, texid);
    for(v=0;v<(2*3.14);v+=0.628)
        for(u=0;u<(2*3.14);u+=0.628)
        {

            glBegin(GL_POLYGON); 
            //if((u ==0) && (v == 0))
            //{
                //glTexCoord2f (0.0, 0.0);
                //cout<<"here1\n";
            //}
            //else if(compare_float(u, (2*3.14 - 0.628)) && 
                    //compare_float(v, 0))
            //{
                //glTexCoord2f (1.0, 0.0);
                //cout<<"here2\n";
            //}
            //else if(compare_float(u, (2*3.14 - 0.628)) &&
                    //compare_float(v, (2*3.14 - 0.628)))
            //{
                //glTexCoord2f (1.0, 1.0);
                //cout<<"here3\n";
            //}
            //else if(compare_float(v, (2*3.14 - 0.628)) && (u == 0))
            //{
                //glTexCoord2f (0.0, 1.0);
                //cout<<"here4\n";
            //}
            v1.Assign(3 * cos (u) + cos (v) * cos (u), 
                      3 * sin (u) + cos (v) * sin (u),sin (v));
            v2.Assign(3 * cos (u+0.628) + cos (v) * cos (u+0.628),
                      3 * sin (u+0.628) + cos (v) * sin (u+0.628),
                      sin (v));
            v3.Assign(3 * cos (u+0.628) + cos (v+0.628) * cos (u+0.628),
                      3 * sin (u+0.628) + cos (v+0.628) * sin (u+0.628),
                      sin (v+0.628));
            v4.Assign(3 * cos (u) + cos (v+0.628) * cos (u),
                      3 * sin (u) + cos (v+0.628) * sin (u),
                      sin (v+0.628));
            temp1.AddSub(v4, v1, 0);
            temp2.AddSub(v2, v1, 0);
            n1.CrossProduct(temp1, temp2);
            n1.Normalize();
            glTexCoord2f (u/level, v/level);
            glNormal3f(n1.x, n1.y, n1.z);
            glVertex3f(v1.x, v1.y, v1.z);
            glTexCoord2f ((u+0.628)/level, v/level);
            glNormal3f(n1.x, n1.y, n1.z);
            glVertex3f(v2.x, v2.y, v2.z);
            glTexCoord2f ((u+0.628)/level, (v+0.628)/level);
            glNormal3f(n1.x, n1.y, n1.z);
            glVertex3f(v3.x, v3.y, v3.z);
            glTexCoord2f (u/level, (v+0.628)/level);
            glNormal3f(n1.x, n1.y, n1.z);
            glVertex3f(v4.x, v4.y, v4.z);
            glEnd();
        }
        //glTexCoord2f (0.0, 1.0);
        //cout<<"here4\n"; u = 0;
        //v1.Assign(3 * cos (u) + cos (v) * cos (u), 
                  //3 * sin (u) + cos (v) * sin (u),sin (v));
        //u+=0.628;
        //v2.Assign(3 * cos (u) + cos (v) * cos (u),
                  //3 * sin (u) + cos (v) * sin (u),sin (v));
        //v+=0.628;
        //v3.Assign(3 * cos (u) + cos (v) * cos (u),
                  //3 * sin (u) + cos (v) * sin (u),sin (v));
        //u-=0.628;
        //v4.Assign(3 * cos (u) + cos (v) * cos (u),
                  //3 * sin (u) + cos (v) * sin (u),sin (v));
        //v-=0.628;
        //temp1.AddSub(v4, v1, 0);
        //temp2.AddSub(v2, v1, 0);
        //n1.CrossProduct(temp1, temp2);
        //n1.Normalize();
        //glNormal3f(n1.x, n1.y, n1.z);
        //glVertex3f(v1.x, v1.y, v1.z);
    //x = 3 * cos (u) + cos (v) * cos (u)
    //y = 3 * sin (u) + cos (v) * sin (u)
    //z = sin (v) 
    //u = 0; v = 0;
    //getxyz();
    glPopMatrix();
}
//////////////////////////////////////////////////////

void display()
{
  GLfloat light_pos[] = {-.5,9,1,1}; 

  glClearColor(0,0,0,1); 
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  glEnable(GL_DEPTH_TEST);  
  glEnable(GL_LIGHTING); 
  glEnable(GL_LIGHT0); 
  glEnable (GL_TEXTURE_2D);

  set_lighting(); 

  //----------------------------------------------
  // now define projection and modelview matrix 
  glMatrixMode(GL_PROJECTION); 
  glLoadIdentity(); 
  gluPerspective(60, 1, .001, 10000); 

  glMatrixMode(GL_MODELVIEW); 
  glLoadIdentity(); 
  glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
  gluLookAt(eye_x,eye_y,eye_z,0,0,0,0,1,0);

  //apply the global transformation to the whole world 
  glMultMatrixf((const float*)globalMatrix); 

  // change the polygon drawing mode as well as the lighting mode 
  if (show_axis == 1) {
    glDisable(GL_LIGHTING); 
    draw_axes(); 
    glEnable(GL_LIGHTING); 
  }
  if (!poly_fill)glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  if (!poly_fill) glDisable(GL_LIGHTING); 
  else glEnable(GL_LIGHTING); 

//----------------------------------------------
// now draw some simiple objects in the scene
  glColor3f(0,1,0); 
  glPushMatrix(); 
  glTranslatef(0,0.03,0); 
  glScalef(1, .01, 1);       // floor
  glutSolidCube(1); 
  glPopMatrix(); 
  glColor3f(.9,0,0); 
  glPushMatrix();
  glTranslatef(0, 0.05, 0);
  //glBindAttribLocation(programObject, 10, "myAttrib");
  GLint loc = glGetAttribLocation(programObject, "tangent");
  glBindAttribLocation(programObject, loc, "tangent");
  glBindTexture(GL_TEXTURE_2D, texid);
  glBegin(GL_POLYGON); 
  glTexCoord2f (0.0, 1.0);
  glNormal3f(0, 1, 0);
  glVertexAttrib3f(loc, 1, 0, 0);
  glVertex3f(-.1, .0, -.1);
  glTexCoord2f (1.0, 1.0);
  glNormal3f(0, 1, 0);
  glVertexAttrib3f(loc, 1, 0, 0);
  glVertex3f(.1, .0, -.1);
  glTexCoord2f (1.0, 0.0);
  glNormal3f(0, 1, 0);
  glVertexAttrib3f(loc, 1, 0, 0);
  glVertex3f(.1, .0, .1);
  glTexCoord2f (0.0, 0.0);
  glNormal3f(0, 1, 0);
  glVertexAttrib3f(loc, 1, 0, 0);
  glVertex3f(-.1, .0, .1);
  glEnd();
  glPopMatrix();

  glColor3f(1,0,0); 
  glPushMatrix(); 
  glTranslatef(.3, 0.08, .3);     // cube object
  glutSolidCube(.1); 
  glPopMatrix(); 

  glColor3f(0,0,1); 
  glPushMatrix(); 
  glTranslatef(-.3, 0.12, -.3);   // sphere object
  glutSolidSphere(.1, 10, 10); 
  glPopMatrix(); 
//----------------------------------------------
// now draw my model(s)
// put your drawing code here ...
//----------------------------------------------
    DrawTorus();

// now swap buffer to draw
  glutSwapBuffers(); 
}

///////////////////////////////////////////////////////////////////
//          Mouse callbacks to specify transformations 
///////////////////////////////////////////////////////////////////
void mymouse(int button, int state, int x, int y)
{
  GLdouble winx, winy, winz; 
  GLdouble winx2, winy2, winz2; 

  if (transform_global == true) {
    if (state == GLUT_DOWN) {
      press_x = x; press_y = y; 
      if (button == GLUT_LEFT_BUTTON)
    xform_mode = XFORM_ROTATE; 
      else if (button == GLUT_RIGHT_BUTTON) 
    xform_mode = XFORM_SCALE; 
    }
    else if (state == GLUT_UP) {
      xform_mode = XFORM_NONE; 
    }
  }
}

/////////////////////////////////////////////////////////

void mymotion(int x, int y)
{
  float dx, dy, dz; 

  if (transform_global == true) {
    if (xform_mode==XFORM_ROTATE) {
      x_angle += (x - press_x)/5.0; 
      if (x_angle > 180) x_angle -= 360; 
      else if (x_angle <-180) x_angle += 360; 
      press_x = x; 
       
      y_angle += (y - press_y)/5.0; 
      if (y_angle > 180) y_angle -= 360; 
      else if (y_angle <-180) y_angle += 360; 
      press_y = y; 
    }
    else if (xform_mode == XFORM_SCALE){
      float old_size = scale_size;
      scale_size *= (1+ (y - press_y)/60.0); 
    if (scale_size <0) scale_size = old_size; 
      press_y = y; 
    }
    glMatrixMode(GL_MODELVIEW); 
    //  global transformation 
    glLoadIdentity(); 
    glRotatef(x_angle, 0, 1,0); 
    glRotatef(y_angle, 1,0,0); 
    glScalef(scale_size, scale_size, scale_size); 
    glGetFloatv(GL_MODELVIEW_MATRIX, (GLfloat*)globalMatrix); 
  }

  glutPostRedisplay(); 
}




///////////////////////////////////////////////////////////////

void mykey(unsigned char key, int x, int y)
{
    switch(key) 
    {
        case 27: free(BitmapBits);free(BitmapInfo);exit(1);
                 glDetachShader(programObject, fragmentShaderObject);
                 glDetachShader(programObject, vertexShaderObject);
                 glDeleteShader(fragmentShaderObject);
                 glDeleteShader(vertexShaderObject);
                 glDeleteProgram(programObject);
            break;
        case 'f': poly_fill = !poly_fill; 
            break; 
        case 'n': 
            if (show_axis == 1) show_axis=-1;
            else show_axis = 1; 
            break;
        case 's': use_glsl = !use_glsl; 
            if (use_glsl) glUseProgramObjectARB(programObject);
            else glUseProgramObjectARB(0);
          break; 
        case 'g': 
        if (transform_global==true) transform_global = false; 
        else transform_global = true; 
        break; 
    }
    glutPostRedisplay(); 
}


static void validateShader(GLuint shader, const char* file = 0) {
    const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    GLsizei length = 0;
    
    glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);
    if (length > 0) {
        cout << "Shader " << shader << " (" << (file?file:"") << 
            ") compile error: " << buffer << endl;
    }
}

static void validateProgram(GLuint program) {
    const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    GLsizei length = 0;
    
    memset(buffer, 0, BUFFER_SIZE);
    glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);
    if (length > 0)
        cout<< "Program " << program << " link error: " << buffer << endl;
    glValidateProgram(program);
    GLint status;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE)
        cout << "Error validating shader " << program << endl;
}
///////////////////////////////////////////////////////////////

void SetupGLSL(){

    programObject = glCreateProgramObjectARB();
    vertexShaderObject = 
            glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    fragmentShaderObject = 
            glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

    readShaderSource("test", &vertexShaderSource,
                         &fragmentShaderSource); 

    glShaderSourceARB(vertexShaderObject,1,
                          (const GLcharARB**)&vertexShaderSource,
                          NULL);
    glShaderSourceARB(fragmentShaderObject,1,
                          (const GLcharARB**)&fragmentShaderSource,
                          NULL);

    glCompileShaderARB(vertexShaderObject);
    validateShader(vertexShaderObject, "test.vert");
    glCompileShaderARB(fragmentShaderObject);
    validateShader(fragmentShaderObject, "test.frag");

    glAttachObjectARB(programObject, vertexShaderObject);
    glAttachObjectARB(programObject, fragmentShaderObject);

    glLinkProgramARB(programObject);
    validateProgram(programObject);
}

//void makeImage()
//{
    //int i, j;
    //for(i=0;i<315;i++)
        //for(j=0;j<322;j++)
        //{

        //}
    //return;
//}

///////////////////////////////////////////////////////////////

int main(int argc, char** argv) 
{
    glutInit(&argc, argv); 
    glutInitDisplayMode(GLUT_RGB|GLUT_DOUBLE|GLUT_DEPTH); 
    glutInitWindowSize(X_RES, Y_RES); 
    win_width = X_RES; win_height = Y_RES; 
    glutCreateWindow(TITLE);
    glutDisplayFunc(display); 
    glutMouseFunc(mymouse); 
    glutMotionFunc(mymotion);
    glutKeyboardFunc(mykey); 
    glewInit(); 
    read_disp_map(argv);
    //read_file(argv);
    if (glewGetExtension("GL_ARB_fragment_shader")      != GL_TRUE ||
        glewGetExtension("GL_ARB_vertex_shader")        != GL_TRUE ||
        glewGetExtension("GL_ARB_shader_objects")       != GL_TRUE ||
        glewGetExtension("GL_ARB_shading_language_100") != GL_TRUE)
    {
        fprintf(stderr,
                "Driver does not support OpenGL Shading Language\n");
        exit(1);
    }
    else fprintf(stderr, "GLSL supported and ready to go\n"); 
    SetupGLSL(); 
    glUseProgramObjectARB(programObject);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
    //makeImage();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &texid);
    glBindTexture(GL_TEXTURE_2D, texid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, BitmapBits);

    glutMainLoop(); 
}

GLubyte* read_disp_map(char *argv[])
{
    GLubyte read_this;
    int i,j,k;
    BitmapBits = LoadDIBitmap(argv[1], &BitmapInfo);
    cout<<BitmapInfo->bmiHeader.biWidth;
    for(i=0,j=0,k=0;i<(317*324*3);i++)
    {
        if(k == 324)
        {
            k = 0;
            j++;
        }
        if(((i+1)%3) == 0)
        {
            disp_map[j][k] = BitmapBits[i];
            k++;
        }
        //else
            //disp_map[j][k] = 0;
    }
}

//bool read_file(char** argv)
//{
    //unsigned int number = 8;
    //char header[8];
    //FILE *fp = fopen(file_name, "rb");
    //if (!fp)
    //{
        //return (ERROR);
    //}
    //fread(header, 1, number, fp);
    //is_png = !png_sig_cmp(header, 0, number);
    //if (!is_png)
    //{
        //return (NOT_PNG);
    //}
    //png_structp png_ptr = png_create_read_struct
       //(PNG_LIBPNG_VER_STRING, (png_voidp)NULL,
        //NULL, NULL);
    //if (!png_ptr)
        //return (ERROR);

    //png_infop info_ptr = png_create_info_struct(png_ptr);
    //if (!info_ptr)
    //{
        //png_destroy_read_struct(&png_ptr,
           //(png_infopp)NULL, (png_infopp)NULL);
        //return (ERROR);
    //}

    //png_infop end_info = png_create_info_struct(png_ptr);
    //if (!end_info)
    //{
        //png_destroy_read_struct(&png_ptr, &info_ptr,
          //(png_infopp)NULL);
        //return (ERROR);
    //}
    //if (setjmp(png_jmpbuf(png_ptr)))
    //{
        //png_destroy_read_struct(&png_ptr, &info_ptr,
           //&end_info);
        //fclose(fp);
        //return (ERROR);
    //}
    //png_init_io(png_ptr, fp);
    //png_set_sig_bytes(png_ptr, number);




    //FILE *fh = fopen(argv[0], "r");
    //if (fh == NULL)
        //return 0;
    //int cnt;char ch;
    //while ((ch = fgetc(fh)) != EOF)
        //cout<<ch<<" ";
    //cout<<endl;cin>>debug;
    //fclose(fh);
    //return 1;
//}
