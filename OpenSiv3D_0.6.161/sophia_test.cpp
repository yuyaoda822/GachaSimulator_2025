# include <Siv3D.hpp>

Vec2 Position(double v0x, double v0y, double x0, double y0, double t) {
	double g = 9.8;
	// 重力加速度の調整
	double adjust = -10;
	return { v0x * t + x0, v0y * t + y0 - adjust * 0.5 * g * t * t };
}

void Main()
{
	Scene::SetBackground(Palette::Lightskyblue);
	double time = 0.0;
	while (System::Update())
	{
		time += Scene::DeltaTime();
		Circle(Position(300.0, 0.0, 0.0, 300.0, time), 50).draw(Color{ 255, 100, 100 });

	}
}
