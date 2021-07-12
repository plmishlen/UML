#include <iostream>
#include<conio.h>
#include<thread>
#include<exception>
using namespace std;

#define Escape 27
#define Enter 13

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
	explicit Tank(int volume) :volume(volume >= 40 && volume <= 120 ? volume : DEFAULT_TANK_VOLUME)
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
	double give_fuel(double amount)
	{
		fuel_level -= amount;
		if (fuel_level < 0)fuel_level = 0;
		return fuel_level;
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
	bool driver_inside;
	bool gas_pedal;
	unsigned int acceleration;
	unsigned int brake;

	struct Control
	{
		std::thread main_thread;
		std::thread panel_thread;
		std::thread engine_idle_thread;
		std::thread free_wheeling_thread;
	}control;								//control - экземпляр (объект) структуры 'Control'

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
		driver_inside = false;
		gas_pedal = false;
		acceleration = 5;
		control.main_thread = std::thread(&Car::control_car, this);	//запускаем метод control_car в потоке main_thread 
		cout << "CarIsReady:\t" << this << endl;
	}
	~Car()
	{
		if (control.engine_idle_thread.joinable())control.engine_idle_thread.join();
		if (control.main_thread.joinable())control.main_thread.join();
		cout << "CarIsOver:\t" << this << endl;
	}
	void get_in()
	{
		driver_inside = true;
		cout << "Вы находитесь в авто!" << endl;
		control.panel_thread = std::thread(&Car::panel, this);
	}
	void get_out()
	{
		driver_inside = false;
		if (control.panel_thread.joinable())control.panel_thread.join();
		system("CLS");
		cout << "Вы вышли из авто!" << endl;
	}
	void fill(unsigned int fuel)
	{
		tank.set_fuel_level(fuel);
	}
	void start()
	{
		if (driver_inside && tank.get_fuel_level() > 0)
		{
			engine.start();
			control.engine_idle_thread = std::thread(&Car::engine_idle, this);
		}
	}
	void stop()
	{
		engine.stop();
		if (control.engine_idle_thread.joinable())control.engine_idle_thread.join();
	}
	void engine_idle()
	{
		using namespace std::literals::chrono_literals;
		while (engine.is_started() && tank.give_fuel(engine.get_consumption_per_second() * 100))
			std::this_thread::sleep_for(1s);
		engine.stop();
	}
	void panel()const
	{
		using namespace std::chrono_literals;
		while (driver_inside)
		{
			system("CLS");
			cout << "Engine is: " << (engine.is_started() ? "started" : "stopped") << endl;
			cout << "Fuel level:\t" << tank.get_fuel_level() << "\t";
			if (tank.get_fuel_level() < tank.get_min_level()) cout << "Low fuel" << endl;
			cout << "Speed:   " << speed << "\t";
			cout << "Max speed: " << max_speed << endl;
			std::this_thread::sleep_for(1s);
		}
	}
	void control_car()
	{

		using namespace std::literals::chrono_literals;
		char key;
		do
		{

			key = _getch();
			switch (key)
			{
			case 'F':
			case 'f':						//fuel
				unsigned int fuel;
				cout << "Введите объем топлива: "; cin >> fuel; cout << endl;
				fill(fuel);
				break;
			case 'I':
			case 'i':						//ignition (зажигание)
				if (driver_inside)
				{
					if (!engine.is_started())start();
					else stop();
				}
				break;
			case'W':
			case'w':
				if (driver_inside && engine.is_started())
				{
					accellerate();
				}
				break;
			case'S':
			case's':
				if (driver_inside && engine.is_started())
				{
					braking();
				}
				break;
			case Enter:
				if (driver_inside)get_out();
				else get_in();
				break;
			case Escape:
				stop();
				get_out();
				cout << "Hasta La Vista, Baby!" << endl; break;
			}
			//	if (driver_inside)panel();
			std::this_thread::sleep_for(0.5s);
			if (speed > 0 && !control.free_wheeling_thread.joinable())control.free_wheeling_thread = std::thread(&Car::free_wheeling, this);
			else if (control.free_wheeling_thread.joinable())control.free_wheeling_thread.join();
		} while (key != Escape);
	}
	void accellerate()
	{
			speed += acceleration;
			using namespace std::literals::chrono_literals;
			std::this_thread::sleep_for(1s);
			//if(engine.get_consumption_per_second())
	}

	void braking()
	{
		if (acceleration>=0)
		{
			speed -= acceleration;
			using namespace std::literals::chrono_literals;
			std::this_thread::sleep_for(1s);
		}
		else stop();
	}
	void free_wheeling()
	{
		using namespace std::literals::chrono_literals;
		if (acceleration)
		{
			while (acceleration)
			{
				//for (speed; speed <= 250; speed++);
				speed++;
				std::this_thread::sleep_for(1s);
			}
		}else if (brake)
		{ 
			while (brake)
			{
				//for (speed; speed > 0; speed--);
				speed--;
				std::this_thread::sleep_for(1s);
			}
		}
	}
	void info()const
	{
		cout << "Engine:\n"; engine.info();
		cout << "Tank:\n"; tank.info();
		cout << "Engine is: " << (engine.is_started() ? "started" : "stopped") << endl;
		if (tank.get_fuel_level() < tank.get_min_level())
			cout << "Speed:\t\t" << speed << "km/h\n";
		cout << "MaxSpeed:\t" << max_speed << "km/h\n";
	}
};

//#define TANK_CHECK
//#define ENGINE_CHECK


int main()
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

	try
	{
		Car car(10, 50, 250);
		//car.info();

		cout << "Нажмите 'Enter', чтобы сесть в авто" << endl;
		cout << "Нажмите 'F', чтобы заправить авто" << endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

}