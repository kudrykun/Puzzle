#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "network/HttpClient.h"
#include "ui/CocosGUI.h"

class HelloWorld : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    void setMainMenu();
    void setNetworkErrLayer();
    virtual void onEnterTransitionDidFinish() override;
    void checkConnectionRequest();
    void onHttpRequestCompleted(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);

    bool network;
    cocos2d::Label* online_label;
    cocos2d::Label* offline_label;
    cocos2d::LayerColor* msgLayer;

    CREATE_FUNC(HelloWorld);
};

#endif
