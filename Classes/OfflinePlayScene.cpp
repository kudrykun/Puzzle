//
// Created by kudrykun on 25.01.18.
//




#include <iostream>

#include "ui/UIVideoPlayer.h"
#include "ui/CocosGUI.h"
#include "OfflinePlayScene.h"
#include "SimpleAudioEngine.h"
#include "curl/curl.h"
#include <rapidjson/document.h>
#include "rapidjson/writer.h"
#include "rapidjson/error/en.h"



USING_NS_CC;

Scene* OfflinePlay::createScene()
{
    log("Scene creation");
    return OfflinePlay::create();
}


bool OfflinePlay::init()
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

    //создаем плеер
    _videoPlayer = experimental::ui::VideoPlayer::create();
    _videoPlayer->setPosition(Vec2(visibleSize.width/2 + origin.x, visibleSize.height/2 + origin.y));
    _videoPlayer->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    _videoPlayer->setContentSize(Size(visibleSize.width *0.8,visibleSize.height *0.8));
    _videoPlayer->setFullScreenEnabled(true);
    _videoPlayer->setKeepAspectRatioEnabled(true);
    this->addChild(_videoPlayer);

    //не загружаем видео заново, если оно уже сохранено
    if(cocos2d::UserDefault::getInstance()->getBoolForKey("file_exist")) {
        _videoPlayer->setFileName(cocos2d::FileUtils::getInstance()->getWritablePath() + "video.mp4");
        _videoPlayer->play();
    }
    else
        loadVideo();



    return true;
}

//увеличиваем количество оффлайн просмотров
void OfflinePlay::onEnterTransitionDidFinish()
{

    log("finish transition to offline");
    int offlineViews = cocos2d::UserDefault::getInstance()->getIntegerForKey("offlineViews");
    log("%d", offlineViews);
    offlineViews++;
    cocos2d::UserDefault::getInstance()->setIntegerForKey("offlineViews", offlineViews);
    Scene::onEnterTransitionDidFinish(); //call base class onExit()
    unscheduleUpdate();
}



void OfflinePlay::loadVideo()
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
    request->setResponseCallback(CC_CALLBACK_2(OfflinePlay::onRequestVideoCompleted, this));
    request->setTag("Post test2");
    network::HttpClient::getInstance()->send(request);
    request->release();
}

//сохраняем видео и начнаем просмотр :)
void OfflinePlay::onRequestVideoCompleted(network::HttpClient *sender, network::HttpResponse *response)
{
    std::vector<char> *buffer = response->getResponseData();
    const char* file_char = buffer->data();
    rapidjson::Document document;
    document.Parse(file_char);


    CURL *curl;
    FILE *fp;
    CURLcode res;
    std::string str = cocos2d::FileUtils::getInstance()->getWritablePath() + "video.mp4";
    curl = curl_easy_init();
    if (curl)
    {
        fp = fopen(str.c_str(),"wb");
        curl_easy_setopt(curl, CURLOPT_URL, document["url"].GetString());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
    }

    _videoPlayer->setFileName(str);
    _videoPlayer->play();
}