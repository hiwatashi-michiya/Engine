#include "Engine/3d/Model.h"

class Player
{
public:
	Player();
	~Player();

	void Initialize();

	void Update();

	void Draw();

private:

	std::unique_ptr<Model> playerModel_;

};
