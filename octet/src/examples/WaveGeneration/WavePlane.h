#ifndef WAVE_PLANE_H_INCLUDED
#define WAVE_PLANE_H_INCLUDED

#include "../../octet.h"
#include "SineWave.h"
#include "GerstnerWave.h"



class WavePlane
{
private:
    typedef octet::mesh mesh;
    typedef octet::ref<mesh> meshRef;
    typedef octet::vec2 vec2;
    typedef octet::vec3 vec3;

    typedef std::pair<GerstnerWave*, float> WeightedWave;
public:
    struct vert{
        vert(octet::vec3p v,octet::vec2p uv, uint32_t col) :pos(v),uvs(uv), color(col){}
        octet::vec3p pos;
        octet::vec2p uvs;
        uint32_t color;
    };


    WavePlane(){
        mesh_ = nullptr;
        activeWaves_ = 0;
    }

    ~WavePlane()
    {

    }

    meshRef GetMesh()
    {
        return mesh_;
    }

    void Init(int xSize, int ySize, float scale)
    {
        mesh_ = new mesh();
        size_.x() = xSize;
        size_.y() = ySize;
        scale_ = scale;



        plane_.resize(size_.x()*size_.y());
        indicies_.resize((size_.x() - 1)*(size_.y() - 1) * 6);

        mesh_->allocate(sizeof(vert)*plane_.size(), sizeof(unsigned)*indicies_.size());
        mesh_->set_params(sizeof(vert), indicies_.size(), plane_.size(), GL_TRIANGLES, GL_UNSIGNED_INT);
        mesh_->clear_attributes();
        mesh_->add_attribute(octet::attribute_pos, 3, GL_FLOAT, 0);

        mesh_->add_attribute(octet::attribute_uv, 2, GL_FLOAT, 24);
        mesh_->add_attribute(octet::attribute_color, 4, GL_UNSIGNED_BYTE, 32, TRUE);


        int i = 0;
        for (int y = 0, t = 0; y < size_.y() - 1; ++y)
        {
            for (int x = 0; x < size_.x() - 1; ++x)
            {
                indicies_[i] = t;
                indicies_[i + 1] = t + size_.x();
                indicies_[i + 2] = t + size_.x() + 1;

                indicies_[i + 3] = t;
                indicies_[i + 4] = t + size_.x() + 1;
                indicies_[i + 5] = t + 1;
                i += 6;
                t++;
            }
            t++;
        }
        octet::gl_resource::wolock il(mesh_->get_indices());
        unsigned * indxP = (unsigned*)il.u8();
        memcpy(indxP, indicies_.data(), sizeof(unsigned)*indicies_.size());

    }

    void InitDefaultWaves()
    {

        waves_.push_back(WeightedWave(new GerstnerWave(16, 0.4, 1, vec2(0, 1), 2.0f), 2.0f));
        waves_.push_back( WeightedWave(new GerstnerWave(44, 1.4, 4, vec2(1, 0), 2.0f), 0.5f));
        waves_.push_back( WeightedWave(new GerstnerWave(32, 1, 2, vec2(-0.7, -0.7), 2.0f), 1.0f));
        waves_.push_back( WeightedWave(new GerstnerWave(100, 1.4, 3, vec2(0.7, 0.3), 1.0f), 1.0f));
        waves_.push_back( WeightedWave(new GerstnerWave(50, 1.4, 2, vec2(0, -1), 2.0f), 1.0f));
        activeWaves_ = 5;
    }
    
    int AddWave(GerstnerWave* g, float weight)
    {
        if (waves_.size() < 20)
        {
            waves_.push_back(WeightedWave(g, weight));
        }
        return waves_.size();

    }

    float GetWeight(int index)
    {
        if (index < waves_.size() && index>0)
        {
            return waves_[index].second;
        }
        return 0;
    }

    void SetWeight(int index, float weight)
    {
        if (index < waves_.size() && index>0)
        {
            waves_[index].second = weight;
        }

    }

    GerstnerWave* GetWave(int index)
    {
        if (index < waves_.size() && index>=0)
        {
            return waves_[index].first;
        }
    }
    //returns size and changes float pointer
    int GetShaderInfo(float** f)
    {
        assert(waveStructs_.size() <= 20);
        *f = (float*)waveStructs_.data();
        return sizeof(GerstnerWave::ShaderWaveStruct)*waveStructs_.size();
    }

    int GetNumWaves()
    {
        return waves_.size();
    }
    
    void SetNumActiveWaves(int i)
    {
        activeWaves_ = i;
    }

    int GetNumActiveWaves()
    {
    
        return activeWaves_;
    }

    void GenerateWaves(float time)
    {
        if (!mesh_)
        {
            assert(mesh_);
            return;
        }
        if (activeWaves_ == 0)
        {
            return;
        }
        /*
        *   Grid defined by
        *   x - >
        *  y 0 1 2 3 4 5 6
        *  | 7 8 9 10 11 12 13
        *  v
        */

        octet::gl_resource::wolock vl(mesh_->get_vertices());
        vert *vtxP = (vert*)vl.u8();

       
        for (int x = 0, i = 0; x < size_.x(); ++x)
        {
            for (int z = 0; z < size_.y(); ++z)
            {
                
                float a = 0;
                vec3 pos(x, 0, z);
                for (int i = 0; i < activeWaves_; ++i)
                {
                    a += waves_[i].first->Apply(vec2(x, z), time, pos);
                }
                plane_[i] = pos;
                ++i;
            }
        }

        for (int i = 0; i < plane_.size(); ++i)
        {
            *vtxP = vert(plane_[i] * scale_, vec2(plane_[i].x() / size_.x(), plane_[i].y()/ size_.y()), 0x00ff00);
            vtxP++;
        }
        
        if (waveStructs_.size() < waves_.size())
        {
            waveStructs_.resize(waves_.size());
        }
            for (int i = 0; i < activeWaves_; ++i)
            {
                waveStructs_[i] = waves_[i].first->ToShader();
            }
        
        
        
    }
private:
    meshRef mesh_;
    std::vector<WeightedWave> waves_;
    std::vector<GerstnerWave::ShaderWaveStruct> waveStructs_;
    int activeWaves_;


    vec2 size_;
    float scale_;

    std::vector<vec3> plane_;
    std::vector<unsigned> indicies_;
};



#endif