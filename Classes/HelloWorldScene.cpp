#include <iostream>

#include "ui/UIVideoPlayer.h"
#include "ui/CocosGUI.h"
#include "cocos2d.h"
#include "HelloWorldScene.h"
#include "OnlinePlayScene.h"
#include "OfflinePlayScene.h"

USING_NS_CC;


Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

bool HelloWorld::init()
{

    if ( !Scene::init() )
    {
        return false;
    }

    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // Устанвливаем слой в качестве бэкграунда
    auto bg = LayerColor::create(Color4B(236,236,236,255));
    this->addChild(bg,-99);

    //создаем кнопки и лэйблы
    setMainMenu();
    // создаем слой с сообщением
    setNetworkErrLayer();

    return true;
}

void HelloWorld::setMainMenu()
{
    // Настраиваем кнопку онлайн воспроизведения
    auto online_btn = ui::Button::create("rounded_button.png");
    online_btn->setTitleText("Воспроизвести видео онлайн");
    online_btn->setBackgroundColor(Color3B(76,178,98));  //см. UIButton.h и UIButton.cpp
    online_btn->setTitleFontSize(14.5);

    online_btn->setPosition(Vec2(this->getBoundingBox().size.width/2 ,this->getBoundingBox().size.height/2 + 30));
    online_btn->setScale(0.5);

    online_btn->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type){
        switch (type)
        {
            case ui::Widget::TouchEventType::ENDED:
                checkConnectionRequest();
                if(network){
                    Director::getInstance()->pushScene(this);
                    Director::getInstance()->replaceScene(OnlinePlay::createScene());
                }
                else{
                    msgLayer->setVisible(true);
                }
                break;
            default:
                break;
        }
    });
    this->addChild(online_btn,1);


    //Настраиваем лейбл онлайн просмотров
    online_label = Label::create();
    online_label->setString("Онлайн воспроизведений - " + std::to_string(cocos2d::UserDefault::getInstance()->getIntegerForKey("onlineViews")));
    online_label->setTextColor(Color4B(125, 125, 125, 255));
    online_label->setScale(0.6);
    online_label->setPosition(Vec2(this->getBoundingBox().size.width/2 + 10 ,this->getBoundingBox().size.height/2 + 10));
    this->addChild(online_label, 1);




    // Настраиваем кнопку оффлайн воспроизведения
    auto offline_btn = ui::Button::create("rounded_button.png");
    offline_btn->setTitleText("Воспроизвести видео оффлайн");
    offline_btn->setBackgroundColor(Color3B(61,160,215));
    offline_btn->setTitleFontSize(14.5);

    offline_btn->setScale(0.5);
    offline_btn->setPosition(Vec2(this->getBoundingBox().size.width/2 ,this->getBoundingBox().size.height/2 - 30));

    offline_btn->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type){
        switch (type)
        {
            case ui::Widget::TouchEventType::ENDED:
                checkConnectionRequest();
                if( cocos2d::UserDefault::getInstance()->getBoolForKey("file_exist") || network){
                    Director::getInstance()->pushScene(this);
                    Director::getInstance()->replaceScene(OfflinePlay::createScene());
                }
                else{
                    msgLayer->setVisible(true);
                }
                break;
            default:
                break;
        }
    });
    this->addChild(offline_btn,1);

    //Настраиваем лейбл оффлайн просмотров
    offline_label = Label::create();
    offline_label->setString("Оффлайн воспроизведений - " + std::to_string(cocos2d::UserDefault::getInstance()->getIntegerForKey("offlineViews")));
    offline_label->setTextColor(Color4B(125, 125, 125, 255));
    offline_label->setScale(0.6);
    offline_label->setPosition(Vec2(this->getBoundingBox().size.width/2 + 10 ,this->getBoundingBox().size.height/2 - 50));
    this->addChild(offline_label, 1);
}

void HelloWorld::setNetworkErrLayer()
{
    //настраиваем слой с сообщением
    msgLayer =  LayerColor::create(Color4B(0,0,0,160));
    this->addChild(msgLayer,100);

    //Контейнер с сообщением
    auto msgBox = ui::VBox::create(Size(150,120));
    msgBox->setPosition(Vec2(this->getBoundingBox().size.width/2 - 75 ,this->getBoundingBox().size.height/2 - 60));
    msgBox->setBackGroundColor(Color3B::WHITE);
    msgBox->setBackGroundColorType(ui::Layout::BackGroundColorType::SOLID);
    msgLayer->addChild(msgBox);

    //Сообщение об отсутствии сети
    auto label = Label::create();
    label->setString("Oops!\nОтсутствует подключение к интернету. ");
    label->setTextColor(Color4B(0,0,0,255));
    label->setHorizontalAlignment(TextHAlignment::CENTER);
    label->setScale(0.6);
    label->setPosition(Vec2(msgBox->getBoundingBox().size.width/2 ,msgBox->getBoundingBox().size.height/2 + 20));
    msgBox->addChild(label, 110);


    //Кнопка закрытия сообщения
    auto ok_button = ui::Button::create("rounded_button.png");
    ok_button->setTitleText("OK");
    ok_button->setBackgroundColor(Color3B(61,160,215));
    ok_button->setTitleFontSize(18);

    ok_button->setScale(0.5);
    ok_button->setPosition(Vec2(this->getBoundingBox().size.width/2 ,this->getBoundingBox().size.height/2 - 30));

    ok_button->addTouchEventListener([&](Ref* sender, ui::Widget::TouchEventType type){
        switch (type)
        {
            case ui::Widget::TouchEventType::ENDED:
                //this->getEventDispatcher()->setEnabled(true);
                msgLayer->setVisible(false);
                break;
            default:
                break;
        }
    });
    msgLayer->addChild(ok_button,1);
    msgLayer->setVisible(false);
}


// При посещении сцены устанавливаем значение количества просмотров и снова запускаем проверку соединения
void HelloWorld::onEnterTransitionDidFinish()
{
    //получаем количество просмотров из UserDefault
    int onlineViews = cocos2d::UserDefault::getInstance()->getIntegerForKey("onlineViews");
    int offlineViews = cocos2d::UserDefault::getInstance()->getIntegerForKey("offlineViews");

    online_label->setString("Онлайн воспроизведений - " + std::to_string(onlineViews));
    offline_label->setString("Оффлайн воспроизведений - " + std::to_string(offlineViews));
    ///data/user/0/Kudry.Puzzle/files/video.mp4

    cocos2d::UserDefault::getInstance()->setBoolForKey("file_exist", cocos2d::FileUtils::getInstance()->isFileExist(cocos2d::FileUtils::getInstance()->getWritablePath() + "video.mp4"));

    Scene::onEnterTransitionDidFinish();
    unscheduleUpdate();

    //проверяем соединение каждые 100ms
    schedule(SEL_SCHEDULE(&HelloWorld::checkConnectionRequest),0.1);
}

//Отправляем запрос для проверки соединения
void HelloWorld::checkConnectionRequest()
{
    auto request = new (std::nothrow) network::HttpRequest();
    request->setUrl("http://www.google.com");
    request->setRequestType(network::HttpRequest::Type::GET);
    request->setResponseCallback(CC_CALLBACK_2(HelloWorld::onHttpRequestCompleted, this));
    request->setTag("Post test2");
    network::HttpClient::getInstance()->send(request);
    request->release();
}

// после проверки соединения запоминаем статус соединения
void HelloWorld::onHttpRequestCompleted(network::HttpClient *sender, network::HttpResponse *response)
{
    if (!response || !response->isSucceed())
        network = false;
    else
        network = true;
    network ? log("connection OK") : log("connection LOST");
}