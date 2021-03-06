#include "glslprogram.h"
#include <stdio.h>
#include <stdlib.h>

#define NVIDIA_SHADER_BINARY	0x00008e21		// nvidia binary enum

struct GLshadertype {
    const char *extension;
    GLenum name;
}
ShaderTypes [ ] = {
    { ".cs",   GL_COMPUTE_SHADER },
    { ".vert", GL_VERTEX_SHADER },
    { ".vs",   GL_VERTEX_SHADER },
    { ".frag", GL_FRAGMENT_SHADER },
    { ".fs",   GL_FRAGMENT_SHADER },
    { ".geom", GL_GEOMETRY_SHADER },
    { ".gs",   GL_GEOMETRY_SHADER },
    { ".tcs",  GL_TESS_CONTROL_SHADER },
    { ".tes",  GL_TESS_EVALUATION_SHADER },
};

struct GLbinarytype {
    const char *extension;
    GLenum format;
}
BinaryTypes [ ] = {
    { ".nvb",    NVIDIA_SHADER_BINARY },
};

extern const char *Gstap;		// set later

static const char *
GetExtension( const char *file ) {
    int n = (int)strlen(file) - 1;	// index of last non-null character

    // look for a '.':

    do {
        if( file[n] == '.' )
            return &file[n];	// the extension includes the '.'
        n--;
    } while( n >= 0 );

    // never found a '.':

    return NULL;
}




// this is what is exposed to the user
// file1 - file5 are defaulted as NULL if not given
// CreateHelper is a varargs procedure, so must end in a NULL argument,
//	which I know to supply but I'm worried users won't

bool
GLSLProgram::Create( const char *file0, const char *file1, const char *file2, const char *file3, const char * file4, const char *file5 ) {
    return CreateHelper( file0, file1, file2, file3, file4, file5, NULL );
}


// this is the varargs version of the Create method

