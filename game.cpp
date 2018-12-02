#include "Framework.h"
#include <iostream>
#include <list>
#include <vector>
#include <time.h>
/* Test Framework realization */

class GameObject
{
public:
	GameObject() { is_alive = true;}
	virtual ~GameObject() {}

	void SetPosition(int x, int y) {
		this->pos_x = x;
		this->pos_y = y;
	}
	void GetPosition(int &x, int &y) const {
		x = pos_x;
		y = pos_y;
	}
	virtual void	kill() {
		is_alive = false;
	}
	virtual bool IsAlive() const {
		return(this->is_alive);
	}
	virtual bool checkColision(GameObject *obj_1, GameObject *obj_2, 
		int A_width, int A_heigth, int B_width, int B_heigth)
	{
		int Ax, Ay, Bx, By;
		obj_1->GetPosition(Ax, Ay);
		obj_2->GetPosition(Bx, By);

		if ((Ax + A_width) >= Bx && Ax <= (Bx + B_width) && //colision box
		   (Ay + A_heigth) >= By && Ay <= (B_heigth + By)){
			return(true);		
		}
		return (false);
	}

protected:
	int pos_x;
	int pos_y;

	bool is_alive;
};

class Player : public GameObject {

public:
	Player() {};
	Player(int x, int y) {
		SetPosition(x, y);
		is_alive = true;
	}
private:

};

class Enemy : public GameObject
{
public:
	Enemy() {}
	Enemy(int x, int y) {
		SetPosition(x, y);
		is_alive = true;

		getScreenSize(dir_x, dir_y);
		dir_y /= 2;
		dir_x /= 2;
		speed = rand() % 5 + 15;
	}
	~Enemy() {}


	bool	attack() {
		if (getTickCount() % speed == 0) {
			if (pos_x <= dir_x && pos_y <= dir_y) {
				pos_x++; pos_y++;
			}
			else if (pos_x >= dir_x && pos_y >= dir_y) {
				pos_x--; pos_y--;
			}
			else if (pos_x >= dir_x && pos_y <= dir_y) {
				pos_x--; pos_y++;
			}
			else if (pos_x <= dir_x && pos_y >= dir_y) {
				pos_x++; pos_y--;
			}
			else if (pos_x == dir_x && pos_y == dir_y) {
				std::cout << true;
				return (true);
			}
		}
		return false;
	}
private:
	int dir_x;
	int dir_y;
	int speed;

};

class Bullet : public GameObject
{
public:

	Bullet(int x, int y, int _pos_x,int _pos_y) : dir_x(x), dir_y(y){
		pos_x = _pos_x;
		pos_y = _pos_y;
	}
	virtual ~Bullet() {}

	void	shoot(){
		if (pos_x <= dir_x && pos_y <= dir_y){
			if (dir_x - pos_x >= dir_y - pos_y)
				pos_x++; 
			else
				pos_y++;
		}
		if (pos_x >= dir_x && pos_y >= dir_y ){
			if (pos_x - dir_x >= pos_y - dir_y)
				pos_x--;
			else
				pos_y--;
		}
		if (pos_x >= dir_x && pos_y <= dir_y ){
			if (pos_x - dir_x >=  dir_y - pos_y )
				pos_x--;
			else
				pos_y++;
		}
		if (pos_x <= dir_x && pos_y >= dir_y){
			if (dir_x - pos_x >= pos_y - dir_y)
				pos_x++;
			else
				pos_y--;
		}
	}

private:
	int dir_x;
	int dir_y;
};


class Game: public Framework {

public:

	Game(int wind_x, int wind_y, int map_x, int map_y, int enemies, int bullets) {
		window_size[0] = wind_x;
		window_size[1] = wind_y;
		_enemies = enemies;
		_bullets_cnt = bullets;
	}
	void getPlayerPos(int x, int y) {
		x = player_pos[0];
		y = player_pos[1];
	};

	virtual void PreInit(int& width, int& height, bool& fullscreen)
	{
		srand(time(NULL));
		if (window_size[0] > 100 && window_size[0] < 3000)
			width = window_size[0];
		else
			width = 500;
		
		if (window_size[1] > 100 && window_size[1] < 3000)
			height = window_size[1];
		else
			height = 500;

		fullscreen = false;
	}

