//
// Created by kudrykun on 25.01.18.
//

#ifndef __OFFLINEPLAY_SCENE_H__
#define __OFFLINEPLAY_SCENE_H__

#include "cocos2d.h"
#include "network/HttpClient.h"

class OfflinePlay : public cocos2d::Scene
{
public:
    static cocos2d::Scene* createScene();

    virtual bool init();
    void loadVideo();
    void onRequestVideoCompleted(cocos2d::network::HttpClient *sender, cocos2d::network::HttpResponse *response);
    virtual void onEnterTransitionDidFinish() override;
    cocos2d::experimental::ui::VideoPlayer* _videoPlayer;

    CREATE_FUNC(OfflinePlay);
};

#endif // __OFFLINEPLAY_SCENE_H__
