#include "Engine/3d/Model.h"

class Player
{
public:
	Player();
	~Player();

	void Initialize();

	void Update();

	void Draw();

	Vector3 GetPosition() {
		return Vector3{ 
			playerModel_->matWorld_.m[3][0],
			playerModel_->matWorld_.m[3][1] ,
			playerModel_->matWorld_.m[3][2] };
	}

private:

	std::unique_ptr<Model> playerModel_;

};
