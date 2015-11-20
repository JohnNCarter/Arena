/*
 * Canned objects
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


FILE *flog;

const double PI = 3.141592653589793;
const double PIo2 = PI/2.;
const double PIo4 = PI/4.;
const double PI2 = PI * 2.;
const float lod = PI/32.;

#include <GL/glew.h>
#include <GLFW/glfw.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "VertexBufferObject.h"
#include "glslprogram.h"
#include "stb_image.h"
#include "utils.h"

#include "Objects.h"


void norm_polygon(VertexBufferObject *C, int a, int b, int c, int d) {
    /*
     draw a polygon via global list of vertices
     Use pointer to VBO
     */

    /*
     * Bits of a cube
     */
    GLfloat vertices[][3] = {{-0.5,-0.5,-0.5},{0.5,-0.5,-0.5},
            {0.5,0.5,-0.5}, {-0.5,0.5,-0.5}, {-0.5,-0.5,0.5},
            {0.5,-0.5,0.5}, {0.5,0.5,0.5}, {-0.5,0.5,0.5}
        };

    GLfloat normals[][3] = {{-1.0,-1.0,-1.0},{1.0,-1.0,-1.0},
            {1.0,1.0,-1.0}, {-1.0,1.0,-1.0}, {-1.0,-1.0,1.0},
            {1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0}
        };

    GLfloat colors[][3] = {{0.0,0.0,0.0},{1.0,0.0,0.0},
            {1.0,1.0,0.0}, {0.0,1.0,0.0}, {0.0,0.0,1.0},
            {1.0,0.0,1.0}, {1.0,1.0,1.0}, {0.0,1.0,1.0}
        };
    printf("Polygon %s\n", C->vboName);
    // calculate center of polygon, divide by 4 then subtracct center coordinates
    glm::vec3 center;
    center = glm::vec3(vertices[a][0], vertices[a][1], vertices[a][2]);
    center += glm::vec3(vertices[b][0], vertices[b][1], vertices[b][2]);
    center += glm::vec3(vertices[c][0], vertices[c][1], vertices[c][2]);
    center += glm::vec3(vertices[d][0], vertices[d][1], vertices[d][2]);
    center = glm::normalize(center/ 4.0f); // Implied subtraction from centre of polygon.
    C->glColor3f(1., 1., 1.0);
    C->glVertex3fv(center);
    C->glColor3f(0., 1., 1.0); // shaded Normals
    C->glVertex3fv(glm::vec3(0.));
    }

VertexBufferObject makeNormals(void) {
    VertexBufferObject Normals;
    Normals.vboName = "Normals";
    printf("WireNormalsMesh\n");
    Normals.SetVerbose(true);
    Normals.CollapseCommonVertices( false );
    Normals.SetTol( .001f );// how close need to be to collapse vertices, ignored at the moment.
    Normals.UseBufferObjects(true); // Not needed as this is the only option.
    Normals.glBegin( GL_LINES);


    norm_polygon(&Normals, 0,3,2,1);
    norm_polygon(&Normals, 2,3,7,6);
    norm_polygon(&Normals, 0,4,7,3);
    norm_polygon(&Normals, 1,2,6,5);
    norm_polygon(&Normals, 4,5,6,7);
    norm_polygon(&Normals, 0,1,5,4);

    Normals.glEnd();
    Normals.Print();
    return Normals;
    }
void vbo_polygon(VertexBufferObject *C, int a, int b, int c, int d) {
    /*
     draw a polygon via global list of vertices
     Use pointer to VBO
     */

    /*
     * Bits of a cube
     */
    GLfloat vertices[][3] = {{-0.5,-0.5,-0.5},{0.5,-0.5,-0.5},
            {0.5,0.5,-0.5}, {-0.5,0.5,-0.5}, {-0.5,-0.5,0.5},
            {0.5,-0.5,0.5}, {0.5,0.5,0.5}, {-0.5,0.5,0.5}
        };

    GLfloat normals[][3] = {{-1.0,-1.0,-1.0},{1.0,-1.0,-1.0},
            {1.0,1.0,-1.0}, {-1.0,1.0,-1.0}, {-1.0,-1.0,1.0},
            {1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0}
        };

    GLfloat colors[][3] = {{0.0,0.0,0.0},{1.0,0.0,0.0},
            {1.0,1.0,0.0}, {0.0,1.0,0.0}, {0.0,0.0,1.0},
            {1.0,0.0,1.0}, {1.0,1.0,1.0}, {0.0,1.0,1.0}
        };
    printf("Polygon %s\n", C->vboName);
    // calculate center of polygon, divide by 4 then subtracct center coordinates
    glm::vec3 center;
    center = glm::vec3(vertices[a][0], vertices[a][1], vertices[a][2]);
    center += glm::vec3(vertices[b][0], vertices[b][1], vertices[b][2]);
    center += glm::vec3(vertices[c][0], vertices[c][1], vertices[c][2]);
    center += glm::vec3(vertices[d][0], vertices[d][1], vertices[d][2]);
    center = glm::normalize(center/ 4.0f); // Implied subtraction from centre of polygon.
    C->glNormal3fv(center);
    C->glVertex3fv(vertices[a]);
    C->glVertex3fv(vertices[b]);
    C->glVertex3fv(vertices[c]);
    C->glNormal3fv(center);
    C->glVertex3fv(vertices[c]);
    C->glVertex3fv(vertices[d]);
    C->glVertex3fv(vertices[a]);
    }

