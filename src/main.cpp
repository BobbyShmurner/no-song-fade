#include "Geode/loader/Log.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>

using namespace geode::prelude;

class $modify(FMODAudioEngineFade, FMODAudioEngine) {
public:
	inline static bool blockFadeIn = true;
	inline static bool blockFadeOut = true;

	void fadeInMusic(float fade_duration, int p1) {
		if (blockFadeIn) return;
		return FMODAudioEngine::fadeInMusic(fade_duration, p1);
	}

	void fadeOutMusic(float fade_duration, int p1) {
		if (blockFadeOut) return;
		return FMODAudioEngine::fadeOutMusic(fade_duration, p1);
	}
};

class $modify(PlayLayerFade, PlayLayer) {
	inline static std::string fadeInSetting;
	inline static std::string fadeOutSetting;
	inline static bool fadeInFromStartPosSetting;

	void doFadeIn() {
		FMODAudioEngine* engine = FMODAudioEngine::sharedEngine();
		engine->fadeInMusic(2, (int)engine);
	}
	
	void doFadeOut() {
		FMODAudioEngine* engine = FMODAudioEngine::sharedEngine();
		engine->fadeOutMusic(2, (int)engine);
	}

	void startMusic() {
		FMODAudioEngineFade::blockFadeIn = true;
		PlayLayer::startMusic();
		FMODAudioEngineFade::blockFadeIn = false;

		bool startOfLevel = m_gameState.m_levelTime <= 0.0;
		bool allowFadeBasedOnTime = startOfLevel || fadeInFromStartPosSetting;

		if (fadeInSetting == "Never Fade In") {
			geode::log::info("Blocking fade in");
		} else if (fadeInSetting == "Always Fade In" && allowFadeBasedOnTime) {
			geode::log::info("Forcing fade in");
			doFadeIn();
		} else {
			if (allowFadeBasedOnTime && m_levelSettings->m_fadeIn) {
				doFadeIn();
			}
		}
	}

	void showCompleteEffect() {
		FMODAudioEngineFade::blockFadeOut = true;
		PlayLayer::showCompleteEffect();
		FMODAudioEngineFade::blockFadeOut = false;

		if (fadeOutSetting == "Never Fade Out") {
			geode::log::info("Blocking fade out");
		} else if (fadeOutSetting == "Always Fade Out") {
			geode::log::info("Forcing fade out");
			doFadeOut();
		} else {
			// Most legendery check of all time
			if (m_levelSettings->m_fadeOut || (int)m_level->m_levelID == 3001) {
				doFadeOut();
			}
		}
	}
};

$execute {
	PlayLayerFade::fadeInSetting = Mod::get()->getSettingValue<std::string>("fadeIn");
    listenForSettingChanges("fadeIn", [](std::string value) {
        PlayLayerFade::fadeInSetting = value;
    });
	
	PlayLayerFade::fadeOutSetting = Mod::get()->getSettingValue<std::string>("fadeOut");
	listenForSettingChanges("fadeOut", [](std::string value) {
		PlayLayerFade::fadeOutSetting = value;
	});

	PlayLayerFade::fadeInFromStartPosSetting = Mod::get()->getSettingValue<bool>("fadeInFromStartPos");
    listenForSettingChanges("fadeInFromStartPos", [](bool value) {
        PlayLayerFade::fadeInFromStartPosSetting = value;
    });
}
