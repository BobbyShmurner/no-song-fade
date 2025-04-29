#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>

using namespace geode::prelude;

class $modify(FMODAudioEngineFade, FMODAudioEngine) {
public:
	inline static bool allowFadeIn = true;
	inline static bool allowFadeOut = true;

	inline static bool allowFadeInSetting;
	inline static bool allowFadeOutSetting;

	void fadeInMusic(float fade_duration, int p1) {
		if (!allowFadeInSetting && !allowFadeIn) {
			geode::log::info("Fade-in blocked");
			return;
		}

		return FMODAudioEngine::fadeInMusic(fade_duration, p1);
	}

	void fadeOutMusic(float fade_duration, int p1) {
		if (!allowFadeOutSetting && !allowFadeOut) {
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

$execute {
	FMODAudioEngineFade::allowFadeInSetting = Mod::get()->getSettingValue<bool>("fadeIn");
    listenForSettingChanges("fadeIn", [](bool value) {
        FMODAudioEngineFade::allowFadeInSetting = value;
    });
	
	FMODAudioEngineFade::allowFadeOutSetting = Mod::get()->getSettingValue<bool>("fadeOut");
	listenForSettingChanges("fadeOut", [](bool value) {
		FMODAudioEngineFade::allowFadeOutSetting = value;
	});
}