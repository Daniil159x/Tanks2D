#ifndef MAPFIELD_HPP
#define MAPFIELD_HPP

#include <QImage>
#include <QJsonObject>

#include <memory>
#include <vector>

class Sprite;
class PlayerSprite;
#include "enums.hpp"

class MapField
{
public:
    MapField(const QString &fileName);

    std::vector<std::unique_ptr<Sprite>> getFiledSprites() const;
    std::vector<std::unique_ptr<PlayerSprite>> getPlayerSprites() const;

    const QImage &getImage_forSprite(int i, QChar cellName, const QImage &default_img) const;

    const QImage &getImage_forPlayer(QChar cellName) const;

    const QImage &getImage_forBullet() const;

    const QImage &getImage_forEffect(int i, const QImage &default_img) const;

    int getSize_effect() const;

    int getSizeImage_forSprites(QChar cellName) const;

    QSize getSize_field() const;

    QSize getSize_sprites() const;

    QSize getSize_bullet() const;

    const QImage &getImage_background() const;

    // FIXME: сделать ревизию кода, и заменить захардженые значения cell'ов
    QChar getPl1CellName() const;

    QChar getPl2CellName() const;

    QChar getBackgroundCellName() const;

    QChar getBulletCellName() const;

private:
    QChar m_pl1_CellName, m_pl2_CellName, m_backgroundCellName, m_bulletCellName;
    QImage m_background;

    QString m_prefix;

    QVector<QString> m_field;
    QRect   m_rect; // = {0, 0, 32, 32};
    QSize   m_filedSize;

    struct dataSprite {
        QVector<QImage> m_vec;
        typeItems       m_type;
        QRect           m_rect;
    };
    QHash<QChar, dataSprite> m_hash;

    QVector<QImage> m_effect;

    QJsonObject m_root;


    void loadField();
    void loadRectSprites();
    QString getStringOfCellSprite() const;
    void loadCellSprite(QChar ch);
};

#endif // MAPFIELD_HPP
