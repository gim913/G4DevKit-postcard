#include "postcard.h"
#include "hardware/screen.h"

static u32 g_randSeed = 0x3bcdef01;
u32 rand() {
	u32 ret = g_randSeed;
	ret ^= ret << 13;
	ret ^= ret >> 17;
	ret ^= ret << 5;
	g_randSeed = ret;
	return ret;
}

typedef struct {
	float x;
	float y;
} Vec2d;

void vectorCtor(Vec2d* self, float x, float y) {
	self->x = x;
	self->y = y;
}

void vectorAdd(Vec2d* self, const Vec2d* r) {
	self->x += r->x;
	self->y += r->y;
}

void vectorMul(Vec2d* self, float scalar) {
	self->x *= scalar;
	self->y *= scalar;
}

typedef struct {
	Vec2d position;
	Vec2d velocity;
	Vec2d acceleration;
} Particle;

void particleCtor(Particle* self, float x, float y) {
	vectorCtor(&self->position, x, y);
	vectorCtor(&self->velocity, (rand() / 4294967296.0) / 2.0, -0.5 - (rand() >> 4) / 2147483648.0);
	vectorCtor(&self->acceleration, 0, 0);
}

void particleAddForce(Particle* self, const Vec2d* force) {
	vectorAdd(&self->acceleration, force);
}

void particleUpdate(Particle* self) {
	vectorAdd(&self->velocity, &self->acceleration);
	vectorAdd(&self->position, &self->velocity);
	vectorCtor(&self->acceleration, 0, 0);
}


Vec2d g_gravity;

#define NUM_PARTS 10
typedef struct {
	Particle particle;
	Particle parts[NUM_PARTS];
	int life[NUM_PARTS];
	u32 hasParts;
	char color;
	bool exploded;
} Rocket;

#define NUM_ROCKETS 5
#define FOR_EACH_ROCKET for (int i = 0; i < NUM_ROCKETS; ++i)
Rocket g_rockets[NUM_ROCKETS];

void rocketCtor(Rocket* self, u32 x, u32 y) {
	particleCtor(&self->particle, x, y);
	self->exploded = false;
	self->hasParts = 1;
	u32 temp = rand() & 0x7;
	self->color = temp + 8;
}

#define grand(start, span) (((rand() >> 1) / 2147483648.0 * (span)) + (start))

#define FOR_EACH_PART for (int i = 0; i < NUM_PARTS; ++i)
void rocketBoom(Rocket* self) {
	 FOR_EACH_PART {
		self->parts[i] = self->particle;
		vectorCtor(&self->parts[i].velocity, grand(-1.0, 2.0), grand(-1.0, 2.0));
		vectorCtor(&self->parts[i].acceleration, 0, 0);
		
		self->life[i] = 200;
	}
}

void rocketUpdate(Rocket* self) {
	if (!self->exploded) {
		particleAddForce(&self->particle, &g_gravity);
		particleUpdate(&self->particle);
		
		if (self->particle.velocity.y >= 0) {
			self->exploded = true;
			rocketBoom(self);
		}
	} else {
		u32 partsAlive = 0;
		FOR_EACH_PART {
			if (self->life[i] > 10) {
				particleAddForce(self->parts + i, &g_gravity);
				particleUpdate(self->parts + i);
				
				self->life[i] -= 5;
				vectorMul(&self->parts[i].velocity, 0.9);
				++partsAlive;
			}
			
			self->hasParts = partsAlive;
		}
	}
}

void particleDraw(Particle* particle, char sign, char color) {
	if (particle->position.x >= 0 && particle->position.x < 80 &&
			particle->position.y >= 0 && particle->position.y < 25) {
		
		putch(particle->position.x, particle->position.y, sign, color);
	}
}

void rocketDraw(Rocket* self) {
	if (!self->exploded) {
		particleDraw(&self->particle, '*', self->color);
	} else {
		if (self->hasParts) {
			FOR_EACH_PART {
				if (self->life[i] > 10) {
					particleDraw(self->parts + i, '.', self->color);
				}
			}
		} else {
			// rocket is fully done so reinstantiate it 
			rocketCtor(self, 39, 23);
		}
	}
}

void postcardCtor() {
	FOR_EACH_ROCKET
		rocketCtor(g_rockets + i, 39, 23);
		
	vectorCtor(&g_gravity, 0, 0.01);
}

void postcardUpdate() {
	FOR_EACH_ROCKET
		rocketUpdate(g_rockets + i);
}

void postcardDraw() {
	FOR_EACH_ROCKET
		rocketDraw(g_rockets + i);
}