bool
GLSLProgram::CreateHelper( const char *file0, ... ) {
    GLsizei n = 0;
    GLchar *buf;
    Valid = true;

    IncludeGstap = false;
    Cshader = Vshader = TCshader = TEshader = Gshader = Fshader = 0;
    Program = 0;
    AttributeLocs.clear();
    UniformLocs.clear();

    if( Program == 0 ) {
        Program = glCreateProgram( );
        CheckGlErrors( "glCreateProgram" );
    }

    va_list args;
    va_start( args, file0 );

    // This is a little dicey
    // There is no way, using var args, to know how many arguments were passed
    // I am depending on the caller passing in a NULL as the final argument.
    // If they don't, bad things will happen.

    const char *file = file0;
    int type;
    while( file != NULL ) {
        int maxBinaryTypes = sizeof(BinaryTypes) / sizeof(struct GLbinarytype);
        type = -1;
        const char *extension = GetExtension( file );
        // fprintf( stderr, "File = '%s', extension = '%s'\n", file, extension );

        for( int i = 0; i < maxBinaryTypes; i++ ) {
            if( strcmp( extension, BinaryTypes[i].extension ) == 0 ) {
                // fprintf( stderr, "Legal extension = '%s'\n", extension );
                LoadProgramBinary( file, BinaryTypes[i].format );
                break;
            }
        }

        int maxShaderTypes = sizeof(ShaderTypes) / sizeof(struct GLshadertype);
        for( int i = 0; i < maxShaderTypes; i++ ) {
            if( strcmp( extension, ShaderTypes[i].extension ) == 0 ) {
                // fprintf( stderr, "Legal extension = '%s'\n", extension );
                type = i;
                break;
            }
        }

        if( type < 0 ) {
            fprintf( stderr, "Unknown filename extension: '%s'\n", extension );
            fprintf( stderr, "Legal Extensions are: " );
            for( int i = 0; i < maxBinaryTypes; i++ ) {
                if( i != 0 )	fprintf( stderr, " , " );
                fprintf( stderr, "%s", BinaryTypes[i].extension );
            }
            fprintf( stderr, "\n" );
            for( int i = 0; i < maxShaderTypes; i++ ) {
                if( i != 0 )	fprintf( stderr, " , " );
                fprintf( stderr, "%s", ShaderTypes[i].extension );
            }
            fprintf( stderr, "\n" );
            Valid = false;
            goto cont;
        }

        GLuint shader;
        switch( ShaderTypes[type].name ) {
        case GL_COMPUTE_SHADER:
            if( ! CanDoComputeShaders ) {
                fprintf( stderr, "Warning: this system cannot handle compute shaders\n" );
                Valid = false;
                goto cont;
            }
            shader = glCreateShader( GL_COMPUTE_SHADER );
            break;

        case GL_VERTEX_SHADER:
            if( ! CanDoVertexShaders ) {
                fprintf( stderr, "Warning: this system cannot handle vertex shaders\n" );
                Valid = false;
                goto cont;
            }
            shader = glCreateShader( GL_VERTEX_SHADER );
            break;

        case GL_TESS_CONTROL_SHADER:
            if( ! CanDoTessControlShaders ) {
                fprintf( stderr, "Warning: this system cannot handle tessellation control shaders\n" );
                Valid = false;
                goto cont;
            }
            shader = glCreateShader( GL_TESS_CONTROL_SHADER );
            break;

        case GL_TESS_EVALUATION_SHADER:
            if( ! CanDoTessEvaluationShaders ) {
                fprintf( stderr, "Warning: this system cannot handle tessellation evaluation shaders\n" );
                Valid = false;
                goto cont;
            }
            shader = glCreateShader( GL_TESS_EVALUATION_SHADER );
            break;

        case GL_GEOMETRY_SHADER:
            if( ! CanDoGeometryShaders ) {
                fprintf( stderr, "Warning: this system cannot handle geometry shaders\n" );
                Valid = false;
                goto cont;
            }
            //glProgramParameteriEXT( Program, GL_GEOMETRY_INPUT_TYPE_EXT,  InputTopology );
            //glProgramParameteriEXT( Program, GL_GEOMETRY_OUTPUT_TYPE_EXT, OutputTopology );
            //glProgramParameteriEXT( Program, GL_GEOMETRY_VERTICES_OUT_EXT, 1024 );
            shader = glCreateShader( GL_GEOMETRY_SHADER );
            break;

        case GL_FRAGMENT_SHADER:
            if( ! CanDoFragmentShaders ) {
                fprintf( stderr, "Warning: this system cannot handle fragment shaders\n" );
                Valid = false;
                goto cont;
            }
            shader = glCreateShader( GL_FRAGMENT_SHADER );
            break;
        }


        // read the shader source into a buffer:

        FILE * in;
        int length;
        FILE * logfile;

        in = fopen( file, "rb" );
        if( in == NULL ) {
            fprintf( stderr, "Cannot open shader file '%s'\n", file );
            Valid = false;
            goto cont;
        }

        fseek( in, 0, SEEK_END );
        length = ftell( in );
        fseek( in, 0, SEEK_SET );		// rewind

        buf = new GLchar[length+1];
        n = 0;
        fread( buf, sizeof(GLchar), length, in );
        buf[length] = '\0';
        fclose( in ) ;

        GLchar *strings[2];

        if( IncludeGstap ) {
            strings[n] = (char *)Gstap;
            n++;
        }

        strings[n] = buf;
        printf("\nShader Source\n****\n%s\n****\n", strings[n]);
        n++;

        // Tell GL about the source:

        glShaderSource( shader, n, (const GLchar **)strings, NULL );
        delete [ ] buf;
        CheckGlErrors( "Shader Source" );

        // compile:

        glCompileShader( shader );
        GLint infoLogLen;
        GLint compileStatus;
        CheckGlErrors( "CompileShader:" );
        glGetShaderiv( shader, GL_COMPILE_STATUS, &compileStatus );

        if( compileStatus == 0 ) {
            fprintf( stderr, "Shader '%s' did not compile.\n", file );
            glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &infoLogLen );
            if( infoLogLen > 0 ) {
                GLchar *infoLog = new GLchar[infoLogLen+1];
                glGetShaderInfoLog( shader, infoLogLen, NULL, infoLog);
                infoLog[infoLogLen] = '\0';
                logfile = fopen( "glsllog.txt", "w");
                if( logfile != NULL ) {
                    fprintf( logfile, "\n%s\n", infoLog );
                    fclose( logfile );
                }
                fprintf( stderr, "\n%s\n", infoLog );
                delete [ ] infoLog;
            }
            glDeleteShader( shader );
            Valid = false;
            goto cont;
        } else {
            if( Verbose )
                fprintf( stderr, "Shader '%s' compiled.\n", file );

            glAttachShader( this->Program, shader );
        }



cont:
        // go to the next file:

        file = va_arg( args, char * );
    }

    va_end( args );

    // link the entire shader program:

    glLinkProgram( Program );
    CheckGlErrors( "Link Shader 1");

    GLchar* infoLog;
    GLint infoLogLen;
    GLint linkStatus;
    glGetProgramiv( this->Program, GL_LINK_STATUS, &linkStatus );
    CheckGlErrors("Link Shader 2");

    if( linkStatus == 0 ) {
        glGetProgramiv( this->Program, GL_INFO_LOG_LENGTH, &infoLogLen );
        fprintf( stderr, "Failed to link program -- Info Log Length = %d\n", infoLogLen );
        if( infoLogLen > 0 ) {
            infoLog = new GLchar[infoLogLen+1];
            glGetProgramInfoLog( this->Program, infoLogLen, NULL, infoLog );
            infoLog[infoLogLen] = '\0';
            fprintf( stderr, "Info Log:\n%s\n", infoLog );
            delete [ ] infoLog;

        }
        glDeleteProgram( Program );
        Valid = false;
    } else {
        if( Verbose )
            fprintf( stderr, "Shader Program linked.\n" );
        // validate the program:

        GLint status;
        glValidateProgram( Program );
        glGetProgramiv( Program, GL_VALIDATE_STATUS, &status );
        if( status == GL_FALSE ) {
            fprintf( stderr, "Program is invalid.\n" );
            Valid = false;
        } else {
            if( Verbose )
                fprintf( stderr, "Shader Program validated.\n" );
        }
    }

    return Valid;
}


