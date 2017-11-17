#ifndef MENU_HPP
#define MENU_HPP

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>

#include "enums.hpp"
#include "settingview.hpp"

namespace Ui {
class Menu;
}

#define FPS 100
#define DELAY_ANIMATION 40ms

class Menu : public QWidget
{
    Q_OBJECT

public:
    explicit Menu(QWidget *parent = 0);
    ~Menu();

    using clock = std::chrono::steady_clock;
    using time_point = clock::time_point;
    using duration = clock::duration;

    using generator_pos_t = std::pair<qreal, qreal> (*)();
    using generator_time_t = duration (*)();

    generator_pos_t getSpeedGen() const;
    generator_pos_t getBulletSpeedGen() const;
    generator_time_t getShotDelayGen() const;

    SettingView::setting getSetting() const noexcept;

signals:
    void start(const QString &pathMap, const QString &name);

private:
    QVBoxLayout *m_layout;

    // main view
    QPushButton *m_pbGame;
    QPushButton *m_pbSetting;
    QPushButton *m_pbExit;

    void initMainView();
    void toMainView();
    void hideMainView();

    // choice maps view
    QListWidget          *m_listWid;
    QPushButton          *m_pbBack;

    void initChoiceMapsView();
    void toChoiceMapsView();
    void hideChoiceMapsView();

    // setting
    SettingView *m_settingView;


    void initSettingView();
    void toSettingView();
    void hideSettingView();

    // progress load map

};

#endif // MENU_HPP
