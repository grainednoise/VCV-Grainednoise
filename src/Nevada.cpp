#include "plugin.hpp"
#include "simplerandom.hpp"


const float CONTROL_SCALE = 10.0;

struct GnoiseChickenheadKnob : RoundKnob {
	GnoiseChickenheadKnob() {
		if (pluginInstance) {
			setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/chickenhead_knob.svg")));
		}
		shadow->opacity = 0.0;
	}
};

struct Nevada : Module {
	enum ParamIds {
		ENERGY_PARAM,
		SHAPE_PARAM,
		DECAY_PARAM,
		SATURATE_PARAM,
		RAND_PARAM,
		SHIELD_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		ENERGY_INPUT,
		SHAPE_INPUT,
		DECAY_INPUT,
		SATURATE_INPUT,
		RAND_INPUT,
		SHIELD_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		MAINOUT_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

	SimpleRandom random;
	float impulseAccumulator;
	float accumulator;
	float acCompensator;

	float prevShapeRaw = -100.0;
	float shape = 0.5;


	float PrevdecayRaw = -100.0;
	float decay = 0.5;

	Nevada() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
		configParam(ENERGY_PARAM, 0.f, 10.0f, 0.f, "");
		configParam(SHAPE_PARAM, 0.f, CONTROL_SCALE, 0.f, "");
		configParam(DECAY_PARAM, 0.f, CONTROL_SCALE, 0.f, "");
		configParam(SATURATE_PARAM, 0.f, CONTROL_SCALE, 0.f, "");
		configParam(RAND_PARAM, 0.f, CONTROL_SCALE, 0.f, "");
		configParam(SHIELD_PARAM, 0.f, CONTROL_SCALE, 0.f, "");
	}

	const float PROB_SCALE = 0.1 * 41100.0;

	void process(const ProcessArgs& args) override {
		float timeStep = args.sampleTime;
				
		float shapeRaw = params[SHAPE_PARAM].getValue();
		if (inputs[SHAPE_INPUT].isConnected()) {
			shapeRaw += inputs[SHAPE_INPUT].getVoltage();
		}

		if (shapeRaw != prevShapeRaw) {
			shape = clamp((float)exp(std::fmin((shapeRaw - CONTROL_SCALE) / 2, 0.0)), 0.0, 1.0);
		}

		float shield = 0.0;
		if (inputs[SHIELD_INPUT].isConnected()) {
			shield *= clamp(inputs[SHIELD_INPUT].getVoltage(), 0.0, CONTROL_SCALE) * params[SHIELD_PARAM].getValue() / CONTROL_SCALE;
		}
		else {
			shield = params[SHIELD_PARAM].getValue();
		}
		float negProb = shield * accumulator;

		float rawProb = 0.0;
		if (inputs[RAND_INPUT].isConnected()) {
			rawProb = clamp(inputs[RAND_INPUT].getVoltage(), 0.0, CONTROL_SCALE) * params[RAND_INPUT].getValue() / CONTROL_SCALE;
		}
		else {
			rawProb = params[RAND_INPUT].getValue();
		}

		float prob = (rawProb * (1 + shield) - negProb) * PROB_SCALE * timeStep;
		if (prob < 0.0) {
			prob = 0.0;
		}

		prob = prob * prob * prob * prob;	// Just for somewhat better scaling
		
		if (prob > random.float_uniform()) {
			float energy;
			if (inputs[ENERGY_INPUT].isConnected()) {
				energy = clamp(inputs[ENERGY_INPUT].getVoltage(), 0.0, CONTROL_SCALE) * params[ENERGY_PARAM].getValue() / CONTROL_SCALE;
			}
			else {
 				energy = params[ENERGY_PARAM].getValue();
			}

			impulseAccumulator = std::fmin(impulseAccumulator + shape * energy, 2.0);
		}

		// Decay calculation

		float decayRaw = params[DECAY_PARAM].getValue();
		if (inputs[DECAY_INPUT].isConnected()) {
			decayRaw = clamp(inputs[DECAY_INPUT].getVoltage() * 0.1 + decayRaw, 0.0, CONTROL_SCALE);
		}

		if (PrevdecayRaw != decayRaw) {
			decay = 1 - exp(decayRaw - 3.175) * 0.001;
			PrevdecayRaw = decayRaw;
		}

		accumulator += impulseAccumulator;
		accumulator = std::fmin(accumulator * decay, 100);

		// Saturation calculation

		float threshold = 0.0;
		if (inputs[SATURATE_INPUT].isConnected()) {
			threshold = clamp(inputs[SATURATE_INPUT].getVoltage() * params[SATURATE_PARAM].getValue(), 0.0, CONTROL_SCALE);
		}
		else {
			threshold =  params[SATURATE_PARAM].getValue();
		}

		float realOutput;
		if (accumulator >= 2 * threshold) {
			realOutput = threshold;
		}
		else {
			realOutput = (1 - accumulator / 4 / threshold) * accumulator;
		}

		realOutput *= 10 / (threshold + 1);

		acCompensator += (realOutput - acCompensator) * 0.003;

		outputs[MAINOUT_OUTPUT].setVoltage(realOutput - acCompensator);

		impulseAccumulator *= (1 - shape);
	}
};


struct NevadaWidget : ModuleWidget {
	NevadaWidget(Nevada* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/nevada.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<GnoiseChickenheadKnob>(mm2px(Vec(10.073, 21.422)), module, Nevada::ENERGY_PARAM));
		addParam(createParamCentered<GnoiseChickenheadKnob>(mm2px(Vec(30.005, 21.422)), module, Nevada::SHAPE_PARAM));
		addParam(createParamCentered<GnoiseChickenheadKnob>(mm2px(Vec(10.073, 50.394)), module, Nevada::DECAY_PARAM));
		addParam(createParamCentered<GnoiseChickenheadKnob>(mm2px(Vec(30.005, 50.394)), module, Nevada::SATURATE_PARAM));
		addParam(createParamCentered<GnoiseChickenheadKnob>(mm2px(Vec(10.073, 79.366)), module, Nevada::RAND_PARAM));
		addParam(createParamCentered<GnoiseChickenheadKnob>(mm2px(Vec(30.139, 79.366)), module, Nevada::SHIELD_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.073, 33.262)), module, Nevada::ENERGY_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.005, 33.262)), module, Nevada::SHAPE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.073, 62.234)), module, Nevada::DECAY_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.005, 62.234)), module, Nevada::SATURATE_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(10.073, 91.206)), module, Nevada::RAND_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.139, 91.206)), module, Nevada::SHIELD_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(28.022, 109.514)), module, Nevada::MAINOUT_OUTPUT));
	}
};


Model* modelNevada = createModel<Nevada, NevadaWidget>("Nevada");