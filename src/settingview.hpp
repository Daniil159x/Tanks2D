#ifndef SETTINGVIEW_HPP
#define SETTINGVIEW_HPP

#include <QWidget>
#include "enums.hpp"

namespace Ui {
class SettingView;
}

class SettingView : public QWidget
{
    Q_OBJECT

public:
    struct setting{
        QColor     colorField;
        complexity complexity_game;
        bool       fullScrean;
    };

    explicit SettingView(QWidget *parent = 0, setting set = {Qt::white, complexity::normal, true});
    ~SettingView();

    const setting& getCurrentSetting() const;

private slots:
    void on_pB_color_released();

    void on_pB_cancel_released();

    void on_pB_ok_released();

signals:
    void pressed_ok();
    void pressed_cancel();

private:
    void editCurrectColor(QColor c);


    Ui::SettingView *ui;

    setting m_curr, m_new;
};

#endif // SETTINGVIEW_HPP
