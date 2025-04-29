#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>

using namespace geode::prelude;

class $modify(FMODAudioEngineFade, FMODAudioEngine) {
public:
	inline static bool allowFadeIn = true;
	inline static bool allowFadeOut = true;

	void fadeInMusic(float fade_duration, int p1) {
		if (!allowFadeIn) {
			geode::log::info("Fade-in blocked");
			return;
		}

		return FMODAudioEngine::fadeInMusic(fade_duration, p1);
	}

	void fadeOutMusic(float fade_duration, int p1) {
		if (!allowFadeOut) {
			geode::log::info("Fade-out blocked");
			return;
		}
		
		return FMODAudioEngine::fadeOutMusic(fade_duration, p1);
	}
};

class $modify(PlayLayer) {
	void startMusic() {
		FMODAudioEngineFade::allowFadeIn = false;
		PlayLayer::startMusic();
		FMODAudioEngineFade::allowFadeIn = true;
	}

	void showCompleteEffect() {
		FMODAudioEngineFade::allowFadeOut = false;
		PlayLayer::showCompleteEffect();
		FMODAudioEngineFade::allowFadeOut = true;
	}
};
