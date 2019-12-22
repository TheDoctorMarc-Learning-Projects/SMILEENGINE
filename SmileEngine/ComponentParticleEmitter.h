#pragma once

class FreeTransform; 
class FreeBillBoard; 

struct Particle
{
	FreeTransform transf; 
	FreeBillBoard billboard; 
};

enum emmissionShape { CIRCLE, CUBE, CONE };
struct emmssionMode { emmissionShape shape; float angle, radius; };

class ComponentParticleEmitter
{
public: 
	enum blendMode{ ADDITIVE, ALPHA_BLEND };
	enum lightMode { PER_EMITTER, PER_PARTICLE, NONE};
	enum particleMode { COLOR, TEXTURE, ANIM_TEXTURE};
	emmssionMode emmissionMode;

private: 
	Particle particles[150]; 

};
