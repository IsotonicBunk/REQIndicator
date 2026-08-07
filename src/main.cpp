#include <Geode/Geode.hpp>
#include <Geode/modify/CCscene.hpp>
#include <prevter.imageplus/include/api.hpp>

using namespace geode::prelude;
using namespace cocos2d;

void removeREQIndicator() {
    auto overlay = geode::OverlayManager::get();
    if (!overlay) return;
    if (auto indic = overlay->getChildByID("REQIndicator"_spr)) indic->removeFromParent();
}

std::string getREQIndicatorPathByName(std::string indic, bool is_custom, std::string custom_path) {
    if (is_custom) return custom_path;

    if (indic == "Simple REQ=OFF") return "reqoff-s.png"_spr;
    if (indic == "Animated REQ=OFF") return "reqoff-a.webp"_spr;
    if (indic == "HOLY REQ=OFF") return "reqoff-h.webp"_spr;
    if (indic == "Simple REQ=ON") return "reqon-s.png"_spr;
    if (indic == "Animated REQ=ON") return "reqon-a.webp"_spr;

    return "reqoff-a.webp"; // default
}

void updateREQIndicator() {
    bool enabled = Mod::get()->getSettingValue<bool>("enabled");
    auto indic = Mod::get()->getSettingValue<std::string>("indic");
    bool is_custom = Mod::get()->getSettingValue<bool>("is-custom");
    auto custom_path = Mod::get()->getSettingValue<std::filesystem::path>("custom-path").string();
    auto scale = Mod::get()->getSettingValue<double>("scale");
    auto offsetX = Mod::get()->getSettingValue<double>("offset-x");
    auto offsetY = Mod::get()->getSettingValue<double>("offset-y");

    

    auto overlay = geode::OverlayManager::get();
    if (!overlay) return;
    
    
    if (overlay->getChildByID("REQIndicator"_spr)) removeREQIndicator();
    if (!enabled) return;
    
    auto path = getREQIndicatorPathByName(indic, is_custom, custom_path).c_str();
    auto spr = imgp::AnimatedSprite::create(path);
    
    if (!spr) return;
    spr->setCurrentFrame(0);
    spr->setID("REQIndicator"_spr);
    spr->setScale(scale);
    spr->setZOrder(9998);
    auto safe = geode::utils::getSafeAreaRect();
    spr->setPosition(ccp(safe.origin.x + (offsetX * scale), safe.origin.y + safe.size.height - (offsetY * scale)));

    overlay->addChild(spr);
}
$on_mod(Loaded) {
    geode::listenForAllSettingChanges([](auto, auto) {
        updateREQIndicator();
    });
    updateREQIndicator();
}


