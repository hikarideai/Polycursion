#include "shader.hpp"
#include <iostream>

ShaderProgram::ShaderProgram() {
	id = -1;
}

ShaderProgram::ShaderProgram(const GLchar *vert_path, const GLchar *frag_path) {
	loadFromFile(vert_path, frag_path);
}

int ShaderProgram::loadFromFile(const GLchar *vert_path, const GLchar *frag_path) {
	auto vert_src = readFile(vert_path);
    auto frag_src = readFile(frag_path);

    loadFromSource(vert_src.c_str(), frag_src.c_str());
    return 0;
}

int ShaderProgram::loadFromSource(const GLchar *vert_src, const GLchar *frag_src) {
	int error = loadVertexFromSource(vert_src);
    if (error) {
        return error;
    }

    error = loadFragmentFromSource(frag_src);
    if (error) {
        return error;
    }

    error = link();
    if (error) {
        return error;
    }
    return 0;
}

int ShaderProgram::loadVertexFromSource(const GLchar *vert_src) {
    vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vert_src, nullptr);
    glCompileShader(vert);
    if (!shaderStatus(vert)) {
        return -1;
    }
    return 0;
}

int ShaderProgram::loadFragmentFromSource(const GLchar *frag_src) {
    frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &frag_src, nullptr);
    glCompileShader(frag);
    if (!shaderStatus(frag)) {
        return -1;
    }
    return 0;
}

int ShaderProgram::link() {
    id = glCreateProgram();
    glAttachShader(id, vert);
    glAttachShader(id, frag);

    glLinkProgram(id);
    if (!programStatus(id)) {
        return -1;
    }

    //Clean up
    glDeleteShader(vert);
    glDeleteShader(frag);
    return 0;
}


int ShaderProgram::linkComp(int n, const GLchar * vars[], GLenum mode) {
    id = glCreateProgram();
    glAttachShader(id, vert);

    std::cout << "Transform Feedback vars:\n";
    for (int i = 0; i < n; i++)
        std::cout << vars[i] << std::endl;
    glTransformFeedbackVaryings(id, n, vars, mode);

    glLinkProgram(id);
    glLinkProgram(id);
    if (!programStatus(id)) {
        return -1;
    }

    //Clean up
    glDeleteShader(vert);
    return 0;
}

void ShaderProgram::setInt(const GLchar *name, int value) const {
	glUniform1i(glGetUniformLocation(id, name), value);
}

void ShaderProgram::setFloat(const GLchar *name, float value) const {
	glUniform1f(glGetUniformLocation(id, name), value);
}

void ShaderProgram::setBool(const GLchar *name, bool value) const {
	glUniform1i(glGetUniformLocation(id, name), static_cast<int>(value));
}
// END ----- ShaderProgram


void use(ShaderProgram *shp) {
	if (shp == nullptr) {
        glUseProgram(0);
	} else {
        glUseProgram(shp->id);
	}
}

bool shaderStatus(GLuint shader) {
    GLint success;
    char log[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, log);
        puts(log);
    }
    return success;
}

bool programStatus(GLuint program) {
    GLint success;
    char log[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, log);
        puts(log);
    }
    return success;
}

std::string readFile(const GLchar *path) {
    std::ifstream f(path);
    std::stringstream sas;
    sas << f.rdbuf();
    return sas.str();
}