VertexBufferObject makeWireCubeMesh(void) {
    VertexBufferObject Cube;
    Cube.vboName = "Cube";
    printf("WireCubeMesh\n");
    Cube.SetVerbose(true);
    Cube.CollapseCommonVertices( false );
    Cube.SetTol( .001f );// how close need to be to collapse vertices, ignored at the moment.
    Cube.UseBufferObjects(true); // Not needed as this is the only option.
    Cube.glBegin( GL_TRIANGLES );

    Cube.glColor3f(0., 0., 1.0); // blue Cube

    vbo_polygon(&Cube, 0,3,2,1);
    vbo_polygon(&Cube, 2,3,7,6);
    vbo_polygon(&Cube, 0,4,7,3);
    vbo_polygon(&Cube, 1,2,6,5);
    vbo_polygon(&Cube, 4,5,6,7);
    vbo_polygon(&Cube, 0,1,5,4);

    Cube.glEnd();
    Cube.Print();
    return Cube;
    }

VertexBufferObject makeWireBoxMesh(void) {
    VertexBufferObject Box;
    Box.vboName = "Box";
    Box.SetVerbose(true);
    Box.CollapseCommonVertices( false );
    Box.SetTol( .001f );// how close need to be to collapse vertices, ignored at the moment.
    Box.UseBufferObjects(true); // Not needed as this is the only option.
    Box.glBegin( GL_LINES );

    Box.glColor3f(0., 0., 1.0); // blue box

    Box.glVertex3f(-0.5, -0.5, 0.); // West Wall
    Box.glVertex3f(-0.5, 0.5, 0.);

    Box.glVertex3f(-0.5, 0.5, 0.);  // North Wall
    Box.glVertex3f(0.5, 0.5, 0.);


    Box.glVertex3f(0.5, 0.5, 0.); // East Wall
    Box.glVertex3f(0.5, -0.5, 0.);


    Box.glVertex3f(0.5, -0.5, 0.); //South Wall
    Box.glVertex3f(-0.5, -0.5, 0.);



    Box.glEnd();
    Box.Print();
    return Box;
    }

VertexBufferObject makeWireCircleMesh(float radius) {
    VertexBufferObject Circle;
    Circle.vboName = "Circle";
    Circle.SetVerbose(true);
    Circle.CollapseCommonVertices( false );
    Circle.SetTol( .001f );// how close need to be to collapse vertices, ignored at the moment.
    Circle.UseBufferObjects(true); // Not needed as this is the only option.
    float theta;
    const float Radius = radius;
    Circle.glBegin( GL_LINES );
    Circle.glColor3f(1.0, 0., 0.);
    glm::vec3 p, n;
    for(theta = 0.0f; theta < PI2; theta += lod) {
        Circle.glVertex3f(Radius * cos(theta), Radius * sin(theta), 0.);
        Circle.glVertex3f(Radius * cos(theta + lod), Radius * sin(theta + lod), 0.);
        }
    Circle.glEnd();
    Circle.Print();
    Circle.makeObj("Circle.obj");
    return Circle;
    }
VertexBufferObject makeWireDiskMesh(float radius) {
    VertexBufferObject Disk;
    Disk.vboName = "Disk";
    Disk.SetVerbose(true);
    Disk.CollapseCommonVertices( false );
    Disk.SetTol( .001f );// how close need to be to collapse vertices, ignored at the moment.
    Disk.UseBufferObjects(true); // Not needed as this is the only option.
    float theta;
    const float Radius = radius;
    Disk.glBegin( GL_TRIANGLE_FAN );
    Disk.glColor3f(1.0, 1., 0.);
    glm::vec3 p, n;
    Disk.glVertex3f(0., 0., 0.); //Start point
    Disk.glVertex3f(Radius * cos(theta), Radius * sin(theta), 0.); // First Point
    for(theta = lod; theta < PI2; theta += lod) {
        Disk.glVertex3f(Radius * cos(theta), Radius * sin(theta), 0.);
        }
    Disk.glEnd();
    Disk.Print();
    Disk.makeObj("Disk.obj");
    return Disk;
    }
