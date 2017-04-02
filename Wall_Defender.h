#ifndef WALLDEFENDER_H_
#define WALLDEFENDER_H_

#include <allegro5/allegro.h>

//Defining images
ALLEGRO_BITMAP *background1 = NULL;
ALLEGRO_BITMAP *background2 = NULL;
ALLEGRO_BITMAP *background3 = NULL;
ALLEGRO_BITMAP *background4 = NULL;
ALLEGRO_BITMAP *backgroundEND = NULL;
ALLEGRO_BITMAP *backgroundCurrent = NULL;
ALLEGRO_BITMAP *bird1 = NULL;
ALLEGRO_BITMAP *bird2 = NULL;
ALLEGRO_BITMAP *birdCurrent = NULL;
ALLEGRO_BITMAP *bmpProjectile = NULL;
ALLEGRO_BITMAP *bmpProjectileHeavy = NULL;
ALLEGRO_BITMAP *bmpEnemyA1 = NULL;
ALLEGRO_BITMAP *bmpEnemyA2 = NULL;
ALLEGRO_BITMAP *bmpEnemyACurrent = NULL;
ALLEGRO_BITMAP *bmpEnemyB1 = NULL;
ALLEGRO_BITMAP *bmpEnemyB2 = NULL;
ALLEGRO_BITMAP *bmpEnemyBCurrent = NULL;

//Structures
struct Enemy {
	int x;
	int health;
	int speed;
	int type;
	bool onScreen;
};

struct Projectile {
	int x, y;
	bool onScreen;
};

//Prototyping
Projectile moveProjectile(Projectile toMove, bool heavy);
Enemy genEnemy(void);
bool checkHit(Projectile projectile, Enemy enemy);

#endif