void
GLSLProgram::DispatchCompute( GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z ) {
    Use( );
    glDispatchCompute( num_groups_x, num_groups_y, num_groups_z );
}


bool
GLSLProgram::IsValid( ) {
    return Valid;
}


bool
GLSLProgram::IsNotValid( ) {
    return ! Valid;
}


void
GLSLProgram::SetVerbose( bool v ) {
    Verbose = v;
}


void
GLSLProgram::Use( ) {
    Use( this->Program );
};


void
GLSLProgram::Use( GLuint p ) {
    if( p != (unsigned int) CurrentProgram ) {
        glUseProgram( p );
        CurrentProgram = p;
    }
};


void
GLSLProgram::UseFixedFunction( ) { // display nothing in core mode
    this->Use( 0 );
};


int
GLSLProgram::GetAttributeLocation( const char *name ) {
    std::map<const char *, int>::iterator pos;

    pos = AttributeLocs.find( (char *) name );
    if( pos == AttributeLocs.end() ) {
        AttributeLocs[name] = glGetAttribLocation( this->Program, name );
    }

    return AttributeLocs[name];
};

void	SaveProgramBinary( const char *, GLenum * );

void
GLSLProgram::SetAttribute( const char *name, GLuint which ) {
    int loc;
    this->Use();
//    printf("Starting Set Att. %d |%s|\n", which, name); // which is returned by vertexBufferOblect..vertexAtributesMap[??]
    loc = GetAttributeLocation( name );
    if( loc  >= 0 ) {
//        fprintf(stderr, "Bind %s [%d]\n", name, which);
        glEnableVertexAttribArray( loc );
        glBindAttribLocation(this->Program, which, name);
    } else {
        fprintf(stderr, "%s [%d] not found or bound (%d returned)\n", name, which, loc);
    }
}



int
GLSLProgram::GetUniformLocation( const char *name ) {
    std::map<const char *, int>::iterator pos;
    pos = UniformLocs.find( name );
//	fprintf(stderr, "set attributes %s, %d")
    if( pos == UniformLocs.end() ) {
        int t = glGetUniformLocation( this->Program, name );
        if(t == -1) {
            fprintf(stderr, "Location for unifowm %s not found\n", name)    ;
            exit(1);
        }
        UniformLocs[name] = t;
        CheckGlErrors( "glGetUniformLocation" );
    }

    return UniformLocs[name];
};


void
GLSLProgram::SetUniform( const char* name, int val ) { // integer
    int loc;
    if( ( loc = GetUniformLocation( name ) )  >= 0 ) {
        this->Use();
        glUniform1i( loc, val );
    }
};

void
GLSLProgram::SetUniform( const char* name, unsigned int val ) { //unsigned integer
    int loc;
    if( ( loc = GetUniformLocation( name ) )  >= 0 ) {
        this->Use();
        glUniform1i( loc, val );
    }
};


void
GLSLProgram::SetUniform( const char* name, float val ) { // single precision floating point
    int loc;
    if( ( loc = GetUniformLocation( name ) )  >= 0 ) {
        this->Use();
        glUniform1f( loc, val );
    }
};