	virtual bool Init() {
	 /*************Sptites*************/ 
		s_circle = createSprite("data/circle.tga");
		s_hero = createSprite("data/hero.png");
		s_bullet = createSprite("data/bullets.png");
		s_enemy = createSprite("data/ufo1.png");
		s_back = createSprite("data/back.jpg");

		getSpriteSize(s_hero, sprt_size[0], sprt_size[1]);
		getSpriteSize(s_bullet, sprt_size[2], sprt_size[3]);
		getSpriteSize(s_enemy, sprt_size[4], sprt_size[5]);
		background[0] = background[1] = -150;
	
		/*************PlayerPos*************/
		
		int wind[2];
		//int	sprite[2];
		getScreenSize(wind[0], wind[1]);
		//getSpriteSize(s_hero, sprite[0], sprite[1]);


		player = new Player(wind[1] / 2 - sprt_size[0] / 2, wind[0] / 2 - sprt_size[1] / 2);
		player->GetPosition(player_pos[0], player_pos[1]);

		/***************Spawner enemies****************/
		for (int i = 0; i < _enemies;) {
			int spawn_x = rand() % wind[0];
			int spawn_y = rand() % wind[1];
			if (!(spawn_x + 100 >= player_pos[0] && //Collision Box check, for not spawn close for player
				spawn_x <= player_pos[0] + 2 * sprt_size[0] &&
				spawn_y + 100 >= player_pos[1] && player_pos[1] + 2 * sprt_size[1])){
				enemies.push_back(new Enemy(spawn_x, spawn_y));
				i++;
			}
		}
		return (true);
	}

	virtual void Close() {
		delete player;
		enemies.clear();
		bullets.clear();
		destroySprite(s_circle);
		destroySprite(s_hero);
		if (s_bullet != NULL)
			destroySprite(s_bullet);
		
	}

	virtual bool Tick() {

		drawSprite(s_back,background[0],background[1]); // drawing background
		for (auto var : enemies) {
			int curr_x, curr_y;
			if (var->attack())
				return (true);
			var->GetPosition(curr_x, curr_y);
			if (player->checkColision((GameObject *)var, (GameObject *)player, 
				sprt_size[4], sprt_size[5], sprt_size[0] - 15, sprt_size[1] - 15)){
				std::cout << "Game over\n" << std::endl;
				Close();
			}
			if (var->IsAlive())
				drawSprite(s_enemy, curr_x, curr_y);
			}
		for(auto bull: bullets){
			int curr_x, curr_y;
			bull->shoot();
			bull->GetPosition(curr_x, curr_y);
			for (auto var : enemies) {
				if (var->checkColision((GameObject *)var, (GameObject *)bull, 
					sprt_size[4], sprt_size[5],  sprt_size[2], sprt_size[3])) {
					var->kill();
					bull->kill();
				}
			}
			if(bull->IsAlive())
				drawSprite(s_bullet, curr_x, curr_y);
		}
		drawSprite(s_hero,player_pos[0], player_pos[1]);
		drawSprite(s_circle, mouse_pos[0] - 15, mouse_pos[1] - 10);
		return false;
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative) {
		mouse_pos[0] = x;
		mouse_pos[1] = y;
		showCursor(false);
	}

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased) {
		if (button == FRMouseButton::LEFT && !isReleased) {
			_bullets_cnt == 0 ? 0 : _bullets_cnt--;
			if (_bullets_cnt == 0){ //if bullets are over oldest remove
				_bullets_cnt++;
				bullets.pop_front();		
			}
			std::cout << "Bullets: [" << _bullets_cnt <<"]" << std::endl;
			bullets.push_back(new Bullet(mouse_pos[0], mouse_pos[1], player_pos[0] + 25, player_pos[1])); // new bullet
		}
	}

	virtual void onKeyPressed(FRKey k) {
		if (k == FRKey::UP) {
			background[1] == 0 ? background[1] -= 15 : background[1] += 5;
			for (auto var : enemies) { //move all enemies and bullets
				int x = 0;int y = 0;
				var->GetPosition(x,y);
				var->SetPosition(x, y + 10);
			}
			for (auto var : bullets) {
				int x = 0; int y = 0;
				var->GetPosition(x, y);
				var->SetPosition(x, y + 10);
			}
		}
		else if (k == FRKey::DOWN) {
			background[1] -=5;
			for (auto var : enemies) {
				int x = 0; int y = 0;
				var->GetPosition(x, y);
				var->SetPosition(x, y - 10);
			}
			for (auto var : bullets) {
				int x = 0; int y = 0;
				var->GetPosition(x, y);
				var->SetPosition(x, y - 10);
			}
		}
		else if (k == FRKey::LEFT) {
			background[0] == 0 ? background[0]-= 15 : background[0] +=5;
			for (auto var : enemies) {
				int x = 0; int y = 0;
				var->GetPosition(x, y);
				var->SetPosition(x - 10, y);
			}
			for (auto var : bullets) {
				int x = 0; int y = 0;
				var->GetPosition(x, y);
				var->SetPosition(x - 10, y);
			}
		}
		else if (k == FRKey::RIGHT) {
			background[0] -=5;
			for (auto var : enemies) {
				int x = 0; int y = 0;
				var->GetPosition(x, y);
				var->SetPosition(x + 10, y);
			}
			for (auto var : bullets) {
				int x = 0; int y = 0;
				var->GetPosition(x, y);
				var->SetPosition(x + 10, y);
			}
		}
	}

	virtual void onKeyReleased(FRKey k) {

	}
