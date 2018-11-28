

#include "Framework.h"
#include <iostream>
#include <list>
/* Test Framework realization */


class MyFramework : public Framework {
private:
	Sprite *_hero;
	Sprite *_bullets;
	Sprite *_enemy;
	Sprite *_circle;
public:

	virtual void PreInit(int& width, int& height, bool& fullscreen)
	{
		width = 500;
		height = 500;
		fullscreen = false;

	}

	virtual bool Init(){
		_circle = createSprite("data/circle.tga");
		_hero = createSprite("data/avatar.jpg");
		_bullets = createSprite("data/bullet.png");
		_enemy = createSprite("data/enemy.png");

		
		return true;
	}

	virtual void Close() {
	}

	virtual bool Tick() {
		int wind[2];
		int	sprite[2];
		static int i;

		getScreenSize(wind[0], wind[1]);
		getSpriteSize(_hero, sprite[0], sprite[1]);
		drawTestBackground();

		drawSprite(_hero, (wind[1] / 2 - sprite[0] / 2), wind[0] / 2 - sprite[1] / 2);

		drawSprite(_circle, i -2 , i++);

		return false;
	}

	virtual void onMouseMove(int x, int y, int xrelative, int yrelative) {

		std::cout << x << " " << y << " " << xrelative << "  " << yrelative << std::endl;

	}

	virtual void onMouseButtonClick(FRMouseButton button, bool isReleased) {
	
		int w = 0; int h = 0;
		Sprite *sprite  = createSprite("data/bullet.png");
		if (button == FRMouseButton::LEFT)
			Close();
	}

	virtual void onKeyPressed(FRKey k) {
		int posx, posy = 0;
		if (k == FRKey::UP)
			posy++;
		else if (k == FRKey::DOWN)
			posy--;
		else if (k == FRKey::LEFT)
			posx--;
		else if (k == FRKey::RIGHT)
			posx++;
			
	}

	virtual void onKeyReleased(FRKey k) {
	}
};

int main(int argc, char *argv[])
{
	return run(new MyFramework);
}
