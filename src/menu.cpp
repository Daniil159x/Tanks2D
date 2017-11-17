#include "menu.hpp"

#include <QDir>
#include <QDebug>

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

#include <QListWidgetItem>
#include <QScrollBar>

#include <QMetaEnum>

#define PATH_MAPS "../../maps/"

Menu::Menu(QWidget *parent) :
    QWidget(parent)
{
    this->setObjectName("Menu");


    QPixmap background(":/back/menu_background.jpg");
    background = background.scaled(background.size() / 1.5);

    this->setFixedSize(background.size());

    QPalette p;
    p.setBrush(QPalette::Background, background);
    this->setPalette(p);

    m_layout = new QVBoxLayout(this);


    // main view
    this->initMainView();

    // choice maps view
    this->initChoiceMapsView();

    // setting view
    this->initSettingView();


    this->setLayout(m_layout);
    this->toMainView();

//    QMetaEnum metaEnum = QMetaEnum::fromType<complexity>();
//    qDebug() << metaEnum.name();
}

Menu::~Menu()
{
}

Menu::generator_pos_t Menu::getSpeedGen() const
{
    switch (m_settingView->getCurrentSetting().complexity_game) {
    case complexity::easy:
        return [](){
            return std::make_pair((0.5 * FPS) / 60, (0.5 * FPS) / 60);
        };
        break;
    case complexity::normal:
        return [](){
            return std::make_pair((0.3 * FPS) / 60, (0.3 * FPS) / 60);
        };
        break;
    case complexity::crazy:
        return [](){
            return std::make_pair((0.1 * FPS) / 60, (0.1 * FPS) / 60);
        };
        break;
    case complexity::non_physical:
        return [](){
            const qreal x = static_cast<qreal>(1 + qrand() % 9) / 10;
            const qreal y = static_cast<qreal>(1 + qrand() % 9) / 10;
            return std::make_pair((x * FPS) / 60, (y * FPS) / 60);
        };
        break;
    default:
        throw std::logic_error(std::string(__FUNCTION__) + ": default branch");
    }
}

Menu::generator_pos_t Menu::getBulletSpeedGen() const
{
    switch (m_settingView->getCurrentSetting().complexity_game) {
    case complexity::easy:
        return [](){
            return std::make_pair((0.6 * FPS) / 60, (0.6 * FPS) / 60);
        };
        break;
    case complexity::normal:
        return [](){
            return std::make_pair((qreal(1.0) * FPS) / 60, (qreal(1.0) * FPS) / 60);
        };
        break;
    case complexity::crazy:
        return [](){
            return std::make_pair((0.5 * FPS) / 60, (0.5 * FPS) / 60);
        };
        break;
    case complexity::non_physical:
        return [](){
            const qreal x = static_cast<qreal>(5 + qrand() % 30) / 10;
            const qreal y = static_cast<qreal>(5 + qrand() % 30) / 10;
            return std::make_pair((x * FPS) / 60, (y * FPS) / 60);
        };
        break;
    default:
        throw std::logic_error(std::string(__FUNCTION__) + ": default branch");
    }
}

Menu::generator_time_t Menu::getShotDelayGen() const
{
    using namespace std::chrono_literals;

    switch (m_settingView->getCurrentSetting().complexity_game) {
    case complexity::easy:
        return []() -> duration {
            return 1500ms;
        };
        break;
    case complexity::normal:
        return []() -> duration {
            return 1000ms;
        };
        break;
    case complexity::crazy:
        return []() -> duration {
            return 500ms;
        };
        break;
    case complexity::non_physical:
        return []() -> duration {
            return std::chrono::milliseconds(500 + qrand() % 1000);
        };
        break;
    default:
        throw std::logic_error(std::string(__FUNCTION__) + ": default branch");
    }
}

SettingView::setting Menu::getSetting() const noexcept
{
    return m_settingView->getCurrentSetting();
}