private:
		std::list <Enemy *> enemies;
		std::list <Bullet *> bullets;
		Player *player;

		int sprt_size[6];
		int player_pos[2];
		int mouse_pos[2];
		int window_size[2];
		int background[2];

		int _bullets_cnt;
		int _enemies;
		int _score;

		Sprite *s_hero;
		Sprite *s_bullet;
		Sprite *s_enemy;
		Sprite *s_circle;
		Sprite *s_back;
};


std::string check_attribute(char *parse) {
	
	std::string lol;
	parse++;
	for (int i = 0; parse[i] != '-' && parse[i]; i++)
		lol += parse[i];

	return (lol);
}


typedef struct s_params
{
	int wind_x;
	int wind_y;
	int map_x;
	int map_y;
	int enemies;
	int bullets;

}				t_params;

int	parser(char **av, int ac, std::string parse, t_params *ptr)
{
	int j = 1;
	std::string window, map, enemies, bullets;
	while (j < ac)
	{
		for (int i = 0; av[j][i] != '\0'; i++)
			parse += av[j][i];
		j++;
	}
	for (int i = 0; i < parse.length(); i++){
		if (parse[i] == '-')
		{
			if (window.length() == 0) {
				window = check_attribute(&parse[i]);
				i += window.length();
			}
			if (map.length() == 0) {
				map = check_attribute(&parse[i]);
				i += map.length();
			}
			if (enemies.length() == 0) {
				enemies = check_attribute(&parse[i]);
				i += enemies.length();
			}
			if (bullets.length() == 0){
				bullets = check_attribute(&parse[i]);
			}
		}
	}
	ptr->wind_x = atoi(window.erase(0, 6).c_str());
	ptr->wind_y = atoi(window.erase(0, 4).c_str());
	ptr->map_x = atoi(map.erase(0, 3).c_str());
	ptr->map_y = atoi(map.erase(0, 5).c_str());
	ptr->bullets = atoi(bullets.erase(0, 8).c_str());
	ptr->enemies = atoi(enemies.erase(0, 11).c_str());

	return (1);
}


int main(int argc, char *argv[])
{
	t_params stock;
	stock.bullets = 10; //default params
	stock.enemies = 2;
	stock.wind_x = 500;
	stock.wind_y = 500;
	std::string parse;
	if (argc > 1)
		parser(argv, argc, parse, &stock);
	return run(new Game(stock.wind_x, stock.wind_y, stock.map_x, stock.map_y, stock.bullets, stock.enemies));
}

