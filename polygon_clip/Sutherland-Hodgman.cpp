#include <cstdio>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cstring>
using namespace std;

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

int winx0 = 200;
int winy0 = 100;
int winx1 = 500;
int winy1 = 400; 

const int MAX = 100;
typedef struct {float x; float y;}Vertex;
typedef struct {Vertex sp; Vertex ep;} Edge;
int height, width;

vector<Vertex> outvtx;
vector<Vertex> invtx;


bool inside(Vertex &testPt, Edge clip) {
    if (clip.ep.x > clip.sp.x) {
        // 裁剪边为窗口下边
        if (testPt.y >= clip.sp.y)
            return true;
    } else if (clip.ep.x < clip.sp.x) {
        // 裁剪边为窗口上边
        if (testPt.y <= clip.sp.y) 
            return true;
    } else if (clip.ep.y > clip.sp.y) {
        // 裁剪边为窗口右边
        if (testPt.x <= clip.sp.x)
            return true;
    } else if (clip.ep.y < clip.sp.y) {
        // 裁剪边为窗口左边
        if (testPt.x >= clip.sp.x)
            return true;
    }
    return false;
}

void Intersect(Vertex &S, Vertex &P, Edge clip, Vertex &intersectPt) {
    if (clip.sp.y == clip.ep.y) {
        // 水平裁剪边
        intersectPt.y = clip.sp.y;
        intersectPt.x = S.x + (clip.sp.y - S.y)*(P.x - S.x)/(P.y - S.y);
    } else {
        // 竖直裁剪边
        intersectPt.x = clip.sp.x;
        intersectPt.y = S.y + (clip.sp.x - S.x) * (P.y - S.y)/(P.x - S.x);
    }
}

void Sutherland_Hodgman(Edge clip) {
    Vertex S, P, ip;
    int j;
    // S = inarr[inlength - 1];
    S = invtx.at(invtx.size() - 1);
    for (j = 0; j < invtx.size(); j++) {
        printf("inarr %d\n", j);
        // P = inarr[j];
        P = invtx.at(j);
        if (inside(P, clip)) {
            if (inside(S, clip)) {
                // s in, p in
                outvtx.push_back(P);
                // outlength += 2;
                printf("(1) p(%f, %f) \n", P.x, P.y);
            } else {
                // s out, p in
                Intersect(S, P, clip, ip);
                // outlength += 4;
                outvtx.push_back(ip);
                outvtx.push_back(P);
                printf("(2) ip(%f, %f) \n", ip.x, ip.y);
                printf("(2) p(%f, %f) \n", P.x, P.y);
            }
        } else if (inside(S, clip)) {
            // s in, p out
            Intersect(S, P, clip, ip);
            outvtx.push_back(ip);
            // outlength += 2;
            printf("(3) ip(%f, %f) \n", ip.x, ip.y);
        }
        S = P; // 将当前点作为下一次的start point
    }
}