void Menu::initMainView()
{
    m_pbGame = new QPushButton("Start", this);
    m_pbSetting = new QPushButton("Setting", this);
    m_pbExit = new QPushButton("Exit", this);

    m_pbGame->setObjectName("GameButton");
    m_pbSetting->setObjectName("SettingButton");
    m_pbExit->setObjectName("ExitButton");


    m_pbGame->setHidden(true);
    m_pbSetting->setHidden(true);
    m_pbExit->setHidden(true);

    m_layout->addWidget(m_pbGame);
    m_layout->addWidget(m_pbSetting);
    m_layout->addWidget(m_pbExit);

    connect(m_pbGame, &QPushButton::pressed, this, &Menu::toChoiceMapsView);
    connect(m_pbSetting, &QPushButton::pressed, this, &Menu::toSettingView);
    connect(m_pbExit, &QPushButton::pressed, this, &Menu::close);
}

void Menu::toMainView()
{
    this->hideChoiceMapsView();
    this->hideSettingView();

    m_pbExit->setHidden(false);
    m_pbGame->setHidden(false);
    m_pbSetting->setHidden(false);
}

void Menu::hideMainView()
{
    m_pbGame->setHidden(true);
    m_pbSetting->setHidden(true);
    m_pbExit->setHidden(true);
}

void Menu::initChoiceMapsView()
{
    m_listWid = new QListWidget(this);
    m_listWid->setObjectName("ListMaps");

    m_pbBack = new QPushButton("Back", this);
    m_pbBack->setObjectName("BackButton");

    m_listWid->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);

    connect(m_listWid, &QListWidget::itemDoubleClicked, this, [&](QListWidgetItem *item){
        emit start(item->toolTip(), item->text());
    });

    connect(m_pbBack, &QPushButton::pressed, this, &Menu::toMainView);

    m_listWid->setHidden(true);
    m_pbBack->setHidden(true);

    m_layout->addWidget(m_listWid);
    m_layout->addWidget(m_pbBack);
}

std::tuple<QString, bool> getNameMaps(const QString &jsonName, const QString &default_name = "no name") {
    QFile json(jsonName);
    json.open(QFile::ReadOnly);

    QJsonParseError err;
    const auto &obj = QJsonDocument::fromJson(json.readAll(), &err).object();

    return {obj["NameMap"].toString(default_name), (err.error == QJsonParseError::NoError)};
}

QStringList getMaps()
{
    QStringList res;

    QDir dir = QDir::current();
    dir.cd(PATH_MAPS);
    auto list = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::SortFlag::Time);

    for(const auto &i : list) {
        QDir jsonFile(dir.path() + "/" + i + "/");
        res += jsonFile.entryInfoList({"*.json"}, QDir::Files).first().absoluteFilePath();
        qDebug() << "Menu::loadMaps()" << "loaded" << res.back();
    }

    return res;
}

void Menu::toChoiceMapsView()
{
    this->hideMainView();

    for(const auto &path : getMaps()) {
        auto [name, ok] = getNameMaps(path);

        if(ok) {
            auto *ptr_item = new QListWidgetItem(name, m_listWid);
            ptr_item->setTextAlignment(Qt::AlignCenter);
            ptr_item->setToolTip(path);
        }
    }

    m_pbBack->setHidden(false);
    m_listWid->setHidden(false);
}

void Menu::hideChoiceMapsView()
{
    m_pbBack->setHidden(true);
    m_listWid->setHidden(true);
    m_listWid->clear();
}

void Menu::initSettingView()
{
    m_settingView = new SettingView(this);
    m_settingView->setHidden(true);
    m_settingView->setGeometry(this->rect());

    connect(m_settingView, &SettingView::pressed_ok, this, &Menu::toMainView);
    connect(m_settingView, &SettingView::pressed_cancel, this, &Menu::toMainView);
}

void Menu::toSettingView()
{
    this->hideMainView();
    m_settingView->show();
}

void Menu::hideSettingView()
{
    m_settingView->setHidden(true);
}
