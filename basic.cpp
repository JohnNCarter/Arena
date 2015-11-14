#include <TinyG/TinyG.h>

#include "./GLSLProgram.h"
#include "./VertexBufferObject.h"

#include "Objects.h"

#include <TinyG/TweakBar2.h>

#include <vector>

// Routine to set a quaternion from a rotation axis and angle
// ( input axis = float[3] angle = float  output: quat = float[4] )
void SetQuaternionFromAxisAngle(const float *axis, float angle, float *quat) {
    float sina2, norm;
    sina2 = (float)sin(0.5f * angle);
    norm = (float)sqrt(axis[0]*axis[0] + axis[1]*axis[1] + axis[2]*axis[2]);
    quat[0] = sina2 * axis[0] / norm;
    quat[1] = sina2 * axis[1] / norm;
    quat[2] = sina2 * axis[2] / norm;
    quat[3] = (float)cos(0.5f * angle);
    }


// Routine to convert a quaternion to a 4x4 matrix
// ( input: quat = float[4]  output: mat = float[4*4] )
void ConvertQuaternionToMatrix(const float *quat, float *mat) {
    float yy2 = 2.0f * quat[1] * quat[1];
    float xy2 = 2.0f * quat[0] * quat[1];
    float xz2 = 2.0f * quat[0] * quat[2];
    float yz2 = 2.0f * quat[1] * quat[2];
    float zz2 = 2.0f * quat[2] * quat[2];
    float wz2 = 2.0f * quat[3] * quat[2];
    float wy2 = 2.0f * quat[3] * quat[1];
    float wx2 = 2.0f * quat[3] * quat[0];
    float xx2 = 2.0f * quat[0] * quat[0];
    mat[0*4+0] = - yy2 - zz2 + 1.0f;
    mat[0*4+1] = xy2 + wz2;
    mat[0*4+2] = xz2 - wy2;
    mat[0*4+3] = 0;
    mat[1*4+0] = xy2 - wz2;
    mat[1*4+1] = - xx2 - zz2 + 1.0f;
    mat[1*4+2] = yz2 + wx2;
    mat[1*4+3] = 0;
    mat[2*4+0] = xz2 + wy2;
    mat[2*4+1] = yz2 - wx2;
    mat[2*4+2] = - xx2 - yy2 + 1.0f;
    mat[2*4+3] = 0;
    mat[3*4+0] = mat[3*4+1] = mat[3*4+2] = 0;
    mat[3*4+3] = 1;
    }


// Routine to multiply 2 quaternions (ie, compose rotations)
// ( input q1 = float[4] q2 = float[4]  output: qout = float[4] )
void MultiplyQuaternions(const float *q1, const float *q2, float *qout) {
    float qr[4];
    qr[0] = q1[3]*q2[0] + q1[0]*q2[3] + q1[1]*q2[2] - q1[2]*q2[1];
    qr[1] = q1[3]*q2[1] + q1[1]*q2[3] + q1[2]*q2[0] - q1[0]*q2[2];
    qr[2] = q1[3]*q2[2] + q1[2]*q2[3] + q1[0]*q2[1] - q1[1]*q2[0];
    qr[3]  = q1[3]*q2[3] - (q1[0]*q2[0] + q1[1]*q2[1] + q1[2]*q2[2]);
    qout[0] = qr[0];
    qout[1] = qr[1];
    qout[2] = qr[2];
    qout[3] = qr[3];
    }




class MyApp: public TinyG::TinyG {
    public:
        MyApp(void) {
            }
        //~ VertexBufferObject VB;
        //~ GLSLProgram OO;
        std::vector<VertexBufferObject> things2draw;
        std::vector<GLSLProgram> things2shade;
        void SetBackgroundColour(float r, float g, float b, float alpha) {
            glClearColor(r, g, b, alpha);
            }



        GLSLProgram SetShaders(char *vert, char *frag) {
            GLSLProgram OO;
            OO.SetVerbose(true);
            OO.SetGstap(false);
            bool good = OO.Create(vert, frag);
            if( !good ) {
                fprintf( stderr, "GLSL Program wasn�t created.\n" );
                exit(0);
                }
            things2shade.push_back(OO);
            return OO;
            }