void
GLSLProgram::SetUniform( const char* name, float val0, float val1, float val2 ) { //explicict vector x, y, z
    int loc;
    if( ( loc = GetUniformLocation( name ) )  >= 0 ) {
        this->Use();
        glUniform3f( loc, val0, val1, val2 );
    }
};

void
GLSLProgram::SetUniform( const char* name, glm::vec4 val ) { // vec3
    int loc;
    if( ( loc = GetUniformLocation( name ) )  >= 0 ) {
        this->Use();
        glUniform4f( loc, val[0], val[1], val[2], val[3] );
    }
};

void
GLSLProgram::SetUniform( const char* name, glm::vec3 val ) { // vec3
    int loc;
    if( ( loc = GetUniformLocation( name ) )  >= 0 ) {
        this->Use();
        glUniform3f( loc, val[0], val[1], val[2] );
    }
};


void
GLSLProgram::SetUniform( const char* name, float vals[3] ) { //array of 3 floats
    int loc;
    if( ( loc = GetUniformLocation( name ) )  >= 0 ) {
        this->Use();
        glUniform3fv( loc, 3, vals );
    }
};

/*
 * Note that for matrix operations using GLM
 * transpose is GL_FALSE, each matrix is assumed to be supplied in column major order.
 * see page 24 of manual (Version 0.9.6)
 */

void
GLSLProgram::SetUniform( const char* name, glm::mat4 m ) { // 4 by 4 matrix
    int loc;
    //fprintf(stderr, "Setting uniform %s (size %d)\n", name, sizeof(glm::mat4));
    if( ( loc = GetUniformLocation( name ) )  >= 0 ) {
        //fprintf(stderr, "uniform %s at %d\n", name, loc);
        this->Use();
        //fprintf(stderr, "Start of matrix %f, end %f\nPointex %p\n", m[0][0], m[3][3], glm::value_ptr(m));
        glUniformMatrix4fv( loc, 1, false, glm::value_ptr(m) );
    }
};
void
GLSLProgram::SetUniform( const char* name, glm::mat3 m ) { // 4 by 4 matrix
    int loc;
    //fprintf(stderr, "Setting uniform %s (size %d)\n", name, sizeof(glm::mat4));
    if( ( loc = GetUniformLocation( name ) )  >= 0 ) {
        //fprintf(stderr, "uniform %s at %d\n", name, loc);
        this->Use();
        //fprintf(stderr, "Start of matrix %f, end %f\nPointex %p\n", m[0][0], m[3][3], glm::value_ptr(m));
        glUniformMatrix3fv( loc, 1, false, glm::value_ptr(m) );
    }
};


void
GLSLProgram::SetInputTopology( GLenum t ) {
    if( t != GL_POINTS  && t != GL_LINES  &&  t != GL_LINES_ADJACENCY_EXT  &&  t != GL_TRIANGLES  &&  t != GL_TRIANGLES_ADJACENCY_EXT ) {
        fprintf( stderr, "Warning: You have not specified a supported Input Topology\n" );
    }
    InputTopology = t;
}


void
GLSLProgram::SetOutputTopology( GLenum t ) {
    if( t != GL_POINTS  && t != GL_LINE_STRIP  &&  t != GL_TRIANGLE_STRIP ) {
        fprintf( stderr, "Warning: You have not specified a supported Onput Topology\n" );
    }
    OutputTopology = t;
}




bool
GLSLProgram::IsExtensionSupported( const char *extension ) {
    // see if the extension is bogus:

    if( extension == NULL  ||  extension[0] == '\0' )
        return false;

    GLubyte *where = (GLubyte *) strchr( extension, ' ' );
    if( where != 0 )
        return false;

    // get the full list of extensions:

    const GLubyte *extensions = glGetString( GL_EXTENSIONS );

    for( const GLubyte *start = extensions; ; ) {
        where = (GLubyte *) strstr( (const char *) start, extension );
        if( where == 0 )
            return false;

        GLubyte *terminator = where + strlen(extension);

        if( where == start  ||  *(where - 1) == '\n'  ||  *(where - 1) == ' ' )
            if( *terminator == ' '  ||  *terminator == '\n'  ||  *terminator == '\0' )
                return true;
        start = terminator;
    }
    return false;
}


int GLSLProgram::CurrentProgram = 0;




