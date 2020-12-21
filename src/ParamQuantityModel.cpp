#include "ParamQuantityModel.hpp"


void ParamQuantityModel::addListener(std::function<void(float)> func) {
    callbacks.push_back(func);
}

void ParamQuantityModel::setValue(float value) {
    float oldValue = getValue();
    ParamQuantity::setValue(value);

    //This might seem overcautious, but the ParamQuantity::setValue() above might do things we don't know about
    float newValue = getValue();

    if (oldValue != newValue) {
        for (auto func : callbacks) {
            func(value);
        }
    }
}