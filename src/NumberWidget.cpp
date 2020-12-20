#include "NumberWidget.hpp"


 NumberWidget::NumberWidget(ParamQuantityModel *paramQuantity) {
     multiline = false;
     this->paramQuantity = paramQuantity;

     if (!paramQuantity) {
        WARN("paramQuantity may not be null");
        return;
     }
    
    updateFromQuantity();

    paramQuantity->addListener([&](float v) { 
        this->updateFromFloat(v);
    });
}

bool NumberWidget::updateFromFloat(float value) {
    unsigned long newNumber = round(value);
    bool changed = newNumber != numberValue;

    if (changed) {
       numberValue = newNumber;
       setText(std::to_string(newNumber));
    }

    return changed;
}

bool NumberWidget::updateFromQuantity() {
    if (!paramQuantity) {
        return false;
    }

    return updateFromFloat(paramQuantity->getValue());
 }

 void NumberWidget::draw(const DrawArgs& args) {
     //Unfortunately, we can't count on this->paramQuantity.setValue() being
     //called prior to (first time) drawing, hence the call below.
     updateFromQuantity();

     LedDisplayTextField::draw(args);
 }

std::string retainNumbers(std::string mostlyNumbers) {
    std::string result = "";
    for (char ch : mostlyNumbers) {
        if (isdigit(ch)) {
            result += ch;
        }
    }

    return result;
}


 void NumberWidget::onSelectKey(const event::SelectKey& e) {
    INFO("Pressed %d", e.key);
    LedDisplayTextField::onSelectKey(e);

    if (updateFromQuantity()) {
        return;
    }

    std::string oldText = text;
    std::string newText = retainNumbers(oldText);
    if (oldText != newText) {
       setText(newText); 
    }

    if (newText != previousStringValue) {
        previousStringValue = newText;
        
        try {
            float newValue = std::stof(newText);
            paramQuantity->setValue(newValue);
        }
        catch (const std::invalid_argument &) {
            WARN("Caught invalid argument exception");
        }
    }
 }

 void NumberWidget::onSelectText(const event::SelectText& e) {
    INFO("onSelectText %d", e.codepoint);
    LedDisplayTextField::onSelectText(e);
    //  ui::TextField::onSelectText(e);
 }