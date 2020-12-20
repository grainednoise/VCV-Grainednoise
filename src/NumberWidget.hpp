#include "plugin.hpp"
#include "ParamQuantityModel.hpp"

class NumberWidget : public LedDisplayTextField {
private:
    unsigned long numberValue;
    ParamQuantityModel *paramQuantity;
    std::string previousStringValue = "";
    bool updateFromFloat(float value);

public:
    NumberWidget(ParamQuantityModel *paramQuantity);
    bool updateFromQuantity();  //Update the displayvalue from the associated quantity
    void onSelectKey(const event::SelectKey& e) override;
    void onSelectText(const event::SelectText& e) override;
    void draw(const DrawArgs& args) override;
};

