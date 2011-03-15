/*
 *  Shader.cpp
 *  CS248-Final-Project
 *
 *  Created by Matthew Fichman on 1/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "Shader.h"
#include <fstream>

#include "Define.h"
#include "Utility.h"

#define ERROR_BUFSIZE 1024

Shader::Shader(const std::string& path) :
    path_(path),
    vertexShaderID_(0),
    fragmentShaderID_(0),
    programID_(0),
    loaded_(false) {

    const GLchar* source[1];
    int length = 0;
    
    // Load the fragment shader and compile
    std::vector<char> fragmentSource = readSource(path + ".frag.glsl");
    source[0] = &fragmentSource.front();
    length = fragmentSource.size()-1;
    GL_CHECK(fragmentShaderID_ = glCreateShader(GL_FRAGMENT_SHADER))
    GL_CHECK(glShaderSource(fragmentShaderID_, 1, source, &length))
    GL_CHECK(glCompileShader(fragmentShaderID_))
        
    // Load the vertex shader and compile
    std::vector<char> vertexSource = readSource(path + ".vert.glsl");
    source[0] = &vertexSource.front();
    length = vertexSource.size()-1;
    GL_CHECK(vertexShaderID_ = glCreateShader(GL_VERTEX_SHADER))
    GL_CHECK(glShaderSource(vertexShaderID_, 1, source, &length))
    GL_CHECK(glCompileShader(vertexShaderID_))
    
    // Create the vertex program
    GL_CHECK(programID_ = glCreateProgram())
    GL_CHECK(glAttachShader(programID_, fragmentShaderID_))
    GL_CHECK(glAttachShader(programID_, vertexShaderID_))
    GL_CHECK(glLinkProgram(programID_))
    
    // Error checking
    GL_CHECK(glGetProgramiv(programID_, GL_LINK_STATUS, (GLint*)&loaded_))
    //glGetShaderiv(vertexShaderID_, GL_COMPILE_STATUS, (GLint*)&loaded_);
    if (!loaded_) {
        GLchar tempErrorLog[ERROR_BUFSIZE];
        GLsizei length;
        glGetShaderInfoLog(fragmentShaderID_, ERROR_BUFSIZE, &length, tempErrorLog);
        errors_ += "Fragment shader errors:\n";
        errors_ += std::string(tempErrorLog, length) + "\n";
        glGetShaderInfoLog(vertexShaderID_, ERROR_BUFSIZE, &length, tempErrorLog);
        errors_ += "Vertex shader errors:\n";
        errors_ += std::string(tempErrorLog, length) + "\n";
        glGetProgramInfoLog(programID_, ERROR_BUFSIZE, &length, tempErrorLog);
        errors_ += "Linker errors:\n";
        errors_ += std::string(tempErrorLog, length) + "\n";
    }
}

Shader::~Shader() {
    glDeleteShader(vertexShaderID_);
    glDeleteShader(fragmentShaderID_);
    glDeleteProgram(programID_);
}

std::vector<char> Shader::readSource(const std::string& path) {

    // Open the file
    std::vector<char> source;
    std::ifstream in(path.c_str());
    if (in.fail()) {
        source.push_back(0);
        return source;
    }
    
    // Seek to the end of the file to get the size
    in.seekg(0, std::ios::end);
    source.reserve((unsigned)(1 + in.tellg()));
    source.resize((unsigned)in.tellg());
    in.seekg(0, std::ios::beg);
    if (source.empty()) {
        source.push_back(0);
        return source;
    }
    
    // Now read the whole buffer in one call, and don't 
    // forget to null-terminate the vector with a zero
    in.read(&source.front(), source.size());
    source.push_back(0);
    
    return source;
}

const std::string& Shader::path() const {
    return path_;
}

GLuint Shader::programID() const {
    return programID_;
}

const std::string& Shader::errors() const {
    return errors_;
}

bool Shader::loaded() const {
    return loaded_;
}

void Shader::initilize(const char* sceneName, const char* configFilename) {

    usePosition  = GetPrivateProfileBool(sceneName, "usePosition", false, configFilename);
    useAmbient   = GetPrivateProfileBool(sceneName, "useAmbient", false, configFilename);
    useDiffuse   = GetPrivateProfileBool(sceneName, "useDiffuse", false, configFilename);
    useSpecular  = GetPrivateProfileBool(sceneName, "useSpecular", false, configFilename);
    useNormal    = GetPrivateProfileBool(sceneName, "useNormal", false, configFilename);
    useTangent   = GetPrivateProfileBool(sceneName, "useTangent", false, configFilename);
    useBitangent = GetPrivateProfileBool(sceneName, "useBitangent", false, configFilename);
    useShadow    = GetPrivateProfileBool(sceneName, "useShadow", false, configFilename);
    useCube      = GetPrivateProfileBool(sceneName, "useCube", false, configFilename);

}

