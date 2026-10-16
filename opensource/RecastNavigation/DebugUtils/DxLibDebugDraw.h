#pragma once
#include "DebugDraw.h"
#include <DxLib.h>
#include <vector>

class DxLibDebugDraw : public duDebugDraw
{
public:
    DxLibDebugDraw()           = default;
    ~DxLibDebugDraw() override = default;

    void depthMask(bool state) override
    {
        // Write to Z buffer toggle
        SetWriteZBuffer3D(state ? TRUE : FALSE);
    }

    void texture(bool /*state*/) override {}

    void begin(duDebugDrawPrimitives prim, float size = 1.0f) override
    {
        prim_ = prim;
        size_ = size;
        verts_.clear();
        cols_.clear();
    }

    void vertex(const float* pos, unsigned int color) override
    {
        VECTOR v{pos[0], pos[1], pos[2]};
        verts_.push_back(v);
        cols_.push_back(color);
    }

    void vertex(const float x, const float y, const float z, unsigned int color) override
    {
        VECTOR v{x, y, z};
        verts_.push_back(v);
        cols_.push_back(color);
    }

    void vertex(const float* pos, unsigned int color, const float* /*uv*/) override
    {
        vertex(pos, color);
    }

    void vertex(const float x, const float y, const float z, unsigned int color, const float /*u*/, const float /*v*/) override
    {
        vertex(x, y, z, color);
    }

    void end() override
    {
        switch(prim_)
        {
        case DU_DRAW_POINTS:
            {
                const float r = std::max(0.01f, size_ * 0.02f);
                for(size_t i = 0; i < verts_.size(); ++i)
                {
                    DrawSphere3D(verts_[i], r, 6, cols_[i], cols_[i], true);
                }
                break;
            }
        case DU_DRAW_LINES:
            {
                for(size_t i = 1; i < verts_.size(); i += 2)
                {
                    DrawLine3D(verts_[i - 1], verts_[i], cols_[i]);
                }
                break;
            }
        case DU_DRAW_TRIS:
            {
                for(size_t i = 2; i < verts_.size(); i += 3)
                {
                    unsigned int c = cols_[i - 2];
                    DrawTriangle3D(verts_[i - 2], verts_[i - 1], verts_[i], c, true);
                }
                break;
            }
        case DU_DRAW_QUADS:
            {
                for(size_t i = 3; i < verts_.size(); i += 4)
                {
                    unsigned int c = cols_[i - 3];
                    DrawTriangle3D(verts_[i - 3], verts_[i - 2], verts_[i - 1], c, true);
                    DrawTriangle3D(verts_[i - 3], verts_[i - 1], verts_[i], c, true);
                }
                break;
            }
        default:
            break;
        }
        verts_.clear();
        cols_.clear();
    }

private:
    duDebugDrawPrimitives prim_ = DU_DRAW_LINES;
    float size_                 = 1.0f;
    std::vector<VECTOR> verts_;
    std::vector<unsigned int> cols_;
};
