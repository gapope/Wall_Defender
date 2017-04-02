#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "Wall_Defender.h"

const int SCREEN_W = 800;
const int SCREEN_H = 600;
const float FPS = 60;
char const *fontName  = "Data/arial.ttf";

int main() {
	//Allegro initialization
	al_init();
	al_init_image_addon();
	al_init_font_addon();
    al_init_ttf_addon();
    al_install_keyboard();
    al_install_mouse();
	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_FONT *text;
	ALLEGRO_FONT *title;
    text = al_load_ttf_font(fontName, 20, 0);
    title = al_load_ttf_font(fontName, 48, 0);
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;
	ALLEGRO_KEYBOARD_STATE key_state;
	ALLEGRO_MOUSE_STATE mouse_state;
	
	//Error checking
	if (!al_init()) {
    	std::cerr << "failed to initialize allegro! "  << std::endl;
		return -1;
	}
	if (!al_install_keyboard()) {
   		std::cerr << "failed to initialize the keyboard! "  << std::endl;
		return -1;
	}
	display = al_create_display(SCREEN_W, SCREEN_H); //Creating screen	
   	if (!display) {
      std::cerr << "failed to create display!" << std::endl;
     
      return -1;
	}
	
	srand(time(0));
	
	//Setting timer
	timer = al_create_timer(0.5 / FPS);
	
	//Loading images
	background1 = al_load_bitmap("Data/Images/background1.bmp");
	background2 = al_load_bitmap("Data/Images/background2.bmp");
	background3 = al_load_bitmap("Data/Images/background3.bmp");
	background4 = al_load_bitmap("Data/Images/background4.bmp");
	backgroundEND = al_load_bitmap("Data/Images/backgroundEND.bmp");
	backgroundCurrent = background1;
	bird1 = al_load_bitmap("Data/Images/bird1.png");
	bird2 = al_load_bitmap("Data/Images/bird2.png");
	birdCurrent = bird1;
	bmpProjectile = al_load_bitmap("Data/Images/projectile.png");
	bmpProjectileHeavy = al_load_bitmap("Data/Images/projectileHeavy.png");
	bmpEnemyA1 = al_load_bitmap("Data/Images/enemyA1.png");
	bmpEnemyA2 = al_load_bitmap("Data/Images/enemyA2.png");
	bmpEnemyACurrent = bmpEnemyA1;
	bmpEnemyB1 = al_load_bitmap("Data/Images/enemyB1.png");
	bmpEnemyB2 = al_load_bitmap("Data/Images/enemyB2.png");
	bmpEnemyBCurrent = bmpEnemyB1;

	//Setting back colour
	al_clear_to_color(al_map_rgb(255, 255, 255));
	
	//Event queue set up
	event_queue = al_create_event_queue();
	
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_mouse_event_source());;
	
	bool start = false;
	
	while (!start) { //Start screen
		//Menu text
		al_draw_text(title, al_map_rgb(0, 0, 0), 400, 85, ALLEGRO_ALIGN_CENTRE, "Wall Defender");
		al_draw_text(text, al_map_rgb(0, 0, 0), 400, 300, ALLEGRO_ALIGN_CENTRE, "PLAY");
		
		al_draw_text(text, al_map_rgb(0, 0, 0), 5, 5, 0, "Coded by Gregory Pope");
		al_draw_text(text, al_map_rgb(0, 0, 0), 5, 30, 0, "Sprites by Zillad Van Jaarsveld");
		
		//Instructions
		al_draw_text(text, al_map_rgb(0, 0, 0), 400, 350, ALLEGRO_ALIGN_CENTRE, "The bird will follow your mouse");
		al_draw_text(text, al_map_rgb(0, 0, 0), 400, 375, ALLEGRO_ALIGN_CENTRE, "Left-click to poop; right-click super-poop");
		al_draw_text(text, al_map_rgb(0, 0, 0), 400, 400, ALLEGRO_ALIGN_CENTRE, "Smaller, faster enemies take 1 hit to kill");
		al_draw_text(text, al_map_rgb(0, 0, 0), 400, 425, ALLEGRO_ALIGN_CENTRE, "Larger slower enemies will take 2 hits");
		
		al_flip_display();
	
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		
		if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN && ev.mouse.x >= 350 && ev.mouse.x <= 450 && 
			ev.mouse.y >= 275 && ev.mouse.y <= 325) { //When start is clicked
			start = true;
		}
	}
	
	//General definitions
	int i = 0, j = 0, timeCount = 0, sinceLast = 0, sinceLastHeavy = 0;
	int wallHealth = 2000, score = 0;
	int projectiles = 0, enemies = 0, maxProjectiles = 10, maxEnemies = 5;
	float bird_x = 375;
	bool quit = false, redraw = true;
	bool hitByHeavy[maxEnemies] = {false};
	
	Projectile projectile[maxProjectiles];
	Projectile projectileHeavy;
	Enemy enemy[maxEnemies];
	
	for (i = 0; i < maxProjectiles; i++) {
		projectile[i].onScreen = false;
		projectile[i].y = 0;
	}
	projectileHeavy.onScreen = false;
	projectileHeavy.y = 0;
	for (i = 0; i < maxEnemies; i++) {
		enemy[i].onScreen = false;
		enemy[i].x = 800;
	}
	
	//Setting mouse
	al_grab_mouse(display);
	al_set_mouse_xy(display, 400, 300);
	
	//Starting timer process
	al_start_timer(timer);
	
	//Looping processes until player presses ESC to quit; gameplay
	while (start) {
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);
		
		if(ev.type == ALLEGRO_EVENT_TIMER) { //General time counters and frame switching
        	redraw = true;
        	timeCount++;
			sinceLast--;
			sinceLastHeavy--;
			if (timeCount % 25 == 0) { //Switching enemy A frame
				if (bmpEnemyACurrent == bmpEnemyA1) {
					bmpEnemyACurrent = bmpEnemyA2;
				}
				else {
					bmpEnemyACurrent = bmpEnemyA1;
				}
			}
			if (timeCount % 20 == 0) { //Switching enemy B frame
				if (bmpEnemyBCurrent == bmpEnemyB1) {
					bmpEnemyBCurrent = bmpEnemyB2;
				}
				else {
					bmpEnemyBCurrent = bmpEnemyB1;
				}
			}
			if (timeCount % 20 == 0) { //Switching bird frame
				if (birdCurrent == bird1) {
					birdCurrent = bird2;
				}
				else {
					birdCurrent = bird1;
				}
			}
      	}
      	else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { //Quitting if window is closed
        	break;
      	}
      	else if (ev.type == ALLEGRO_EVENT_MOUSE_AXES) { //Matching bird position to mouse
        	bird_x = ev.mouse.x;
        	
        	if (ev.mouse.x < 70) {
        		al_set_mouse_xy(display, 70, ev.mouse.y);
			}
      	}
      	else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) { //Launching projectiles on mouse-click
      		if (sinceLast <= 0 && ev.mouse.x >= 70 && ev.mouse.button == 1) { //Launching projectile
				projectile[projectiles].onScreen = true;
      			projectile[projectiles].x = ev.mouse.x - (al_get_bitmap_width(bmpProjectile) / 2);
      			projectile[projectiles].y = 75;
      			projectiles++;
      		
      			if (projectiles > maxProjectiles) {
      				projectiles = 0;
				}	
				
				for (i = 0; i < maxEnemies; i++) {
					hitByHeavy[i] = false;
				}
				
				sinceLast = 80;
			}
			else if (sinceLastHeavy <= 0 && ev.mouse.x >= 70 && ev.mouse.button == 2) { //Launching standard heavy projectile
				projectileHeavy.onScreen = true;
      			projectileHeavy.x = ev.mouse.x - (al_get_bitmap_width(bmpProjectileHeavy) / 2);
      			projectileHeavy.y = 75;
      			sinceLastHeavy = 600;
			}
		}
      	else if (ev.type == ALLEGRO_EVENT_KEY_DOWN && ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) { //Quits if ESC is pressed
      		start = false;
		}
		
		//Drawing background, general calculations and checks
		if (redraw && al_is_event_queue_empty(event_queue)) {
        	redraw = false;
        	
        	//Timing for enemy generation
        	if (timeCount % 120 == 0) {
        		for (i = 0; i < maxEnemies ; i++) {
        			if (!enemy[i].onScreen) {
						enemy[i] = genEnemy();
						break;
					}	
				}
			}
        	
        	//Timing for object movement
        	if (timeCount % 5 == 0) {
				for (i = 0; i < maxEnemies; i++) { 
					if (projectileHeavy.onScreen && !hitByHeavy[i] && projectileHeavy.y >= SCREEN_H - 
					al_get_bitmap_height(bmpProjectileHeavy) - 15 && enemy[i].x >= projectileHeavy.x - 50 && 
					enemy[i].x <= projectileHeavy.x + al_get_bitmap_width(bmpProjectileHeavy) + 50) { //Checking heavy hit
						
						enemy[i].health -= 2;
						hitByHeavy[i] = true;
						if (enemy[i].health <= 0) {
							enemy[i].onScreen = false;
							score += (enemy[i].type + 1) * 100;
						}
					}
					
					if (enemy[i].onScreen && enemy[i].x > 75) { //Moving enemies
    					enemy[i].x -= enemy[i].speed;
					}
				}
				
        		for (i = 0; i < maxProjectiles; i++) { //Moving projectiles
        			projectile[i] = moveProjectile(projectile[i], false);
				}
				
				projectileHeavy = moveProjectile(projectileHeavy, true); //Moving heavy projectile
				
				//Checking for enemy/projectile collision
				for (i = 0; i < maxProjectiles; i++) {
					for (j = 0; j < maxEnemies; j++) {
						if (projectile[i].onScreen && enemy[j].onScreen) {
							if (checkHit(projectile[i], enemy[j])) {
								enemy[j].health--;
								projectile[i].onScreen = false;
								if (enemy[j].health <= 0) {
									enemy[j].onScreen = false;
									score += (enemy[j].type + 1) * 100;
								}
							}
						}
					}
				}
			}
        	
        	if (timeCount % 30 == 0) { //Enemies attacking wall if they are against it
        		for (i = 0; i < maxEnemies; i++) {
        			if (enemy[i].onScreen && enemy[i].x <= 75) {
						if (enemy[i].type == 0) {
							wallHealth -= 7;
						}
						else if (enemy[i].type == 1) {
							wallHealth -= 5;
						}
					}
					if (wallHealth <= 0) { //Game over
        				start = false;
					}
				}
			}
        	
        	//Changing background/wall hight based on damage
        	if ((float)wallHealth / 2000 <= 0.25) {
        		backgroundCurrent = background4;
			}
			else if ((float)wallHealth / 2000 <= 0.50) {
        		backgroundCurrent = background3;
			}
			else if ((float)wallHealth / 2000 <= 0.75) {
        		backgroundCurrent = background2;
			}
			else {
				backgroundCurrent = background1;
			}
        	
        	al_draw_bitmap(backgroundCurrent, 0, 0, 0);
 
        	al_draw_bitmap(birdCurrent, bird_x - (al_get_bitmap_width(birdCurrent)/2), 25, 0);
 
			for (i = 0; i < maxProjectiles; i++) { //Drawing projectiles
				if (projectile[i].onScreen) {
					al_draw_bitmap(bmpProjectile, projectile[i].x, projectile[i].y, 0);
				}
			}
			
			if (projectileHeavy.onScreen) { //Drawing heavy projectile
				al_draw_bitmap(bmpProjectileHeavy, projectileHeavy.x, projectileHeavy.y, 0);
			}
			
			for (i = 0; i < maxEnemies; i++) { //Drawing enemies
				if (enemy[i].onScreen) {
					if (enemy[i].type == 0){
						al_draw_bitmap(bmpEnemyACurrent, enemy[i].x, 600 - 110, 0);
					}
					else if (enemy[i].type == 1) {
						al_draw_bitmap(bmpEnemyBCurrent, enemy[i].x, 600 - 90 , 0);
					}
				}
			}
 
 			//Writing info
 			al_draw_textf(text, al_map_rgb(0, 0, 0), 550, 0, 0, "wall health: %d", wallHealth);
 			al_draw_textf(text, al_map_rgb(0, 0, 0), 50, 0, 0, "%d points", score);
 
        	al_flip_display();
    	}
	}
	
	while (!quit && wallHealth <= 0) { //Game over screen
		al_draw_bitmap(backgroundEND, 0, 0, 0);
 		al_draw_text(text, al_map_rgb(255, 0, 0), 400, 250, ALLEGRO_ALIGN_CENTRE, "GAME OVER");
		al_draw_textf(text, al_map_rgb(0, 0, 0), 400, 300, ALLEGRO_ALIGN_CENTRE, "%d points", score);
		al_draw_text(text, al_map_rgb(0, 0, 0), 400, 555, ALLEGRO_ALIGN_CENTRE, "Press ESC to quit");
		al_flip_display();
		
		al_get_keyboard_state(&key_state);
		if (al_key_down(&key_state, ALLEGRO_KEY_ESCAPE)){ //Quits when ESC is pressed
			quit = true;
		}
	}
	
	//Freeing memory
	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_bitmap(background1);
	al_destroy_bitmap(background2);
	al_destroy_bitmap(background3);
	al_destroy_bitmap(background4);
	al_destroy_bitmap(backgroundEND);
	al_destroy_bitmap(backgroundCurrent);
	al_destroy_bitmap(bird1);
	al_destroy_bitmap(bird2);
	al_destroy_bitmap(birdCurrent);
	al_destroy_bitmap(bmpProjectile);
	al_destroy_bitmap(bmpProjectileHeavy);
	al_destroy_bitmap(bmpEnemyA1);
	al_destroy_bitmap(bmpEnemyA2);
	al_destroy_bitmap(bmpEnemyACurrent);
	al_destroy_bitmap(bmpEnemyB1);
	al_destroy_bitmap(bmpEnemyB2);
	al_destroy_bitmap(bmpEnemyBCurrent);
	return 0;
}

