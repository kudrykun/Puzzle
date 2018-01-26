//
// Created by kudrykun on 25.01.18.
//
#ifndef __ONLINEPLAY_SCENE_H__
#define __ONLINEPLAY_SCENE_H__

#include "cocos2d.h"
#include "network/HttpClient.h"

class OnlinePlay : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    void loadVideo();
    void onRequestVideoCompleted(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    virtual void onEnterTransitionDidFinish() override;
    cocos2d::experimental::ui::VideoPlayer* _videoPlayer;
    // implement the "static create()" method manually
    CREATE_FUNC(OnlinePlay);
};

#endif // __ONLINEPLAY_SCENE_H__

