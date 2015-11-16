#ifndef GLSLPROGRAM_H
#define GLSLPROGRAM_H


#include <ctype.h>
#define _USE_MATH_DEFINES
#include <stdio.h>
#include <math.h>


#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <map>
#include <stdarg.h>

#ifndef GL_COMPUTE_SHADER
#define GL_COMPUTE_SHADER	0x91B9
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


inline int GetOSU( int flag ) {
    int i;
    glGetIntegerv( flag, &i );
    return i;
    }


void	CheckGlErrors( const char* );



class GLSLProgram {
    private:
        std::map<const char *, int>	AttributeLocs;
        const char *			Cfile;
        unsigned int		Cshader;
        const char *			Ffile;
        unsigned int		Fshader;
        const char *			Gfile;
        GLuint			Gshader;
        bool			IncludeGstap;
        GLenum			InputTopology;
        GLenum			OutputTopology;
        GLuint			Program;
        const char *			TCfile;
        GLuint			TCshader;
        const char *			TEfile;
        GLuint			TEshader;
        std::map<const char *, int>	UniformLocs;
        bool			Valid;
        const char *			Vfile;
        GLuint			Vshader;
        bool			Verbose;

        static int		CurrentProgram;

        void	AttachShader( GLuint );
        bool	CanDoBinaryFiles;
        bool	CanDoComputeShaders;
        bool	CanDoFragmentShaders;
        bool	CanDoGeometryShaders;
        bool	CanDoTessControlShaders;
        bool	CanDoTessEvaluationShaders;
        bool	CanDoVertexShaders;
        int	    CompileShader( GLuint );
        bool	CreateHelper( const char *, ... );
        int	    GetAttributeLocation( const char * );
        int	    GetUniformLocation( const char * );

    public:
////    GLSLProgram(void );
        GLSLProgram() {
            Verbose = false;
            InputTopology  = GL_TRIANGLES;
            OutputTopology = GL_TRIANGLE_STRIP;

            CanDoComputeShaders      = true; //IsExtensionSupported( "GL_ARB_compute_shader" );
            CanDoVertexShaders      = true; //IsExtensionSupported( "GL_ARB_vertex_shader" );
            CanDoTessControlShaders = true; //IsExtensionSupported( "GL_ARB_tessellation_shader" );
            CanDoTessEvaluationShaders = CanDoTessControlShaders;
            CanDoGeometryShaders    = true; //IsExtensionSupported( "GL_EXT_geometry_shader4" );
            CanDoFragmentShaders    = true; //IsExtensionSupported( "GL_ARB_fragment_shader" );
            CanDoBinaryFiles        = true; //IsExtensionSupported( "GL_ARB_get_program_binary" );
            /*

                fix to stop programs hanging jnc 19/8/2015

            */
//            fprintf(stderr, "Disabled because OpenGL 4 does not have all these symbols and check hangs\n");
//            fprintf( stderr, "Can do: " );
//            if( CanDoComputeShaders )		fprintf( stderr, "compute shaders, " );
//            if( CanDoVertexShaders )		fprintf( stderr, "vertex shaders, " );
//            if( CanDoTessControlShaders )		fprintf( stderr, "tess control shaders, " );
//            if( CanDoTessEvaluationShaders )	fprintf( stderr, "tess evaluation shaders, " );
//            if( CanDoGeometryShaders )		fprintf( stderr, "geometry shaders, " );
//            if( CanDoFragmentShaders )		fprintf( stderr, "fragment shaders, " );
//            if( CanDoBinaryFiles )			fprintf( stderr, "binary shader files " );
//            fprintf( stderr, "\n" );
            }
        ~GLSLProgram( void) {
            Verbose = true;
            };
        bool	Create( const char *, const char * = NULL, const char * = NULL, const char * = NULL, const char * = NULL, const char * = NULL );
        void	DispatchCompute( GLuint, GLuint = 1, GLuint = 1 );
        bool    Create_from_GLSL(const char *);
        bool	IsExtensionSupported( const char * );
        bool	IsNotValid( );
        bool	IsValid( );
        void	LoadBinaryFile( const char * );
        void	LoadProgramBinary( const char *, GLenum );
        void	SaveBinaryFile( const char * );
        void	SaveProgramBinary( const char *, GLenum * );
        void	SetAttribute( const char *, GLuint );
        void	SetGstap( bool );
        void	SetInputTopology( GLenum );
        void	SetOutputTopology( GLenum );
        void	SetUniform( const char *, int );
        void	SetUniform( const char *, unsigned int );
        void	SetUniform( const char *, float );
        void	SetUniform( const char *, float, float, float );
        void	SetUniform( const char *, float[3] );
        void	SetUniform( const char *, glm::vec3 );
        void	SetUniform( const char *, glm::vec4 );
        void    SetUniform( const char *, glm::mat3);
        void    SetUniform( const char *, glm::mat4);
        void	SetVerbose( bool );
        void	Use( );
        void	Use( GLuint );
        void	UseFixedFunction( );
    };

#endif		// #ifndef GLSLPROGRAM_H
