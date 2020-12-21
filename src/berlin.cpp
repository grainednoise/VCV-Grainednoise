#include "plugin.hpp"
#include "berlincode.hpp"
#include "ParamQuantityModel.hpp"
#include "NumberWidget.hpp"


struct Berlin : Module {
	enum ParamIds {
		XSIZE_PARAM,
		YSIZE_PARAM,
		SEED_PARAM,
		XSCALE_PARAM,
		YSCALE_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		XCOORDINATE_INPUT,
		YCOORDINATE_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		MAINOUT_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	const int CONTROL_CHANGE_RATE_DIVISOR = 20;
	int controlChangeCounter = CONTROL_CHANGE_RATE_DIVISOR;
	float currentXScale = 0.0;
	float currentYScale = 0.0;

	std::shared_ptr<Noise2D> noiseGen;

	Berlin() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(XSIZE_PARAM, 2.f, 16.f, 4.f, "X Size of random array");
		configParam(YSIZE_PARAM, 2.f, 16.f, 4.f, "Y size of random array");
		configParam<ParamQuantityModel>(SEED_PARAM, 1.f, 9999999.f, 1.f, "Random number seed");
		configParam(XSCALE_PARAM, 0.f, 2.f, 1.f, "X-scale multiplies input");
		configParam(YSCALE_PARAM, 0.f, 2.f, 1.f, "Y-scale multiplies input");
 	
		noiseGen = std::make_shared<Noise2D>();
	}

	void process(const ProcessArgs& args) override {
		if (++controlChangeCounter > CONTROL_CHANGE_RATE_DIVISOR) {
			controlChangeCounter = 0;

			int xSize = (int)round(params[XSIZE_PARAM].getValue());
			int ySize = (int)round(params[YSIZE_PARAM].getValue());
			unsigned long seed = (unsigned long)round(params[SEED_PARAM].getValue());

			noiseGen->setYSize(ySize);
			noiseGen->setXSize(xSize);
			noiseGen->setSeed(seed);

			currentXScale = params[XSCALE_PARAM].getValue();
			currentYScale = params[YSCALE_PARAM].getValue();
		}

		float xCoordinate = inputs[XCOORDINATE_INPUT].getVoltage() * currentXScale;
		float yCoordinate = inputs[YCOORDINATE_INPUT].getVoltage() * currentYScale;

		float output = noiseGen->getValue(xCoordinate, yCoordinate);

		outputs[MAINOUT_OUTPUT].setVoltage(output);
	}
};

struct GnoiseNormalKnob : RoundKnob {
	GnoiseNormalKnob() {
		if (pluginInstance) {
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/bakelite1.svg")));
		}
		shadow->opacity = 0.1;
	}
};

struct GnPlusButton : app::SvgSwitch {
	GnPlusButton() {
		momentary = false;
		shadow->opacity = 0.05;
		if (pluginInstance) {
			addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/gn_btn_up.svg")));
		}
	}
};

struct BerlinWidget : ModuleWidget {
	BerlinWidget(Berlin* module) {
		// WARN("Creating BERLIN widget %d", module);
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/berlin.svg")));

		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<GnoiseNormalKnob>(mm2px(Vec(9.154, 71.962)), module, Berlin::XSIZE_PARAM));
		addParam(createParamCentered<GnoiseNormalKnob>(mm2px(Vec(25.725, 71.962)), module, Berlin::YSIZE_PARAM));
		addParam(createParamCentered<GnPlusButton>(mm2px(Vec(17.5, 61.25)), module, Berlin::SEED_PARAM));
		addParam(createParamCentered<GnoiseNormalKnob>(mm2px(Vec(9.154, 90.872)), module, Berlin::XSCALE_PARAM));
		addParam(createParamCentered<GnoiseNormalKnob>(mm2px(Vec(25.725, 91.072)), module, Berlin::YSCALE_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.288, 109.781)), module, Berlin::XCOORDINATE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(25.792, 109.781)), module, Berlin::YCOORDINATE_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(42.295, 109.781)), module, Berlin::MAINOUT_OUTPUT));

		NoiseDisplay *ndisp = new NoiseDisplay;

		if (module) {
			ndisp->setNoiseGen(module->noiseGen);
		}
		ndisp->box.pos =  mm2px(Vec(4.143, 9.5));
		ndisp->box.size = mm2px(Vec(42.763, 42.763));
		addChild(ndisp);

		//When displayed in the module browser, 'module' will be NULL
		ParamQuantityModel *pq = module ? dynamic_cast<ParamQuantityModel *>(module->paramQuantities[Berlin::SEED_PARAM]) : NULL;

		NumberWidget *seedText = new NumberWidget(pq);
		seedText->box.pos = mm2px(Vec(22.5, 58));
		seedText->box.size = mm2px(Vec(26, 6.7));
		seedText->color = NVGcolor{1.0, 1.0, 1.0, 1.0};
		seedText->textOffset = Vec(0, 0);
		addChild(seedText);
	}
};


Model* modelBerlin = createModel<Berlin, BerlinWidget>("Berlin");