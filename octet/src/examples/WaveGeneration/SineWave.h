#ifndef SINE_WAVE_H_INCLUDED
#define SINE_WAVE_H_INCLUDED
#include "../../octet.h"

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <math.h>
class SineWave
{
public:
    struct ShaderWaveStruct
    {
        ShaderWaveStruct()
        {
            frequency = 0;
            convSpeed = 0;
        }
        ShaderWaveStruct(float dirX, float dirY, float ampli, float freq, float cSpeed) :
            direction(octet::vec2(dirX, dirY).normalize()*ampli), frequency(freq), convSpeed(cSpeed){}
        //amplitude stored as length of direction
        octet::vec2p direction;
        float frequency;
        float convSpeed;
    };
public:
    SineWave(float wav, float ampl, float speed,
        const octet::vec2& dir) :wavelength_(wav), amplitude_(ampl),
        speed_(speed), direction_(dir){
        frequency_ = (2 * M_PI) / wavelength_;
        convSpeed_ = speed_*frequency_;

    }

    ShaderWaveStruct ToShader()
    {
        return ShaderWaveStruct(direction_.x(), direction_.y(), amplitude_, frequency_, convSpeed_);
    }

    void Init(float wav, float ampl,
        float speed, const octet::vec2& dir){
        wavelength_ = wav;
        amplitude_ = ampl;
        speed_ = speed;
        direction_ = dir;
        direction_ = direction_.normalize();
        frequency_ = (2 * M_PI) / wavelength_;
        convSpeed_ = speed_*frequency_;
    }

    float Apply(const octet::vec2& position, float t, octet::vec3 &newPos)const
    {
        float height = amplitude_*sinf(direction_.dot(position) * frequency_ + t * convSpeed_);
        newPos = (position.x(), height, position.y());
        return height;
    }

    octet::vec3 GetNormal(const octet::vec2& position, float t)const
    {
        return octet::vec3(
            -(frequency_ * direction_.x() * amplitude_ *
            cosf(direction_.dot(position) * frequency_ + t * convSpeed_)),
            1,
            -(frequency_ * direction_.y() * amplitude_ *
            cosf(direction_.dot(position) * frequency_ + t * convSpeed_)));
    }
private:
    float wavelength_;
    float amplitude_;
    float speed_;

    float frequency_;
    float convSpeed_;

    octet::vec2 direction_;
};



#endif