        void Render(VertexBufferObject VB, GLSLProgram OO, glm::mat4 MVP) {
            VB.SelectVAO();

            OO.Use();
            OO.SetAttribute( "aPosition", VB.vertexAtributesMap[VERTEX_ATTRIBUTE ]);
            OO.SetAttribute( "aColor", VB.vertexAtributesMap[COLOR_ATTRIBUTE ] );
            OO.SetUniform("uMVPmatrix", MVP); /* Bind/copy our modelmatrix (MVP) variable to be a uniform called uMVPmatrix in our shaderprogram */

            VB.Draw();
            OO.Use(0);
            VB.DeSelectVAO();
            glFlush();
            }

        //~ void Render(int object, int shader, glm::mat4 MVP) {
        //~ VertexBufferObject VB = things2draw[object];
        //~ GLSLProgram OO = things2shade[shader];
        //~ VB.SelectVAO();
        //~ OO.SetUniform("uMVPmatrix", MVP); /* Bind/copy our modelmatrix (MVP) variable to be a uniform called uMVPmatrix in our shaderprogram */
        //~ VB.Draw();
        //~ VB.DeSelectVAO();
        //~ glFlush();
        //~ }

        //~ void Render(float *quatern, VertexBufferObject VB) {
        //~ VB.SelectVAO();
        //~ OO.SetUniform("uMVPmatrix", MVP); /* Bind/copy our modelmatrix (MVP) variable to be a uniform called uMVPmatrix in our shaderprogram */
        //~ VB.Draw();
        //~ VB.DeSelectVAO();
        //~ glFlush();
        //~ }
    };



