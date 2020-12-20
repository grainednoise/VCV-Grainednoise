#pragma once

#include <rack.hpp>
#include <functional>

using namespace rack;


class ParamQuantityModel : public ParamQuantity {
    private:
        std::vector<std::function<void(float)>> callbacks;
    
    public:
        void addListener(std::function<void(float)> func);
        void setValue(float value) override;
};