#ifndef CHECK_GL_ERRORS
#define CHECK_GL_ERRORS
void
CheckGlErrors( const char* caller ) {
    unsigned int gle = glGetError();

    if( gle != GL_NO_ERROR ) {
        fprintf( stderr, "GL Error discovered from caller %s: ", caller );
        switch (gle) {
        case GL_INVALID_ENUM:
            fprintf( stderr, "Invalid enum.\n" );
            break;
        case GL_INVALID_VALUE:
            fprintf( stderr, "Invalid value.\n" );
            break;
        case GL_INVALID_OPERATION:
            fprintf( stderr, "Invalid Operation.\n" );
            break;
        case GL_STACK_OVERFLOW:
            fprintf( stderr, "Stack overflow.\n" );
            break;
        case GL_STACK_UNDERFLOW:
            fprintf(stderr, "Stack underflow.\n" );
            break;
        case GL_OUT_OF_MEMORY:
            fprintf( stderr, "Out of memory.\n" );
            break;
        }
        return;
    }
}
#endif



void
GLSLProgram::SaveProgramBinary( const char * fileName, GLenum * format ) {
    glProgramParameteri( this->Program, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE );
    GLint length;
    glGetProgramiv( this->Program, GL_PROGRAM_BINARY_LENGTH, &length );
    GLubyte *buffer = new GLubyte[length];
    glGetProgramBinary( this->Program, length, NULL, format, buffer );

    fprintf( stderr, "Program binary format = 0x%04x\n", *format );

    FILE * fpout = fopen( fileName, "wb" );
    if( fpout == NULL ) {
        fprintf( stderr, "Cannot create output GLSL binary file '%s'\n", fileName );
        return;
    }
    fwrite( buffer, length, 1, fpout );
    fclose( fpout );
    delete [ ] buffer;
}


void
GLSLProgram::LoadProgramBinary( const char * fileName, GLenum format ) {
    FILE *fpin = fopen( fileName, "rb" );
    if( fpin == NULL ) {
        fprintf( stderr, "Cannot open input GLSL binary file '%s'\n", fileName );
        return;
    }
    fseek( fpin, 0, SEEK_END );
    GLint length = (GLint)ftell( fpin );
    GLubyte *buffer = new GLubyte[ length ];
    rewind( fpin );
    fread( buffer, length, 1, fpin );
    fclose( fpin );

    glProgramBinary( this->Program, format, buffer, length );
    delete [ ] buffer;

    GLint   success;
    glGetProgramiv( this->Program, GL_LINK_STATUS, &success );

    if( !success ) {
        fprintf( stderr, "Did not successfully load the GLSL binary file '%s'\n", fileName );
        return;
    }
}



void
GLSLProgram::SetGstap( bool b ) {
    fprintf(stderr, "Pre 3.1 not supported\n");
    IncludeGstap = false;
}


const char *Gstap = {
    "#ifndef GSTAP_H\n\
#define GSTAP_H\n\
\n\
\n\
// gstap.h -- useful for glsl migration\n\
// from:\n\
//		Mike Bailey and Steve Cunningham\n\
//		\"Graphics Shaders: Theory and Practice\",\n\
//		Second Edition, AK Peters, 2011.\n\
\n\
\n\
\n\
// we are assuming that the compatibility #version line\n\
// is given in the source file, for example:\n\
// #version 400 compatibility\n\
\n\
\n\
// uniform variables:\n\
\n\
#define uModelViewMatrix		gl_ModelViewMatrix\n\
#define uProjectionMatrix		gl_ProjectionMatrix\n\
#define uModelViewProjectionMatrix	gl_ModelViewProjectionMatrix\n\
#define uNormalMatrix			gl_NormalMatrix\n\
#define uModelViewMatrixInverse		gl_ModelViewMatrixInverse\n\
\n\
// attribute variables:\n\
\n\
#define aColor				gl_Color\n\
#define aNormal				gl_Normal\n\
#define aVertex				gl_Vertex\n\
\n\
#define aTexCoord0			gl_MultiTexCoord0\n\
#define aTexCoord1			gl_MultiTexCoord1\n\
#define aTexCoord2			gl_MultiTexCoord2\n\
#define aTexCoord3			gl_MultiTexCoord3\n\
#define aTexCoord4			gl_MultiTexCoord4\n\
#define aTexCoord5			gl_MultiTexCoord5\n\
#define aTexCoord6			gl_MultiTexCoord6\n\
#define aTexCoord7			gl_MultiTexCoord7\n\
\n\
\n\
#endif		// #ifndef GSTAP_H\n\
\n\
\n"
};
