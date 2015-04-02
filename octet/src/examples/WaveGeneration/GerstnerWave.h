#ifndef G_WAVE_H_INCLUDED
#define G_WAVE_H_INCLUDED
#include "../../octet.h"

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <math.h>
class GerstnerWave
{
public:
#pragma pack(push, 0)
    struct ShaderWaveStruct
    {
        ShaderWaveStruct()
        {
            frequency = 0;
            convSpeed = 0;
        }
        ShaderWaveStruct(float dirX, float dirY, float ampli, float freq, float cSpeed,float q) :
            direction(octet::vec2(dirX, dirY).normalize()*ampli), frequency(freq), convSpeed(cSpeed),qVal(q){}
        //amplitude stored as length of direction
        octet::vec2p direction;
        float frequency;
        float convSpeed;
        float qVal;
    };
#pragma pack(pop)


public:
    GerstnerWave()
    {}
    GerstnerWave(float wav, float ampl, float speed,
        const octet::vec2& dir, float q) :wavelength_(wav), amplitude_(ampl),
        speed_(speed), direction_(dir.normalize()), q_(q){
        frequency_ = (2 * M_PI) / wavelength_;
        convSpeed_ = speed_*frequency_;


    }

    ShaderWaveStruct ToShader()
    {
        return ShaderWaveStruct(direction_.x(), direction_.y(), amplitude_, frequency_, convSpeed_,q_);
    }

    void Init(float wav, float ampl,
        float speed, const octet::vec2& dir, float q){
        wavelength_ = wav;
        amplitude_ = ampl;
        speed_ = speed;
        direction_ = direction_.normalize();
        frequency_ = (2 * M_PI) / wavelength_;
        convSpeed_ = speed_*frequency_;
        q_ = q;
    }

    float GetWavelength()
    {
        return wavelength_;
    }

    float GetSpeed()
    {
        return speed_;
    }

    float GetAmplitude()
    {
        return amplitude_;
    }

    float GetQ()
    {
        return q_;
    }

    octet::vec2 GetDir()
    {
        return direction_;
    }
    void SetWavelength(float wave)
    {
        wavelength_ = wave;
        frequency_ = (2 * M_PI) / wavelength_;
        convSpeed_ = speed_*frequency_;
    }

    void SetSpeed(float s)
    {
        speed_ = s;
        convSpeed_ = speed_*frequency_;
    }

    void SetAmplitude(float a)
    {
        amplitude_ = a;
    }

    void SetQ(float q)
    {
        q_ = q;
    }

    void SetDirection(const octet::vec2 & dir)
    {
        direction_ = dir.normalize();
    }

    float Apply(const octet::vec2& position, float t,octet::vec3 &newPos)
    {
        
        octet::vec3 n(
            q_*amplitude_*direction_.x() * cosf(frequency_*direction_.dot(position) + t* convSpeed_),
            amplitude_*sinf(direction_.dot(position) * frequency_ + t * convSpeed_),
            q_*amplitude_*direction_.y() * cosf(frequency_*direction_.dot(position) + t* convSpeed_));
        newPos += n;
        return n.y();
    }

    octet::vec3 GetNormal(const octet::vec2& position, float t)const
    {
        return octet::vec3(
            -(direction_.x()*frequency_*amplitude_*cosf(frequency_*direction_.dot(position) + t*convSpeed_)),
            1 - (q_*frequency_*amplitude_ *sinf(frequency_ * direction_.dot(position) + t*convSpeed_)),
            -(direction_.y()*frequency_*amplitude_*cosf(frequency_*direction_.dot(position) + t*convSpeed_)));
    }
    
private:
    float wavelength_;
    float amplitude_;
    float speed_;
    float q_;

    octet::vec2 direction_;
   
    float frequency_;
    float convSpeed_;

   
};



#endif