int main(int argc, char**argv) {
    
    // g++ -lglut -lGL -lGLU -lGLEW -lglfw Sutherland-Hodgman.cpp -o Sutherland-Hodgman
    // 编译命令
    // 修改输入的多边形, 修改 pointnum 和 invtx
    int pointnum = 3;
    Vertex v0 = {100, 200};
    Vertex v1 = {300, 460};
    Vertex v2 = {400, 50};
    // Vertex v0 = {50, 300};
    // Vertex v1 = {100, 10};
    // Vertex v2 = {300, 150};
    // Vertex v3 = {500, 450};
    
    invtx.push_back(v0);
    invtx.push_back(v1);
    invtx.push_back(v2);
    // invtx.push_back(v3);
    // 以上为修改部分

    // input polygon
    GLfloat polygon2[MAX];
    int pi = 0;
    for (int i = 0; i < invtx.size(); i++) {
        polygon2[pi++] = invtx[i].x;
        polygon2[pi++] = invtx[i].y;
        polygon2[pi++] = 0;
    }

    Vertex win0 = {winx0, winy0};
    Vertex win1 = {winx1, winy0};
    Vertex win2 = {winx1, winy1};
    Vertex win3 = {winx0, winy1}; 
    Edge e0 = {win0, win1};
    Edge e1 = {win1, win2};
    Edge e2 = {win2, win3};
    Edge e3 = {win3, win0};
    Edge clipEdges [] = {e0, e1, e2, e3};
    Sutherland_Hodgman(clipEdges[0]);
    for (int i = 1; i < 4; i++) {
        printf("-----------------clip: %d\n", i);

        int tmp = invtx.size();
        // pop 的时候存一下vector的大小
        for (int j = 0; j < tmp; j++) {
            invtx.pop_back();
        }
        for (int j = 0; j < outvtx.size(); j++) {
            invtx.push_back(outvtx.at(j));
        }
        tmp = outvtx.size();
        for (int j = 0; j < tmp; j++) {
            outvtx.pop_back();
        }
        for (int j = 0; j < invtx.size(); j++) {
            printf("in: %f, %f \n", invtx.at(j).x, invtx.at(j).y);
        }
        Sutherland_Hodgman(clipEdges[i]);
        
    }
  
    GLFWwindow *window;
    
    // Initialize the library
    if ( !glfwInit( ) )
    {
        return -1;
    }
    
    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow( SCREEN_WIDTH, SCREEN_HEIGHT, "Polygon fill", NULL, NULL );
    
    if ( !window )
    {
        glfwTerminate( );
        return -1;
    }
    
    // Make the window's context current
    glfwMakeContextCurrent( window );
    
    glViewport( 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT ); // specifies the part of the window to which OpenGL will draw (in pixels), convert from normalised to pixels
    glMatrixMode( GL_PROJECTION ); // projection matrix defines the properties of the camera that views the objects in the world coordinate frame. Here you typically set the zoom factor, aspect ratio and the near and far clipping planes
    glLoadIdentity( ); // replace the current matrix with the identity matrix and starts us a fresh because matrix transforms such as glOrpho and glRotate cumulate, basically puts us at (0, 0, 0)
    glOrtho( 0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 1 ); // essentially set coordinate system
    glMatrixMode( GL_MODELVIEW ); // (default matrix mode) modelview matrix defines how your objects are transformed (meaning translation, rotation and scaling) in your world
    glLoadIdentity( ); // same as above comment
    
    GLfloat polygonVertices[] =
    {
        winx1, winy1, 0,
        winx1, winy0, 0,
        winx0, winy0, 0,
        winx0, winy1, 0,
    };

    // clipped polygon
    GLfloat polygon3[MAX];
    pi = 0;
    for (int i = 0; i < outvtx.size(); i++) {
        polygon3[pi++] = outvtx[i].x;
        polygon3[pi++] = outvtx[i].y;
        polygon3[pi++] = 0;
    }

    // Loop until the user closes the window
    while ( !glfwWindowShouldClose( window ) )
    {
        glClearColor(1, 1, 1, 1);
        glClear( GL_COLOR_BUFFER_BIT );

        // render OpenGL here
        glEnableClientState( GL_VERTEX_ARRAY );
        glPolygonMode( GL_FRONT_AND_BACK, GL_LINE ); 

        // the rectangle
        glColor3f(0, 0, 0);
        glVertexPointer( 3, GL_FLOAT, 0, polygonVertices );
        glDrawArrays( GL_POLYGON, 0, 4 );
        
        // the input polygon
        glColor3f(0, 0, 1);
        glVertexPointer( 3, GL_FLOAT, 0, polygon2 );
        glDrawArrays( GL_POLYGON, 0, pointnum );
        glPolygonMode( GL_FRONT_AND_BACK, GL_FILL ); 

        // the clipped polygon
        glColor3f(1, 0, 0);
        glVertexPointer( 3, GL_FLOAT, 0, polygon3 );
        glDrawArrays( GL_POLYGON, 0, outvtx.size() );

        glDisableClientState( GL_VERTEX_ARRAY );
        
        // Swap front and back buffers
        glfwSwapBuffers( window );
        
        // Poll for and process events
        glfwPollEvents( );
    }
    
    glfwTerminate( );
    
    return 0;
}