//Function to move launched projectiles
Projectile moveProjectile(Projectile toMove, bool heavy) {
   	if (toMove.onScreen) {
    	toMove.y += 8;
	}
	if (toMove.y >= SCREEN_H - al_get_bitmap_height(bmpProjectile) - 10 && !heavy) {
		toMove.onScreen = false;
	}
	if (toMove.y >= SCREEN_H - al_get_bitmap_height(bmpProjectileHeavy) - 8 && heavy) {
		toMove.onScreen = false;
	}
	return toMove;
}

//Function to create enemies
Enemy genEnemy(void) {
	Enemy newEnemy;
	
	newEnemy.onScreen = true;
	newEnemy.x = 800;
	newEnemy.type = rand() % 2;
	
	//Setting enemy's walking speed based on type
	if (newEnemy.type == 0) {
		newEnemy.speed = 2;
		newEnemy.health = 2;
	}
	else if (newEnemy.type == 1) {
		newEnemy.speed = 5;
		newEnemy.health = 1;
	}
	
	return newEnemy;
}

//Function to check if enemy and projectile are intersecting
bool checkHit(Projectile projectile, Enemy enemy) {
	int projectile_top = projectile.y;
	int projectile_bottom = projectile.y + al_get_bitmap_height(bmpProjectile);
	int projectile_left = projectile.x;
	int projectile_right = projectile.x + al_get_bitmap_width(bmpProjectile);
	
	int enemy_top = 0;
	int enemy_bottom = 0;
	int enemy_left = enemy.x;
	int enemy_right = 0;
	
	if (enemy.type == 0) {
		enemy_top = 600 - 100;
		enemy_bottom = 600 - 110 + al_get_bitmap_height(bmpEnemyACurrent);
		enemy_right = enemy_left + al_get_bitmap_width(bmpEnemyACurrent);
	}
	else if (enemy.type == 1) {
		enemy_top = 600 - 90;
		enemy_bottom = 600 - 90 + al_get_bitmap_height(bmpEnemyBCurrent);
		enemy_right = enemy_left + al_get_bitmap_width(bmpEnemyBCurrent);
	}
	
	//Checking for intersection between projectile and enemy boundaries
	if (enemy_bottom < projectile_top || enemy_top > projectile_bottom || 
		enemy_right < projectile_left || enemy_left > projectile_right) {
		return false;
	}
	else {
		return true;
	}
}
