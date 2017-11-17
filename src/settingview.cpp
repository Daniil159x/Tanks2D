#include "settingview.hpp"
#include "ui_settingview.h"

#include <QColorDialog>
#include <QDebug>

SettingView::SettingView(QWidget *parent, setting set) :
    QWidget(parent),
    ui(new Ui::SettingView), m_curr(set), m_new(set)
{
    ui->setupUi(this);

    for(int i = 0; i < meta<complexity>().keyCount(); ++i){
        ui->cbBox_compexity->addItem(meta<complexity>().key(i));
    }

    ui->cbBox_compexity->setCurrentText(meta<complexity>().valueToKey(enum_cast(m_curr.complexity_game)));

    this->editCurrectColor(m_curr.colorField);
}

SettingView::~SettingView()
{
    delete ui;
}

const SettingView::setting &SettingView::getCurrentSetting() const
{
    return m_curr;
}

void SettingView::on_pB_color_released()
{
    auto cl = QColorDialog::getColor(m_curr.colorField, this,
                    "Color for field", QColorDialog::ShowAlphaChannel);
    editCurrectColor(cl);
}

void SettingView::editCurrectColor(QColor c)
{
    if(c.isValid()){
        QPixmap px(ui->label_currColor->size());
        px.fill(c);
        ui->label_currColor->setPixmap(px);
        m_new.colorField = c;
    }
}

void SettingView::on_pB_cancel_released()
{
    ui->chBox_fullScreen->setChecked(m_curr.fullScrean);
    ui->cbBox_compexity->setCurrentText(meta<complexity>().valueToKey(enum_cast(m_curr.complexity_game)));
    editCurrectColor(m_curr.colorField);

    emit pressed_cancel();
}

void SettingView::on_pB_ok_released()
{
    m_curr.colorField = m_new.colorField;
    m_curr.complexity_game = enum_cast<complexity>(meta<complexity>().value(ui->cbBox_compexity->currentIndex()));
    m_curr.fullScrean = ui->chBox_fullScreen->isChecked();

    emit pressed_ok();
}