//VertexBufferObject makeWireDiskMesh(float radius) {
//    VertexBufferObject Disk;
//    Disk.vboName = "Disk";
//    Disk.SetVerbose(true);
//    Disk.CollapseCommonVertices( false );
//    Disk.SetTol( .001f );// how close need to be to collapse vertices, ignored at the moment.
//    Disk.UseBufferObjects(true); // Not needed as this is the only option.
//    float theta;
//    const float Radius = radius;
//    Disk.glBegin( GL_TRIANGLE_FAN );
//    Disk.glColor3f(1.0, 1., 0.);
//    glm::vec3 p, n;
//    Disk.glVertex3f(0., 0., 0.); //Start point
//    Disk.glVertex3f(Radius * cos(theta), Radius * sin(theta), 0.); // First Point
//    for(theta = lod; theta < PI2; theta += lod) {
//        Disk.glVertex3f(Radius * cos(theta), Radius * sin(theta), 0.);
//        }
//    Disk.glEnd();
//    Disk.Print();
//    Disk.makeObj("Disk.obj");
//    return Disk;
//    }
VertexBufferObject makeSolidDiskMesh(float radius) {
    VertexBufferObject Disk;
    Disk.vboName = "Disk";
    Disk.SetVerbose(true);
    Disk.CollapseCommonVertices( false );
    Disk.SetTol( .001f );// how close need to be to collapse vertices, ignored at the moment.
    Disk.UseBufferObjects(true); // Not needed as this is the only option.
    float theta;
    const float Radius = radius;
    Disk.glBegin( GL_TRIANGLE_FAN );
//    Disk.glColor3f(1.0, 1., 0.);
    Disk.glNormal3fv(glm::normalize(glm::vec3(.0, 1., 1.)));
    glm::vec3 p, n;
    Disk.glVertex3f(0., 0., 0.); //Start point
    Disk.glVertex3f(Radius * cos(theta), Radius * sin(theta), 0.); // First Point
    for(theta = lod; theta < PI2; theta += lod) {
        Disk.glVertex3f(Radius * cos(theta), Radius * sin(theta), 0.);
        }
    Disk.glEnd();
    Disk.Print();
    Disk.makeObj("Disk.obj");
    return Disk;
    }

VertexBufferObject makeWireAxisMesh(float Length) {
    VertexBufferObject Axis;
    Axis.vboName = "Axis";
    Axis.SetVerbose(true);
    Axis.CollapseCommonVertices( false );
    Axis.SetTol( .001f );// how close need to be to collapse vertices, ignored at the moment.
    Axis.UseBufferObjects(true); // Not needed as this is the only option.
    float theta;
    Axis.glBegin( GL_LINES );

    Axis.glColor3f(1.0, 0., 0.); // red is X
    Axis.glVertex3f(0., 0., 0.);
    Axis.glVertex3f(Length, 0., 0.);

    Axis.glColor3f(0.0, 1.0, 0.); // Green is Y
    Axis.glVertex3f(0., 0., 0.);
    Axis.glVertex3f(0., Length, 0.);

    Axis.glColor3f(.0, 0., 1.); // Blue is Z
    Axis.glVertex3f(0., 0., 0.);
    Axis.glVertex3f(0., 0., Length);

    Axis.glEnd();
    Axis.Print();
    Axis.makeObj("Axis.obj");
    return Axis;
    }

VertexBufferObject makeWireTetrahedronMesh(void) {
    VertexBufferObject Tetrahedron;
    struct Vertex {
        GLfloat position[4];
        GLfloat color[3];
        };

    struct Vertex tetrahedron[12] = {
            {{  0.25,  0.25,  0.25  }, {  1.0f,  0.0f,  0.0f  }},
            {{ -0.25, -0.25,  0.25  }, {  1.0f,  0.0f,  0.0f  }},
            {{ -0.25,  0.25, -0.25  }, {  1.0f,  0.0f,  0.0f  }},
            {{  0.25,  0.25,  0.25  }, {  0.0f,  1.0f,  0.0f  }},
            {{ -0.25, -0.25,  0.25  }, {  0.0f,  1.0f,  0.0f  }},
            {{  0.25, -0.25, -0.25  }, {  0.0f,  1.0f,  0.0f  }},
            {{  0.25,  0.25,  0.25  }, {  0.0f,  0.0f,  1.0f  }},
            {{ -0.25,  0.25, -0.25  }, {  0.0f,  0.0f,  1.0f  }},
            {{  0.25, -0.25, -0.25  }, {  0.0f,  0.0f,  1.0f  }},
            {{ -0.25, -0.25,  0.25  }, {  1.0f,  1.0f,  1.0f  }},
            {{ -0.25,  0.25, -0.25  }, {  1.0f,  1.0f,  1.0f  }},
            {{  0.25, -0.25, -0.25  }, {  1.0f,  1.0f,  1.0f  }}
        };

    Tetrahedron.vboName = "Tetrahedron";
    Print("Start Definng shape");
    Tetrahedron.SetVerbose(true);
    fprintf(stderr, "Starting up\n");
    Tetrahedron.CollapseCommonVertices( false );
    Tetrahedron.SetTol( .001f );
    Tetrahedron.UseBufferObjects(true);
    Tetrahedron.glBegin( GL_TRIANGLES );
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 3; j++) {
            int k = i * 3 + j;
            Tetrahedron.glVertex3fv(tetrahedron[k].position);
            Tetrahedron.glColor3fv(tetrahedron[k].color);
            }
        }
    Tetrahedron.glEnd();
    Tetrahedron.Print();

    return Tetrahedron;
    }
