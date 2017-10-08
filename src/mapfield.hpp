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

    const QImage &getImage_forSprite(int i, QChar Spr, const QImage &default_img) const;

    const QImage &getImage_forPlayer(dir d, QChar Spr) const;

    const QImage &getImage_forBullet(dir d) const;

    const QImage &getImage_forEffect(int i, const QImage &default_img) const;

    int getSize_effect() const;

    int getSize_forSprites(QChar Spr) const;

    QSize getSize_field() const;


    QSize getSize_sprites() const;

    const QImage &getImage_background() const;

private:
    QImage m_background;


    QVector<QString> m_field;
    QRect   m_rect = {0, 0, 32, 32};
    QSize   m_filedSize;

    struct dataSprite {
        QVector<QImage> m_vec;
        typeItems       m_type;
    };
    QHash<QChar, dataSprite> m_hash;

    QVector<QImage> m_effect;
};

#endif // MAPFIELD_HPP
