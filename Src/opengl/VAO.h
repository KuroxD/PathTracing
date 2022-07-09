#pragma once
#include <GL/glew.h>
#include "Define.h"

struct VAO {
	unsigned int vao, vbo, ebo;
    int indexSize = -1;
    int vertexSize = -1;
	VAO() {
        const float Pos[] = {
        -1.0f,1.0f, -1.0f,-1.0f,
        1.0f,-1.0f, 1.0f,1.0f
        };
        
        const unsigned int Index[] = {
            0,1,2,
            2,0,3
        };
        vertexSize = 4;
        indexSize = 6;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Pos), Pos, GL_STATIC_DRAW);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glGenBuffers(1, &ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Index), Index, GL_STATIC_DRAW);
	}


    void Use() {
        glBindVertexArray(vao);
    }

    void Draw() {
        if (indexSize > 0)
            GLCall(glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, NULL));
        else if (vertexSize > 0)
            GLCall(glDrawArrays(GL_TRIANGLES, 0, vertexSize));
    }
};