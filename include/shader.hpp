#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>
#include <bits/stdc++.h>

#include <fstream>
#include <sstream>

extern const int VERTEX_SHADER_INIT_ERROR;
extern const int FRAGMENT_SHADER_INIT_ERROR;
extern const int PROGRAM_INIT_ERROR;

class ShaderProgram;
void use(ShaderProgram *shp = nullptr);
bool shaderStatus(GLuint shader);
bool programStatus(GLuint program);
std::string readFile(const GLchar *path);

class ShaderProgram {
public:
	GLuint id;
	GLuint vert, frag;

	ShaderProgram();
	ShaderProgram(const GLchar *vert_path, const GLchar *frag_path);

	int loadFromFile(const GLchar *vert_path, const GLchar *frag_path);
    int loadFromSource(const GLchar *vert_src, const GLchar *frag_src);
	int loadVertexFromSource(const GLchar *vert_src);
    int loadFragmentFromSource(const GLchar *frag_src);
    int link();
    int linkComp(int n, const GLchar * vars[], GLenum mode);

	void setInt(const GLchar *name, int value) const;
	void setFloat(const GLchar *name, float value) const;
	void setBool(const GLchar *name, bool value) const;
};

#endif
