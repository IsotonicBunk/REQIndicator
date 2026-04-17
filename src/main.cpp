//sry for my stupid code :/

#include <Geode/Geode.hpp>
#include <Geode/modify/CCDirector.hpp>
#include <sstream>
#include <iomanip>

using namespace geode::prelude;
using namespace cocos2d;

class GifPlayer : public CCNode {
private:
    CCSprite* m_sprite = nullptr;
    std::vector<CCSpriteFrame*> m_frames;
    int m_index = 0;
    float m_delay = 0.05f;

public:
    static GifPlayer* create(int frameCount, float fps = 30.f, bool reqon = false) {
        auto ret = new GifPlayer();
        if (ret && ret->init(frameCount, fps, reqon)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    bool init(int frameCount, float fps, bool reqon) {
        if (!CCNode::init()) return false;

        m_delay = 1.f / fps;

        std::string modID = "isotonicbunk.req-indicator";
        std::string prefix = modID + "/";   
        //if (reqon) {
        //    prefix = prefix + "reqon/";
        //};
        auto cache = CCTextureCache::sharedTextureCache();

        for (int i = 1; i <= frameCount; ++i) {
            std::stringstream ss;
            if (!reqon) {
            ss << prefix << "reqoff_" 
               << std::setw(4) << std::setfill('0') << i << ".png";
            } else {
            ss << prefix << "" 
               << std::setw(4) << std::setfill('0') << i << ".png";
            }
            std::string path = ss.str();

            auto texture = cache->addImage(path.c_str(), true);
            if (!texture) {
                log::error("failed to load frame: {}", path);
                continue;
            }

            auto rect = CCRectMake(0, 0, texture->getContentSize().width, texture->getContentSize().height);
            auto frame = CCSpriteFrame::createWithTexture(texture, rect);

            if (frame) {
                frame->retain();
                m_frames.push_back(frame);
            }
        }

        if (m_frames.empty()) {
            log::error("no frames was loaded");
            return false;
        }

        m_sprite = CCSprite::createWithSpriteFrame(m_frames[0]);
        if (!m_sprite) return false;

        this->addChild(m_sprite);
        this->schedule(schedule_selector(GifPlayer::updateFrame), m_delay);

        log::info("reqoff/on animation loaded with {} frames", m_frames.size());
        return true;
    }

    void updateFrame(float) {
        if (m_frames.empty() || !m_sprite) return;
        m_index = (m_index + 1) % m_frames.size();
        m_sprite->setDisplayFrame(m_frames[m_index]);
    }

    void setTopLeft(float x = 45.f, float y = 16.f) {
        auto winSize = CCDirector::sharedDirector()->getWinSize();
        this->setAnchorPoint({0, 1});
        this->setPosition({x, winSize.height - y});
    }

    ~GifPlayer() {
        for (auto f : m_frames) if (f) f->release();
    }
};



class $modify(ReqLaunch, CCDirector) {
    void willSwitchToScene(CCScene* scene) {
        auto displayGif = Mod::get()->getSettingValue<bool>("enabled");
        auto isReqon = Mod::get()->getSettingValue<bool>("onreq");
        if (displayGif) {
            CCDirector::willSwitchToScene(scene);
            if (!scene) return;


            if (scene->getChildByID("req-indic"_spr)) return;

            auto gif = GifPlayer::create(54, 30.f, false);
            if (isReqon) {
                gif = GifPlayer::create(54, 30.f, true);
            }
            if (!gif) {
                log::error("failed to create reqoff/on animation");
                return;
            }

            gif->setID("req-indic"_spr);
            //if (!isReqon) {
                gif->setTopLeft(45.f, 15.f);
                gif->setScale(0.6f);
            /*} else {
                gif->setTopLeft(45.f, 12.f);
                gif->setScale(0.2f);
            }*/
            gif->setZOrder(9999);
            gif->setVisible(true);
            //gif->setOpacity(255);

            scene->addChild(gif);

            log::info("reqoff/on added to scene");
        } else log::info("reqoff/on disabled");
    }
};