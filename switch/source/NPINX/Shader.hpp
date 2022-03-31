#pragma once

namespace Npi
{
    class Shader
    {
        public:
        Shader(GLenum type, const char* source);
        Shader(const char* verts, const char* frags);
        void Use();
        ~Shader();
        GLuint GetProgram(){ return s_program; }
        private:
        GLuint CompileVertexShader(GLenum type, const char* source);
        GLuint CompileFragmentShader(GLenum type, const char* source);
        GLuint vsh;
        GLuint fsh;
        static GLuint s_program;

    };
}