int main(void) {
    int wire = 0;
    int depth = 1;
    int autorotate = 1;
    int overhead = false;
    int quit_app = 0;
    float quat[4];
    float CameraX = 2.;
    float fov = 45.;
    int nbox = 4;
    MyApp app;
    int k = 0;
    float axis[3] = {1., 0., 0.};
    float raxis[3] = {1., .5, -.7};
    SetQuaternionFromAxisAngle(axis, 0., quat);
    app.StartUp("The Arena.");
    // Setup Drawing
    //
    VertexBufferObject Tetrahedron = makeWireTetrahedronMesh();
    VertexBufferObject Axis = makeWireAxisMesh(100.);
    VertexBufferObject Disk = makeWireDiskMesh(5.);
    VertexBufferObject Cube = makeWireCubeMesh();
    printf("VBO Cube number = %d\n", Cube.numVertices);
//    app.things2draw.push_back(Tetrahedron);
//    app.things2draw.push_back(Axis);
//    app.things2draw.push_back(Disk);
//    app.things2draw.push_back(Cube);
    GLSLProgram OO = app.SetShaders("vbo.vert", "vbo.frag");
//    GLSLProgram OO2 = app.SetShaders("vbo.vert", "vbo2.frag");
//    app.SetShaders(Tetrahedron);
//    app.SetShaders(Axis);
//    app.SetShaders(Disk);
//    app.SetShaders(Cube);
    app.SetBackgroundColour(0.5, 0.5, 0.5, 1.);
    // Setup UI
    //
    TweakBar::AntTweakBar bar;
    bar.Start("C", app.width, app.height);
    bar.Define(" GLOBAL help='This is a AntTweakBar program. Remember that TwDraw must come after the clear but before the swap buffers.' "); // Message added to the help bar.
    bar.AddVarRW("W", TW_TYPE_BOOL32, &wire, "label='Wireframe' key=w help='Toggle wireframe display mode.' "); // wireframe
    bar.AddVarRW("Z", TW_TYPE_BOOL32, &depth, "label='Z-Buffer' key=z help='Toggle z-buffer mode display mode.' "); // z-buffer
    bar.AddVarRW("OV", TW_TYPE_BOOL32, &overhead, "label='Overhead|Tank'  help='Go to overhead view.' "); // manual rotate flag
    bar.AddVarRW("CX", TW_TYPE_FLOAT, &CameraX, "label='Camera X'  help='Move Camera X.' "); // manual X
    bar.AddVarRW("FOV", TW_TYPE_FLOAT, &fov, "label='Field of View'  help='Control Field of view' "); // manual FOV
    bar.AddVarRW("AR", TW_TYPE_BOOL32, &autorotate, "label='Auto Rotate' key=r help='This applies continual rotation.' "); // auto rotate
    bar.AddVarRW("NB", TW_TYPE_INT32, &nbox, "label='Number of boxes' key=r help='This controls the number of boxes drawn.' "); // auto rotate
    bar.AddVarRW("RS", TW_TYPE_QUAT4F, &quat, "label='Rotate Shape'  help='Rotate this to spin object.' "); // manual rotate cpntrol
    bar.Space();
    bar.AddVarRW("Quit", TW_TYPE_BOOL32, &quit_app, "label='Quit' key=q help='Forces the program to quit.' "); // Quit
    bar.Define("C/CX min = 1. max = 10. step = 0.1");
    bar.Define("C/FOV min = 1. max = 60. step = 0.5");
    bar.Define("C/NB min =2 max = 16 step = 1");

    glEnable(GL_DEPTH_TEST);
    float aspect = 1./(float(app.height)/float(app.width));
    printf("Aspect Ratio = %f\n", aspect);

    while(!app.Stop()) {
        int object, shader;
        if(quit_app)
            break;
        if(depth)
            glEnable(GL_DEPTH_TEST);
        else
            glDisable(GL_DEPTH_TEST);
        if(wire)
            glPolygonMode( GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode( GL_FRONT_AND_BACK, GL_FILL);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if(autorotate) {
            float angle = (float) (k % 3600) / 10.;
            SetQuaternionFromAxisAngle(raxis, angle, quat);
            k++;
            }
        glm::mat4 rotation;
        ConvertQuaternionToMatrix(quat, glm::value_ptr(rotation)); //glm::value_ptr used to get pointer to underlying matrix data.
        glm::mat4 Projection = glm::perspective(glm::radians(fov), aspect, 0.1f, 100.0f);
        glm::mat4 Projection45 = glm::perspective(glm::radians(45.f), aspect, 0.1f, 100.0f);
//        glm::mat4  = glm::mat4(1.0);
        glm::mat4 TetraPosition = glm::translate(glm::mat4(1.), glm::vec3(0.f, 0.f, 0.0f));
        glm::mat4 CamPosition, MVP;
        if(overhead) {
            CamPosition = glm::lookAt(glm::vec3(0., 5., 25.), glm::vec3(0.), glm::vec3(1.0, 0., 0.));
            }
        else {
            CamPosition = glm::lookAt(glm::vec3(CameraX, 0., 2.5), glm::vec3(0., 0., 0.), glm::vec3(0., 0., 1.));
            }

        MVP = Projection * CamPosition * TetraPosition * rotation;
        app.Render(Tetrahedron, OO, MVP);
        MVP = Projection45 * CamPosition;
        app.Render(Axis, OO, MVP);
        app.Render(Disk, OO, MVP);

        float fnbox = (float) nbox;
        float sbox = 2.5 * 16. / (fnbox - 1.);

        for(float i = 0; i<fnbox; i++) {
            float angle = 2. * M_PI * i/ fnbox;
            glm::mat4 scaleCube = glm::mat4(1.0);//glm::scale( glm::vec3(2.0f, 1.f, 1.f));
            scaleCube[1][1] = sbox;
            glm::mat4 translateCube = glm::translate(glm::mat4(1.), glm::vec3(5.f, 0.f, 0.5f));
            glm::mat4 rotateCube = glm::rotate(glm::mat4(1.), angle, glm::vec3(0.f, 0.f, 1.f));
            MVP = Projection * CamPosition * rotateCube * translateCube * scaleCube;
            app.Render(Cube, OO, MVP);
            }
        bar.Draw();
        app.SwapBuffers();// Swap front and back rendering buffers
        }
    printf("Bye-Bye\n");
    }