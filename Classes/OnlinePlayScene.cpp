//
// Created by kudrykun on 25.01.18.
//


#include <iostream>

#include "ui/UIVideoPlayer.h"
#include "ui/CocosGUI.h"
#include "OnlinePlayScene.h"
#include <rapidjson/document.h>
#include "rapidjson/writer.h"
#include "rapidjson/error/en.h"


USING_NS_CC;

Scene* OnlinePlay::createScene()
{
    log("Scene creation");
    return OnlinePlay::create();
}

bool OnlinePlay::init()
{
    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Цвет фона
    auto bg = LayerColor::create(Color4B(0,0,0,255));
    this->addChild(bg,-99);

    //Кнопка возврата
    auto close_btn = ui::Button::create("arrow.png");
    close_btn->setBackgroundColor(Color3B(61,160,215));
    close_btn->setAnchorPoint(Vec2(1,1));

    close_btn->setScale(0.4);
    close_btn->setPosition(Vec2(visibleSize.width-10,visibleSize.height-10));
    this->addChild(close_btn,100);
    close_btn->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type){
        switch (type)
        {
            case ui::Widget::TouchEventType::ENDED:
                Director::getInstance()->popScene();
                break;
            default:
                break;
        }
    });

    //начинаем загрузку видео
    loadVideo();

    //создаем плеер
    _videoPlayer = experimental::ui::VideoPlayer::create();
    _videoPlayer->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
    _videoPlayer->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    _videoPlayer->setContentSize(Size(visibleSize.width *0.8,visibleSize.height *0.8));
    _videoPlayer->setFullScreenEnabled(true);
    _videoPlayer->setKeepAspectRatioEnabled(true);
    this->addChild(_videoPlayer);

    return true;
}


//увеличиваем количество онлайн просмотров
void OnlinePlay::onEnterTransitionDidFinish()
{
    int onlineViews = cocos2d::UserDefault::getInstance()->getIntegerForKey("onlineViews");
    onlineViews++;
    cocos2d::UserDefault::getInstance()->setIntegerForKey("onlineViews", onlineViews);
    Scene::onEnterTransitionDidFinish();
    unscheduleUpdate();
}



void OnlinePlay::loadVideo()
{
    std::vector<std::string> headers;
    headers.push_back("Content-Type:application/json; charset=utf-8");
    headers.push_back("Accept:application/json");


    network::HttpRequest* request = new (std::nothrow) network::HttpRequest();
    request->setUrl("https://puzzle-english.com/api/test.php");
    char data[] = "Hello World!";
    request->setRequestData(data,12);
    request->setRequestType(network::HttpRequest::Type::GET);
    request->setHeaders(headers);
    request->setResponseCallback(CC_CALLBACK_2(OnlinePlay::onRequestVideoCompleted, this));
    request->setTag("Post test2");
    network::HttpClient::getInstance()->send(request);
    request->release();
}


void OnlinePlay::onRequestVideoCompleted(network::HttpClient *sender, network::HttpResponse *response)
{
    std::vector<char> *buffer = response->getResponseData();
    const char* file_char = buffer->data();
    rapidjson::Document document;
    document.Parse(file_char);

    _videoPlayer->setURL(document["url"].GetString());
    _videoPlayer->play();

}
