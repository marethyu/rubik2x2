#ifndef _MY_GL_H_
#define _MY_GL_H_

#include <vector>
#include <algorithm>
#include <limits>

#include "linalg.h"

namespace mygl
{
    struct Colour
    {
        uint8_t r, g, b, a;
        uint32_t argb;

        Colour()
        {}

        Colour(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
          : r(r), g(g), b(b), a(a)
        {
            argb = (a << 24) | (r << 16) | (g << 8) | b;
        }

        Colour AdjustBrightness(float L) const
        {
            uint8_t newr = uint8_t(L * r);
            uint8_t newg = uint8_t(L * g);
            uint8_t newb = uint8_t(L * b);

            return Colour(newr, newg, newb, a);
        }

        Colour Contrast() const
        {
            uint8_t newr = 255 - r;
            uint8_t newg = 255 - g;
            uint8_t newb = 255 - b;

            return Colour(newr, newg, newb, a);
        }
    };

    const int MAXV = 50;
    const int MAXTRI = 101;
    const int MAXT = 101;

/*
    When you describe a 3D model, it is more convenient to write coordinates relative global origin (0, 0, 0) (see below for reference), don't worry about any linear transformations

    Coordinate system

        y
        |
        |
        +---x
       /
      z

     x - right
     y - up
     z - out the screen

*/
    struct Triangle
    {
        bool filled;
        Colour colour;
        int vertex[3]; // index to vertex array in model (saving vertex itself is wasteful)
    };

    struct Model
    {
        int nvert;
        int ntrig;

        vec4f vertex[MAXV];
        Triangle triangle[MAXTRI];
    };

    const Colour RED(255, 0, 0, 255);
    const Colour ORANGE(255, 127, 0, 255);
    const Colour YELLOW(255, 255, 0, 255);
    const Colour GREEN(0, 255, 0, 255);
    const Colour BLUE(0, 0, 255, 255);
    const Colour INDIGO(75, 0, 130, 255);
    const Colour VIOLET(148, 0, 211, 255);
    const Colour BLACK(0, 0, 0, 255);
    const Colour WHITE(255, 255, 255, 255);

    const float ZMIN = 1e-9; // cannot be less than zero

    // Platform indepentent base class for programs that use 3D graphics
    class RendererBase3D
    {
    public:
        RendererBase3D(int width, int height);
        ~RendererBase3D();

        // Must be overriden
        virtual void Init() = 0;
        virtual void Update() = 0;
        virtual void Render() = 0;
    protected:
        int width;
        int height;

        std::vector<uint32_t> pixels;
        std::vector<float> zdepth;

        /* Coordinate system:
           x goes right starting from top left corner
           y goes down starting from top left corner
           z goes into page starting from top left corner
         */
        void DrawFilledTriangleBarycentric(const vec3f& v1, const vec3f& v2, const vec3f& v3, const Colour& colour); // Warning: vertexes might need to be arranged in clockwise direction
        void DrawWireframeTriangleDDA(const vec3f& v1, const vec3f& v2, const vec3f& v3, const Colour& colour);
        void DrawLineDDA(const vec3f& v1, const vec3f& v2, const Colour& colour);

        virtual void PutPixel(int x, int y, float depth, uint32_t argb); // can be optionally overriden

        void ClearScreen();
    };

    RendererBase3D::RendererBase3D(int width, int height)
      : width(width), height(height), pixels(width * height), zdepth(width * height)
    {}

    RendererBase3D::~RendererBase3D()
    {}

    // https://austinmorlan.com/posts/drawing_a_triangle/
    // TODO https://fgiesen.wordpress.com/2013/02/10/optimizing-the-basic-rasterizer/
    void RendererBase3D::DrawFilledTriangleBarycentric(const vec3f& v1, const vec3f& v2, const vec3f& v3, const Colour& colour)
    {
        // Area of the parallelogram formed by edge vectors
        float area = (v3[0] - v1[0]) * (v2[1] - v1[1]) - (v3[1] - v1[1]) * (v2[0] - v1[0]);

        // top left and bottom right points of a bounding box
        float xmin = std::min({v1[0], v2[0], v3[0]});
        float xmax = std::max({v1[0], v2[0], v3[0]});
        float ymin = std::min({v1[1], v2[1], v3[1]});
        float ymax = std::max({v1[1], v2[1], v3[1]});

        // basic clipping
        int x1 = std::max(int(std::floor(xmin)), 0);
        int x2 = std::min(int(std::floor(xmax)), width - 1);
        int y1 = std::max(int(std::floor(ymin)), 0);
        int y2 = std::min(int(std::floor(ymax)), height - 1);

        for (int y = y1; y <= y2; ++y)
        {
            for (int x = x1; x <= x2; ++x)
            {
                float px = x + 0.5f;
                float py = y + 0.5f;

                // Barycentric weights
                float w1 = ((px - v2[0]) * (v3[1] - v2[1]) - (py - v2[1]) * (v3[0] - v2[0])) / area;
                float w2 = ((px - v3[0]) * (v1[1] - v3[1]) - (py - v3[1]) * (v1[0] - v3[0])) / area;
                float w3 = ((px - v1[0]) * (v2[1] - v1[1]) - (py - v1[1]) * (v2[0] - v1[0])) / area;

                if ((w1 >= 0.0f) & (w2 >= 0.0f) & (w3 >= 0.0f))
                {
                    float z = w1 * v1[2] + w2 * v2[2] + w3 * v3[2];
                    float depth = 1.0f / z;

                    PutPixel(x, y, depth, colour.argb);
                }
            }
        }
    }

    void RendererBase3D::DrawWireframeTriangleDDA(const vec3f& v1, const vec3f& v2, const vec3f& v3, const Colour& colour)
    {
        // TODO check bounds
        DrawLineDDA(v1, v2, colour);
        DrawLineDDA(v1, v3, colour);
        DrawLineDDA(v2, v3, colour);
    }

    // TODO integer DDA might be faster
    void RendererBase3D::DrawLineDDA(const vec3f& v1, const vec3f& v2, const Colour& colour)
    {
        float dx = v2[0] - v1[0];
        float dy = v2[1] - v1[1];
        float dz = v2[2] - v1[2];

        float step = std::fabs(dx) >= std::fabs(dy) ? std::fabs(dx) : std::fabs(dy);

        dx /= step;
        dy /= step;
        dz /= step;

        float x = v1[0];
        float y = v1[1];
        float z = v1[2];

        for (int i = 0; i <= step; ++i)
        {
            PutPixel(x, y, 1.0f / z, colour.argb);

            x += dx;
            y += dy;
            z += dz;
        }
    }

    void RendererBase3D::PutPixel(int x, int y, float depth, uint32_t argb)
    {
        int offset = y * width + x;

        if (zdepth[offset] < depth)
        {
            zdepth[offset] = depth;
            pixels[offset] = argb;
        }
    }

    void RendererBase3D::ClearScreen()
    {
        std::fill(zdepth.begin(), zdepth.end(), ZMIN);
        std::fill(pixels.begin(), pixels.end(), 0);
    }
}

#endif /* _MY_GL_H_ */