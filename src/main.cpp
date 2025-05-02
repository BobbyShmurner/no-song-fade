#include "Geode/loader/Log.hpp"
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>

using namespace geode::prelude;

class $modify(FMODAudioEngineFade, FMODAudioEngine) {
public:
	inline static bool blockFadeIn = false;
	inline static bool blockFadeOut = false;

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

	// The second parameter is the fade seems inconsistent between platforms
	// Passing 0 seems to be fine, so I'm gonna keep doing that until something breaks :)
	void doFadeIn() {
		FMODAudioEngine::sharedEngine()->fadeInMusic(2, 0);
	}
	
	void doFadeOut() {
		FMODAudioEngine::sharedEngine()->fadeOutMusic(2, 0);
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
			if (m_levelSettings->m_fadeOut || m_level->m_levelID.value() == 3001) {
				doFadeOut();
			}
		}
	}

	void onQuit() {
		FMODAudioEngineFade::blockFadeIn = false;
		FMODAudioEngineFade::blockFadeOut = false;
		PlayLayer::onQuit();
	}
};

$on_mod(Loaded) {
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
