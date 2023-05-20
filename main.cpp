#include "Callable.h"
#include "Signal.h"

#include "Object.h"
#include "AutoSignal.h"

#include <iostream>
#include <unordered_set>

#include <random>
#include <time.h>

class TemperatureModel
{
public:
    AutoSignal<float> temperatureChanged;

    float getTemperature() const
    {
        return temp_;
    }

    void update()
    {
        const int r = rand() % 5 - 2;
        float new_temp = temp_ + (float)r;
        set_temperature(new_temp);
    }

private:
    void set_temperature(float temp)
    {
        if (temp_ == temp)
        {
            return;
        }
        temp_ = temp;
        temperatureChanged(temp_);
    }

private:
    float temp_{0};
};


class TemperatureView : public Object
{
public:
    TemperatureView(TemperatureModel* model)
    {
        model->temperatureChanged.connect(this, [](float temp){ std::cout << "changed: " << temp << "\n"; });
    }
};

class TemperatureView2 : public Object
{
public:
    TemperatureView2(TemperatureModel* model)
    {
        model->temperatureChanged.connect(this, [](float temp){ std::cout << "changed2: " << temp << "\n"; });
    }
};

int main()
{
	srand(time(nullptr));

    auto model = new TemperatureModel();

    std::cout << "view 1\n";
    auto view = new TemperatureView(model);
    model->update();
    model->update();
    model->update();
    model->update();
    model->update();
    model->update();
    std::cout << "view 2\n";
    auto view2 = new TemperatureView2(model);
    model->update();
    model->update();
    model->update();
    model->update();
    delete view;
    std::cout << "delete view 1\n";
    model->update();
    model->update();
    model->update();
    delete view2;
    std::cout << "delete view 2\n";
    model->update();
    model->update();
    model->update();
    model->update();
    model->update();
    model->update();
    model->update();
    std::cout << "delete model\n";
    delete model;
    std::cout << "delete end\n";
}