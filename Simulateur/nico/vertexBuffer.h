#pragma once
#include <nico/openGlObject.h>
#include <glad/glad.h>
#include <vector>
#include <nico/Vertex.h>

namespace nico {
    struct layout {
        GLuint location;
        int size;
    };
    template <typename T>
    class VertexBuffer : public GlObject
    {
    private:
        unsigned int stride;
        unsigned int offset;
    public:
        VertexBuffer();
        VertexBuffer(std::vector<T>* data, unsigned int strideInNumberOfcoords = 0);
        void setData(std::vector<T>* data, unsigned int strideInNumberOfcoords = 0);
        void setStride(unsigned int newStride);
        void addLayout(layout lay);
        void addLayout(uint32_t layout, int sizeOfOneVertex);
        void bind();
        void unbind();
        unsigned int* getIdPointer();
    };

    template<typename T>
    inline VertexBuffer<T>::VertexBuffer()
    {
        stride = 0;
        offset = 0;

    }
    //this create it, bind it, and send data
    template<typename T>
    inline VertexBuffer<T>::VertexBuffer(std::vector<T>* data, unsigned int strideInNumberOfcoords) : stride(strideInNumberOfcoords)
    {
        glGenBuffers(1,&id);
        bind();
        glBufferData(GL_ARRAY_BUFFER, sizeof(T) * data->size(), data->data(), GL_STATIC_DRAW);
        offset = 0;
    }
    template<typename T>
    inline void VertexBuffer<T>::setData(std::vector<T>* data, unsigned int strideInNumberOfcoords)
    {
        stride = strideInNumberOfcoords;
        glGenBuffers(1, &id);
        bind();
        glBufferData(GL_ARRAY_BUFFER, sizeof(T) * data->size(), data->data(), GL_STATIC_DRAW);
    }
    template<typename T>
    inline void VertexBuffer<T>::setStride(unsigned int newStride)
    {
        stride = newStride;
    }
    template<typename T>
    inline void VertexBuffer<T>::addLayout(layout lay)
    {
        static_assert(false,"this type is not supported by nico::vertexBuffer class\n");
    }
    inline void VertexBuffer<float>::addLayout(layout lay)
    {
        glEnableVertexAttribArray(lay.location);
        glVertexAttribPointer(lay.location, lay.size, GL_FLOAT, false, stride * sizeof(float),
            (void*)(offset * sizeof(float)));
        offset += lay.size;
    }
    inline void VertexBuffer<Vertex>::addLayout(layout lay)
    {
        glEnableVertexAttribArray(lay.location);
        glVertexAttribPointer(lay.location, lay.size, GL_FLOAT, false, sizeof(Vertex),
            (void*)(offset * sizeof(float)));
        offset += lay.size;
    }
    template<typename T>
    inline void VertexBuffer<T>::addLayout(uint32_t layout, int sizeOfOneVertex)
    {
        static_assert(false, "this type is not supported by nico::vertexBuffer class");
    }
    template<>
    inline void VertexBuffer<float>::addLayout(uint32_t layout, int sizeOfOneVertex)
    {
        glEnableVertexAttribArray(layout);
        glVertexAttribPointer(layout, sizeOfOneVertex, GL_FLOAT, false, stride * sizeof(float),
            (void*)(offset * sizeof(float)));
        offset += sizeOfOneVertex;
    }


    template<typename T>
    inline void VertexBuffer<T>::bind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, id);
    }
    template<typename T>
    inline void VertexBuffer<T>::unbind()
    {
        glBindBuffer(GL_ARRAY_BUFFER, NULL);
    }

    template<typename T>
    inline unsigned int* VertexBuffer<T>::getIdPointer()
    {
        return &id;
    }
    /*
    lay.location and lay.size are values to put in the vertex shader like this :
    layout(location = ($location)) in vec($size) var;
     */

    //vertexArray class
    class VertexArray : public GlObject{
    public:
        //this create it and bind it
        VertexArray() {
            glGenVertexArrays(1, &id);
            bind();
        }
        ~VertexArray() {
            glDeleteVertexArrays(1, &id);
        }
        void bind() {
            glBindVertexArray(id);
        }
        void unbind() {
            glBindVertexArray(0);
        }
    };

    //indicesBuffer
    template <typename T>
    class indicesBuffer : public GlObject
    {
    public:
        indicesBuffer();
        indicesBuffer(std::vector<T> data);
        void setData(std::vector<T> data);
        void bind();
        void unbind();
    };
    //this does nothing
    template<typename T>
    inline indicesBuffer<T>::indicesBuffer()
    {
    }
    //this create it, bind it, and send data
    template<typename T>
    inline indicesBuffer<T>::indicesBuffer(std::vector<T> data)
    {
        glGenBuffers(1, &id);
        bind();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(T) * data.size(), &data[0], GL_STATIC_DRAW);
    }
    template<typename T>
    inline void indicesBuffer<T>::setData(std::vector<T> data)
    {
        glGenBuffers(1, &id);
        bind();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(T) * data.size(), &data[0], GL_STATIC_DRAW);
    }
    template<typename T>
    inline void indicesBuffer<T>::bind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
    }
    template<typename T>
    inline void indicesBuffer<T>::unbind()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
    }
}


