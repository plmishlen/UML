#include <iostream>
#include<conio.h>
using namespace std;

const unsigned int DEFAULT_TANK_VOLUME = 60;
const unsigned int MIN_FUEL_LEVEL = 5;
const unsigned int DEFAULT_CONSUMPTION = 8;
class Tank
{
	const unsigned int volume;
	double fuel_level;
	unsigned int min_level;
	unsigned int max_level;

public:
	unsigned int get_volume()const
	{
		return volume;
	}
	double get_fuel_level()const
	{
		return fuel_level;
	}
	unsigned int get_min_level()const
	{
		return min_level;
	}
	void set_fuel_level(double fuel)
	{
		if (fuel < 0)return;
		if (this->fuel_level + fuel < volume)
			fuel_level += fuel;
		else
			fuel_level = volume;
	}
	explicit Tank(int volume):volume(volume >= 40 && volume <= 120 ? volume : DEFAULT_TANK_VOLUME)
	{
		/*if (volume >= 40 && volume <= 120)
			this->volume = volume;
		else
			this->volume = 60;*/
		//this->volume = volume >= 40 && volume <= 120 ? volume : DEFAULT_TANK_VOLUME;
		fuel_level = 0;
		min_level = MIN_FUEL_LEVEL;
		cout << "TankReady:\t" << this << endl;
	}
	~Tank()
	{
		cout << "TankIsOver:\t" << this << endl;
	}
	void info()const
	{
		cout << "Volume: " << volume << endl;
		cout << "Fuel: \t";
		if (fuel_level < 10)cout << 0;
		cout << fuel_level << endl;
		cout << "MinFuel:";
		if (min_level < 10)cout << 0;
		cout << min_level << endl;
	}
};

class Engine 
{
	//Engine properties:
	unsigned int volume;
	unsigned int power;
	double consumption;					//расход литров на 100 км
	double consumption_per_second;		//расход литров в секунду
	
	//Engine state:
	bool started;

public:
	double get_consumption()const
	{
		return consumption;
	}
	double get_consumption_per_second()const
	{
		return consumption_per_second;
	}
	explicit Engine(double consumption)
	{
	/*	if (consumption >= 4 && consumption <= 25)
			this->consumption = consumption;
		else
			consumption = 8;*/
		this->consumption = consumption >= 4 && consumption <= 25 ? consumption : DEFAULT_CONSUMPTION;
		this->consumption_per_second = consumption * 5e-5;	//consumption * 5*10^-4
		started = false;
		cout << "EngineReady:\t" << this << endl;
	}
	~Engine()
	{
		cout << "EngineIsOver:\t" << this << endl;
	}

	bool is_started()const
	{
		return started;
	}
	void start()
	{
		started = true;
	}
	void stop()
	{
		started = false;
	}
	void info()const
	{
		cout << "Consumption:\t\t" << consumption << endl;
		cout << "ConsumptionPerSecond:\t" << consumption_per_second << endl;
	}
};

class Car
{
	Engine engine;
	Tank tank;
	const unsigned int max_speed;
	unsigned int speed;
	bool driver;
public:
	/*Car(const Engine& engine, const Tank& tank) :engine(engine), tank(tank)
	{
		cout << "CarIsReady:\t" << this << endl;
	}*/
	Car(double consumption, unsigned int volume, const unsigned int max_speed) :
		engine(consumption), tank(volume), max_speed(max_speed >= 90 && max_speed <= 400 ? max_speed : 250), speed(0)
	{
		//this->engine = consumption;
		//this->max_speed;
		driver = false;
		cout << "CarIsReady:\t" << this << endl;
	}
	~Car()
	{
		cout << "CarIsOver:\t" << this << endl;
	}
	void get_in()
	{
		driver = true;
	}
	void get_out()
	{
		driver = false;
	}
	bool driver_inside()const
	{
		return driver;
	}
	void fill(unsigned int fuel)
	{
		tank.set_fuel_level(fuel);
	}
	void start()
	{
		engine.start();
	}
	void stop()
	{
		engine.stop();
	}
	void display()const
	{
		cout << "Engine is: " << (engine.is_started() ? "started" : "stopped") << endl;
		cout << "Fuel level:\t" << tank.get_fuel_level() << "\t";
		if (tank.get_fuel_level() < tank.get_min_level()) cout << "Low fuel" << endl;
		cout << "Speed:   " << speed << "\t";
		cout << "Max speed: " << max_speed << endl;

	}
	void info()const
	{
		cout << "Engine:\n"; engine.info();
		cout << "Tank:\n"; tank.info();
		cout << "Engine is: " << (engine.is_started() ? "started" : "stopped") << endl;
		if(tank.get_fuel_level()<tank.get_min_level())
		cout << "Speed:\t\t" << speed << "km/h\n";
		cout << "MaxSpeed:\t" << max_speed << "km/h\n";
	}
};

//#define TANK_CHECK
//#define ENGINE_CHECK
#define Escape 27
#define Enter 13

void main()
{
	setlocale(LC_ALL, "");
#ifdef TANK_CHECK
	Tank tank(-35);
	tank.info();
	cout << "-----------------" << endl;
	tank.set_fuel_level(30);
	tank.info();
	cout << "-----------------" << endl;
	tank.set_fuel_level(40);
	tank.info();
#endif // TANK_CHECK

#ifdef ENGINE_CHECK
	Engine engine(10);
	engine.info();
#endif // ENGINE_CHECK

	Car car(10,50,250);
	//car.info();
	
	cout << "Press Enter to get in" << endl;
	cout << "Press F to fill the car" << endl;
	char key;
	do
	{
		key = _getch();
		switch (key)
		{
		case 'F':
		case 'f':
			unsigned int fuel;
			cout << "¬ведите объем топлива: "; cin >> fuel; cout << endl;
			car.fill(fuel);
			break;
		case 'S':
		case 's':
			if (car.driver_inside())
				car.start();
		case Enter: car.get_in(); break;
			if (car.driver_inside())car.get_out();
			else car.get_in(); break;
		case Escape:cout << "Get out" << endl; break;
		}
		if (car.driver_inside())car.display();
		else system("CLS");
	} while (key != 27);

}